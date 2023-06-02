#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"
extern int proof_size;

// setup G(group), generater g vector number of polynomials Fx
int pokRep_setup(_struct_polynomial_pp_* pp, const int lamda, const int m, const int d, _struct_poly_ *poly)
{
    // pp = public parameter, lamda = security level, m = batch(1), d = 2^10, poly = poly
    int flag = 1;
    int qbit = 0;
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* bn_tmp = BN_new();
    BIGNUM* bn_tmp2 = BN_new();
    BIGNUM* tmp = BN_new();
    BIGNUM* bn_4 = BN_new();
	BIGNUM* bn_3 = BN_new();

    fmpz_t fmpz_tmp;

    fmpz_init(fmpz_tmp);
    fmpz_init(pp->b);
    fmpz_init(pp->p);
    fmpz_init(pp->q);
    fmpz_init(pp->cm_pp.L);
    fmpz_init(poly->z);
    fmpz_init(poly->fz);
    (pp->cm_pp).security_level = lamda;
    qfb_init((pp->cm_pp).g); // qfb
    fmpz_init((pp->cm_pp).G);

    
	BN_set_word(bn_4, 4);
	BN_set_word(bn_3, 3);
    
    pp->n = ceil(log2(d));
    pp->d = d;

    // pp->p 128비트 random prime 생성
    BN_generate_prime_ex(bn_tmp, 128, 1, NULL, NULL, NULL);
    fmpz_set_str(pp->p, BN_bn2hex(bn_tmp), 16); 

    // poly->z lamda비트 0 <= bn_tmp2 < bn_tmp 범위의 random  16진수 생성
    BN_rand_range_ex(bn_tmp2, bn_tmp, lamda, ctx); // mod p 범위 안에서 뽑기
    fmpz_set_str(poly->z, BN_bn2hex(bn_tmp2), 16);

    // f(z) 계산해서 poly에 저장
    for(int i=0; i< poly->d; i++){
        fmpz_powm_ui(fmpz_tmp, poly->z, i, pp->p);
        fmpz_mul(fmpz_tmp, fmpz_tmp, poly->Fx[i]);
        fmpz_mod(fmpz_tmp, fmpz_tmp, pp->p);

        fmpz_add(poly->fz, poly->fz, fmpz_tmp);
        fmpz_mod(poly->fz, poly->fz, pp->p);
    }

    // q range 범위 변경 128
    // trans-dark: qbit = 128*(2*pp->n + 1)+1;

    qbit = 128*(2*pp->n + 1)+1; 

    // set b <- {(p - 1)+(m - 1)(p - 1)^2}p^n
    fmpz_sub_ui(pp->b, pp->p, 1);
    fmpz_pow_ui(fmpz_tmp, pp->p, (pp->n));
    fmpz_mul(pp->b, fmpz_tmp, pp->b);  

	fmpz_zero(pp->q);
	fmpz_setbit(pp->q, qbit); // set qbit with pp->q

    // pp->cmp_pp: G = lamda size prime*prime, g = lamda/2 size random prime 
    KeyGen_Class_setup(&(pp->cm_pp), lamda);

    // pp->R = pp->n size array, lamda/2 크기의 prime 생성

    fmpz_one(fmpz_tmp);
    pp->R = (qfb_t*)malloc(sizeof(qfb_t) * pp->n);
    
    // 23.05 pp->cm_pp.g와 동일하게 뽑기
    for(int i=0 ; i < pp->n; i++) {
        qfb_init(pp->R[i]);
        do{
            BN_generate_prime_ex(tmp, lamda/4, 0, bn_4, bn_3, NULL);
            fmpz_set_str(fmpz_tmp, BN_bn2hex(tmp), 16);
            qfb_prime_form(pp->R[i], pp->cm_pp.G, fmpz_tmp);
        }while (!qfb_is_primitive(pp->R[i]) || !qfb_is_reduced(pp->R[i]) || fmpz_cmp((pp->R[i])->a, (pp->R[i])->b) <= 0 ); 
    }

    BN_CTX_free(ctx);
    BN_free(bn_tmp);
    fmpz_clear(fmpz_tmp);
    return flag;
}

// compute D_i = R_i^(g_{i,R})
int open_multipoly(qfb_t* D, const _struct_polynomial_pp_* pp, const _struct_poly_ poly, const fmpz_t q, const int i)
{
    _struct_commit_ cm_tmp;
    _struct_pp_ pp_tmp;
    
    // proof 안에 있는 d만 init 해주고 index 맞춰서 넣어주기
    // pf->D[i]

    qfb_init(cm_tmp.C); 
    fmpz_init_set(pp_tmp.G, pp->cm_pp.G);
    qfb_init(pp_tmp.g);
    qfb_set(pp_tmp.g, pp->R[i]);

    // index i에 대해서만 d_i <- R_i^g_(i, R)(q) 계산 
    commit_precompute(&cm_tmp, pp_tmp, poly, q, i); //<--- 23.05.12 division by ZERO IN CASE 3
    qfb_set(D[i], cm_tmp.C);
    // printf("d[%d]", i); fmpz_print(D[i]); printf("\n");

    qfb_clear(cm_tmp.C);
    fmpz_clear(pp_tmp.G);
    qfb_clear(pp_tmp.g);
}

// multipoly_open compute r, Q
// proof->r, proof->s, proof->Q, l_prime, pp, pp->q, &gR, poly
int pokRep_open(fmpz_t r, fmpz_t s[], qfb_t Q, const fmpz_t l, const _struct_polynomial_pp_* pp,
                    const fmpz_t q, _struct_poly_* g[], const _struct_poly_* f)
{
    _struct_pp_ pp_tmp; 
    _struct_open_ open;
    _struct_commit_ cm;

    fmpz_init(open.r);
    qfb_init(open.Q);
    fmpz_init_set(pp_tmp.G, pp->cm_pp.G);

    qfb_principal_form(Q,pp->cm_pp.G); // set Q to 1
    qfb_init(pp_tmp.g);
    qfb_set(pp_tmp.g, pp->cm_pp.g);

    pokRep_open_precom(&open, &cm, &pp_tmp, l, f, q, -1); // compute r ← x_1 mod ℓ, Q
    fmpz_set(r, open.r); 
    fmpz_mod(r, r, l);

    // Q <- G_1^(bn_dv) mod G
    qfb_nucomp(Q,Q,open.Q,pp->cm_pp.G,pp->cm_pp.L);
    qfb_reduce(Q,Q,pp->cm_pp.G);

    // s, Q계산
    for(int i=0 ; i < pp->n ; i++){
        fmpz_init(s[i]);
        // fmpz_set(pp_tmp.g, pp->R[i]);
        qfb_set(pp_tmp.g,pp->R[i]);
        pokRep_open_precom(&open, &cm, &pp_tmp, l, &(*g)[i], q, i);
        fmpz_set(s[i], open.r);
        fmpz_mod(s[i], s[i], l);
        qfb_nucomp(Q,Q,open.Q,pp->cm_pp.G,pp->cm_pp.L);
        qfb_reduce(Q,Q,pp->cm_pp.G);
    }
    
    fmpz_clear(open.r);
    qfb_clear(open.Q);
    fmpz_clear(pp_tmp.G);
    qfb_clear(pp_tmp.g);
    qfb_clear(cm.C);
}
// prover compute proof: D_i, y_{i,r}, alpha
int Open(_struct_proof_ *proof, _struct_polynomial_pp_* pp, _struct_commit_* cm, _struct_poly_* poly)
{
    unsigned long long int OPEN_RUNTIME = 0;
    unsigned long long int RunTime[2] = {0};
    unsigned long long int* pRuntime = &OPEN_RUNTIME;
    BN_CTX* ctx = BN_CTX_new();
    fmpz_t CD;
    fmpz_t l_prime;
    fmpz_t fmpz_tmp;
    fmpz_t fmpz_tmp1, fmpz_tmp2;
    fmpz_t alphaI; // alpha_i

    int i, j, d;
    int n = pp->n, qbit = fmpz_bits(pp->q) - 1; 

    static _struct_poly_ gL, gX; // gX: g_1(X)
    _struct_poly_* gR = NULL;

    fmpz_one(CD);
    fmpz_init(l_prime);
    fmpz_init(fmpz_tmp);
    fmpz_init(proof->gx);
    fmpz_init(proof->r);
    qfb_init(proof->Q);
    fmpz_init(fmpz_tmp1);
    fmpz_init(fmpz_tmp2);
    fmpz_init(alphaI);

    proof->s = (fmpz_t*)calloc(pp->n, sizeof(fmpz_t));
    proof->D = (qfb_t*)calloc(pp->n, sizeof(qfb_t));
    proof->y = (fmpz_t*)calloc(pp->n, sizeof(fmpz_t));
    proof->n = pp->n;

    // D벡터 메모리 할당, 벡터 길이: n 
    for(i = 0; i < n; i++) qfb_init(proof->D[i]);

    // y 벡터 메모리 할당, 벡터 길이: n 
    for(i = 0; i < n; i++) fmpz_init(proof->y[i]);

    TimerOn();

    // set gX degree
    gX.d = poly->d + 1;
    (*pRuntime) += TimerOff();

    gX.Fx = (fmpz_t*)calloc(gX.d, sizeof(fmpz_t));
    for(i = 0; i < gX.d; i++)
        fmpz_init(gX.Fx[i]);

    // initialize g
    for(i = 0; i < poly->d; i++)
        fmpz_set(gX.Fx[i], poly->Fx[i]);

    TimerOn();

    // set gL degree and init Fx
    gL.d = (poly->d+1)/2;
    (*pRuntime) += TimerOff();
    gL.Fx = (fmpz_t*)calloc(gL.d, sizeof(fmpz_t));
    for(i=0; i<gL.d; i++)
        fmpz_init(gL.Fx[i]);
    
    // g_1, g_(1, R), ... ,g_(n, R) 까지 다항식 저장할 공간 생성
    gR = (_struct_poly_*)calloc(n, sizeof(_struct_poly_));

    TimerOn();
    d = poly->d;

    // g_1(X)를 절반씩 자르기 시작. (d차 Fx를 n개 갖고 있는 gX)
    // g_(1, L)(X) = g_1[:(d+1)/2]
    // n번 반복, 절반씩 자르기
    for( i = 0; i < n; i++ ){

        // 다항식 차수가 홀수인 경우, +1 해놓고 최고차항 계수 0으로 설정
        if(d%2 != 0){
            d++;
            fmpz_zero(gX.Fx[d-1]);
        }

        // 다항식 차수 절반으로 줄여 gL과 gR로 분할
        d /= 2;

        // g_(i, R)다항식 저장 공간 할당 
        gR[i].Fx = (fmpz_t*)calloc(d, sizeof(fmpz_t));
        gR[i].d = d;

        // set gL, gR from gx
        for(j=0; j<d; j++){
            fmpz_init(gR[i].Fx[j]);
            fmpz_set(gL.Fx[j], gX.Fx[j]); // g_(i, L): g_i 왼쪽 부분 자르기
            fmpz_set(gR[i].Fx[j], gX.Fx[d + j]); // g_(i, R): g_i 나머지 부분 자르기 
        }
        // d_i <- R_i^g_(i, R)(q)
        open_multipoly(proof->D, pp, gR[i], pp->q, i);
        
        // y[i] += g_(i, R)[j]*z^j
        for(j=0;j<d;j++){
            fmpz_powm_ui(fmpz_tmp1, poly->z, j, pp->p); // z^j mod p
            fmpz_mul(fmpz_tmp1, fmpz_tmp1, gR[i].Fx[j]); // g_(i,r)[j]*z^j
            fmpz_mod(fmpz_tmp1, fmpz_tmp1, pp->p); // g_(i,r)[j]*z^j mod p
            fmpz_add(proof->y[i], proof->y[i], fmpz_tmp1); // y[i] += g_(i,r)[j]*z^j
            fmpz_mod(proof->y[i], proof->y[i], pp->p); //  mod p
        }

        get_alpha_SHA256(alphaI, cm->C, i); // get random alpha
        fmpz_mod(alphaI, alphaI, pp->p); // alpha_i <- alpha_i mod p 

        for(j=0;j<d;j++){
            fmpz_mul(fmpz_tmp2, alphaI, gR[i].Fx[j]); // alphaI * g_{i,R}
            fmpz_add(gX.Fx[j], gL.Fx[j], fmpz_tmp2); // g_(i+1) <- g_(i, L) + alpha_i * g_(i, R)
        }
    }

    (*pRuntime) += TimerOff();
    RunTime[0] += TimerOff();
    
    
    // 최종 상수항
    fmpz_set(proof->gx, gX.Fx[0]);

    TimerOn();
    
    // make l prime using proof->D
    Hprime_func(l_prime, proof->D, proof->n, cm->C);
    // input (G, g벡터, r벡터), CD, (f(q)벡터, g(q)벡터)
    pokRep_open(proof->r, proof->s, proof->Q, l_prime, pp, pp->q, &gR, poly);

    OPEN_RUNTIME += TimerOff();
    RunTime[1] += TimerOff();

    printf("__Poly_Open_01 %12llu [us]\n", RunTime[0]);
    printf("__Poly_Open_02 %12llu [us]\n", RunTime[1]);
    

    fmpz_clear(fmpz_tmp1);
    fmpz_clear(fmpz_tmp2);
    fmpz_clear(alphaI);

    for(i=0; i<gX.d; i++)
        fmpz_clear(gX.Fx[i]);
    for(i=0; i<gL.d; i++)
        fmpz_clear(gL.Fx[i]);
    for(int i = 0 ; i < pp->n; i++)
    {
        for(int j = 0; j < gR[i].d; j++)
            fmpz_clear(gR[i].Fx[j]);
            free(gR[i].Fx);
    }
    free(gX.Fx);
    free(gL.Fx);
    free(gR);

    BN_CTX_free(ctx);
    fmpz_clear(l_prime);
    fmpz_clear(CD);

    return OPEN_RUNTIME;
}

int Verify(_struct_polynomial_pp_* pp, _struct_commit_* cm, fmpz_t z, fmpz_t fz, _struct_proof_ *proof)
{
    unsigned long long int RunTime[2] = {0};
    struct timeval before[2]={0}, after[2] = {0};
    int flag = 1;
    char buffer[1000]={0};
    FILE *fp;
    BN_CTX* ctx = BN_CTX_new();
    fmpz_t l;
    qfb_t CD;
    fmpz_t fmpz_tmp, fmpz_tmp2;
    fmpz_t one;

    fmpz_t s_i, s_L;
    fmpz_t y_i, y_L;
    fmpz_t q_2exp, z_2exp;
    fmpz_t alphaI, alphaI_s, alphaI_y; // alpha_i

    fmpz_init(l);
    qfb_init(CD);
    fmpz_init(fmpz_tmp);
    fmpz_init(fmpz_tmp2);

    fmpz_one(one);

    Hprime_func(l, proof->D, proof->n, cm->C); // <--- 23.05.23 issue not equal with open.proof

    // C * product_(1,mu) D_i
    qfb_set(CD, cm->C);
    for(int i = 0; i< pp->n; i++){
        qfb_nucomp(CD, CD, proof->D[i], pp->cm_pp.G, pp->cm_pp.L);
        qfb_reduce(CD, CD, pp->cm_pp.G);
    }

    TimerOn2(before);
    flag = PoKRep_Ver(proof->r, proof->Q, CD, proof->s, pp->R, l, pp); // <------ 23.05.23 new issue
    RunTime[0] += TimerOff2(before, after);

    printf("pokrep result>> %d\n", flag); // accept 확인

    fmpz_init(s_i);
    fmpz_init(s_L);
    fmpz_init(y_i);
    fmpz_init(y_L);

    fmpz_init(alphaI);
    fmpz_init(alphaI_s);
    fmpz_init(alphaI_y);

    fmpz_init(q_2exp);
    fmpz_init(z_2exp);

    // s_1, y_1 compute
    fmpz_set(s_i, proof->r);
    fmpz_set(y_i, fz);

    TimerOn2(before+1);

    for(int i = 0; i < proof->n ; i++){
        fmpz_mul_2exp(fmpz_tmp, one, (proof->n - i - 1)); // 2^(n-i)
        
        // q^2^(n-i), z^2^(n-i) compute
        fmpz_powm(q_2exp, pp->q, fmpz_tmp, l);
        fmpz_powm(z_2exp, z, fmpz_tmp, pp->p);

        get_alpha_SHA256(alphaI, cm->C, i);
        fmpz_mod(alphaI_s, alphaI, l); // alphaI_i <- alphaI mod l

        fmpz_mul(s_L, q_2exp, proof->s[i]);
        fmpz_mod(s_L, s_L, l);
        fmpz_sub(s_L, s_i, s_L);
        fmpz_mod(s_L, s_L, l); // s_(i,L) 계산 완료
        
        fmpz_mul(fmpz_tmp2, alphaI_s, proof->s[i]);
        fmpz_mod(fmpz_tmp2, fmpz_tmp2, l); 
        fmpz_add(s_i, s_L, fmpz_tmp2);
        fmpz_mod(s_i, s_i, l); // s_(i+1) 계산 완료

        fmpz_mod(alphaI_y, alphaI, pp->p); // alphaI_i <- alphaI mod l

        fmpz_mul(y_L, z_2exp, proof->y[i]);
        fmpz_mod(y_L, y_L, pp->p);
        fmpz_sub(y_L, y_i, y_L);
        fmpz_mod(y_L, y_L, pp->p); // y_(i,L) 계산 완료

        fmpz_mul(fmpz_tmp2, alphaI_y, proof->y[i]);
        fmpz_mod(fmpz_tmp2, fmpz_tmp2, pp->p); 
        fmpz_add(y_i, y_L, fmpz_tmp2);
        fmpz_mod(y_i, y_i, pp->p); // y_(i+1) 계산 완료
    }
    RunTime[1] += TimerOff2(before+1, after+1);

    fmpz_mod(fmpz_tmp, proof->gx, l);
    flag &= fmpz_equal(fmpz_tmp, s_i);
    printf("g =? s_i>> %d\n", flag);

    fmpz_mod(fmpz_tmp, proof->gx, pp->p);
    flag &= fmpz_equal(fmpz_tmp, y_i);
    printf("g =? y_i>> %d\n", flag);

    fmpz_abs(fmpz_tmp, proof->gx); // 바운드도 accept 
    flag &= fmpz_cmp(fmpz_tmp, pp->b) < 0 ? 1 : 0; 
    printf("|g| < b >> %d\n", flag);


    printf("PoKReP_verify [0]%12llu\n", RunTime[0]);
    printf("verify for [1]%12llu\n", RunTime[1]);
    return flag;
}
// g^r*
int PoKRep_Ver(fmpz_t r, qfb_t Q, qfb_t CD, fmpz_t* s, qfb_t* R, const fmpz_t l, const _struct_polynomial_pp_* pp){
    qfb_t G_prime;
    qfb_t G_1;
    qfb_t qfb_tmp, qfb_tmp2;

    qfb_init(G_prime);
    qfb_init(G_1);
    qfb_init(qfb_tmp);
    qfb_init(qfb_tmp2);

    qfb_set(G_1, pp->cm_pp.g);

    // g^r 계산
    qfb_pow_with_root(G_prime, G_1, pp->cm_pp.G, r, pp->cm_pp.L);
    qfb_reduce(G_prime, G_prime, pp->cm_pp.G);

    // product G_i^r_i
    for(int i=0; i<pp->n; i++){
        qfb_t tmp1;
        fmpz_t tmp2;
        qfb_init(tmp1);
        fmpz_init(tmp2);
        qfb_set(tmp1, R[i]);
        fmpz_set(tmp2, s[i]);

        // r_i^s_i mod G
        qfb_pow_with_root(qfb_tmp2, tmp1, pp->cm_pp.G, tmp2, pp->cm_pp.L);
        qfb_reduce(qfb_tmp2, qfb_tmp2, pp->cm_pp.G);
        qfb_nucomp(G_prime, G_prime, qfb_tmp2, pp->cm_pp.G, pp->cm_pp.L); // g^r * (r_i^s_i)
        qfb_reduce(G_prime, G_prime, pp->cm_pp.G);
    }

    // G prime = Q^l * product G_i^r_i
    qfb_pow_with_root(qfb_tmp, Q, pp->cm_pp.G, l, pp->cm_pp.L);
    qfb_reduce(qfb_tmp, qfb_tmp, pp->cm_pp.G);
    qfb_nucomp(G_prime, G_prime, qfb_tmp, pp->cm_pp.G, pp->cm_pp.L);
    qfb_reduce(G_prime, G_prime, pp->cm_pp.G);

    return qfb_equal(CD, G_prime);
}
#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"
extern int proof_size;


// D를 벡터로 가져와서 str 싹 다 연결해서 l 출력 
int Hprime_func(fmpz_t output, const fmpz_t* in, const int n, const fmpz_t in2)
{
    unsigned char digest[SHA256_DIGEST_LENGTH]={0};
	unsigned char mdString[SHA256_DIGEST_LENGTH*2+1]={0};
	
    // D벡터 전부 str 가져와서 붙이기

    char** str_in = calloc(n, sizeof(char*));
    int str_len_total = 0;

    for(int i = 0; i < n; i++) {
        str_in[i] = fmpz_get_str(NULL, 16, in[i]);
        str_len_total += strlen(str_in[i]);
    }
    char *str_in2 = fmpz_get_str(NULL, 16, in2);
    str_len_total += strlen(str_in2);

	char *str_concat = calloc(str_len_total + 1, sizeof(char));

    for(int i = 0; i < n; i++) {
        strcat(str_concat, str_in[i]);
    }
    strcat(str_concat, str_in2);

	int concat_len = 0;

	SHA256(str_concat, strlen(str_concat), digest);   
	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

	mpz_t u, w;
	mpz_init_set_str(u,(char*)mdString,16);
	mpz_init(w);
	mpz_nextprime(w,u);	
	fmpz_set_mpz(output, w);

	mpz_clear(u);
	mpz_clear(w);
    for(int i = 0; i < n; i++) free(str_in[i]);
	free(str_in);
	free(str_in2);
	free(str_concat);

    return 1;
}

// 베타 랜덤 생성
// 기존: b_iL, b_iR 생성
// 업데이트: b_i 생성
int get_alpha_SHA256(fmpz_t alphaI, fmpz_t input, int idx)
{
    unsigned char digest[SHA256_DIGEST_LENGTH]={0};
	unsigned char mdString[SHA256_DIGEST_LENGTH/2+1]={0};
    char* str_input;
    fmpz_t tmpz_tmp;

    fmpz_init_set(tmpz_tmp, input);
    // fmpz_print(input);
    // printf("\n");
    fmpz_add_ui(tmpz_tmp, tmpz_tmp, 2*idx);
    str_input = fmpz_get_str(NULL, 16, tmpz_tmp);

	SHA256(str_input, strlen(str_input), digest);  
    for(int i = 0; i < SHA256_DIGEST_LENGTH/4; i++){
        digest[i] = (digest[i] ^ digest[16+i] ^ digest[8+i] ^ digest[24+i]);       
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    }    
    //printf("hash1 : %s\n", mdString);
    fmpz_set_str(alphaI, mdString, 16);
    free(str_input);

    fmpz_clear(tmpz_tmp);
    return 1;
}


// setup G(group), generater g vector number of polynomials Fx
int pokRep_setup(_struct_polynomial_pp_* pp, const int lamda, const int m, const int d, _struct_poly_ *poly)
{
    // pp = public parameter, lamda = security level, m = batch(1), d = 2^10, poly = poly
    int flag = 1;
    int qbit = 0;
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* bn_tmp = BN_new();
    BIGNUM* bn_tmp2 = BN_new();
    fmpz_t fmpz_tmp;

    fmpz_init(fmpz_tmp);
    fmpz_init(pp->b);
    fmpz_init(pp->p);
    fmpz_init(pp->q);
    fmpz_init(poly->z);
    fmpz_init(poly->fz);
    (pp->cm_pp).security_level = lamda;
    fmpz_init((pp->cm_pp).g);
    fmpz_init((pp->cm_pp).G);
    
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

    // q range
    qbit = 128*(2*pp->n + 1)+1; 

    // set b <- {(p - 1)+(m - 1)(p - 1)^2}p^n
    fmpz_sub_ui(pp->b, pp->p, 1);
    fmpz_pow_ui(fmpz_tmp, pp->p, (pp->n));
    fmpz_mul(pp->b, fmpz_tmp, pp->b); 

	fmpz_zero(pp->q);
	fmpz_setbit(pp->q, qbit); // set qbit with pp->q

    // pp->cmp_pp: G = lamda size prime*prime, g = lamda/2 size random prime 
    KeyGen_RSAsetup(&(pp->cm_pp), lamda);

    // pp->R = pp->n size array, lamda/2 크기의 prime 생성
    fmpz_one(fmpz_tmp);
    pp->R = (fmpz_t*)malloc(sizeof(fmpz_t) * pp->n);
    for(int i = 0; i < pp->n; i++){
        fmpz_init(pp->R[i]);
        BN_generate_prime_ex(bn_tmp, lamda>>1, 0, NULL, NULL, NULL);
        fmpz_set_str(pp->R[i], BN_bn2hex(bn_tmp), 16);
    }

    BN_CTX_free(ctx);
    BN_free(bn_tmp);
    fmpz_clear(fmpz_tmp);
    
    return flag;
}

// compute D_i = R_i^(g_{i,R})
int multipoly_commit(fmpz_t* D, const _struct_polynomial_pp_* pp, const _struct_poly_ poly, const fmpz_t q, const int i)
{
    _struct_commit_ cm_tmp;
    _struct_pp_ pp_tmp;
    
    // proof 안에 있는 d만 init 해주고 index 맞춰서 넣어주기
    // pf->D[i]
    fmpz_init(cm_tmp.C); 
    fmpz_init_set(pp_tmp.G, pp->cm_pp.G);
    fmpz_init(pp_tmp.g);
    
    // index i에 대해서만 d_i <- R_i^g_(i, R)(q) 계산 
    fmpz_set(pp_tmp.g, pp->R[i]);
    commit_precompute(&cm_tmp, pp_tmp, poly, q, i);
    fmpz_set(D[i], cm_tmp.C);
    // printf("d[%d]", i); fmpz_print(D[i]); printf("\n");

    fmpz_clear(cm_tmp.C);
    fmpz_clear(pp_tmp.G);
    fmpz_clear(pp_tmp.g);
}

// Prover intialize g 
// compute D_i, g_{i,R}, y_{i,R}
int OpenBound(fmpz_t* D, fmpz_t* y, _struct_poly_* gR[], fmpz_t gx, _struct_polynomial_pp_* pp, _struct_commit_* cm, _struct_poly_* poly, unsigned long long int* pRuntime)
{
    int i, j, d;
    int n = pp->n, qbit = fmpz_bits(pp->q) - 1; 
    BN_CTX* ctx;
    fmpz_t fmpz_tmp1, fmpz_tmp2;
    fmpz_t alphaI; // alpha_i
    static _struct_poly_ gL, gX; // gX: g_1(X)

    fmpz_init(fmpz_tmp1);
    fmpz_init(fmpz_tmp2);
    fmpz_init(alphaI);

    // D벡터 메모리 할당, 벡터 길이: n 
    for(i = 0; i < n; i++) fmpz_init(D[i]);

    // y 벡터 메모리 할당, 벡터 길이: n 
    for(i = 0; i < n; i++) fmpz_init(y[i]);

    TimerOn();
    // g_i(X) (여기서 i = 1) 계산 
    gX.d = poly->d + 1;
    (*pRuntime) += TimerOff();

    gX.Fx = (fmpz_t*)calloc(gX.d, sizeof(fmpz_t));
    for(i = 0; i < gX.d; i++)
        fmpz_init(gX.Fx[i]);
    // initialize g
    for(i = 0; i < poly->d; i++)
        fmpz_set(gX.Fx[i], poly->Fx[i]);

    TimerOn();
    gL.d = (poly->d+1)/2;
    (*pRuntime) += TimerOff();
    
    gL.Fx = (fmpz_t*)calloc(gL.d, sizeof(fmpz_t));
    for(i=0; i<gL.d; i++)
        fmpz_init(gL.Fx[i]);
    
    // g_1, g_(1, R), ... ,g_(n, R) 까지 다항식 저장할 공간 생성
    *gR = (_struct_poly_*)calloc(n, sizeof(_struct_poly_));

    TimerOn();
    d = poly->d;

    // g_1(X)를 절반씩 자르기 시작 
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
        (*gR)[i].Fx = (fmpz_t*)calloc(d, sizeof(fmpz_t));
        (*gR)[i].d = d;

        // 베타 랜덤으로 설정 -> 알파로 변경 (논문 PC.Open 7번 참고)
        get_alpha_SHA256(alphaI, cm->C, i);
        fmpz_mod(alphaI, alphaI, pp->p); // alpha_i <- alpha_i mod p 

        for(j=0; j<d; j++){            
            fmpz_init((*gR)[i].Fx[j]);
 
            fmpz_set(gL.Fx[j], gX.Fx[j]); // g_(i, L): g_i 왼쪽 부분 자르기
            fmpz_set((*gR)[i].Fx[j], gX.Fx[d + j]); // g_(i, R): g_i 나머지 부분 자르기 

            fmpz_mul(fmpz_tmp2, alphaI, (*gR)[i].Fx[j]); // alphaI * g_{i,R}

            fmpz_add(gX.Fx[j], gL.Fx[j], fmpz_tmp2); // g_(i+1) <- g_(i, L) + alpha_i * g_(i, R)
            
            // y[i] += g_(i, R)[j]*z^j
            fmpz_powm_ui(fmpz_tmp1, poly->z, j, pp->p); // z^j mod p
            fmpz_mul(fmpz_tmp1, fmpz_tmp1, (*gR)[i].Fx[j]); // g_(i,r)[j]*z^j
            fmpz_mod(fmpz_tmp1, fmpz_tmp1, pp->p); // g_(i,r)[j]*z^j mod p
            
            fmpz_add(y[i], y[i], fmpz_tmp1); // y[i] += g_(i,r)[j]*z^j
            fmpz_mod(y[i], y[i], pp->p); //  mod p
        }// 여기까지의 gX가 g_i(X)

        // d_i <- R_i^g_(i, R)(q)
        multipoly_commit(D, pp, (*gR)[i], pp->q, i);
        
    } // 여기까지 반복 완료하면 gR: g_1, g_(1, R), g_(2, R), ..., g_(n, R) 까지 저장한 다항식 배열

    (*pRuntime) += TimerOff();
    
    // 최종 상수항
    fmpz_set(gx, gX.Fx[0]);

    fmpz_clear(fmpz_tmp1);
    fmpz_clear(fmpz_tmp2);
    fmpz_clear(alphaI);
    for(i=0; i<gX.d; i++)
        fmpz_clear(gX.Fx[i]);
    for(i=0; i<gL.d; i++)
        fmpz_clear(gL.Fx[i]);

    free(gX.Fx);
    free(gL.Fx);

    return 1;
}


// multipoly_open
int pokRep_open(fmpz_t r, fmpz_t s[], fmpz_t Q, const fmpz_t l, const _struct_polynomial_pp_* pp,
                    const fmpz_t q, _struct_poly_* g[], const _struct_poly_* f)
{
    _struct_pp_ pp_tmp; 
    _struct_open_ open;
    _struct_commit_ cm;

    fmpz_init(open.r);
    fmpz_init(open.Q);
    fmpz_init_set(pp_tmp.G, pp->cm_pp.G);

    fmpz_one(Q);  
    fmpz_init_set(pp_tmp.g, pp->cm_pp.g);

    open_precompute(&open, &cm, &pp_tmp, l, f, q, -1); // r 계산
    fmpz_set(r, open.r); 
    fmpz_mod(r, r, l); // r = f(q) mod l
    fmpz_mul(Q,Q,open.Q); // Q <- G_1^(bn_dv) mod G
    fmpz_mod(Q,Q,pp_tmp.G);

    // s 벡터 계산
    for(int i =0; i < pp->n; i++){
        fmpz_init(s[i]);

        fmpz_set(pp_tmp.g, pp->R[i]); // Q 오른쪽 계산 
        open_precompute(&open, &cm, &pp_tmp, l, &(*g)[i], q, i);
        fmpz_set(s[i], open.r);
        fmpz_mod(s[i], s[i], l);
        fmpz_mul(Q,Q,open.Q);
        fmpz_mod(Q,Q,pp_tmp.G);
    }

    fmpz_clear(open.r);
    fmpz_clear(open.Q);
    fmpz_clear(pp_tmp.G);
    fmpz_clear(pp_tmp.g);
    fmpz_clear(cm.C);
}


// prover compute proof: D_i, y_{i,r}, alpha
int Open(_struct_proof_ *proof, _struct_polynomial_pp_* pp, _struct_commit_* cm, _struct_poly_* poly)
{
    unsigned long long int OPEN_RUNTIME = 0;
    unsigned long long int* pRuntime = &OPEN_RUNTIME;
    BN_CTX* ctx = BN_CTX_new();
    fmpz_t CD;
    fmpz_t l_prime;
    fmpz_t fmpz_tmp;

    _struct_poly_* gR = NULL;
    fmpz_one(CD);
    fmpz_init(l_prime);
    fmpz_init(fmpz_tmp);
    fmpz_init(proof->gx);
    fmpz_init(proof->r);
    fmpz_init(proof->Q);
    
    proof->s = (fmpz_t*)calloc(pp->n, sizeof(fmpz_t));
    proof->D = (fmpz_t*)calloc(pp->n, sizeof(fmpz_t));
    proof->y = (fmpz_t*)calloc(pp->n, sizeof(fmpz_t));
    proof->n = pp->n;

    // open bound 꺼내오기(논문에 존재X). 
    OpenBound(proof->D, proof->y, &gR, proof->gx, pp, cm, poly, pRuntime);    
    //









    //
    TimerOn();
    
    Hprime_func(l_prime, proof->D, proof->n, cm->C);

    // input (G, g벡터, r벡터), CD, (f(q)벡터, g(q)벡터)
    pokRep_open(proof->r, proof->s, proof->Q, l_prime, pp, pp->q, &gR, poly);

    OPEN_RUNTIME += TimerOff();

    BN_CTX_free(ctx);
    fmpz_clear(l_prime);
    fmpz_clear(CD);
    for(int i = 0 ; i < pp->n; i++)
    {
        for(int j = 0; j < gR[i].d; j++)
            fmpz_clear(gR[i].Fx[j]);
    }
    free(gR);

    return OPEN_RUNTIME;
}


int Verify(_struct_polynomial_pp_* pp, _struct_commit_* cm, fmpz_t z, fmpz_t fz, _struct_proof_ *proof)
{    
    int flag = 1;
    char buffer[1000]={0};
    FILE *fp;
    BN_CTX* ctx = BN_CTX_new();
    fmpz_t l;
    fmpz_t CD;
    fmpz_t fmpz_tmp, fmpz_tmp2;
    fmpz_t one;

    fmpz_t s_i, s_L;
    fmpz_t y_i, y_L;
    fmpz_t q_2exp, z_2exp;
    fmpz_t betaI, betaI_s, betaI_y; // beta_i

    fmpz_init(l);
    fmpz_init(CD);
    fmpz_init(fmpz_tmp);
    fmpz_init(fmpz_tmp2);

    fmpz_one(one);

    Hprime_func(l, proof->D, proof->n, cm->C); 

    // C * product_(1,mu) D_i
    fmpz_set(CD, cm->C);
    for(int i = 0; i< pp->n; i++){
        fmpz_mul(CD, CD, proof->D[i]);
        fmpz_mod(CD, CD, pp->cm_pp.G);
    }

    flag = PoKRep_Ver(proof->r, proof->Q, CD, proof->s, pp->R, l, pp);
    printf("pokrep result>> %d\n", flag); // accept 확인

    fmpz_init(s_i);
    fmpz_init(s_L);
    fmpz_init(y_i);
    fmpz_init(y_L);

    fmpz_init(betaI);
    fmpz_init(betaI_s);
    fmpz_init(betaI_y);

    fmpz_init(q_2exp);
    fmpz_init(z_2exp);

    // s_1, y_1 compute
    fmpz_set(s_i, proof->r);
    fmpz_set(y_i, fz);

    for(int i = 0; i < proof->n ; i++){
        fmpz_mul_2exp(fmpz_tmp, one, (proof->n - i - 1)); // 2^(n-i)
        
        // q^2^(n-i), z^2^(n-i) compute
        fmpz_powm(q_2exp, pp->q, fmpz_tmp, l);
        fmpz_powm(z_2exp, z, fmpz_tmp, pp->p);

        get_alpha_SHA256(betaI, cm->C, i);
        fmpz_mod(betaI_s, betaI, l); // beta_i <- beta_i mod l


        fmpz_mul(s_L, q_2exp, proof->s[i]);
        fmpz_mod(s_L, s_L, l);
        fmpz_sub(s_L, s_i, s_L);
        fmpz_mod(s_L, s_L, l); // s_(i,L) 계산 완료
        
        fmpz_mul(fmpz_tmp2, betaI_s, proof->s[i]);
        fmpz_mod(fmpz_tmp2, fmpz_tmp2, l); 
        fmpz_add(s_i, s_L, fmpz_tmp2);
        fmpz_mod(s_i, s_i, l); // s_(i+1) 계산 완료

        fmpz_mod(betaI_y, betaI, pp->p); // beta_i <- beta_i mod p

        fmpz_mul(y_L, z_2exp, proof->y[i]);
        fmpz_mod(y_L, y_L, pp->p);
        fmpz_sub(y_L, y_i, y_L);
        fmpz_mod(y_L, y_L, pp->p); // y_(i,L) 계산 완료

        fmpz_mul(fmpz_tmp2, betaI_y, proof->y[i]);
        fmpz_mod(fmpz_tmp2, fmpz_tmp2, pp->p); 
        fmpz_add(y_i, y_L, fmpz_tmp2);
        fmpz_mod(y_i, y_i, pp->p); // y_(i+1) 계산 완료

    }


    fmpz_mod(fmpz_tmp, proof->gx, l);
    flag &= fmpz_equal(fmpz_tmp, s_i);
    printf("g =? s_i>> %d\n", flag);

    fmpz_mod(fmpz_tmp, proof->gx, pp->p);
    flag &= fmpz_equal(fmpz_tmp, y_i);
    printf("g =? y_i>> %d\n", flag);

    fmpz_abs(fmpz_tmp, proof->gx); // 바운드도 accept 
    flag &= fmpz_cmp(fmpz_tmp, pp->b) < 0 ? 1 : 0; 
    printf("|g| < b >> %d\n", flag);

    return flag;
}

// g^r*
int PoKRep_Ver(fmpz_t r, fmpz_t Q, fmpz_t CD, fmpz_t* s, fmpz_t* R, const fmpz_t l, const _struct_polynomial_pp_* pp){
    fmpz_t G_prime;
    fmpz_t G_1;
    fmpz_t fmpz_tmp, fmpz_tmp2;

    fmpz_init(G_prime);
    fmpz_init(G_1);
    fmpz_init(fmpz_tmp);
    fmpz_init(fmpz_tmp2);

    fmpz_set(G_1, pp->cm_pp.g);

    // g^r 계산
    fmpz_powm(G_prime, G_1, r, pp->cm_pp.G);

    // product G_i^r_i
    for(int i=0; i<pp->n; i++){
        fmpz_t tmp1, tmp2;
        fmpz_init(tmp1);
        fmpz_init(tmp2);
        fmpz_set(tmp1, R[i]);
        fmpz_set(tmp2, s[i]);

        // r_i^s_i mod G
        fmpz_powm(fmpz_tmp2, tmp1, tmp2, pp->cm_pp.G);
        fmpz_mul(G_prime, G_prime, fmpz_tmp2); // g^r * (r_i^s_i)
        fmpz_mod(G_prime, G_prime, pp->cm_pp.G);
    }

    // G prime = Q^l * product G_i^r_i
    fmpz_powm(fmpz_tmp, Q, l, pp->cm_pp.G);
    fmpz_mul(G_prime, G_prime, fmpz_tmp);
    fmpz_mod(G_prime, G_prime, pp->cm_pp.G);

    return fmpz_equal(CD, G_prime);
}
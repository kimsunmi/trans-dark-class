#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"

// group G에서 g, m, 스칼라 q를 뽑아 g*m 연산과 g^q연산의 차이


int main(int argc, char *argv[])
{
    _struct_polynomial_pp_ class_pp = {0};
    // struct rsa rsa_pp;
    qfb_t* pre_table;
    qfb_t class_g;
    qfb_t tmp_g;

    
    BIGNUM* tmp = BN_new();
    BIGNUM* bn_4 = BN_new();
	BIGNUM* bn_3 = BN_new();
    
    fmpz_t fmpz_p;
    fmpz_t q;
    int qbit;

    fmpz_init(q);
    Read_pp(&class_pp);

    unsigned long long int Runtime_class_g = 0, Runtime_class_s = 0, Runtime_class_g2=0, Runtime_class_s2=0;

    qfb_init(class_g);
    qfb_init(tmp_g);

    pre_table = (qfb_t*)malloc(sizeof(qfb_t)*class_pp.d);
    for(int i = 0; i < class_pp.d; i++){
        qfb_init(pre_table[i]);
    }

    do{        
        BN_generate_prime_ex(tmp, class_pp.cm_pp.security_level/4, 0, bn_4, bn_3, NULL);
		fmpz_set_str(fmpz_p, BN_bn2hex(tmp), 16);
		qfb_prime_form(class_g, class_pp.cm_pp.G, fmpz_p);
    }while (!qfb_is_primitive(class_g) || !qfb_is_reduced(class_g) || fmpz_cmp((class_g)->a, (class_g)->b) <= 0 ); 

    // printf("g를 새로 뽑은 것: ");
    // qfb_print(class_g);
    // printf("\n");
    // qfb_print(class_pp.cm_pp.g);
    // printf("\n");
    // 시간차이를 보기 위해 d번 연산 반복-> G*g vs g^q(scalar)
    // G*g
    // g^q
    TimerOn();
    for(int i = 1; i < 1000; i++){
        qfb_nucomp(tmp_g, class_g, class_pp.cm_pp.g, class_pp.cm_pp.G, class_pp.cm_pp.L);
        qfb_reduce(tmp_g, tmp_g, class_pp.cm_pp.G);
        //qfb_nucomp(class_g, class_g, class_pp.cm_pp.g, class_pp.cm_pp.G, class_pp.cm_pp.L);
        //qfb_reduce(class_g, class_g, class_pp.cm_pp.G);
    }
    Runtime_class_g = TimerOff();

    printf("\n");
    qfb_set(pre_table[0], class_pp.cm_pp.g);
    qbit = 128*(2*class_pp.n + 1)+1;
	fmpz_setbit(class_pp.q, qbit); // set qbit with pp->q

    // fmpz_setbit(q, 1);
    // fmpz_print(q);
    // printf("\n\n");

    TimerOn();
    for(int j=1; j < 1000; j++)
    {
        qfb_pow_with_root(tmp_g, class_pp.cm_pp.g, class_pp.cm_pp.G, class_pp.q , class_pp.cm_pp.L); 
        qfb_reduce(tmp_g, tmp_g, class_pp.cm_pp.G);
    }
    Runtime_class_s = TimerOff();

    printf("group_Time %12llu [us]\n", Runtime_class_g);
    printf("scalar_Time %12llu [us]\n", Runtime_class_s);


    fmpz_clear(fmpz_p);
    fmpz_clear(q);
    for(int i=0; i<class_pp.n; i++){
        qfb_clear(pre_table[i]);
    }
    free(pre_table);

	return 0;
}

#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"

// group G에서 g, m, 스칼라 q를 뽑아 g*m 연산과 g^q연산의 차이
struct class
{
    fmpz_t G; // group
    qfb_t g_1; // generator
    qfb_t g_2; // generator
    qfb_t g1; // generator
    qfb_t g2; // generator
    fmpz_t L; // discriminant
};
struct rsa
{
    fmpz_t G; // Group (rsa modulus)
    qfb_t g; // generator
};

int main(int argc, char *argv[])
{
    struct class class_pp;
    struct rsa rsa_pp;
    
    BIGNUM* tmp = BN_new();
    BIGNUM* bn_4 = BN_new();
	BIGNUM* bn_3 = BN_new();
    
    fmpz_t fmpz_p;
    fmpz_t q;

	fmpz_init(fmpz_p);
    fmpz_init(class_pp.G);
    qfb_init(class_pp.g_1);
    qfb_init(class_pp.g_2);
    fmpz_init(class_pp.L);

    unsigned long long int RunTime = 0;

    int n = 10; // degree의 최대 차수
    int d = 1<<10; // degree
    int qbit = 128*(2*n + 1)+1; // scalar q

	fmpz_zero(q);
	fmpz_setbit(q, qbit); // set qbit with pp->q

    int lamda = 512; // security level

    // class group: make G, g_1, g_2
    do{
	 	BN_generate_prime_ex(tmp, lamda, 1, bn_4, bn_3, NULL);
		fmpz_set_str(class_pp.G, BN_bn2hex(tmp), 16);
		fmpz_neg(class_pp.G, class_pp.G);
	}while(BN_num_bits(tmp) != lamda);

    do{
        BN_generate_prime_ex(tmp, lamda/4, 0, bn_4, bn_3, NULL);
		fmpz_set_str(fmpz_p, BN_bn2hex(tmp), 16);
		qfb_prime_form(class_pp.g_1, class_pp.G, fmpz_p);
    }while (!qfb_is_primitive(class_pp.g_1) || !qfb_is_reduced(class_pp.g_1) || fmpz_cmp((class_pp.g_1)->a, (class_pp.g_1)->b) <= 0 ); 
    do{
        BN_generate_prime_ex(tmp, lamda/4, 0, bn_4, bn_3, NULL);
		fmpz_set_str(fmpz_p, BN_bn2hex(tmp), 16);
		qfb_prime_form(class_pp.g_2, class_pp.G, fmpz_p);
    }while (!qfb_is_primitive(class_pp.g_2) || !qfb_is_reduced(class_pp.g_2) || fmpz_cmp((class_pp.g_2)->a, (class_pp.g_2)->b) <= 0 ); 

    qfb_set(class_pp.g1, class_pp.g_1);
    qfb_set(class_pp.g2, class_pp.g_2);

    // 시간차이를 보기 위해 d번 연산 반복-> G*g vs g^q(scalar)
    // G*g
    TimerOn();
    for(int i = 0; i < d; i++){
        qfb_nucomp(class_pp.g_1, class_pp.g_1, class_pp.g_2, class_pp.G, class_pp.L);
        qfb_reduce(class_pp.g_1, class_pp.g_1, class_pp.G);
    }
    RunTime = TimerOff();
    printf("group_Time %12llu [us]\n", RunTime);

    qfb_set(class_pp.g_1, class_pp.g1);
    qfb_set(class_pp.g_2, class_pp.g2);

    TimerOn();

    // g^q
    for(int i = 0; i < d; i++){
        qfb_pow_with_root(class_pp.g_1, class_pp.g_2, class_pp.G, q, class_pp.L);
        qfb_reduce(class_pp.g_1, class_pp.g_1, class_pp.G);
    }
    RunTime = TimerOff();
    printf("scalar_Time %12llu [us]\n", RunTime);

	return 0;
}

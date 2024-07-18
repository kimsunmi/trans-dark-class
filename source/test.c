#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"
#include <openssl/rand.h>


// group G에서 g, m, 스칼라 q를 뽑아 g*m 연산과 g^q연산의 차이


int main(int argc, char *argv[])
{
    unsigned long long int Runtime_class_g = 0, Runtime_class_s = 0, Runtime_class_g2=0, Runtime_class_s2=0;

    _struct_polynomial_pp_ class_pp = {0};
    size_t count;
    unsigned char *bin;
    mpz_t mpz_q;


    // struct rsa rsa_pp;
    qfb_t* pre_table;
    qfb_t class_g;
    qfb_t tmp_g;
    qfb_t tmp2_g;
    qfb_t class_g2;
    
    BIGNUM* tmp = BN_new();
    BIGNUM* tmp2 = BN_new();

    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* bn_4 = BN_new();
	BIGNUM* bn_3 = BN_new();
    BN_set_word(bn_4, 4);
	BN_set_word(bn_3, 3);

    fmpz_t fmpz_p;
    fmpz_t q;
    fmpz_t alpha;
    fmpz_t ratio_q;
    fmpz_t ratio_r;
    fmpz_t result;
    int qbit;

    fmpz_init(class_pp.cm_pp.G);
    qfb_init(class_pp.cm_pp.g);
    fmpz_init(class_pp.cm_pp.L);
    fmpz_init(ratio_q);
    fmpz_init(ratio_r);
    fmpz_init(result);

    if(argc == 2)
        class_pp.cm_pp.security_level = atoi(argv[1]);

    fmpz_init(q);
    fmpz_init(alpha);
    Read_pp(&class_pp);
    class_pp.n = 10;
    class_pp.d = 1<<(class_pp.n);
    qbit = 128*(2*class_pp.n + 1)+1;
	fmpz_setbit(class_pp.q, qbit); // set qbit with pp->q
    printf("qbit: %d\n", qbit);

    qfb_init(class_g);
    qfb_init(tmp_g);
    qfb_init(class_g2);
    qfb_init(tmp2_g);

	printf("lambda, d: %d %d\n", class_pp.cm_pp.security_level, class_pp.d);			

	// do{
	//  	BN_generate_prime_ex(tmp, (class_pp.cm_pp).security_level, 1, bn_4, bn_3, NULL);
	// 	fmpz_set_str(class_pp.cm_pp.G, BN_bn2hex(tmp), 16);
	// 	fmpz_neg(class_pp.cm_pp.G, class_pp.cm_pp.G);
	// }while(BN_num_bits(tmp) != (class_pp.cm_pp).security_level);

    fmpz_abs(class_pp.cm_pp.L, class_pp.cm_pp.G);
	fmpz_root(class_pp.cm_pp.L, class_pp.cm_pp.L, 4);

    for(int i=0; i<1000;i++){
        do{        
            BN_generate_prime_ex(tmp, class_pp.cm_pp.security_level/4, 0, bn_4, bn_3, NULL);
		    fmpz_set_str(fmpz_p, BN_bn2hex(tmp), 16);
		    qfb_prime_form(class_g, class_pp.cm_pp.G, fmpz_p);
        }while (!qfb_is_primitive(class_g) || !qfb_is_reduced(class_g) || fmpz_cmp((class_g)->a, (class_g)->b) <= 0 ); 

        printf("i: %d \n",i);
        do{        
            BN_generate_prime_ex(tmp, class_pp.cm_pp.security_level/4, 0, bn_4, bn_3, NULL);
		    fmpz_set_str(fmpz_p, BN_bn2hex(tmp), 16);
		    qfb_prime_form(class_g2, class_pp.cm_pp.G, fmpz_p);
        }while (!qfb_is_primitive(class_g2) || !qfb_is_reduced(class_g2) || fmpz_cmp((class_g2)->a, (class_g2)->b) <= 0 ); 
        
        TimerOn();
        qfb_nucomp(tmp_g, class_g, class_g2, class_pp.cm_pp.G, class_pp.cm_pp.L);
        qfb_reduce(tmp_g, tmp_g, class_pp.cm_pp.G);
        Runtime_class_g += TimerOff();
        if (RAND_poll() != 1) {
            printf("RAND_poll() failed\n");
        }
        // BN_rand(tmp, 2688, 0, 0);
        // fmpz_set_str(alpha, BN_bn2hex(tmp), 16); 
        // TimerOn();
        // qfb_pow_with_root(tmp_g, class_g, class_pp.cm_pp.G, alpha, class_pp.cm_pp.L); 
        // qfb_reduce(tmp_g,tmp_g, class_pp.cm_pp.G);
        // Runtime_class_g2 += TimerOff();
        
        BN_rand(tmp, 256, 0, 0);
        fmpz_set_str(alpha, BN_bn2hex(tmp), 16); 
        TimerOn();
        qfb_pow_with_root(tmp_g, class_g, class_pp.cm_pp.G, alpha, class_pp.cm_pp.L); 
        qfb_reduce(tmp_g,tmp_g, class_pp.cm_pp.G);
        Runtime_class_s2 += TimerOff();
    }
    // printf("g1*g2_ %12llu [us]\n", Runtime_class_g);
    // printf("g^a_[0_q-1] %12llu [us]\n", Runtime_class_g2);
    printf("g^b_[0_256] %12llu [us]\n", Runtime_class_s2);

    
    // 시간차이를 보기 위해 d번 연산 반복-> G*g vs g^q(scalar)
    // G*g
    // g^q
    // qfb_set(tmp_g, class_g);
    
    // TimerOn();
    // qfb_pow_with_root(tmp_g, tmp_g, class_pp.cm_pp.G, class_pp.q, class_pp.cm_pp.L); 
    // qfb_reduce(tmp_g, tmp_g, class_pp.cm_pp.G);
    // Runtime_class_s = TimerOff();
    // printf("g^q_exp_Time %12llu [us]\n", Runtime_class_s);

    // qfb_set(tmp_g, class_g);
    // TimerOn();
    // for(int j=0; j < class_pp.d; j++)
    // {
    //     qfb_pow_with_root(tmp_g, tmp_g, class_pp.cm_pp.G, class_pp.q , class_pp.cm_pp.L); 
    //     qfb_reduce(tmp_g, tmp_g, class_pp.cm_pp.G);
    // }
    // Runtime_class_s2 = TimerOff();
    // printf("g^q^d_exp_Time %12llu [us]\n", Runtime_class_s2);

    // int num1=0;
    // int num2 =0;
    // fmpz_t tmp3;
    // fmpz_init(tmp3);
    // for(int i = 0; i<1000; i++){
        // BN_rand(tmp, 2688, 0, 0);
        // fmpz_set_str(alpha, BN_bn2hex(tmp), 16); 
        // qfb_pow_with_root(tmp_g, tmp_g, class_pp.cm_pp.G, alpha, class_pp.cm_pp.L); 

        // if(fmpz_cmp(alpha, tmp3) == 0) printf("not random ");
        // fmpz_set(tmp3, alpha);
        // if(fmpz_cmp(alpha, class_pp.q) > 0 ) num1++;
        // else if(fmpz_cmp(alpha, class_pp.q) < 0 ) num2++;
        // else printf(" equal ");
    // }
    // printf("a>q: %d\n",num1);
    // printf("a<q: %d\n",num2);

    fmpz_clear(fmpz_p);
    fmpz_clear(q);

	return 0;
}


/*
int main(){
    fmpz_t num; //  0110 1011
    fmpz_t num2; // 1111 1111
    fmpz_t test; // 0110 1011
    int i_num[12] = {0,1,2,3,4};
    int k_num[8] =  {0,1,2,3,4,5,6,7,8};
    fmpz_init(num);
    fmpz_init(num2);
    fmpz_init(test);

    for(int i = 0;i<8; i++){
        fmpz_set_ui(num,i); // num=i

        printf("\n-----------\n");
        printf("fmpz_num: ");
        fmpz_print(num);
        printf("\n");
        // fmpz_setbit(num, i_num[i]); // n번째 bit를 1로 만든다.
        // fmpz_setbit(num2, k_num[i]);
        // printf("fmpz_setbit: ");
        // fmpz_print(num);
        // printf("\n");
        // printf("fmpz_bits: ");
        // gmp_printf("%Mu\n", fmpz_bits(num)); //비트수만큼 돌려줌
        for(int k = 0; k<4;k++){
            if(fmpz_tstbit(num, k)){
                printf("k: %d ",k);
            }
        }
       // printf("%d 번째 result: %d",i,i_num[i]%10);
        // printf("\n--------------------\n");
    }
    // fmpz_and(test, num, num2);
    // fmpz_print(test);
    // printf("\n");     
    // fmpz_init(num2);
    // fmpz_bit_pack(num2, 2, 0, num, 1, 0);  // arr은 init이어야하고 negate를 1로 설정 시 coeff 오른쪽에 shit만큼 bit가 들어간게 arr에 저장됨
    
    // printf("\n");
    // fmpz_print(num2);
    // printf("\n");
    // fmpz_print(num);

    // for(int i = 0; i< fmpz_bits(test); i++){
    //     if(i == 0) printf("hello");
    //     else printf("hi");
    // } 11010110 

}
*/
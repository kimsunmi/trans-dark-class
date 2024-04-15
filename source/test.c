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
    qfb_t tmp2_g;

    
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
    qfb_init(tmp2_g);

    do{        
        BN_generate_prime_ex(tmp, class_pp.cm_pp.security_level/4, 0, bn_4, bn_3, NULL);
		fmpz_set_str(fmpz_p, BN_bn2hex(tmp), 16);
		qfb_prime_form(class_g, class_pp.cm_pp.G, fmpz_p);
    }while (!qfb_is_primitive(class_g) || !qfb_is_reduced(class_g) || fmpz_cmp((class_g)->a, (class_g)->b) <= 0 ); 

    // 시간차이를 보기 위해 d번 연산 반복-> G*g vs g^q(scalar)
    // G*g
    // g^q
    qfb_set(tmp_g, class_g);
    qbit = 128*(2*class_pp.n + 1)+1;
	fmpz_setbit(class_pp.q, qbit); // set qbit with pp->q

    TimerOn();
    for(int j=0; j < qbit; j++){
        qfb_nucomp(tmp_g, tmp_g, class_g, class_pp.cm_pp.G, class_pp.cm_pp.L);
        qfb_reduce(tmp_g, tmp_g, class_pp.cm_pp.G);
        // qfb_nucomp(class_g, class_g, class_pp.cm_pp.g, class_pp.cm_pp.G, class_pp.cm_pp.L);
        // qfb_reduce(class_g, class_g, class_pp.cm_pp.G);
    }
    Runtime_class_g = TimerOff();

    printf("\n");
    
    qfb_set(tmp_g, class_pp.cm_pp.g);
    TimerOn();
    for(int j=1; j < 2; j++)
    {
        qfb_pow_with_root(tmp_g, tmp_g, class_pp.cm_pp.G, class_pp.q , class_pp.cm_pp.L); 
        qfb_reduce(tmp_g, tmp_g, class_pp.cm_pp.G);
    }
    Runtime_class_s = TimerOff();

    printf("g^q_scalar_Time %12llu [us]\n", Runtime_class_g);
    printf("g^q_exp_Time %12llu [us]\n", Runtime_class_s);


    fmpz_clear(fmpz_p);
    fmpz_clear(q);

	return 0;
}

/*
int main(){
    fmpz_t num; //  0110 1011
    fmpz_t num2; // 1111 1111
    fmpz_t test; // 0110 1011
    int i_num[12] = {0,1,0,3,0,5,6,0,0,9,10,11};
    int k_num[8] =  {0,1,2,3,4,5,6,7,8};
    fmpz_init(num);
    fmpz_init(num2);
    fmpz_init(test);

    for(int i = 0;i<8; i++){
        printf("-----------\n");
        printf("fmpz_num: ");
        fmpz_print(num);
        printf("\n");
        fmpz_setbit(num, i_num[i]); // n번째 bit를 1로 만든다.
        fmpz_setbit(num2, k_num[i]);
        printf("fmpz_setbit: ");
        fmpz_print(num);
        printf("\n");
        printf("fmpz_bits: ");
        gmp_printf("%Mu\n", fmpz_bits(num)); //비트수만큼 돌려줌
        // if(fmpz_tstbit(num, i)){
        //     printf("yes\n");
        // }
        // printf("%d 번째 result: %d",i,i_num[i]%10);
        // printf("\n--------------------\n");
    }
    fmpz_and(test, num, num2);
    fmpz_print(test);
    printf("\n");     
    fmpz_init(num2);
    fmpz_bit_pack(num2, 2, 0, num, 1, 0);  // arr은 init이어야하고 negate를 1로 설정 시 coeff 오른쪽에 shit만큼 bit가 들어간게 arr에 저장됨
    
    printf("\n");
    fmpz_print(num2);
    printf("\n");
    fmpz_print(num);

    // for(int i = 0; i< fmpz_bits(test); i++){
    //     if(i == 0) printf("hello");
    //     else printf("hi");
    // } 11010110 

}
*/
#include "../hedder/util.h"
//#include <omp.h>

int main(int argc, char *argv[])
{
    int flag = 1;
    int m = 1;
    int logD, degree;
	int lamda = 512; 
    int qbit;
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* bn_tmp = BN_new();
	BIGNUM* bn_p = BN_new();
	BIGNUM* bn_q = BN_new();
    fmpz_t fmpz_tmp_1;
    fmpz_t* fmpz_GR;
    fmpz_t* fmpz_tmp;
    fmpz_t G, b, p, q;
    fmpz_t phi_p, phi_q, phi_G;

	if(argc == 2)
		logD = atoi(argv[1]);
    else if(argc == 3)
    {
		lamda = atoi(argv[1]);
		logD = atoi(argv[2]);        
    }
	else
		logD = 10;

    qbit = 128*(2*logD + 1);
    degree = 1<<logD;
    //////////////////////////////////////////////////////

    int n = ceil(log2((1<<logD))); 
    fmpz_GR = (fmpz_t*)calloc(n + 1, sizeof(fmpz_t));
    fmpz_tmp = (fmpz_t*)calloc(n + 1, sizeof(fmpz_t));
    //////////////////////////////////////////////////////

    fmpz_init(phi_p);
    fmpz_init(phi_q);
    fmpz_init(phi_G);
    fmpz_init(G);
    fmpz_init(p);
    fmpz_init(q);
    fmpz_init(b);
    fmpz_init(fmpz_tmp_1);
    printf("set G\r\n");
	do{
		BN_generate_prime_ex(bn_p,(lamda>>1),1,NULL,NULL,NULL);
		BN_generate_prime_ex(bn_q,(lamda>>1),1,NULL,NULL,NULL);
		BN_mul(bn_tmp, bn_p, bn_q, ctx);
		fmpz_set_str(G, BN_bn2hex(bn_tmp), 16);
        fmpz_set_str(phi_p, BN_bn2hex(bn_p), 16);
        fmpz_set_str(phi_q, BN_bn2hex(bn_q), 16);
	}while(BN_num_bits(bn_tmp) != lamda);

    fmpz_sub_ui(phi_p, phi_p, 1);
    fmpz_sub_ui(phi_q, phi_q, 1);
    fmpz_mul(phi_G, phi_p, phi_q);

    //////////////////////////////////////////////////////

    printf("set p\r\n");
	BN_generate_prime_ex(bn_tmp, 128, 1, NULL, NULL, NULL);
    fmpz_set_str(p, BN_bn2hex(bn_tmp), 16);

    //fmpz_set_ui(fmpz_tmp_1, (n)+2);
    //fmpz_set_ui(pp->b, m);
    fmpz_pow_ui(fmpz_tmp_1, p, (n)+2);
    fmpz_mul_ui(fmpz_tmp_1, fmpz_tmp_1, m);
    fmpz_tdiv_q_2exp(b, fmpz_tmp_1, 1);

	fmpz_zero(q);
	fmpz_setbit(q, qbit); //qbit

    fmpz_one(fmpz_tmp_1);
    // #pragma omp parallel for
    for(int i = 0; i <= n; i++){
        fmpz_init(fmpz_GR[i]);
        
        BN_generate_prime_ex(bn_tmp, 128, 1, NULL, NULL, NULL);
        fmpz_set_str(fmpz_GR[i], BN_bn2hex(bn_tmp), 16);
        fmpz_init_set(fmpz_tmp[i], fmpz_GR[i]);
    }

    printf("NEW!!\r\n");
    fmpz_mod(phi_q, q, phi_G);    
    // #pragma omp parallel for
    for(int i = 0; i<= n; i++)
    {
        FILE *fp;
        char str1[100] = {0};

        sprintf(str1, "precom/%d/G%d.txt", degree, i);
        printf("%s\r\n", str1);

        fp = fopen(str1, "w");
        printf("test\r\n");
        fprintf(fp, "%s\r\n", fmpz_get_str(NULL, 16, fmpz_GR[i]));
        if(fmpz_cmp(fmpz_tmp[i], fmpz_GR[i]) != 0)  
            printf("%d error\r\n", i);
        
        for(int j = 1; j < (i==0 ? degree : degree/2); j++)
        {
            printf("%d\r\n", j);
            fmpz_powm(fmpz_GR[i], fmpz_GR[i], phi_q, G);
            // fmpz_powm(fmpz_tmp[i], fmpz_tmp[i], q, G);
            // if(fmpz_cmp(fmpz_tmp[i], fmpz_GR[i]) != 0){
            //     printf("%d %d error\r\n", i, j);
            //     break;
            // }
            fprintf(fp, "%s\r\n", fmpz_get_str(NULL, 16, fmpz_GR[i]));
        }
        fclose(fp);
    }

    FILE *fp;
    char str1[100] = "precom/";
    char str2[100] = {0};
    sprintf(str2, "%d", degree );
    strcat(str1, str2);
    strcat(str1, "/G.txt");
    fp = fopen(str1, "w");
    fprintf(fp, "%s\r\n", fmpz_get_str(NULL, 16, G));
    fclose(fp);

    BN_free(bn_tmp);
	BN_free(bn_p);
	BN_free(bn_q);
	BN_CTX_free(ctx);
    fmpz_clear(fmpz_tmp_1);
    
    return flag;
}

// int start_precomputation(_struct_polynomial_pp_* pp, const _struct_poly_ poly)
// {
// 	int i, j;
// 	if(isprecomputed==0)
// 	{
// 		FILE *fp;
// 		unsigned long long int RunTime1=0;
// 		TimerOff();
// 		printf("Start precomputation\n");
// 		TimerOn();

// 		precompute_num = poly.d;
// 		pre_table = (fmpz_t**)calloc(sizeof(fmpz_t*), n + 1);
// 		for(i = 0; i < n + 1; i++)
// 			pre_table[i] = (fmpz_t*)calloc(sizeof(fmpz_t), precompute_num);

// 		fmpz_init_set(pre_table[0][0], pp->cm_pp.g);
// 		for(int i=1; i<= n; i++)
// 			fmpz_init_set(pre_table[i][0], pp->R[i-1]);		


// 		#pragma omp parallel for
// 		for(int i=0; i<= n; i++)
// 		{	
// 			for(int j = 1; j < precompute_num/2; j++)
// 			{
// 				fmpz_init(pre_table[i][j]);
// 				fmpz_powm(pre_table[i][j], pre_table[i][j-1], pp->q, pp->cm_pp.G);
// 			}
// 		}
		
// 		for(j = precompute_num/2; j < precompute_num; j++)
// 		{
// 			fmpz_init(pre_table[0][j]);
// 			fmpz_powm(pre_table[0][j], pre_table[0][j-1], pp->q, pp->cm_pp.G);
// 		}


// 		RunTime1 = TimerOff();
// 		printf("PRE_COMPUTE %12llu [us]\n", RunTime1);
// 		fp = fopen("record/precompute.txt", "a+");
// 		fprintf(fp, "%d %d %llu\n", pp->cm_pp.lamda, poly.d, RunTime1);			
// 		fclose(fp);
// 		TimerOn();
// 	}
// 	isprecomputed = 1;
	
// 	return isprecomputed;
// }

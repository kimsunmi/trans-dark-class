#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"
#include <unistd.h>
#include <omp.h>
extern int global_num_threads;

static fmpz_t **pre_table;
static int precompute_num = 0, isprecomputed = 0;

struct timeval before[10]={0}, after[10] = {0};
unsigned long long int RunTime[10] = {0};

int KeyGen_RSAsetup( _struct_pp_ *pp, const int lamda )
{
	BIGNUM* p = BN_new();
	BIGNUM* q = BN_new();
	BIGNUM* tmp = BN_new();
	BN_CTX* ctx = BN_CTX_new();

	do{
		BN_generate_prime_ex(p,(lamda>>1),0,NULL,NULL,NULL);
		BN_generate_prime_ex(q,(lamda>>1),0,NULL,NULL,NULL);
		BN_mul(tmp,p,q, ctx);
		// G = lamda size prime*prime
		fmpz_set_str(pp->G, BN_bn2hex(tmp), 16);
	}while(BN_num_bits(tmp) != lamda);

	// g = lamda/2 size random prime 
	BN_generate_prime_ex(tmp,lamda >> 1,0,NULL,NULL,NULL);
	fmpz_set_str(pp->g, BN_bn2hex(tmp), 16);

	BN_free(p);
	BN_free(q);
	BN_free(tmp);
	BN_CTX_free(ctx);

	return 1;
}

int pp_init(_struct_pp_* pp)
{
	fmpz_init(pp->G);
	fmpz_init(pp->g);

	return 1;
}

int pp_clear(_struct_pp_* pp)
{
	fmpz_clear(pp->G);
	fmpz_clear(pp->g);

	return 1;
} 

// make precomputation table (0,0)...(0,2^10)
//                           ...
//                           (n,0)...(n,2^10)
int start_precomputation(_struct_polynomial_pp_* pp, const _struct_poly_ poly)
{
	int i, j;
	if(isprecomputed==0)
	{
		FILE *fp;
		int isfile_exist = 1;
		unsigned long long int RunTime1=0;
		unsigned long long int RunTime2=0;
		int logD = ceil(log(poly.d)/log(2));
		TimerOff();
		printf("Start precomputation\n");
		TimerOn();

		char str1[100] = {0}, str2[3050] = {0};
		sprintf(str1, "precom/%d/", 1<<logD);
		if(access(str1, F_OK) >= 0)
		{
			sprintf(str1, "precom/%d/G.txt", 1<<logD);
			if(access(str1, F_OK) >= 0)
			{
				for(int i = 0; i<= pp->n; i++)
				{
					sprintf(str1, "precom/%d/G%d.txt", 1<<logD, i);
					if(access(str1, F_OK) < 0 || access(str1, R_OK) < 0){
						isfile_exist = 0;
						break;
					}
				}
			}
			else
				isfile_exist = 0;
		}else
			isfile_exist = 0;

		precompute_num = (isfile_exist == 1) ? (1<<logD) : (poly.d);
		pre_table = (fmpz_t**)calloc(pp->n + 1, sizeof(fmpz_t*));
		for(i = 0; i < pp->n + 1; i++)
			pre_table[i] = (fmpz_t*)calloc(precompute_num, sizeof(fmpz_t));

		// if file exist, read polynomial.
		if(isfile_exist == 1)
		{
			printf("Read precompute data\n");
			sprintf(str1, "precom/%d/G.txt", precompute_num);
			printf("%s %d %d\r\n", str1, logD, precompute_num);

			fp = fopen(str1, "r");
			fscanf(fp, "%s", str2);	
			fmpz_set_str(pp->cm_pp.G, str2, 16);
			fclose(fp);

			//#pragma omp parallel for
			for(int i=0; i<= pp->n; i++){	
				FILE *fp1;
				char str3[100] = {0}, str2[3050] = {0};
				sprintf(str3, "precom/%d/G%d.txt", precompute_num, i);
				//printf("%s %d %d\r\n", str3, logD, i);

				fp1 = fopen(str3, "r");
				for(int j = 0; j < (i==0 ? precompute_num : precompute_num/2); j++)
				{
					fmpz_init(pre_table[i][j]);
					fscanf(fp1, "%s", str2);		
					fmpz_set_str(pre_table[i][j], str2, 16);
				}
				fclose(fp1);
			}
			printf("overwrite pp\r\n");
			fmpz_set(pp->cm_pp.g, pre_table[0][0]);
			for(int i=1; i<= pp->n; i++)
				fmpz_set(pp->R[i-1], pre_table[i][0]);	
		
			Write_pp(pp);
		}
		else
		{
			// set precomputation table with generator and pp->R. [0][0] ~ [i][0]
			fmpz_init_set(pre_table[0][0], pp->cm_pp.g);
			for(int i=1; i<= pp->n; i++){
				fmpz_init_set(pre_table[i][0], pp->R[i-1]);		
			
			}
			TimerOff();
			TimerOn();
			// compute precomputation table [1][1] ~ [n][2^(D-1)-1] that R[0]^1,R[0]^q,...,R[0]^{q^(2^precomute_num-1)} mod G
			// #pragma omp parallel for
			for(int i=1; i<= pp->n; i++)
			{	
				for(int j = 1; j < precompute_num/2; j++)
				{
					fmpz_init(pre_table[i][j]);
					fmpz_powm(pre_table[i][j], pre_table[i][j-1], pp->q, pp->cm_pp.G);
				}
			}
			RunTime1 = TimerOff();
			TimerOn();
			// [0][1] ~ [0][D] = q^1 ~ q^D 
			for(j = 1; j < precompute_num; j++)
			{
				fmpz_init(pre_table[0][j]);
				fmpz_powm(pre_table[0][j], pre_table[0][j-1], pp->q, pp->cm_pp.G);
			}
			RunTime2 = TimerOff();
		}


		RunTime1 += RunTime2;
		printf("PRE_COMPUTE_(r_i + g) %12llu [us]\n", RunTime1);
		printf("PRE_COMPUTE_g %12llu [us]\n", RunTime2);
		fp = fopen("record/precompute.txt", "a+");
		fprintf(fp, "%d %d %llu precompute only g\n", pp->cm_pp.security_level, poly.d, RunTime2);			
		fprintf(fp, "%d %d %llu precompute all\n", pp->cm_pp.security_level, poly.d, RunTime1);			
		fclose(fp);
		TimerOn();
	}
	isprecomputed = 1;
	
	return isprecomputed;
}

// compute Commitment = G^f(q,q^2,...)
int commit_precompute(_struct_commit_* cm, const _struct_pp_ pp, const _struct_poly_ poly, const fmpz_t q, int index)
{
	static int isfirst = 1;
	int flag = 1, i = 0;
	fmpz_t fmpz_tmp;

	fmpz_init(fmpz_tmp);
	fmpz_one(cm->C);

	int num_threads = 1;
	fmpz_t* parallel_fmpz = (fmpz_t*)malloc(sizeof(fmpz_t) * num_threads);
	fmpz_t* parallel_tmp = (fmpz_t*)malloc(sizeof(fmpz_t) * num_threads);

	if(isprecomputed){	
		// #pragma omp parallel for
		for(int j=0; j<num_threads; j++)
		{
			fmpz_init_set_ui(parallel_fmpz[j],1); // set one
			fmpz_init(parallel_tmp[j]); // set zero
			
			// [{pre_table[0][0]^Fx[0] (mod G)}*...*{pre_table[0][d]^Fx[d] (mod G)}](mod G)
			// product g_i^Fx_i from i = 1 to d 
			for(int i = (j*poly.d)/num_threads; i < ((1+j)*poly.d)/num_threads; i++)
			{
				fmpz_powm(parallel_tmp[j], pre_table[index+1][i], poly.Fx[i], pp.G);
				fmpz_mul(parallel_fmpz[j], parallel_fmpz[j], parallel_tmp[j]);
				fmpz_mod(parallel_fmpz[j], parallel_fmpz[j], pp.G);
			}
			fmpz_clear(fmpz_tmp);
		}

		// set Commitment (mod G)
		for(int j=0; j<num_threads; j++)
		{
			fmpz_mul(cm->C, cm->C, parallel_fmpz[j]);
			fmpz_mod(cm->C, cm->C, pp.G);
			fmpz_clear(parallel_fmpz[j]);
			fmpz_clear(parallel_tmp[j]);
		}
	}
	else
	{
		if(isfirst != 0)
		{
			printf("Need precomputation\n");
			isfirst = 0;
		}
				
		for(i = poly.d - 1; i >= 0; i--)
		{
			fmpz_powm(cm->C, cm->C, q, pp.G);
			fmpz_powm(fmpz_tmp, pp.g, poly.Fx[i], pp.G);

			fmpz_mul(fmpz_tmp, cm->C, fmpz_tmp);
			fmpz_mod(cm->C, fmpz_tmp, pp.G);
		}

	}
	free(parallel_fmpz);
	free(parallel_tmp);
	return flag;
}

int commit_new(_struct_commit_* cm, const _struct_pp_ pp, const _struct_poly_ poly, const fmpz_t q)
{
	int flag = 1, i = 0;
	fmpz_t fmpz_tmp;

	fmpz_init(fmpz_tmp);
	fmpz_one(cm->C);

	for(i = poly.d - 1; i >= 0; i--)
	{
		fmpz_powm(cm->C, cm->C, q, pp.G);
		fmpz_powm(fmpz_tmp, pp.g, poly.Fx[i], pp.G);

		fmpz_mul(fmpz_tmp, cm->C, fmpz_tmp);
		fmpz_mod(cm->C, fmpz_tmp, pp.G);
	}
	fmpz_clear(fmpz_tmp);

	return flag;
}

int commit_init(_struct_commit_* cm){
	fmpz_init(cm->C);
	return 1;
}
int commit_clear(_struct_commit_* cm){
	fmpz_clear(cm->C);
	return 1;
}

// 
int open_precompute(_struct_open_* open, _struct_commit_* cm, const _struct_pp_* pp, const fmpz_t l, const _struct_poly_* poly, const fmpz_t q, int index)
{
	int numbits = fmpz_bits(q)-1;	
	int flag = 1, i = 0, j = 0, cnt = 0;
	fmpz_t bn_tmp1;
	fmpz_t bn_tmp2;
	fmpz_t bn_tmp3;
	fmpz_t bn_dv;
	fmpz_t bn_rem;
	_struct_commit_ cm_tmp;
	commit_init(&cm_tmp);

	fmpz_init(bn_tmp1);
	fmpz_init(bn_tmp2);
	fmpz_init(bn_tmp3);
	fmpz_init(bn_dv);
	fmpz_init(bn_rem);	

	TimerOn2(before);
	fmpz_zero(open->r);

	// f(q) 연산
	for(i = poly->d-1; i >= 0; i--)
	{
		fmpz_mul_2exp(open->r, open->r, numbits); // r*2^numbits
		fmpz_add(open->r, open->r, poly->Fx[i]);
	}
	RunTime[0] += TimerOff2(before, after);

	TimerOn2(before+1);
	fmpz_zero(bn_tmp1);

	// g_(i, r)(q)계산 
	for(i = poly->d-1; i >= 0; i--)
	{
		for(int j = 0; j < fmpz_bits(poly->Fx[i]); j++)
		{
			if(fmpz_tstbit(poly->Fx[i], j) == 1)
				fmpz_setbit(bn_tmp1, j + i*numbits);
		}
	}
	RunTime[1] += TimerOff2(before+1, after+1);

	TimerOn2(before+2);

	// r = f(q) mod l
	// f(q) / 1 - 몫: bn_dv, 나머지: bn_rem
	fmpz_tdiv_qr(bn_dv, bn_rem, bn_tmp1, l);
	fmpz_set(open->r, bn_rem);

	int num_threads = 1; //global_num_threads;//(int)omp_get_num_threads();
	RunTime[2] += TimerOff2(before+2, after+2);

	if(num_threads == 1)
	{
		TimerOn2(before+3);
		fmpz_powm(open->Q,	pp->g, bn_dv, pp->G); // Q <- G_1^(bn_dv) mod G
		RunTime[3] += TimerOff2(before+3, after+3);	
	}
	else
	{
		TimerOn2(before+3);
		int dv_bits = fmpz_bits(bn_dv);
		int offset = 0;
		fmpz_t* parallel_tmp = (fmpz_t*)malloc(sizeof(fmpz_t) * num_threads);
		//printf("thread : %d %d\n", num_threads, dv_bits);
		
		for(i=0; i<num_threads; i++)
			fmpz_init(parallel_tmp[i]);

		_struct_poly_ f_out;
		f_out.d = poly->d;	
		f_out.Fx = (fmpz_t*)calloc(f_out.d, sizeof(fmpz_t));

		RunTime[3] += TimerOff2(before+3, after+3);	
		while(offset* numbits < dv_bits)
		{
			TimerOn2(before+4);
			// #pragma omp parallel for
			for(int i=0; i < num_threads; i++)
			{	
				fmpz_zero(parallel_tmp[i]);
				for(int j=0; j<numbits && numbits*(i+offset) + j < dv_bits; j++)
				{
					if(fmpz_tstbit(bn_dv, numbits*(i+offset) + j) == 1){
						fmpz_setbit(parallel_tmp[i], j);								
					}		
				}

			}
			RunTime[4] += TimerOff2(before+4, after+4);

			TimerOn2(before+5);
			for(int i = 0; i < num_threads && i + offset < f_out.d; i++)
				fmpz_init_set(f_out.Fx[i + offset], parallel_tmp[i]);	
			offset += num_threads;
			RunTime[5] += TimerOff2(before+5, after+5);
		}

		TimerOn2(before+6);
		commit_precompute(&cm_tmp, *pp, f_out, q, index);
		fmpz_set(open->Q, cm_tmp.C);
		RunTime[6] += TimerOff2(before+6, after+6);

		commit_clear(&cm_tmp);
		for(i=0; i<num_threads; i++)
			fmpz_clear(parallel_tmp[i]);
		free(parallel_tmp);
	}

	// if(poly->d == 1)
	// {
	// 	printf("[0]%12llu\n", RunTime[0]);
	// 	printf("[1]%12llu\n", RunTime[1]);
	// 	printf("[2]%12llu\n", RunTime[2]);
	// 	printf("/////////////\n");
	// 	printf("[3]%12llu\n", RunTime[3]);
	// 	printf("[4]%12llu\n", RunTime[4]);
	// 	printf("[5]%12llu\n", RunTime[5]);
	// 	printf("[6]%12llu\n", RunTime[6]);
	// }

	fmpz_clear(bn_tmp1);
	fmpz_clear(bn_tmp2);
	fmpz_clear(bn_tmp3);
	fmpz_clear(bn_dv);
	fmpz_clear(bn_rem);	

	return flag; 
}

int open_new(_struct_open_* open, _struct_commit_* cm, const _struct_pp_* pp, const fmpz_t l, const _struct_poly_* poly, const fmpz_t q)
{
	int numbits = fmpz_bits(q)-1;	
	int flag = 1, i = 0, j = 0;
	fmpz_t bn_tmp1;
	fmpz_t bn_tmp2;
	fmpz_t bn_tmp3;
	fmpz_t bn_dv;
	fmpz_t bn_rem;

	fmpz_init(bn_tmp1);
	fmpz_init(bn_tmp2);
	fmpz_init(bn_tmp3);
	fmpz_init(bn_dv);
	fmpz_init(bn_rem);	

	flag = 1;
	//TimerOn();
	fmpz_zero(open->r);
	//printf("numbits : %d %d\n", numbits, (numbits-1)*(poly.d));
	for(i = poly->d-1; i >= 0; i--)
	{
		fmpz_mul_2exp(open->r, open->r, numbits);
		fmpz_add(open->r, open->r, poly->Fx[i]);
		fmpz_mod(open->r, open->r, l);
	}
	fmpz_zero(bn_tmp1);
	for(i = poly->d-1; i >= 0; i--)
	{
		for(int j = 0; j < fmpz_bits(poly->Fx[i]); j++)
		{
			if(fmpz_tstbit(poly->Fx[i], j) == 1)
				fmpz_setbit(bn_tmp1, j + i*numbits);
		}
	}

	fmpz_tdiv_qr(bn_dv, bn_rem, bn_tmp1, l);
	fmpz_powm(open->Q,	pp->g, bn_dv, pp->G);

	fmpz_clear(bn_tmp1);
	fmpz_clear(bn_dv);
	fmpz_clear(bn_rem);

	return flag; 
}

int open_init(_struct_open_* open)
{
	fmpz_init(open->r);
	fmpz_init(open->Q);
}

int open_clear(_struct_open_* open)
{
	fmpz_clear(open->r);
	fmpz_clear(open->Q);	
}

int Ver(const _struct_open_ open, const _struct_commit_ cm, const _struct_pp_ pp, const fmpz_t l)
{
	int flag = 1, i = 0;
	BN_CTX* ctx = BN_CTX_new();
	fmpz_t Cprime;
	fmpz_t bn_tmp1;
	fmpz_t bn_tmp2;

	fmpz_init(Cprime);
	fmpz_init(bn_tmp1);
	fmpz_init(bn_tmp2);

	fmpz_powm(bn_tmp1, open.Q, l, pp.G);
	fmpz_powm(bn_tmp2, pp.g, open.r, pp.G);
	fmpz_mul(Cprime, bn_tmp1, bn_tmp2);
	fmpz_mod(Cprime, Cprime, pp.G);

	if(fmpz_cmp(Cprime, cm.C) == 0)
		flag = 1;
	else
		flag = 0;

	return flag;
}
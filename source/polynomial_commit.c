#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"
#include <unistd.h>
#include <omp.h>
extern int global_num_threads;

static qfb_t** pre_table;
static int precompute_num = 0, isprecomputed = 0;

struct timeval before[10]={0}, after[10] = {0};
unsigned long long int RunTime[10] = {0};
// make precomputation table (0,0)...(0,2^10)
//                           ...
//                           (n,0)...(n,2^10)
int start_precomputation(_struct_polynomial_pp_* pp, const _struct_poly_ poly)
{
	int i, j;

	if(isprecomputed==0)
	{
		FILE *fp;
		unsigned long long int RunTime1 = 0;
		static int d;
		TimerOff();
		printf("Start precomputation\n");
		TimerOn();
		d = poly.d;
		pre_table = (qfb_t**)calloc(pp->n + 1, sizeof(qfb_t*));
		for(i = 0; i < pp->n + 1; i++)
			pre_table[i] = (qfb_t*)calloc(d, sizeof(qfb_t));
		// set precomputation table with generator and pp->R. [0][0] ~ [i][0]
		qfb_init(pre_table[0][0]);
		qfb_set(pre_table[0][0],pp->cm_pp.g);

		// R[i]
		for(i=1; i<= pp->n; i++)
		{
			qfb_init(pre_table[i][0]);
			qfb_set(pre_table[i][0],pp->R[i-1]);
		}

		for(i=1; i <= pp->n; i++)
		{
			d /= 2;
			for(j=1; (j < d); j++){
				
				qfb_init(pre_table[i][j]);
				qfb_pow_with_root(pre_table[i][j], pre_table[i][j-1], pp->cm_pp.G, pp->q, pp->cm_pp.L); // pow.c
				qfb_reduce(pre_table[i][j], pre_table[i][j], pp->cm_pp.G);
			}
		}
		for(j=1; j<poly.d; j++)
		{
			qfb_init(pre_table[0][j]);
			qfb_pow_with_root(pre_table[0][j], pre_table[0][j-1], pp->cm_pp.G, pp->q, pp->cm_pp.L); // pow.c
			qfb_reduce(pre_table[0][j], pre_table[0][j], pp->cm_pp.G);
		}
		RunTime1 = TimerOff();
		printf("Commit__PRE_Table %12llu [us]\n", RunTime1);
		fp = fopen("record/precompute.txt", "a+");		
		fprintf(fp, "%d %d %llu precompute\n", pp->cm_pp.security_level, poly.d, RunTime1);			
		fclose(fp);
		TimerOn();
	}

	isprecomputed = 1;

	return isprecomputed;
}

// open 시 compute Commitment = G^f(q,q^2,...), d_i = R_i^g_(i, R)(q) 계산 
int commit_precompute(_struct_commit_* cm, const _struct_pp_ pp, const _struct_poly_ poly, const fmpz_t q, int index)
{
	static int isfirst = 1;
	int flag = 1, i = 0, j=0;
	int n = ceil(log(poly.d));
	
	if(isprecomputed){
		qfb_t qfb_tmp;

		qfb_init(qfb_tmp);
		qfb_principal_form(cm->C, pp.G);
		// [{pre_table[0][0]^Fx[0] (mod G)}*...*{pre_table[0][d]^Fx[d] (mod G)}](mod G)
		// product g_i^Fx_i from i = 1 to d 

		for(i = 0; i < poly.d; i++)
		{
			qfb_pow_with_root(qfb_tmp, pre_table[index+1][i], pp.G, poly.Fx[i], pp.L);
			qfb_reduce(qfb_tmp, qfb_tmp, pp.G);
			qfb_nucomp(cm->C, cm->C, qfb_tmp, pp.G, pp.L);
			qfb_reduce(cm->C, cm->C, pp.G);
		}

		qfb_clear(qfb_tmp);


	}
	else
	{
		if(isfirst != 0)
		{
			printf("Need precomputation\n");
			isfirst = 0;
		}
	}
	return flag;
}

int commit_new(_struct_commit_* cm, const _struct_polynomial_pp_* pp, const _struct_poly_ poly, const fmpz_t q)
{
	int i=0, j=0;
	int flag = 1;
	qfb_t qfb_tmp;

	qfb_init(qfb_tmp);

	qfb_principal_form(cm->C, pp->cm_pp.G); 
	qfb_principal_form(qfb_tmp, pp->cm_pp.G);
	for(i = poly.d - 1; i >= 0; i--)
	{
		qfb_pow_with_root(cm->C,cm->C,pp->cm_pp.G,pp->q,pp->cm_pp.L);
		qfb_reduce(cm->C,cm->C,pp->cm_pp.G);
		qfb_pow_with_root(qfb_tmp, pp->cm_pp.g, pp->cm_pp.G, poly.Fx[i],pp->cm_pp.L);
		qfb_nucomp(cm->C, cm->C, qfb_tmp, pp->cm_pp.G, pp->cm_pp.L);
		qfb_reduce(cm->C, cm->C, pp->cm_pp.G);
	}
	qfb_clear(qfb_tmp);

	return flag;

}

int commit_init(_struct_commit_* cm){
	qfb_init(cm->C);
	return 1;
}
int commit_clear(_struct_commit_* cm){
	qfb_clear(cm->C);
	return 1;
}

// open 시 compute r
int pokRep_open_precom(_struct_open_* open, _struct_commit_* cm, const _struct_pp_* pp, const fmpz_t l, const _struct_poly_* poly, const fmpz_t q, int index)
{
	int numbits = fmpz_bits(q)-1;
	int flag = 1, i = 0, j = 0, cnt = 0;
	fmpz_t bn_tmp1;
	fmpz_t bn_tmp2;
	fmpz_t bn_tmp3;
	fmpz_t bn_dv;
	fmpz_t div_bn_dv;
	fmpz_t mod_bn_dv;
	qfb_t Q_pow;
	fmpz_t bn_rem;
	fmpz_t q_bit;
	_struct_commit_ cm_tmp;
	commit_init(&cm_tmp);

	fmpz_init(bn_tmp1);
	fmpz_init(bn_tmp2);
	fmpz_init(bn_tmp3);
	fmpz_init(bn_dv);
	fmpz_init(div_bn_dv);
	fmpz_init(mod_bn_dv);

	qfb_init(Q_pow);
	fmpz_init(bn_rem);	

	TimerOn2(before);
	fmpz_zero(open->r);

	// Fx[d-1]*{2^numbits*(d-1)}+...+Fx[0], where 2^numbits = q = F(q)
	for(i = poly->d-1; i >= 0; i--)
	{
		fmpz_mul_2exp(open->r, open->r, numbits); // r*2^q -> f[d-1]*2^q
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
	int num_threads = 1;

	RunTime[2] += TimerOff2(before+2, after+2);

	if(num_threads == 1)
	{
		TimerOn2(before+3);
		// Q <- G_1^(bn_dv) mod G

		int n = 0; // q진법의 index
		fmpz_init_set_ui(q_bit, numbits);

		fmpz_tdiv_r_2exp(mod_bn_dv, bn_dv, q_bit);
		qfb_pow_with_root(Q_pow, pre_table[index+1][n], pp->G, mod_bn_dv, pp->L);
		qfb_reduce(Q_pow, Q_pow, pp->G);
		fmpz_tdiv_q_2exp(bn_dv, bn_dv, q_bit);
		qfb_set(open->Q, Q_pow);
		n++;

		for(i = n; i < poly->d ; i++){
			fmpz_tdiv_r_2exp(mod_bn_dv, bn_dv, q_bit);
			qfb_pow_with_root(Q_pow, pre_table[index+1][i], pp->G, mod_bn_dv, pp->L);
			qfb_reduce(Q_pow, Q_pow, pp->G);
			fmpz_tdiv_q_2exp(bn_dv, bn_dv, q_bit);
			qfb_nucomp(open->Q, open->Q, Q_pow, pp->G, pp->L);
			qfb_reduce(open->Q, open->Q, pp->G);
		}

		// qfb_pow_with_root(Q_pow, pre_table[index+1][poly->d], pp->G, bn_dv, pp->L);

		RunTime[3] += TimerOff2(before+3, after+3);	
	}
	else
	{
		TimerOn2(before+3);
		int dv_bits = fmpz_bits(bn_dv);
		int offset = 0;
		fmpz_t* parallel_tmp = (fmpz_t*)malloc(sizeof(fmpz_t) * num_threads);
		
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

		//fmpz_set(open->Q, cm_tmp.C);
		qfb_set(open->Q,cm_tmp.C);
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
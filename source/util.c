#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"

int Read_pp(_struct_polynomial_pp_* pp)
{
	FILE *fp;
	int i = 0, flag = 1;
	unsigned char *str;//[100000] = {0};

	str = (unsigned char *)calloc(1000, sizeof(unsigned char));  
	fp = fopen("./Txt/pp.txt", "r");
	fscanf(fp, "%x", &(pp->cm_pp.security_level));

	fmpz_init(pp->cm_pp.G);
	fmpz_init(pp->cm_pp.g);
	fmpz_init(pp->b);
	fmpz_init(pp->p);
	fmpz_init(pp->q);

	fscanf(fp, "%s", str);
	fmpz_set_str(pp->cm_pp.G, str, 16);

	fscanf(fp, "%s", str);
	fmpz_set_str(pp->cm_pp.g, str, 16);

	fscanf(fp, "%x", &(pp->n));
	if(pp->R == NULL)
		pp->R = (fmpz_t*)malloc(sizeof(fmpz_t) * pp->n);
	
	for(i=0; i<pp->n; i++)
	{
		fmpz_init(pp->R[i]);
		fscanf(fp, "%s", str);
		fmpz_set_str(pp->R[i], str, 16);
	}
	fscanf(fp, "%s", str);
	fmpz_set_str(pp->b, str, 16);

	fscanf(fp, "%s", str);
	fmpz_set_str(pp->p, str, 16);

	fscanf(fp, "%x", &i);
	fmpz_setbit(pp->q, i);

	fscanf(fp, "%x", &(pp->d));

	fclose(fp);
	free(str);
	return flag;
}

int Write_pp(const _struct_polynomial_pp_* pp)
{
	int pp_size = 0;
	pp_size += fmpz_bits(pp->cm_pp.G);
	// printf("|N| %d\n", fmpz_bits(pp->cm_pp.G));
	pp_size += fmpz_bits(pp->cm_pp.g);
	// printf("|G| %d\n", fmpz_bits(pp->cm_pp.g));
	pp_size += fmpz_bits(pp->p);
	// printf("|p| %d\n", fmpz_bits(pp->p));
	for(int i=0;i<pp->n;i++) {
		pp_size += fmpz_bits(pp->R[i]);
		// printf("|R[%d]| %d\n", i, fmpz_bits(pp->R[i]));
	}
	printf("parameter size %d\n", pp_size);

	FILE *fp;
	fp = fopen("./Txt/pp.txt","w");
	fprintf(fp,"%x\n", (pp->cm_pp).security_level);
	fprintf(fp,"%s\n", fmpz_get_str(NULL, 16, pp->cm_pp.G));
	fprintf(fp,"%s\n", fmpz_get_str(NULL, 16, pp->cm_pp.g));
    fprintf(fp,"%x\n", pp->n);
    for(int i =0; i<pp->n; i++)
        fprintf(fp,"%s\n", fmpz_get_str(NULL, 16, pp->R[i]));
    fprintf(fp,"%s\n", fmpz_get_str(NULL, 16, pp->b));
    fprintf(fp,"%s\n", fmpz_get_str(NULL, 16, pp->p));
    fprintf(fp,"%x\n", (unsigned int)fmpz_bits(pp->q)-1);
	fprintf(fp,"%x\n", pp->d);
    fclose(fp);
}

//
int Read_poly(_struct_poly_* poly)
{
	FILE *fp;
	int i = 0, flag = 1;
	unsigned char *str;
	str = (unsigned char *)calloc(1000, sizeof(unsigned char));    

	fp = fopen("./Txt/poly.txt", "r");
	
	fscanf(fp, "%s", str);
	poly->d = atoi(str);

	poly->Fx = (fmpz_t*)calloc(poly->d, sizeof(fmpz_t));
	for(i=0; i<poly->d; i++)
	{			
		fmpz_init(poly->Fx[poly->d-i-1]);			     
		fscanf(fp, "%s", str);
		fmpz_set_str(poly->Fx[poly->d-i-1], str, 16);
	}

	fscanf(fp, "%s", str);
	fmpz_set_str(poly->z, str, 16);

	fscanf(fp, "%s", str);
	fmpz_set_str(poly->fz, str, 16);

	fclose(fp);
	free(str);
	return flag;
}

//"./Txt/commit.txt"
int Read_Commit(const char* path, _struct_commit_* cm)
{
	FILE *fp;
	int cnt = 0, flag = 1;
	char *buff;

	fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END); 
    cnt = ftell(fp);      
	fseek(fp, 0, SEEK_SET ); 

	//fmpz_init(cm->C);
	buff = (char *)calloc(cnt + 1, sizeof(char));    

	fscanf(fp, "%s", buff);
	fmpz_set_str(cm->C, buff, 16);

	fclose(fp);
	free(buff);
	return (flag > 0 ? 1 : 0);
}

//"./Txt/commit.txt"
int Write_Commit(const char* path, const _struct_commit_* cm)
{
	int commit_size = 0;
	commit_size += fmpz_bits(cm->C);

	printf("commit size %d\n", commit_size);

	FILE *fp;
	int i = 0, flag = 1;

	fp = fopen(path, "w");
	flag *= fprintf(fp,"%s\n", 
		fmpz_get_str(NULL, 16, (cm->C)));

	fclose(fp);
	return (flag != 0 ? 1 : 0);
}

// pf 에 y랑 D추가됨 
int Write_proof(const _struct_proof_ *proof )
{
	int proof_size = 0;
	proof_size += fmpz_bits(proof->Q);
	for(int i=0; i<proof->n; i++){
		proof_size += fmpz_bits(proof->D[i]);
		proof_size += fmpz_bits(proof->s[i]);
		proof_size += fmpz_bits(proof->y[i]);
	}
	proof_size += fmpz_bits(proof->gx);
	proof_size += fmpz_bits(proof->r);

	printf("proof size %d\n", proof_size);


	FILE *fp;
	int i = 0, flag = 1;
	unsigned char *str;

	fp = fopen("./Txt/proof.txt","w");
	fprintf(fp,"%s\n", fmpz_get_str(NULL, 16,proof->Q));
    fprintf(fp,"%x\n", proof->n);
	for(int i =0; i<proof->n; i++)
        fprintf(fp,"%s\n", fmpz_get_str(NULL, 16,proof->D[i]));
    for(int i =0; i<proof->n; i++)
        fprintf(fp,"%s\n", fmpz_get_str(NULL, 16,proof->s[i]));
	for(int i =0; i<proof->n; i++)
        fprintf(fp,"%s\n", fmpz_get_str(NULL, 16,proof->y[i]));
    fprintf(fp,"%s\n", fmpz_get_str(NULL, 16,proof->gx));
    fprintf(fp,"%s\n", fmpz_get_str(NULL, 16,proof->r));

	fclose(fp);
	return (flag > 0 ? 1 : 0);	
}


// y, D 추가해서 읽어야함
int Read_proof(_struct_proof_ *proof)
{
	FILE *fp = NULL;
	unsigned char *buffer;// [10000]={0};
	int i = 0, flag = 1, cnt;

	buffer = (unsigned char *)calloc(1000, sizeof(unsigned char));    

	fmpz_init(proof->Q);
	// fmpz_init(proof->D);
	fmpz_init(proof->r);
	fmpz_init(proof->gx);

    fp = fopen("./Txt/proof.txt", "r");
    fscanf(fp, "%s", buffer);
	fmpz_set_str(proof->Q, buffer, 16);

    fscanf(fp, "%x", &proof->n);

	proof->D = (fmpz_t*)malloc(sizeof(fmpz_t)* proof->n);
    for(int i = 0; i < proof->n; i++)
    {
        fmpz_init(proof->D[i]);
        fscanf(fp, "%s", buffer);
		fmpz_set_str(proof->D[i], buffer, 16);
    }

	proof->s = (fmpz_t*)malloc(sizeof(fmpz_t)* proof->n);
    for(int i = 0; i < proof->n; i++)
    {
        fmpz_init(proof->s[i]);
        fscanf(fp, "%s", buffer);
		fmpz_set_str(proof->s[i], buffer, 16);
    }

	proof->y = (fmpz_t*)malloc(sizeof(fmpz_t)* proof->n);
    for(int i = 0; i < proof->n; i++)
    {
        fmpz_init(proof->y[i]);
        fscanf(fp, "%s", buffer);
		fmpz_set_str(proof->y[i], buffer, 16);
    }

    fscanf(fp, "%s", buffer);
	fmpz_set_str(proof->gx, buffer, 16);

    fscanf(fp, "%s", buffer);
	fmpz_set_str(proof->r, buffer, 16);


	fclose(fp);
	free(buffer);
	fp = NULL;
	return (flag > 0 ? 1 : 0);	
}

// make Fx[d-1..0]
int make_poly(_struct_poly_* poly, int d)
{
	// poly random
	flint_rand_t state;
	int i = 0, flag = 1;

	flint_randinit(state);

	// d = 2^10
	poly->d = d;
	poly->Fx = (fmpz_t*)calloc(d, sizeof(fmpz_t));

	// Fx[d-1..0] 32bit init
	for(int i = 0; i < d; i++)
	{
		fmpz_init(poly->Fx[d-i-1]);
		fmpz_randbits(poly->Fx[d-i-1], state, 32);
		fmpz_abs(poly->Fx[d-i-1], poly->Fx[d-i-1]);
	}

	return flag;
}

int write_poly(const _struct_poly_* poly){
	FILE *fp;
	int i = 0, flag = 1;
	unsigned char* str;

	fp = fopen("./Txt/poly.txt", "w");

	flag &= fprintf(fp, "%d\n", poly->d);
	for(int i = 0; i < poly->d; i++)
	{
		//fmpz_set_ui(bn_tmp, 1+i); // 1+i // random 
		str = fmpz_get_str(NULL, 16, poly->Fx[poly->d-i-1]);
		flag &= fprintf(fp, "%s ", str);
	}

	flag &= fprintf(fp, "\n");

	str = fmpz_get_str(NULL, 16, poly->z);
	flag &= fprintf(fp, "%s\n", str);
	
	str = fmpz_get_str(NULL, 16, poly->fz);
	flag &= fprintf(fp, "%s\n", str);

	fclose(fp);

	return flag;
}


void generate_random_table(const char* path, int n, int lambda)
{    
	FILE *fp;
	fp = fopen(path, "w");
	flint_rand_t state;
	fmpz_t bn_tmp;
    BIGNUM* p = BN_new();
	BN_generate_prime_ex(p, lambda, 0,NULL,NULL,NULL);
	

	flint_randinit(state);

	fmpz_init(bn_tmp);
    //fmpz_randprime(bn_tmp, state, lambda, 1);
	fprintf(fp, "%s\r\n", BN_bn2hex(p));
    for(int i = 0 ; i<n ; i++){
        fmpz_randbits(bn_tmp, state, lambda);
		fmpz_abs(bn_tmp,bn_tmp);
		fprintf(fp, "%s ", fmpz_get_str(NULL, 16, bn_tmp));

        fmpz_randbits(bn_tmp, state, lambda);
		fmpz_abs(bn_tmp,bn_tmp);
		fprintf(fp, "%s\r\n", fmpz_get_str(NULL, 16, bn_tmp));
    }
	fclose(fp);
	fmpz_clear(bn_tmp);
	BN_free(p);

}

int read_random_table(const char* path, int n, fmpz_t l, fmpz_t* bL, fmpz_t*bR)
{
	FILE *fp;
	fp = fopen(path, "r");
	unsigned char buffer[1000]={0};
	flint_rand_t state;

	flint_randinit(state);

    fscanf(fp, "%s", buffer);
	fmpz_set_str(l, buffer, 16);
	if(bL!= NULL && bR != NULL)
	{
		for(int i = 0 ; i<n ; i++){
			fscanf(fp, "%s", buffer);
			fmpz_set_str(bL[i], buffer, 16);
			fscanf(fp, "%s", buffer);
			fmpz_set_str(bR[i], buffer, 16);
		}
	}
}

int getfilesize(char* path)
{
    int size;
    FILE *fp = fopen(path, "r");    
    fseek(fp, 0, SEEK_END);    
    size = ftell(fp);          
    fclose(fp);

    fp = fopen("./Size.Txt", "a+");   
	fprintf(fp, "%s ", path);
	fprintf(fp, "%d\n", size);
    fclose(fp);

    return size;
}
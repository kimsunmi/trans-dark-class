#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
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

// make alpha
int get_alpha_SHA256(fmpz_t alphaI, fmpz_t input, int idx)
{
    unsigned char digest[SHA256_DIGEST_LENGTH]={0};
	unsigned char mdString[SHA256_DIGEST_LENGTH/2+1]={0};
    char* str_input;
    fmpz_t tmpz_tmp;

    fmpz_init_set(tmpz_tmp, input);

    fmpz_add_ui(tmpz_tmp, tmpz_tmp, 2*idx);
    str_input = fmpz_get_str(NULL, 16, tmpz_tmp);

	SHA256(str_input, strlen(str_input), digest);  
    for(int i = 0; i < SHA256_DIGEST_LENGTH/4; i++){
        digest[i] = (digest[i] ^ digest[16+i] ^ digest[8+i] ^ digest[24+i]);       
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    }
    fmpz_set_str(alphaI, mdString, 16);
    free(str_input);

    fmpz_clear(tmpz_tmp);
    return 1;
}

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
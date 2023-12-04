#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"

int Read_pp(_struct_polynomial_pp_* pp)
{
	FILE *fp;
	int i = 0, flag = 1;
	unsigned char *str;//[100000] = {0};

	fmpz_init(pp->cm_pp.G);
	qfb_init(pp->cm_pp.g);
	fmpz_init(pp->b);
	fmpz_init(pp->p);
	fmpz_init(pp->q);

	str = (unsigned char *)calloc(1000, sizeof(unsigned char));

	fp = fopen("./Txt/pp.txt", "r");
	fscanf(fp, "%x", &(pp->cm_pp.security_level));

	fscanf(fp, "%s", str);
	fmpz_set_str(pp->cm_pp.G, str, 16);

	fmpz_abs(pp->cm_pp.L, pp->cm_pp.G);
	fmpz_root(pp->cm_pp.L, pp->cm_pp.L, 4);

	fscanf(fp, "%s", str);
	fmpz_set_str(pp->cm_pp.g->a, str, 16);
	fscanf(fp, "%s", str);
	fmpz_set_str(pp->cm_pp.g->b, str, 16);
	fscanf(fp, "%s", str);
	fmpz_set_str(pp->cm_pp.g->c, str, 16);

	fscanf(fp, "%x", &(pp->n));

	if(pp->R == NULL)
	    pp->R = (qfb_t*)malloc(sizeof(qfb_t) * pp->n);
	for(i=0; i<pp->n; i++)
	{
		qfb_init(pp->R[i]);
		fscanf(fp, "%s", str);
		fmpz_set_str(pp->R[i]->a, str, 16);
		fscanf(fp, "%s", str);
		fmpz_set_str(pp->R[i]->b, str, 16);
		fscanf(fp, "%s", str);
		fmpz_set_str(pp->R[i]->c, str, 16);
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
	pp_size += fmpz_bits(pp->cm_pp.g->a);
	pp_size += fmpz_bits(pp->cm_pp.g->b);
	pp_size += fmpz_bits(pp->cm_pp.g->c);
	// printf("|G| %d\n", fmpz_bits(pp->cm_pp.g));
	pp_size += fmpz_bits(pp->p);
	// printf("|p| %d\n", fmpz_bits(pp->p));
	for(int i=0;i<pp->n;i++) {
		pp_size += fmpz_bits(pp->R[i]->a);
		pp_size += fmpz_bits(pp->R[i]->b);
		pp_size += fmpz_bits(pp->R[i]->c);
		// printf("|R[%d]| %d\n", i, fmpz_bits(pp->R[i]));
	}
	printf("parameter size %d\n", pp_size);

	FILE *fp;
	fp = fopen("./Txt/pp.txt","w");
	fprintf(fp,"%x\n", (pp->cm_pp).security_level);
	fprintf(fp,"%s\n", fmpz_get_str(NULL, 16, pp->cm_pp.G));
	fprintf(fp,"%s %s %s\n", fmpz_get_str(NULL, 16, pp->cm_pp.g->a), fmpz_get_str(NULL, 16, pp->cm_pp.g->b), fmpz_get_str(NULL, 16, pp->cm_pp.g->c));
    fprintf(fp,"%x\n", pp->n);
    for(int i =0; i<pp->n; i++)
        fprintf(fp,"%s %s %s\n", fmpz_get_str(NULL, 16, pp->R[i]->a), fmpz_get_str(NULL, 16, pp->R[i]->b), fmpz_get_str(NULL, 16, pp->R[i]->c));
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

	qfb_init(cm->C);
	buff = (char *)calloc(cnt + 1, sizeof(char));

	fscanf(fp, "%s", buff);
	fmpz_set_str(cm->C->a, buff, 16);
	fscanf(fp, "%s", buff);
	fmpz_set_str(cm->C->b, buff, 16);
	fscanf(fp, "%s", buff);
	fmpz_set_str(cm->C->c, buff, 16);

	fclose(fp);
	free(buff);
	return (flag > 0 ? 1 : 0);
}

//"./Txt/commit.txt"
int Write_Commit(const char* path, const _struct_commit_* cm)
{
	int commit_size = 0;
	commit_size += fmpz_bits(cm->C->a);
	commit_size += fmpz_bits(cm->C->b);
	commit_size += fmpz_bits(cm->C->c);

	// printf("commit size %d\n", commit_size);

	FILE *fp;
	int i = 0, flag = 1;

	fp = fopen(path, "w");
	flag *= fprintf(fp,"%s %s %s\n", 
		fmpz_get_str(NULL, 16, (cm->C)->a),
		fmpz_get_str(NULL, 16, (cm->C)->b),
		fmpz_get_str(NULL, 16, (cm->C)->c));

	fclose(fp);
	return (flag != 0 ? 1 : 0);
}

int Write_proof(const _struct_proof_ *proof )
{
	int proof_size = 0;
	proof_size += fmpz_bits(proof->Q->a);
	proof_size += fmpz_bits(proof->Q->b);
	proof_size += fmpz_bits(proof->Q->c);
	for(int i=0; i<proof->n; i++){
		proof_size += fmpz_bits(proof->D[i]->a);
		proof_size += fmpz_bits(proof->D[i]->b);
		proof_size += fmpz_bits(proof->D[i]->c);
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
	fprintf(fp,"%s %s %s\n", fmpz_get_str(NULL, 16, proof->Q->a),fmpz_get_str(NULL, 16, proof->Q->b),fmpz_get_str(NULL, 16, proof->Q->c));
    fprintf(fp,"%x\n", proof->n);
	for(int i =0; i<proof->n; i++)
        fprintf(fp,"%s %s %s\n", fmpz_get_str(NULL, 16,proof->D[i]->a),fmpz_get_str(NULL, 16,proof->D[i]->b),fmpz_get_str(NULL, 16,proof->D[i]->c));
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

	qfb_init(proof->Q);
	fmpz_init(proof->r);
	fmpz_init(proof->gx);

    fp = fopen("./Txt/proof.txt", "r");
    fscanf(fp, "%s", buffer);
	fmpz_set_str(proof->Q->a, buffer, 16);
    fscanf(fp, "%s", buffer);
	fmpz_set_str(proof->Q->b, buffer, 16);
    fscanf(fp, "%s", buffer);
	fmpz_set_str(proof->Q->c, buffer, 16);

    fscanf(fp, "%x", &proof->n);

	proof->D = (qfb_t*)malloc(sizeof(qfb_t)* proof->n);
    for(int i = 0; i < proof->n; i++)
    {
        qfb_init(proof->D[i]);
        fscanf(fp, "%s", buffer);
		fmpz_set_str(proof->D[i]->a, buffer, 16);
		fscanf(fp, "%s", buffer);
		fmpz_set_str(proof->D[i]->b, buffer, 16);
		fscanf(fp, "%s", buffer);
		fmpz_set_str(proof->D[i]->c, buffer, 16);
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
// l_prime, proof->D, proof->n, cm->C
int Hprime_func(fmpz_t output, const qfb_t in1, const int n, const qfb_t in2)
{
    unsigned char digest[SHA256_DIGEST_LENGTH]={0};
	unsigned char mdString[SHA256_DIGEST_LENGTH*2+1]={0};
	
    // D벡터 전부 str 가져와서 붙이기

    char *str_in1_a = fmpz_get_str(NULL, 16, in1->a);
    char *str_in1_b = fmpz_get_str(NULL, 16, in1->b);
    char *str_in1_c = fmpz_get_str(NULL, 16, in1->c);
    char *str_in2_a = fmpz_get_str(NULL, 16, in2->a);
    char *str_in2_b = fmpz_get_str(NULL, 16, in2->b);
    char *str_in2_c = fmpz_get_str(NULL, 16, in2->c);
	char *str_concat = calloc(strlen(str_in1_a) + strlen(str_in1_b) + strlen(str_in1_c) 
								+ strlen(str_in2_a) + strlen(str_in2_b) + strlen(str_in2_c) + 1, sizeof(char));
	//char *output_string;
	int concat_len = 0;

	memcpy(str_concat + concat_len, str_in1_a, sizeof(char) * (strlen(str_in1_a)));	concat_len += strlen(str_in1_a);
	memcpy(str_concat + concat_len, str_in1_b, sizeof(char) * (strlen(str_in1_b)));	concat_len += strlen(str_in1_b);
	memcpy(str_concat + concat_len, str_in1_c, sizeof(char) * (strlen(str_in1_c)));	concat_len += strlen(str_in1_c);
	memcpy(str_concat + concat_len, str_in2_a, sizeof(char) * (strlen(str_in2_a)));	concat_len += strlen(str_in2_a);
	memcpy(str_concat + concat_len, str_in2_b, sizeof(char) * (strlen(str_in2_b)));	concat_len += strlen(str_in2_b);
	memcpy(str_concat + concat_len, str_in2_c, sizeof(char) * (strlen(str_in2_c)));	concat_len += strlen(str_in2_c);

	SHA256(str_concat, strlen(str_concat), digest);   
	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

	mpz_t u, w;
	mpz_init_set_str(u,(char*)mdString,16);
	mpz_init(w);
	mpz_nextprime(w,u);	
	fmpz_set_mpz(output, w);

    //printf(">>"); fmpz_print(output); printf("\n");

	mpz_clear(u);
	mpz_clear(w);
	free(str_in1_a);
	free(str_in1_b);
	free(str_in1_c);
	free(str_in2_a);
	free(str_in2_b);
	free(str_in2_c);
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

int KeyGen_Class_setup( _struct_pp_ *pp, const int lamda )
{
	fmpz_t fmpz_p;
	fmpz_init(fmpz_p);

	BIGNUM* tmp = BN_new();
	BIGNUM* bn_4 = BN_new();
	BIGNUM* bn_3 = BN_new();

	BN_set_word(bn_4, 4);
	BN_set_word(bn_3, 3);

	// prime 생성해서 pp->G에 set. G=-G로 변경
	do{
	 	BN_generate_prime_ex(tmp, lamda, 1, bn_4, bn_3, NULL);
		fmpz_set_str(pp->G, BN_bn2hex(tmp), 16);
		fmpz_neg(pp->G, pp->G);
	}while(BN_num_bits(tmp) != lamda);

	//pp->g에 0초과 fmpz_p이하 G로 discriminant하여 set, fmpz_p: lamda/4 prime
    do{        
        BN_generate_prime_ex(tmp, lamda/4, 0, bn_4, bn_3, NULL);
		fmpz_set_str(fmpz_p, BN_bn2hex(tmp), 16);
		qfb_prime_form(pp->g, pp->G, fmpz_p);
    }while (!qfb_is_primitive(pp->g) || !qfb_is_reduced(pp->g) || fmpz_cmp((pp->g)->a, (pp->g)->b) <= 0 ); 

	fmpz_abs(pp->L, pp->G);
	fmpz_root(pp->L, pp->L, 4);
	
	BN_free(bn_4);
	BN_free(bn_3);
	BN_free(tmp);

	fmpz_clear(fmpz_p);

	return 1;
}

int pp_init(_struct_pp_* pp)
{
	fmpz_init(pp->G);
	qfb_init(pp->g);

	return 1;
}

int pp_clear(_struct_pp_* pp)
{
	fmpz_clear(pp->G);
	qfb_clear(pp->g);

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
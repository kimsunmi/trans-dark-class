#ifndef _REDUCIBLE_COMMITMENT   
	#include <stdio.h>
	#include <string.h>
	#include <sys/time.h>

	#include <openssl/bn.h>
	#include <openssl/sha.h>
	#include <gmp.h>
	#include <math.h>

	#include <flint/fmpz.h>

	typedef struct{
		int security_level;
		fmpz_t G; // rsa modulus?
		fmpz_t g; // generator?
	}_struct_pp_;

	typedef struct{
		fmpz_t C;
	}_struct_commit_;

	typedef struct{
		fmpz_t r;
		fmpz_t Q;
	}_struct_open_;

	typedef struct{
		fmpz_t* Fx;
		fmpz_t z;
		fmpz_t fz;
		int d;
	}_struct_poly_;

	typedef struct{
		_struct_pp_ cm_pp;
		fmpz_t* R;
		int n;
		fmpz_t b;
		fmpz_t p;
		fmpz_t q;
		int d;
	}_struct_polynomial_pp_;

	typedef struct{
		fmpz_t Q; // pf size
		// D 계산 값이 벡터로 나와야 함 
		// D length == n 
		fmpz_t* D; // pf size
		int n; 
		fmpz_t* s; // pf size
		fmpz_t* y; // pf size
		fmpz_t gx; // pf size
		fmpz_t r; // pf size
	}_struct_proof_;
	

    int KeyGen_RSAsetup( _struct_pp_ *pp, const int lamda );
    int pp_init(_struct_pp_* pp);
    int pp_clear(_struct_pp_* pp);

    int start_precomputation(_struct_polynomial_pp_* pp, const _struct_poly_ poly);
    int commit_new(_struct_commit_* cm, const _struct_pp_ pp, const _struct_poly_ poly, const fmpz_t q);        
    int commit_precompute(_struct_commit_* cm, const _struct_pp_ pp, const _struct_poly_ poly, const fmpz_t q, int index);    
    int commit_init(_struct_commit_* cm);
    int commit_clear(_struct_commit_* cm);

    int open_new(_struct_open_* open, _struct_commit_* cm, const _struct_pp_* pp, const fmpz_t l, const _struct_poly_* poly, const fmpz_t q);    
    int open_precompute(_struct_open_* open, _struct_commit_* cm, const _struct_pp_* pp, const fmpz_t l, const _struct_poly_* poly, const fmpz_t q, int index);
    int open_clear(_struct_open_* open);

    //int Ver(const _struct_open_ open, const _struct_commit_ cm, const _struct_pp_ pp, const BIGNUM* l);

	#define _REDUCIBLE_COMMITMENT
#endif

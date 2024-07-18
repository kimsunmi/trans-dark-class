#ifndef _POLYNOMIAL_COMMIT
	#include <stdio.h>
	#include <string.h>
	#include <sys/time.h>

	#include <gmp.h>
	#include <openssl/bn.h>
	#include <openssl/sha.h>
	#include <math.h>

	#include <flint/fmpz.h>
	#include <antic/qfb.h>

// open, polynomial_pp class group 시 x?

	typedef struct{
		int security_level;
		fmpz_t G; // Group (rsa modulus)
		qfb_t g; // generator
		fmpz_t L; // discriminant
	}_struct_pp_;

	typedef struct{
		qfb_t C; // commitment
	}_struct_commit_;
	
	typedef struct{
		fmpz_t* Fx; // polynomial coefficient
		fmpz_t* z; // random Z
		fmpz_t* bz; // binary z (e.g. 1,z1,z2,z2z1,...)
		fmpz_t fz; // F[z]
		int d; // degree
	}_struct_poly_;

	// qfb_t 형변환 필요 Q
	typedef struct{
		qfb_t Q; // pf size
		// D length == n
		qfb_t* D; // pf size
		int n; // mu
		fmpz_t* s; // pf size
		fmpz_t* y; // pf size
		fmpz_t gx; // pf size
		fmpz_t r; // pf size
	}_struct_proof_;

	typedef struct{
		fmpz_t r; // pokRep_open r
		qfb_t Q; // pokRep_Open Q
	}_struct_open_;

	typedef struct{
		_struct_pp_ cm_pp;
		qfb_t* R; // R_vector
		int n; // mu
		fmpz_t b; // bound
		fmpz_t p; // prime (generating G)
		fmpz_t q; // prime (generating G)
		int d; // polynomial degree
	}_struct_polynomial_pp_;

    int start_precomputation(_struct_polynomial_pp_* pp, const _struct_poly_ poly);
    int commit_new(_struct_commit_* cm, const _struct_polynomial_pp_* pp, const _struct_poly_ poly, const fmpz_t q);        
    int commit_precompute(_struct_commit_* cm, const _struct_pp_ pp, const _struct_poly_ poly, const fmpz_t q, int index);    
    int commit_init(_struct_commit_* cm);
    int commit_clear(_struct_commit_* cm);

    int pokRep_open_precom(_struct_open_* open, _struct_commit_* cm, const _struct_pp_* pp, const fmpz_t l, const _struct_poly_* poly, const fmpz_t q, int index);

	#define _POLYNOMIAL_COMMIT
#endif

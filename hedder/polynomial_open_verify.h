#ifndef _POLYNOMIAL_OPEN_VERIFY
    #include "../hedder/polynomial_commit.h"

    int Open(_struct_proof_ *proof, _struct_polynomial_pp_* pp, _struct_commit_* cm, _struct_poly_* poly);
    int multipoly_commit(qfb_t* D, const _struct_polynomial_pp_* pp, const _struct_poly_ poly, const fmpz_t q, const int i);
    int pokRep_setup(_struct_polynomial_pp_* pp, const int lamda, const int m, const int logD, _struct_poly_* poly);
    int pokRep_open(fmpz_t r, fmpz_t s[], qfb_t Q, const fmpz_t l, const _struct_polynomial_pp_* pp,
                    const fmpz_t q, _struct_poly_* g[], const _struct_poly_* f);
    int Verify(_struct_polynomial_pp_* pp, _struct_commit_* cm, fmpz_t* z, fmpz_t fz,_struct_proof_ *proof);
    int PoKRep_Ver(fmpz_t r, qfb_t Q, qfb_t CD, fmpz_t* s, qfb_t* R, const fmpz_t l, const _struct_polynomial_pp_* pp);

	#define _POLYNOMIAL_OPEN_VERIFY
#endif



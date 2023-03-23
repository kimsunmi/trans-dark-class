#ifndef _REDUCIBLE_POLYNOMIAL_COMMITMENT   
    #include "../hedder/Reducible_commitment.h"

    int pokRep_setup(_struct_polynomial_pp_* pp, const int lamda, const int m, const int logD, _struct_poly_* poly);
    int Open(_struct_proof_ *proof, _struct_polynomial_pp_* pp, _struct_commit_* cm, _struct_poly_* poly);
    int Verify(_struct_polynomial_pp_* pp, _struct_commit_* cm, fmpz_t z, fmpz_t fz,_struct_proof_ *proof);

	#define _REDUCIBLE_POLYNOMIAL_COMMITMENT
#endif



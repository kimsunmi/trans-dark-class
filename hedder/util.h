#ifndef _UTIL_H
    #include "../hedder/polynomial_commit.h"
    #include "../hedder/polynomial_open_verify.h"
    
    int Read_pp(_struct_polynomial_pp_* pp);
    int Write_pp(const _struct_polynomial_pp_* pp);
    int pp_init(_struct_pp_* pp);
    int pp_clear(_struct_pp_* pp);

	int make_poly(_struct_poly_* poly, int d);
    int Read_poly(_struct_poly_* poly);
    int write_poly(const _struct_poly_* poly);

    int Read_Commit(const char* path, _struct_commit_* cm);    
    int Write_Commit(const char* path, const _struct_commit_* cm);

    int Write_proof(const _struct_proof_* proof );
	int Read_proof(_struct_proof_ *proof);

    int Hprime_func(fmpz_t output, const qfb_t in1, const int n, const qfb_t in2);
    int get_alpha_SHA256(fmpz_t alphaI, fmpz_t input, int idx, fmpz_t p);

    int KeyGen_Class_setup( _struct_pp_ *pp, const int lamda );
    
    int getfilesize(char* path);
    
	void TimerOn();
	unsigned long long int TimerOff();

	void TimerOn2(struct timeval* before);
	unsigned long long int TimerOff2(struct timeval* before, struct timeval* after);

    #define _UTIL_H
#endif


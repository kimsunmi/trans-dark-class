#ifndef _UTIL_H
    #include "../hedder/Reducible_commitment.h"

    int Read_pp(_struct_polynomial_pp_* pp);
    int Write_pp(const _struct_polynomial_pp_* pp);

	int make_poly(_struct_poly_* poly, int d);
    int Read_poly(_struct_poly_* poly);
    int write_poly(const _struct_poly_* poly);

    int Read_Commit(const char* path, _struct_commit_* cm);    
    int Write_Commit(const char* path, const _struct_commit_* cm);

    int Write_proof(const _struct_proof_* proof );
	int Read_proof(_struct_proof_ *proof);

    void generate_random_table(const char* path, int n, int lambda);
    int read_random_table(const char* path, int n, fmpz_t l, fmpz_t* bL, fmpz_t*bR);
    int getfilesize(char* path);
    
	void TimerOn();
	unsigned long long int TimerOff();

	void TimerOn2(struct timeval* before);
	unsigned long long int TimerOff2(struct timeval* before, struct timeval* after);

    #define _UTIL_H
#endif


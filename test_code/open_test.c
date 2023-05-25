#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"
int global_num_threads;
int proof_size = 0;

// Read_pp / Read_Commit / Read_poly : read pp, cm, poly from "./Txt/pp.txt ./Txt/cm.txt ./Txt/poly.txt" that computed in commit_test.c setup_test.c
// start_precomputation: make table base of g and R. g,g^2,...,g^pp->n, R[0],R[0]^q,...,R[0]^{q^(2^d-1)},...,R[1],...,R[n],...
// Open: make proof with pp, cm, poly
int main(int argc, char *argv[])
{
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    unsigned long long OPEN_RUNTIME;
    global_num_threads = 1;

    _struct_polynomial_pp_ pp = {0};
    _struct_poly_ poly = {0};
    _struct_open_ open = {0};
    _struct_commit_ cm = {0};
    _struct_proof_ proof = {0};

    fmpz_init(cm.C);
    fmpz_init(open.r);
    fmpz_init(open.Q);
   
	TimerOn();
    Read_pp(&pp);
    Read_Commit("./Txt/commit.txt", &cm);
    Read_poly(&poly);
    RunTime_IO = TimerOff();

    // precomputation table base of g and R. (g,g^2,... R[0],R[0]^q,...)
    start_precomputation(&pp, poly);

    TimerOn();

    // run proof algorithm
    OPEN_RUNTIME = Open(&proof, &pp, &cm, &poly);

	printf("__Poly_Open %12llu [us]\n", OPEN_RUNTIME);

    TimerOn();
    // write proof(Q,D,(r,s),gx,y) in file "./Txt/proof.txt"
    Write_proof(&proof);

    printf("open의 proof: \n");
    printf("Q: ");
    qfb_print(proof.Q);
    printf("\n");
    printf("D: ");
    qfb_print(proof.D);
    printf("\n");
    printf("n: %d",proof.n);
    printf("\n");
    printf("c: ");
    fmpz_print(cm.C);
    printf("\n");

    RunTime_IO += TimerOff();
	printf("__Open_I/O_ %12llu [us]\n", RunTime_IO);

	FILE *fp;
	fp = fopen("record/open.txt", "a+");
	fprintf(fp, "%d %d %llu %llu\n", pp.cm_pp.security_level, poly.d, RunTime_IO, RunTime);			
	fclose(fp);

    for(int i=0; i<poly.d; i++)
		fmpz_clear(poly.Fx[poly.d-i-1]);
    for(int i =0; i < pp.n; i++) {
        fmpz_clear(proof.s[i]);
        fmpz_clear(proof.y[i]);
        qfb_clear(proof.D[i]);
        qfb_clear(pp.R[i]);
    }
    free(poly.Fx);
    free(proof.s);
    free(proof.y);
    free(proof.D);
    free(pp.R);
	return 0;
}

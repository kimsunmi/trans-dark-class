#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"
int global_num_threads;
int proof_size = 0;

int main(int argc, char *argv[])
{
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    global_num_threads = 1;
    // if(argc == 1)
    // {     
    //     global_num_threads = atoi(argv[1]) < omp_get_max_threads() ? atoi(argv[1]) : omp_get_max_threads();
    // } 
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

    // read precomputation table
    start_precomputation(&pp, poly);

    TimerOn();
    
    unsigned long long OPEN_RUNTIME = Open(&proof, &pp, &cm, &poly);

	printf("__Poly_Open %12llu [us]\n", OPEN_RUNTIME);

    TimerOn();
    Write_proof(&proof);
    RunTime_IO += TimerOff();
	printf("__Open_I/O_ %12llu [us]\n", RunTime_IO);

	FILE *fp;
	fp = fopen("record/open.txt", "a+");
	fprintf(fp, "%d %d %llu %llu\n", pp.cm_pp.security_level, poly.d, RunTime_IO, RunTime);			
	fclose(fp);

	return 0;
}

#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"
int global_num_threads;

int main(int argc, char *argv[])
{
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    
    global_num_threads = 1;
    // global_num_threads = omp_get_max_threads();
    // if(argc > 1)
    // {     
    //     global_num_threads = atoi(argv[1]) < omp_get_max_threads() ? atoi(argv[1]) : omp_get_max_threads();
    // }
    _struct_polynomial_pp_ pp = {0};
    _struct_poly_ poly = {0};
    _struct_commit_ cm = {0};

    fmpz_init(cm.C);
	TimerOn();
    Read_pp(&pp);
    Read_poly(&poly);
    RunTime_IO = TimerOff();
    start_precomputation(&pp, poly);


	TimerOn();
    commit_init(&cm);
    commit_precompute(&cm, pp.cm_pp, poly, pp.q, -1);
    commit_clear(&cm);
    RunTime = TimerOff();
	printf("Commit_PRE_ %12llu [us]\n", RunTime);
    // printf("pre "); fmpz_print(cm.C); printf("\n");

	TimerOn();
    commit_init(&cm);
    commit_new(&cm, pp.cm_pp, poly, pp.q);
    commit_clear(&cm);
    RunTime = TimerOff();
	printf("Commit_NEW_ %12llu [us]\n", RunTime);
    // printf("ori "); fmpz_print(cm.C); printf("\n");

	TimerOn();
    Write_Commit("./Txt/commit.txt", &cm);
    RunTime_IO += TimerOff();
	printf("Commit_I/O_ %12llu [us]\n", RunTime_IO);

	return 0;
}

#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"
int global_num_threads = 1;

// compute commitment with/without precomputation table
// start_precomputation: make table base of g and R. g,g^2,...,g^pp->n, R[0],R[0]^q,...,R[0]^{q^(2^d-1)},...,R[1],...,R[n],...
// commit_precom: compute g^Fx[0],...,g^Fx[d] with precomputation table
// commit_new: compute g^Fx[0],...,g^Fx[d] without precomputation table

int main(int argc, char *argv[])
{
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    
    /* setting for thread only one use */
    // global_num_threads = omp_get_max_threads();
    // if(argc > 1)
    // {     
    //     global_num_threads = atoi(argv[1]) < omp_get_max_threads() ? atoi(argv[1]) : omp_get_max_threads();
    // }
    _struct_polynomial_pp_ pp = {0};
    _struct_poly_ poly = {0};
    _struct_commit_ cm = {0};

    qfb_init(cm.C);
	TimerOn();
    Read_pp(&pp);
    Read_poly(&poly);
    RunTime_IO = TimerOff();

    // precomputation table base of g and R. (g,g^2,...g^pp->n , R[0],R[0]^q,...,R[n]^{q^(2^d-1)})
    start_precomputation(&pp, poly);

    // compute commitment with precomputation table
	TimerOn();
    commit_init(&cm);

    // product g_i^Fx_i from i = 1 to d 
    commit_precompute(&cm, pp.cm_pp, poly, pp.q, -1);
    commit_clear(&cm);
    RunTime = TimerOff();
	printf("Commit_with_Precompute_Table %12llu [us]\n", RunTime);

    // compute commitment without precomputation table
	TimerOn();
    commit_init(&cm);
    commit_new(&cm, &pp, poly, pp.q);
    RunTime = TimerOff();

	printf("Commit_NEW_ %12llu [us]\n", RunTime);

	TimerOn();
    Write_Commit("./Txt/commit.txt", &cm);
    RunTime_IO += TimerOff();

	printf("Commit_I/O_ %12llu [us]\n", RunTime_IO);

    commit_clear(&cm);

    for(int i=0; i<poly.d; i++)
	{			
		fmpz_clear(poly.Fx[poly.d-i-1]);
	}
    for(int i=0; i<pp.n; i++)
	{
		qfb_clear(pp.R[i]);
	}
    free(poly.Fx);
    free(pp.R);
	return 0;
}

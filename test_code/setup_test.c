#include "../hedder/polynomial_commit.h"
#include "../hedder/polynomial_open_verify.h"
#include "../hedder/util.h"
int global_num_threads = 1;

int main(int argc, char *argv[])
{
    int m  = 1; // polynomial batch = 1
    int LOG_D,D; // degree
	int security_level = 512; // lamda
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    
    _struct_polynomial_pp_ pp={0};
    _struct_poly_ poly={0};

	if(argc == 2)
		LOG_D = atoi(argv[1]);
	else if(argc == 3){
		security_level = atoi(argv[1]);
		LOG_D = atoi(argv[2]);
	}
	else
		LOG_D = 10;

	D =  (1<<(LOG_D)); // 2^degree

	make_poly(&poly, D);

	TimerOn();
	// setup G(group), generater g vector number of polynomials Fx
    pokRep_setup(&pp, security_level, m, D, &poly);
	printf("d_bit %d d-%d q-%d\n", LOG_D, D, (int)fmpz_bits(pp.q)-1);
    RunTime = TimerOff();
	
	printf("KeyGen_Time %12llu [us]\n", RunTime);

	write_poly(&poly);

	TimerOn(); 
    Write_pp(&pp);
    RunTime_IO = TimerOff();

	printf("KeyGen_I/O_ %12llu [us]\n", RunTime_IO);

	for(int i=0; i < D; i++){
		fmpz_clear(poly.Fx[D-i-1]);
	}
	
    for(int i = 0; i < pp.n; i++){
        qfb_clear(pp.R[i]);
    }

	free(poly.Fx);
	free(pp.R);
	fmpz_clear(pp.q);

	FILE *fp;
	fp = fopen("record/setup.txt", "a+");
	fprintf(fp, "%d %d %llu %llu\n", pp.cm_pp.security_level, poly.d, RunTime_IO, RunTime);			
	fclose(fp);
	
	return 0;
}

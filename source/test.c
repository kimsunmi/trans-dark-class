#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"
#include <omp.h>
int global_num_threads = 1;
int main(int argc, char *argv[])
{
    FILE *fp;
    char buffer[10000];
    int flag = 1;
    int m = 1;
    int LOG_D;
	int security_level = 2048; // 
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    
    _struct_polynomial_pp_ pp, pp1, pp2, pp3;
    _struct_poly_ poly;
    _struct_open_ open;
    _struct_commit_ cm, cm1, cm2;
    _struct_proof_ proof1, proof2;

    fmpz_t l;
    fmpz_init(l);
    fmpz_init(cm.C);
    fmpz_init(open.r);
    fmpz_init(open.Q);
    global_num_threads = 1;
    
	if(argc == 2)
		LOG_D = atoi(argv[1]);
    else if(argc == 3)
    {
		security_level = atoi(argv[1]);
		LOG_D = atoi(argv[2]);        
    }
    else if(argc == 4)
    {
		security_level = atoi(argv[1]);
		LOG_D = atoi(argv[2]);          
        global_num_threads = atoi(argv[3]) < omp_get_max_threads() ? atoi(argv[3]) : omp_get_max_threads();
    }
	else
		LOG_D = 10;
	printf("d_bit %d d-%d\r\n", LOG_D, (1<<(LOG_D)));

	make_poly((1<<(LOG_D)));
    Read_poly(&poly);

	TimerOn();
    poly_commitment_setup(&pp, security_level, m, poly.d);
    RunTime = TimerOff();
	printf("KeyGen_Time %12llu\r\n", RunTime);

	TimerOn();
    Write_pp(&pp);
    RunTime_IO = TimerOff();
	printf("KeyGen_I/O_ %12llu\r\n\n", RunTime_IO);
    

	fp = fopen("record/setup.txt", "a+");
	fprintf(fp, "%4d %9d %12llu %12llu %2d\r\n", pp.cm_pp.security_level, poly.d, RunTime_IO, RunTime, global_num_threads);
	fclose(fp);

    start_precomputation(&pp, poly);

	//////////////////////////////////////////////////////////////////

	TimerOn();
    Read_pp(&pp1);
    RunTime_IO = TimerOff();

	TimerOn();
    commit_precompute(&cm, pp1.cm_pp, poly, pp1.q, -1);
    RunTime = TimerOff();
	printf("Commit_Time %12llu\r\n", RunTime);

	TimerOn();
    Write_Commit("./Txt/commit.txt", &cm);
    RunTime_IO += TimerOff();
	printf("Commit_I/O_ %12llu\r\n\n", RunTime_IO);

	fp = fopen("record/commit.txt", "a+");
	fprintf(fp, "%4d %9d %12llu %12llu %2d\r\n", pp1.cm_pp.security_level, poly.d, RunTime_IO, RunTime, global_num_threads);
	fclose(fp);

    //////////////////////////////////////////////////////////////////
	TimerOn();
    Read_pp(&pp2);
    Read_Commit("./Txt/commit.txt", &cm1);
    RunTime_IO = TimerOff();

    TimerOn();
    Open(&proof1, &pp2, &cm1, &poly);
    RunTime = TimerOff();
	printf("__Poly_Open %12llu\r\n", RunTime);

    TimerOn();
    Write_proof(&proof1);
    RunTime_IO += TimerOff();
	printf("__Open_I/O_ %12llu\r\n\n", RunTime_IO);

	fp = fopen("record/open.txt", "a+");
	fprintf(fp, "%4d %9d %12llu %12llu %2d\r\n", pp2.cm_pp.security_level, poly.d, RunTime_IO, RunTime, global_num_threads);	
	fclose(fp);

    //////////////////////////////////////////////////////////////////
	TimerOn();
    Read_pp(&pp3);
    Read_Commit("./Txt/commit.txt", &cm2);
    Read_proof(&proof2);
    RunTime_IO = TimerOff();

    TimerOn();
    flag = Verify(&proof2, &pp3, &cm2);
    RunTime = TimerOff();
    printf("Poly_Verify %12llu\r\n", RunTime);
    printf("Verify_I/O_ %12llu [%d]\r\n\n", RunTime_IO, flag);


	fp = fopen("record/verify.txt", "a+");
	fprintf(fp, "%4d %9d %12llu %12llu %2d [%d]\r\n", pp3.cm_pp.security_level, poly.d, RunTime_IO, RunTime, global_num_threads, flag);			
	fclose(fp);

	fp = fopen("record/size.txt", "a+");
	fprintf(fp, "%d %d %d %d\r\n", pp3.cm_pp.security_level, getfilesize("Txt/pp.txt"), getfilesize("Txt/commit.txt"), getfilesize("Txt/proof.txt"));
	fclose(fp);

	return 0;
}

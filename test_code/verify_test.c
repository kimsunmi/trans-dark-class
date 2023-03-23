#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"
int global_num_threads = 1;

int main(int argc, char *argv[])
{
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    int flag = 0;
    _struct_polynomial_pp_ pp = {0};
    _struct_poly_ poly = {0};
    _struct_open_ open = {0};
    _struct_commit_ cm = {0};
    _struct_proof_ proof = {0};

    fmpz_t l;

    fmpz_init(l);
    fmpz_init(cm.C );
    fmpz_init(open.r);
    fmpz_init(open.Q);

	TimerOn();
    Read_poly(&poly);
    Read_pp(&pp);
    Read_Commit("./Txt/commit.txt", &cm);
    Read_proof(&proof);
    RunTime_IO = TimerOff();

    TimerOn();
    flag = Verify(&pp, &cm, poly.z, poly.fz, &proof);
    RunTime = TimerOff();
    printf("Poly_Verify %12llu [us]\n", RunTime);
    printf("Verify_I/O_ %12llu [us]\n", RunTime_IO);
    printf("Verify Result [%d]\n", flag);

	getfilesize("Txt/pp.txt");
	getfilesize("Txt/commit.txt");
	getfilesize("Txt/proof.txt");

	return 0;
}

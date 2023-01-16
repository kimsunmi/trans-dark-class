#include "../hedder/Reducible_commitment.h"
#include "../hedder/Reducible_polynomial_commitment.h"
#include "../hedder/util.h"
int global_num_threads = 1;

int main(int argc, char *argv[])
{
    int m = 1; // polynomial batch = 1
    int LOG_D;
	int security_level = 2048; // 추후 테스트에 2048로 수정
	unsigned long long int RunTime = 0, RunTime_IO = 0;
    
    _struct_polynomial_pp_ pp = {0};
    _struct_poly_ poly = {0};

	if(argc == 2)
		LOG_D = atoi(argv[1]);
	else if(argc == 3){
		security_level = atoi(argv[1]);
		LOG_D = atoi(argv[2]);
	}
	else
		LOG_D = 10;


	/*
	 * make poly 안에 &poly 넣어서 애초에 거기다가 저장 (이 때 z, fz는 설정되지 않음)
	 * poly_commitment_setup에다가 &poly 넘겨서 z 생성 후 poly->z 에 저장
	 * poly->fz 는 위 함수 안에서 같이 계산 // timer에 넣을 것인가? 이건 별도 생각 필요
	 * poly_commitment_setup 끝나고 나서 write_poly 실행 
	 * 그럼 이 안에서는 read_poly를 쓸 필요가 없음
	 * 
	 * read_poly 안에서 z, fz 읽는거 추가해야함 
	*/

	make_poly(&poly, (1<<(LOG_D))); // 2^10

	printf("d_bit %d d-%d\n", LOG_D, (1<<(LOG_D)));
	TimerOn();
    // poly_commitment_setup(&pp, security_level, m, (LOG_D)); 
    poly_commitment_setup(&pp, security_level, m, (1<<(LOG_D)), &poly); 
    RunTime = TimerOff();
	printf("KeyGen_Time %12llu [us]\n", RunTime);

	write_poly(&poly);

	TimerOn(); 
    Write_pp(&pp);
    RunTime_IO = TimerOff();
	printf("KeyGen_I/O_ %12llu [us]\n", RunTime_IO);


	FILE *fp;
	fp = fopen("record/setup.txt", "a+");
	fprintf(fp, "%d %d %llu %llu\n", pp.cm_pp.security_level, poly.d, RunTime_IO, RunTime);			
	fclose(fp);

	return 0;
}

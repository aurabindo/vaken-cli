#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 3
#define SOPS_NUM 6
#define MAX 25
#define MIN 1

enum sops { L, R, U, D, B, F};
struct sops_t {
    enum sops opr;
}sobj;

char get_opr() {
    struct timeval tm;
    gettimeofday(&tm, NULL);
    srandom(tm.tv_sec + tm.tv_usec * 1000000ul);       

    enum sops rando;
    rando = rand() % SOPS_NUM ;
    char ret_val;
    switch (rando) {
	case L :    ret_val = 'L';
		    break;
	
	case R :    ret_val = 'R';
		    break;
	
	case U :    ret_val = 'U';
		    break;
	
	case D :    ret_val = 'D';
		    break;
	
	case B :    ret_val = 'B';
		    break;
	
	case F :    ret_val = 'F';
		    break;
	default :   ret_val = 'L';
	
    }
    return ret_val;
}

int main(int argc, const char *argv[])
{
	int i,j, max, min = 1;
	int keysize;
	int kz;
	time_t t;
	struct timeval tm;
	FILE *fp;

	fp = fopen("keyfile","w");
	gettimeofday(&tm, NULL);
	srandom(tm.tv_sec + tm.tv_usec * 1000000ul);
	
	printf("\nEnter the Keysize: ");
	scanf("%d",&keysize);

	char *cur_line, *save_cur_line;
	cur_line = (char *) malloc(sizeof(char)*(keysize));
	if (!cur_line) {
	    printf("\nNo memory left!\n");
	    exit(1);
	}

	save_cur_line = cur_line;

	for (i=0; i<keysize; i++) {
	    kz = (rand() % MAX) + MIN;
	    for (j = 0; j<kz; j++) {
		cur_line[j] = get_opr() ;
	    }
	    cur_line[j] = '\0';
	    fprintf(fp,"%d:%s\n",i,cur_line);
	    cur_line = save_cur_line;
	}
	fclose(fp);
	free(save_cur_line);
	return 0;
}

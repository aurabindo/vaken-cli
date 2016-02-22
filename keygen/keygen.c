/*******************************************************************************
*
*   Copyright (C) 2014 Aurabindo J <mail@aurabindo.in>
*   All rights reserved.
*
*
*
*   This file is a part of the NPJ Variable Key Cryptography project. 
*   See LICENSE.txt for legal terms.
*
********************************************************************************/

#include "keygen.h"

char get_opr() {
    sops rando;
    struct timeval tm;
    
    gettimeofday(&tm, NULL);
    srandom(tm.tv_sec + tm.tv_usec * 1000000ul);       

    rando = rand() % SOPS_NUM ;
   
    if ( ( 'A' <= rando ) && ( rando <= 'G' ) )
	return rando;
    else
	return rando;
}

int main(int argc, const char *argv[])
{
	int i,j, max, min = 1;
	int keysize;
	int kz;
	time_t t;
	struct timeval tm;
	FILE *fp, *fp_xor;
	unsigned char xor[N][N];

	fp = fopen("KEYBANK","w");
	if (!fp) {
	    perror("KEYBANK:");
	}

	fp_xor = fopen("XOR","w");
	if (!xor) {
	    perror("XOR:");
	}

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
	unsigned int temp;
	for (i = 0; i < N; i++) {
	    for (j = 0; j < N; j++) {

		xor[i][j] = (rand() % 255) + MIN;
		fwrite((void *) &xor[i][j],sizeof(char),1,fp_xor);
	    }
	    printf("\n");
	}
	fclose(fp_xor);
	return 0;
}

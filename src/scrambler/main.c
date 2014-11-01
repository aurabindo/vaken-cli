
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


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define DATA_FILE "payload.txt"
#define N 3 // Max Rows and Cols
#define sz sizeof(int)


enum dim { r,c };

typedef struct permute_info permute_info;
struct permute_info {
    enum dim sel;
    int this, that;
};


int ** permute(int ***a, permute_info per);	//Do the transformation
int ** gen_i();			//Make I matrix
int ** mat_mul( int ***a, int ***b);		//Multiply matrices
int ** mat_in();
void mat_pr(int **a);
void mat_pr_hex(int **a);
void create_mat_2d(int ***gen);
void free_mat_2d(int ***ptr);

int main(int argc, const char *argv[])
{
    FILE *fp_pl;
    int **result;
    static int **input;
    int ret,i,j,data_len;

    fp_pl = fopen(DATA_FILE,"r");
    if (!fp_pl) {
	perror("scrambler: payload:");
	exit(1);
    }
    
    create_mat_2d(&input);
    ret = 0;
    for (i = 0; i < N; i++) {
	for (j = 0; j <N; j++) {
	    ret += fread((void *) &input[i][j],sz, 1, fp_pl);
	}
    }

    if (ret < N) //for marking where a file has ended.
	data_len = ret;
    else
	data_len = N;

    permute_info per;
    per.sel = c;
    per.this = 0;
    per.that = 1;

    result = permute(&input,per);
    
    printf("Input Matrix was: \n");
    mat_pr_hex(input);

    printf("Matrix after permutation is: \n");
    mat_pr_hex(result);
    
    free_mat_2d(&result);
    free_mat_2d(&input);

    fclose(fp_pl);
    return 0;
}

void free_mat_2d(int ***ptr) {
    int i;
    for (i = 0; i < N; i++) {
	free((*ptr)[i]);
    }
    free(*ptr);
}

void create_mat_2d(int ***gen) {
    int i;
    *gen = malloc(N * sizeof(int *));
    for (i = 0; i < N; i++) {
	(*gen)[i] = malloc(N * sizeof(int));	
    }
}

void mat_pr(int **a) {
    int i, j;
    for (i = 0; i < N; i++) {
	printf("Row %d\t", (i+1));
	for (j = 0; j < N; j++) {
	    printf("%d\t",a[i][j]);
	}
	printf("\n");
    }
}

void mat_pr_hex(int **a) {
    int i, j;
    
    printf("Printing in Hex\n");
    for (i = 0; i < N; i++) {
	printf("Row %d\t", (i+1));
	for (j = 0; j < N; j++) {
	    printf("0x%x\t",a[i][j]);
	}
	printf("\n");
    }
}

int ** permute(int ***mat, permute_info per) {
    static int **i, **result;
    int this, that;

    this = per.this;
    that = per.that;

    i = gen_i();
    
    i[this][this] = 0;
    i[this][that] = 1;
    i[that][this] = 1;
    i[that][that] = 0;
    
    if (per.sel == c)
	result = mat_mul(mat,&i);
    else
	result = mat_mul(&i,mat);

    free_mat_2d(&i);

    return result;

}

int ** gen_i() {
    int i,j;
    static int **mat;
    create_mat_2d(&mat);
    
    for (i = 0; i < N; i++) {
    	for (j = 0; j < N; j++) {
	    if (i==j)
		mat[i][j] = 1;
	    else
		mat[i][j] = 0;
    	}
    }

    return mat;
}

int ** mat_mul( int ***a, int ***b) {
    int i,j,k;
    static int **pro;

    create_mat_2d(&pro);

    for (i = 0; i<N; i++)
	for (j = 0; j < N; j++)
	    for (k = 0; k < N; k++)
		pro[i][j] += (*a)[i][k] * (*b)[k][j];

    return pro;
}

int ** mat_in()
{
    int i,j;
    int **mat;

    create_mat_2d(&mat);
    printf("Entering %d X %d Matrix\n",N,N);

    for (i = 0; i < N; i++) {
	printf("Enter row %d: ", (i+1));
	for (j = 0; j < N; j++) {
	    scanf("%d",&mat[i][j]);
	}
    }

    return (int **) mat;
}

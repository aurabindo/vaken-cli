
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
#include <string.h>

#define DATA_FILE "payload.txt"
#define OUT_FILE "encrypted"

#define N 3 // Max Rows and Cols
#define sz sizeof(char)
#define MAX 26
#define KEYFILE "keyfile"
#define SOPS_NUM 7


enum dim { r,c };

typedef struct permute_info permute_info;
struct permute_info {
    enum dim sel;
    int this, that;
};


enum sops { A, B, C, D, E, F, G};
struct sops_t {
    enum sops opr;
}sobj;

char ** keybank;				//To hold global keybank

int ** permute(int ***a, permute_info per);	//Do the transformation
int ** gen_i();					//Make I matrix
int ** mat_mul( int ***a, int ***b);		//Multiply matrices
int ** mat_in();
char * fetch_key(int key);
void mat_pr(int **a);
void mat_fpr(int **a, FILE *fp);
void mat_pr_hex(int **a);
void mat_pr_char(int **a);
void create_mat_2d(int ***gen);
void free_mat_2d(int ***ptr);

int main(int argc, const char *argv[])
{
    static FILE *fp_pl, *fp_out;
    int **result;
    static int **input;
    char *todo_keys;
    int ret,i, save_i,j, save_j,data_len, count;

    fp_pl = fopen(DATA_FILE,"r");
    if (!fp_pl) {
	perror("scrambler: main:");
	exit(1);
    }
    
    fp_out = fopen(OUT_FILE, "w");
    if (!fp_out) {
	perror("scrambler: main:");
	exit(1);
    }

    create_mat_2d(&input);
    save_i = 0;
    save_j = 0;
    while (!feof(fp_pl)) {
	ret = 0;

	for (i = 0; i < N; i++) {
	    for (j = 0; j < N; j++) {
		input[i][j] = 0;
	    }
	}
	for (i = 0; i < N; i++) {
	    for (j = 0; j <N; j++) {
		ret = fread((void *) &input[i][j],sz, 1, fp_pl);
		if (ret < 1) {
		    save_i = i;
		    save_j = j;
		    break;
		}
		else
		    count++;
	    }
	    if (ret < 1)
		break;
	}

	if (count < N*N) {		//fill the rest of the space with 0
	    data_len = count;
	    while (count < N) {
		for (i = save_i; i < N; i++)
		    for (j = save_j; j < N; j++)
			input[i][j] = 0;
	    }
	}
	else
	    data_len = N*N;
	
	struct timeval tm;
	enum sops rando;
	
	gettimeofday(&tm, NULL);
	srandom(tm.tv_sec + tm.tv_usec * 1000000ul);       
	rando = rand() % SOPS_NUM;
	
	//todo_keys = fetch_key(rando);
	todo_keys = fetch_key(0);
	permute_info per;
	
	for (i = 0; i < strlen(todo_keys); i++) {
	    switch (todo_keys[i]) {
		case 'A'    :	// c0 to c1
				per.sel = c;
				per.this = 0;
				per.that = 1;
				break;
		case 'B'    :	//c0 to c2
				per.sel = c;
				per.this = 0;
				per.that = 2;
				break;
		case 'C'    :	//c1 to c2
				per.sel = c;
				per.this = 1;
				per.that = 2;
				break;
		case 'D'    :	//r0 to r1
				per.sel = r;
				per.this = 0;
				per.that = 1;
				break;
		case 'E'    :	//r0 to r2
				per.sel = r;
				per.this = 0;
				per.that = 2;
				break;
		case 'F'    :	//r1 to r2
				per.sel = r;
				per.this = 1;
				per.that = 2;
				break;
		case 'G'    :	//may be xor ? right now same as F
				per.sel = r;
				per.this = 0;
				per.that = 1;
				break;
		default	    :	//may be xor again? right now same as F
				per.sel = r;
				per.this = 0;
				per.that = 1;
	    }

	    result = permute(&input,per);
//	    printf("Input Matrix was: \n");
//	    mat_pr_char(input);

//	    printf("Matrix after permutation is: \n");
//	    mat_pr_char(result);
	    //mat_fpr(result,fp_out); //printing to file
	    

	    
	}
	
	printf("Input Matrix was: \n");
	mat_pr_char(input);
	printf("Matrix after permutation is: \n");
	mat_pr_char(result);
	fprintf(fp_out,"%d ",0);
	int w,x; 

	for (w = 0; w < N; w++) {
	    for (x = 0; x < N; x++) {
		fprintf(fp_out,"%d ",result[w][x]);
	    }
	    //printf("\n");
	}
	fprintf(fp_out,"\n");

	for (w = 0; w < N; w++) {
	    for (x = 0; x < N; x++) {
		input[w][x] = result[w][x];
	    }
	}
    

    }

    free_mat_2d(&result);
    free_mat_2d(&input);
    fclose(fp_pl);
    fclose(fp_out);
    return 0;
}


char* fetch_key(int key){
    static char Rotation_string[MAX];
    int needle;

    FILE* fp;
    fp = fopen("keyfile","r");
    
    if (!fp) {
	perror("fetch_key: ");
	exit(1);
    }

    while(1){
	fscanf(fp,"%d:%s",&needle,Rotation_string);
	if (needle == key) {
	    return Rotation_string;
	}
	else if (feof(fp) && needle != key){
	    printf("Key not found!");
	    return NULL;
	}
    }
    
    fclose(fp); 
    return Rotation_string;
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
    if (!gen) {
	perror("create_mat_2d: ");
	exit(1);
    }
    for (i = 0; i < N; i++) {
	(*gen)[i] = malloc(N * sizeof(int));	
    }
}

void mat_fpr(int **a, FILE *fp_out) {
    int i, j;
    for (i = 0; i < N; i++) {
//	printf("Row %d\t", (i+1));
	for (j = 0; j < N; j++) {
	    fprintf(fp_out,"%d ",a[i][j]);
	}
	printf("\n");
    }
    printf("\n");
}

void mat_pr_char(int **a) {
    int i, j;
    for (i = 0; i < N; i++) {
	printf("Row %d\t", (i+1));
	for (j = 0; j < N; j++) {
	    printf("%c\t",a[i][j]);
	}
	printf("\n");
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
    static int **mat;

    create_mat_2d(&mat);
    printf("Entering %d X %d Matrix\n",N,N);

    for (i = 0; i < N; i++) {
	printf("Enter row %d: ", (i+1));
	for (j = 0; j < N; j++) {
	    scanf("%d",&mat[i][j]);
	}
    }

    return mat;
}

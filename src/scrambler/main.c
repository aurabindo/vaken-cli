#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define DATA_FILE "payload.txt"
#define N 3 // Max Rows and Cols

int payload[N][N];

enum dim { r,c };

typedef struct permute_info permute_info;
struct permute_info {
    enum dim sel;
    int this, that;
};

int ** permute(int a[][], permute_info per);	//Do the transformation
int ** make_i(int a[][]);			//Make I matrix
int ** mat_mul( int a[][], int b[][]);		//Multiply matrices
int ** mat_in();

int main(int argc, const char *argv[])
{
    FILE *fp_pl;
    fp_pl = fopen(DATA_FILE,"r");
    if (!fp_pl) {
	perror("scrambler: payload:");
	exit(1);
    }
	

    return 0;
}

int ** permute(int mat[][], permute_info per) {
    static int i[N][N];
    i = make_i(i);

    i[this][this] = 0;
    i[this][that] = 1;
    i[that][this] = 1;
    i[that][that] = 0;
    
    if (per.sel == c)
	mat = mat_mul(mat,i);
    else
	mat = mut_mul(i,mat);
    return a;

}

int ** make_i(int a[][]) {
    int i,j;
    static int mat[N][N];
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

int ** mat_mul( int a[][], int b[][]) {
    int i,j,k;
    static int pro[N][N];

    for (i = 0; i<N; i++)
	for (j = 0; j < N; j++) {
	    for (k = 0; k < N; k++) {
		pro[i][j] = a[i][k] * b[k][j];	
	    }	    
	}

    return c;
}

int ** mat_in()
{
    int i,j;
    static int mat[N][N];
    printf("Entering %d X %d Matrix\n",N,N);

    for (i = 0; i < N; i++) {
	printf("Enter row %d", (i+1));
	for (j = 0; j < N; j++) {
	    scanf("%d",&mat[i][j]);
	}
	printf("\n");
    }

    return mat;
}

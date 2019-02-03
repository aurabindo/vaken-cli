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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define PLD_FILE "payload.txt"
#define ENC_FILE "encrypted"
#define DEC_FILE "decrypted"
#define XOR_NOISE "xorfile"

#define N 3 // Max Rows and Cols
#define sz sizeof(char)
#define MAX 26
#define KEYFILE "keyfile"
#define SOPS_NUM 7

enum dim {
    r,
    c
};

typedef struct permute_info permute_info;
struct permute_info {
    enum dim sel;
    int this, that;
};


enum sops {
    A,
    B,
    C,
    D,
    E,
    F,
    G
};

struct sops_t {
    enum sops opr;
}sobj;

char ** keybank;				//To hold global keybank

int ** permute(int ***a, permute_info per);	//Do the transformation
int ** xor(int ***a);			//Do the transformation
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

int main(int argc, char *argv[])
{
    static FILE *fp_in, *fp_out;
    int **result;
    static int **input;
    char *todo_keys;
    char in_file[MAX], out_file[MAX];
    int ret,i, save_i,j, save_j,data_len, count, encrypt, decrypt, opt, d_key;
    int key_read_flag_dec =0;
    char d_key_char;
    int xor_flag = 0;

    encrypt = 0;
    decrypt = 0;

    if (argc <2) {
    printf("Wrong Usage\n");
    exit(1);
    }

    while ((opt = getopt(argc, argv, "ed")) != -1) {
    switch (opt) {
        case 'e':
            encrypt = 1;
            decrypt = 0;
            strcpy(in_file,PLD_FILE);
            strcpy(out_file,ENC_FILE);
            break;
        case 'd':
            decrypt = 1;
            encrypt = 0;
            strcpy(in_file,ENC_FILE);
            strcpy(out_file,DEC_FILE);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-ed]\n",
                        argv[0]);
            exit(1);
        }
    }

    printf("encrypt=%d; decrypt=%d\n", encrypt, decrypt);


    fp_in = fopen(in_file,"r");
    if (!fp_in) {
        perror("scrambler: in_file:");
        exit(1);
    }

    fp_out = fopen(out_file, "w");
    if (!fp_out) {
        perror("scrambler: out_file:");
        exit(1);
    }

    create_mat_2d(&input);
    save_i = 0;
    save_j = 0;
    while (!feof(fp_in)) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
            input[i][j] = 0;
            }
        }
        key_read_flag_dec = 1;
        ret = 0;
        int ret_dec = 10;
        int input_temp;
        count =0;
        for (i = 0; i < N; i++) {
            for (j = 0; j <N; j++) {
                if (decrypt && key_read_flag_dec) {
                    ret_dec = fscanf(fp_in, "%d",&d_key);
                    //d_key = d_key_char - 0x30; //convert to integer
                    key_read_flag_dec = 0;
                    //fscanf(fp_in,"%c",&d_key);
                }
                if (encrypt){
                    ret = fread((void *) &input[i][j],sz, 1, fp_in);
                    if (ret < 1) {
                        save_i = i;
                        save_j = j;
                        break;
                    }
                    else
                        count++;
                } else if (decrypt) {
                    ret_dec = fscanf(fp_in,"%d", &input_temp);
                    if (ret_dec != EOF) {
                        input[i][j] = (char) input_temp;
                        count++;
                    }
                    else
                        break;
                }
            }

            if ((encrypt && (ret < 1)) || (decrypt && count == N * N))
                break;
        }

        if (count < N*N)		//fill the rest of the space with 0
            data_len = count;
        else
            data_len = N*N;

        struct timeval tm;
        enum sops rand_key;

        gettimeofday(&tm, NULL);
        srandom(tm.tv_sec + tm.tv_usec * 1000000ul);
        rand_key = rand() % SOPS_NUM;

        //todo_keys = fetch_key(rando);
        if (encrypt)
            todo_keys = fetch_key(rand_key);
        else if (decrypt)
            todo_keys = fetch_key(d_key);

        permute_info per;
        int len;
        len = strlen(todo_keys);
        for (i = 0; i < len; i++) {
            switch (todo_keys[i]) {
            case 'A':	// c0 to c1
                per.sel = c;
                per.this = 0;
                per.that = 1;
                break;
            case 'B':	//c0 to c2
                per.sel = c;
                per.this = 0;
                per.that = 2;
                break;
            case 'C':	//c1 to c2
                per.sel = c;
                per.this = 1;
                per.that = 2;
                break;
            case 'D':	//r0 to r1
                per.sel = r;
                per.this = 0;
                per.that = 1;
                break;
            case 'E':	//r0 to r2
                per.sel = r;
                per.this = 0;
                per.that = 2;
                break;
            case 'F':	//r1 to r2
                per.sel = r;
                per.this = 1;
                per.that = 2;
                break;
            case 'G':	// XOR
            default:
                xor_flag = 1;
            }

            if (!xor_flag)
                result = permute(&input,per);
            else
                result = xor(&input);
        }

        printf("Input Matrix was: \n");
        mat_pr_char(input);
        printf("Matrix after permutation is: \n");
        mat_pr_char(result);


        int w,x;
        if (encrypt) {
            fprintf(fp_out,"%d ",rand_key);
            for (w = 0; w < N; w++) {
                for (x = 0; x < N; x++) {
                    fprintf(fp_out,"%d ",result[w][x]);
                }
            }
            fprintf(fp_out,"\n");
        } else if (decrypt) {

            for (w = 0; w < N; w++) {
                for (x = 0; x < N; x++) {
                    fwrite((void *) &result[w][x],sz,1,fp_out);
                }
            }
        }
        for (w = 0; w < N; w++) {
            for (x = 0; x < N; x++) {
                input[w][x] = result[w][x];
            }
        }
    }

    free_mat_2d(&result);
    //free_mat_2d(&input);
    fclose(fp_in);
    fclose(fp_out);
    return 0;
}

int ** xor(int ***a) {
    FILE *fp_x;
    int i,j,ret;
    fp_x = fopen(XOR_NOISE,"r");
    if (!fp_x) {
        perror("xor noise:");
        exit(1);
    }

    static int **xor;
    create_mat_2d(&xor);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            ret = fread((void *) &xor[i][j],sizeof(char),1,fp_x);
        }
    }
    printf("Xor key is: \n");
    mat_pr_hex(xor);

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            (*a)[i][j] = (*a)[i][j] ^ xor[i][j];
        }
    }
    fclose(fp_x);
    free_mat_2d(&xor);
    return *a;
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
        (*gen)[i] = malloc(N * sizeof(char));
    }
}

void mat_fpr(int **a, FILE *fp_out) {
    int i, j;
    for (i = 0; i < N; i++) {
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

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

#ifndef __KEYGEN__
#define __KEYGEN__

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

typedef unsigned char 	t_u8;
typedef char		t_8;
typedef unsigned short	t_u16;
typedef short		t_16;
typedef unsigned int	t_u32;
typedef int		t_32;
typedef unsigned long	t_ul;
typedef long		t_l;

#define KEYBANK		keyfile
#define XOR		xorfile
#define N 		3
#define SOPS_NUM 	7
#define MAX 		25
#define MIN 		3

typedef enum {
	A, 
	B, 
	C, 
	D, 
	E, 
	F, 
	G
} sops;

struct sops_t {
    sops opr;
}sobj;


#endif

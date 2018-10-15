
/**********************************************
/	Author					Tyler Brady
/	Student Number			040 852 457
/	Assignment Number		1
/	Compiler				MS Visual Studio 2015
/	File Name				buffer.h
/	CST8152_010				Compilers
/	Lab Section				302
/	Professor				Svillen Ranev
/	Assignment Due			Feb 9th 2018
/	Assignment Submitted	Feb 9th 2018
/	Purpose					To create a buffer and associated functions.
/ 
/	
/	Buffer * b_allocate();
/	pBuffer b_addc();
/	int b_clear();
/	void b_free();
/	int b_isfull();
/	short b_limit();
/	short b_capacity();
/	short b_mark();
/	int b_mode();
/	size_t b_incfactor();
/	int b_load();
/	int b_isempty();
/	int b_eob();
/	char b_getc();
/	int b_print();
/	Buffer * b_compact();
/	char b_rflag();
/	short b_retract();
/	short b_reset();
/	short b_getcoffset();
/	int b_rewind();
/	char * b_location();
************************************************/

#ifndef BUFFER_H_
#define BUFFER_H_

#pragma warning( disable:4001 )
#pragma warning( disable:4244 )


/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

														   /* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

														   /* constant definitions */
														   /* You may add your own constant definitions here */
#define RT_FAIL1 -1         /* fail return value */
#define RT_FAIL2 -2         /* fail return value */
#define LOAD_FAIL -2       /* load fail error */
#define SET_R_FLAG 1       /* realloc flag set value */
#define F_PASS1 1			/* function pass value*/
#define F_PASS0 0			/* function pass value*/
#define F_PASSN -1			/* function pass value*/




														   /* user data type declarations */
typedef struct BufferDescriptor {
	char *cb_head;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;  /* the offset (in chars) to the add-character location */
	short getc_offset;  /* the offset (in chars) to the get-character location */
	short markc_offset; /* the offset (in chars) to the mark location */
	char  inc_factor; /* character array increment factor */
	char  r_flag;     /* reallocation flag */
	char  mode;       /* operational mode indicator*/
	int   eob;       /* end-of-buffer flag */
} Buffer, *pBuffer;


/* function declarations */
/*
Place your function declarations here.
Do not include the function header comments here.
Place them in the buffer.c file
*/


Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode);
pBuffer b_addc(pBuffer const pBD, char symbol);
int b_clear(Buffer * const pBD);
void b_free(Buffer * const pBD);
int b_isfull(Buffer * const pBD);
short b_limit(Buffer * const pBD);
short b_capacity(Buffer * const pBD);
short b_mark(pBuffer const pBD, short mark);
int b_mode(Buffer * const pBD);
size_t b_incfactor(Buffer * const pBD);
int b_load(FILE * const fi, Buffer * const pBD);
int b_isempty(Buffer * const pBD);
int b_eob(Buffer * const pBD);
char b_getc(Buffer * const pBD);
int b_print(Buffer * const pBD);
Buffer * b_compact(Buffer * const pBD, char symbol);
char b_rflag(Buffer * const pBD);
short b_retract(Buffer * const pBD);
short b_reset(Buffer * const pBD);
short b_getcoffset(Buffer * const pBD);
int b_rewind(Buffer * const pBD);
char * b_location(Buffer * const pBD, short loc_offset);
#endif



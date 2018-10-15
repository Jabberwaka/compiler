
/**********************************************
/	Author					Tyler Brady
/	Student Number			040 852 457
/	Assignment Number		2
/	File Name				table.
/	CST8152_010				Compilers
/	Lab Section				013
/	Professor				Svillen Ranev
/	Assignment Due			March 23rd 2018
/	Assignment Submitted	March 23rd 2018
/	Purpose					To create/initialize the tables necessary 
							for a scanner to function
************************************************/
 

/* Filename: table.h
* Transition Table and function declarations necessary for the scanner implementation
* as required for CST8152 - Assignment #2.
* Version: 1.18.1
* Date: 1 February 2018
* Provided by: Svillen Ranev
* The file is incomplete. You are to complete it.
***************************************************
* REPLACE THIS HEADER WITH YOUR HEADER
***************************************************
*/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

#define ES 12    /* Error state */
#define ER 13    /* Error state */
#define IS -1    /* Inavalid state */

/* State transition table definition */

#define TABLE_COLUMNS 8
/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {

	//			 [A-F]	x alpha 
	//							0	1-9	 .	$	other
	/* State 0  */{1,	1,	1,	6,	4,	ES,	ES,	ES},			//NON ACCEPTING STATE
	/* State 1  */{1,	1,	1,	1,	1,	2,	3,	2  },			//NON ACCEPTING STATE
	/* State 2  */{IS,	IS,	IS,	IS,	IS, IS, IS, IS },			//ACCEPTING STATE - VID AVID/KW - ASWR
	/* State 3  */{IS,	IS,	IS,	IS,	IS, IS, IS, IS },			//ACCEPTING STATE - VID SVID/ASNR
	/* State 4  */{ES,	ES,	ES,	4,	4,	7,	5,	5 },			//NON ACCEPTING STATE
	/* State 5  */{IS,	IS	IS,	IS, IS,	IS,	IS,	IS},			//ACCEPTING STATE - DIL - ASWR
	/* State 6  */{ES,	9,	ES,	6,	ES,	7,	5,	5},				//NON ACCEPTING STATE - HEX
	/* State 7  */{8,	8,	8,	7,	7,	8,	8,	8},				//NON ACCEPTING STATE - HEX
	/* State 8  */{IS,	IS,	IS,	IS,	IS,	IS,	IS,	IS},			//ACCPETING STATE - FPL - ASWR
	/* State 9  */{10,	ES,	ES,	10,	10,	ER,	ES,	ER},			//NON ACCEPTING STATE - HEX
	/* State 10 */{10, ES, ES, 10, 10, 11, 11, 11},				//NON ACCEPTING STATE - HEX
	/* State 11 */{IS,	IS,	IS,	IS,	IS,	IS,	IS,	IS},			//ACCEPTING STATE	HIL - ASWR
	/* State 12 */{IS, IS, IS, IS, IS, IS, IS, IS},				//ACCEPTING STATE	ES - ASNR
	/* State 13 */{IS, IS, IS, IS, IS, IS, IS, IS}				//ACCEPTING STATE	ER- ASWR
};

/* Accepting state table definition */
#define ASWR     10  /* accepting state with retract */
#define ASNR     11  /* accepting state with no retract */
#define NOAS     12  /* not accepting state */

	int as_table[] =
	/*STATE 0*/{ NOAS,
	/*STATE 1 */ NOAS,
	/*STATE 2 */ ASWR,
	/*STATE 3 */ ASNR,
	/*STATE 4 */ NOAS,
	/*STATE 5 */ ASWR,
	/*STATE 6 */ NOAS,
	/*STATE 7 */ NOAS,
	/*STATE 8 */ ASWR,
	/*STATE 9 */ NOAS,
	/*STATE 10*/ NOAS,
	/*STATE 11*/ ASWR,
	/*STATE 12*/ ASNR,
	/*STATE 13*/ ASWR};

Token aa_func02(char *lexeme);
Token aa_func03(char *lexeme); 
Token aa_func05(char *lexeme);
Token aa_func08(char *lexeme);
Token aa_func11(char *lexeme);
Token aa_func12(char *lexeme);

typedef Token(*PTR_AAF)(char *lexeme);


PTR_AAF aa_table[] = { NULL, NULL, aa_func02, aa_func03,
						NULL, aa_func05, NULL, NULL, aa_func08,
						NULL, NULL, aa_func11, aa_func12, aa_func12 };

#define KWT_SIZE  10

char * kw_table[] =
{
	"ELSE",
	"FALSE",
	"IF",
	"PLATYPUS",
	"READ",
	"REPEAT",
	"THEN",
	"TRUE",
	"WHILE",
	"WRITE"
};

#endif



/**********************************************
/	Author					Tyler Brady
/	Student Number			040 852 457
/	Assignment Number		2
/	File Name				scanner.c
/	CST8152_010				Compilers
/	Lab Section				013
/	Professor				Svillen Ranev
/	Assignment Due			March 23rd 2018
/	Assignment Submitted	March 23rd 2018
/	Purpose					SCANNER.C: Functions implementing a Lexical Analyzer (Scanner)
*							 as required for CST8152, Assignment #3
************************************************/


/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer* str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

						 /* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/

					   /* No other global variable declarations/definitiond are allowed */

					   /* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atolh(char * lexeme); /* converts hexadecimal string to decimal value */

								  /*
								  Function name	scanner_init
								  Purpose:		Initializes the scanner

								  In Parameters	pBuffer
								  Out parameters	int
								  Version			1.0
								  Author			Svillen Ranev
								  */
int scanner_init(Buffer * sc_buf) {
	if (b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
											   /* in case the buffer has been read previously  */
	b_rewind(sc_buf);
	b_clear(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
						/*scerrnum = 0;  *//*no need - global ANSI C */
}

/*
Function name	malar_next_token
Purpose:		Runs through the held program in the buffer. It takes every char and runs it through either
an exception, a char recognized by itself, or through the finite state machine.

In Parameters	pBuffer
Out parameters	Token
Version			1.0
Author			Tyler Brady
*/
Token malar_next_token(Buffer * sc_buf)
{
	Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
	short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/
	int accept = NOAS; /* type of state - initially not accepting */

	while (1) { /* endless loop broken by token returns it will generate a warning */

		c = b_getc(sc_buf);


		switch (c) {

		case'.':	//logical operators 
			c = b_getc(sc_buf);
			if (c == 'A') {								//AND
				c = b_getc(sc_buf);
				if (c == 'N') {
					c = b_getc(sc_buf);
					if (c == 'D') {
						c = b_getc(sc_buf);
						if (c == '.') {
							t.code = LOG_OP_T;
							t.attribute.log_op = AND;
							return t;
						}
						b_retract(sc_buf);				//retracting back to first .
						b_retract(sc_buf);
						b_retract(sc_buf);
						b_retract(sc_buf);
						t.attribute.err_lex[0] = '.';
						t.code = ERR_T;
						return t;
					}
					else {
						b_retract(sc_buf);				//retracting back to first .
						b_retract(sc_buf);
						b_retract(sc_buf);
						t.attribute.err_lex[0] = '.';
						t.code = ERR_T;
						return t;
					}
				}
				else {
					b_retract(sc_buf);					//retracting back to first .
					b_retract(sc_buf);
					t.code = ERR_T;
					return t;
				}
			}
			else if (c == 'O') {						//OR
				c = b_getc(sc_buf);
				if (c == 'R') {
					c = b_getc(sc_buf);
					if (c == '.') {
						t.code = LOG_OP_T;
						t.attribute.log_op = OR;
						return t;
					}
					b_retract(sc_buf);					//retracting back to first .
					b_retract(sc_buf);
					b_retract(sc_buf);
					t.attribute.err_lex[0] = '.';
					t.code = ERR_T;
					return t;
				}
				else {
					b_retract(sc_buf);					//retracting back to first .
					b_retract(sc_buf);
					t.attribute.err_lex[0] = '.';
					t.code = ERR_T;
					return t;
				}
			}
			else {
				b_retract(sc_buf);						//retracting back to first .
				t.code = ERR_T;
				t.attribute.err_lex[0] = '.';
				return t;
			}

			//white space cases to ignore
		case' ':
			break;

		case'\v':	//vertical tab
			break;

		case'\t':	//horizontal tab
			break;

		case'\f':	//formfeed
			break;

			//new lines
		case'\n':
			c = b_getc(sc_buf);
			if (c != '\r') {			//checking for case of CR LF WHICH IS LEGAL
				c = b_retract(sc_buf);	//if not that case, we retract back
			}
			++line;
			break;

		case'\r':
			++line;
			break;

			//brackets
		case'(':
			t.code = LPR_T;
			return t;

		case ')':
			t.code = RPR_T;
			return t;

		case'{':
			t.code = LBR_T;
			return t;

		case'}':
			t.code = RBR_T;
			return t;


		case'=':								//assignment operator
			c = b_getc(sc_buf);
			if (c == '=') {						//relational operator
				t.code = REL_OP_T;
				t.attribute.rel_op = 0;
				return t;
			}

			b_retract(sc_buf);					//retract to before we made this mistake
			t.code = ASS_OP_T;
			return t;

			//other relational operators
		case'<':
			c = b_getc(sc_buf);
			if (c == '>') {
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			b_retract(sc_buf);
			t.code = REL_OP_T;
			t.attribute.rel_op = LT;
			return t;

		case'>':
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;

		case';':
			t.code = EOS_T;
			return t;

		case',':
			t.code = COM_T;
			return t;

			//string literal
		case'"':
			b_mark(sc_buf, b_getcoffset(sc_buf));				//sets mark at current getc offset
			lex_buf = b_allocate(25, 10, 'a');					//initialize temporary buffer

			if (lex_buf == NULL) {								//error checking if lex_buf creation failed
				scerrnum = 1;									//set scerrnum to non negative
				strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");//set attribute to RUN TIME ERROR
				t.code = RTE_T;									//set code to RTE_T token
				return t;
			}

			int i;									//iterator
			i = 0;									//setting i to 0
			c = b_getc(sc_buf);						//go ahead, start checking through string

			while (c != '"') {						//while we havent found end of string

				if (c == '\n') {			//if we reach end of line, ++line
					c = b_getc(sc_buf);		//in case its a legal \n\r 
					if (c != '\r') {
						b_retract(sc_buf);
					}
					c = '\n';
					++line;
				}
				if (c == '\r') {
					++line;
				}

				if (c == '\0' || c == 255) {		//error checking for SEOF
					int it;										//iterator
					int length = strlen(lex_buf->cb_head);		//length of the string in the temp buffer
					t.attribute.err_lex[0] = '"';
					if (length > ERR_LEN) {
						for (it = 1; it < ERR_LEN - 3; ++it) {
							t.attribute.err_lex[it] = lex_buf->cb_head[it - 1];
						}
						t.attribute.err_lex[it++] = '.';
						t.attribute.err_lex[it++] = '.';
						t.attribute.err_lex[it++] = '.';
						t.attribute.err_lex[it] = '\0';
						t.code = ERR_T;
						free(lex_buf);
						return t;
					}
					for (it = 1; it < length; ++it) {
						t.attribute.err_lex[it] = lex_buf->cb_head[it - 1];
					}
					t.attribute.err_lex[it] = '\0';
					free(lex_buf);
					t.code = ERR_T;
					return t;
				}//end of error checking for SEOF char

				b_addc(lex_buf, c);								//add character in string to temp buffer
				c = b_getc(sc_buf);								//get next char from string, repeat loop
			}
			//exited the loop, we have a string now in the temp buffer
			b_addc(lex_buf, '\0');								//add a \0 to create the c style string
			int length = strlen(lex_buf->cb_head) + 1;			//length of the string in the temp buffer
			int j;												//iterator
			char tempc;											//character to pass

			for (j = 0; j < lex_buf->addc_offset; ++j) {		//for loop to add to string literal table
				tempc = b_getc(lex_buf);						//setting temporary char
				b_addc(str_LTBL, tempc);						//adding from lex buff
			}
			b_free(lex_buf);												//freeing the lex_buf
			t.attribute.str_offset = (str_LTBL->addc_offset) - length;		//gets the beginning of the string
			t.code = STR_T;
			return t;


			//arithametic cases
		case'-':
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;

		case'+':
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;

		case'*':
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;

		case'/':
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;

		case'#':
			t.code = SCC_OP_T;
			return t;

			//comment
		case'!':
			c = b_getc(sc_buf);
			if (c != '!') {
				t.attribute.err_lex[0] = ('!');
				t.attribute.err_lex[1] = (c);
				while (c != '\n') {		//runs until we get new line
					c = b_getc(sc_buf);
				}
				++line;
				t.code = ERR_T;
				return t;
			}

			while (c != '\n') {		//runs until we get new line
				c = b_getc(sc_buf);
			}
			++line;
			break;

			//terminal cases
		case '\0':
			t.code = SEOF_T;
			return t;

		case 0xFF:
			t.code = SEOF_T;
			return t;

		case EOF:
			t.code = SEOF_T;
			return t;

		default:
			//all alphabet and digits get run through  finite state machine
			if (isalpha(c) != 0 || isdigit(c) != 0) {				//we run it through the state table
				b_mark(sc_buf, b_getcoffset(sc_buf) - 1);				//set the mark at beginning of lexeme
				state = 0;											//set beginning state
				state = get_next_state(state, c, &accept);			//get next state that the lexeme follows

				while (accept == NOAS) {							//while the state is non-accepting
					c = b_getc(sc_buf);								//get the next character
					state = get_next_state(state, c, &accept);		//get next state that the lexeme follows
				}

				if (accept == ASWR) {								//if we need to retract
					b_retract(sc_buf);								//we retract the getc by 1
				}													//otherwise, we can set the lexend

				lexend = b_getcoffset(sc_buf);						//position of last character in lexeme
				lexstart = b_reset(sc_buf);							//set the lexstart to the beginning of the lexeme
				lex_buf = b_allocate(15, 10, 'a');					//initialize temporary buffer

				if (lex_buf == NULL) {								//error checking if lex_buf creation failed
					scerrnum = 1;									//set scerrnum to non negative
					strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");//set attribute to RUN TIME ERROR
					t.code = RTE_T;									//set code to RTE_T token
					return t;										//return t
				}

				char lexc;
				while (lexstart < lexend) {							//while lexstart is less then lexend
					lexc = b_getc(sc_buf);							//get char from sc_buf
					b_addc(lex_buf, lexc);							//add the char
					++lexstart;										//then go up one more char
				}													//when we break the loop, we have our item

				b_addc(lex_buf, '\0');								//complete the c style string
				t = aa_table[state](lex_buf->cb_head);				//call the function table
				b_free(lex_buf);									//now done with lex_buff, must free
				b_mark(sc_buf, lexend);								//set the mark at end of lexeme
				b_reset(sc_buf);									//set getc back to where it was
				return t;
			}
			else {													//it is an illegal character
				t.code = ERR_T;
				t.attribute.err_lex[0] = c;
				t.attribute.err_lex[1] = '\0';
				return t;
			}
		}
	}
}



/*
Function name	get_next_state
Purpose:		State starts at zero, accept starts at NOAS, and char is the character we input to go through
the state table until we have an accepting state, a token.

In Parameters	int, char, int*
Out parameters	int
Version			1.0
Author			Svillen Ranev
*/
int get_next_state(int state, char c, int *accept) {
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:

	Assertion failed: test, file filename, line linenum

	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	*accept = as_table[next];
	return next;
}


/*
Function name	char_class
Purpose:		The char inputs to this function.
The function then shows which column the char is on in the state table.

In Parameters	char
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int char_class(char c)
{
	int val;														//the value to return

	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {			//all alpha 
		if (c == 'x') {												//for use with identifying hex
			val = 1;
			return val;
		}
		if (c == 'A' || c == 'B' || c == 'C' ||
			c == 'D' || c == 'E' || c == 'F') {						//if here, then its a hex
			val = 0;
			return val;
		}
		val = 2;													//if those special cases didnt get returned, we return this
	}
	else if (c >= '1' && c <= '9') {								//digits
		val = 4;
	}
	else if (c == '0') {											//0
		val = 3;
	}
	else if (c == '.') {											//period
		val = 5;
	}
	else if (c == '$') {											//$ symbol
		val = 6;
	}
	else {															//other	
		val = 7;
	}

	return val;
}
 
/*
Function name	aa_func02
Purpose:		Creates an AVID_T token from the char[] input.
Error checks along the way, as well, makes sure that the char[] is not
a keyword. Depending on those outcomes, it can additionally return a ERR_T token
or a KW_T token. This function is a retracting state.

In Parameters	char[]
Out parameters	Token
Version			1.0
Author			Tyler Brady
*/
Token aa_func02(char lexeme[]) {
	Token t;
	if (isdigit(lexeme[0]) != 0) {					//checks if first char is a digit, if it is
		if (strlen(lexeme) > ERR_LEN) {				//if not, we check if the avid can fit in err_len
			int i;									//iterator
			for (i = 0; i < ERR_LEN - 3; ++i) {		//loops through copying in
				t.attribute.err_lex[i] = lexeme[i];
			}
			t.attribute.err_lex[i++] = '.';			//adds the .'s
			t.attribute.err_lex[i++] = '.';
			t.attribute.err_lex[i++] = '.';
			t.attribute.err_lex[i] = '\0';			//creates the c style string
		}
		else {										//otherwise we can copy it in
			strcpy(t.attribute.err_lex, lexeme);
			t.attribute.err_lex[strlen(lexeme)] = '\0';
		}
		t.code = ERR_T;
		return t;									//set and return
	}

	//checking for keyword
	int key;							//for return value of iskeyword function
	key = iskeyword(lexeme);			//calls with the string passed in function
	if (key != -1) {					//if the function doesnt return -1
		t.code = KW_T;					//assign proper values
		t.attribute.kwt_idx = key;
		return t;
	}

	int length;							//will hold length of lexeme
	int i;								//iterator
	length = strlen(lexeme);
	i = 0;

	if (length > VID_LEN) {							//if the length is greater than the limit of VID_LEN
		while (i < VID_LEN) {						//loop through to get the first VID_LEN amount
			t.attribute.vid_lex[i] = lexeme[i];
			++i;
		}
	}
	else {											//otherwise the length is less than the limit
		while (i < length) {						//loop through and copy into vid_lex
			t.attribute.vid_lex[i] = lexeme[i];
			++i;
		}
	}

	t.attribute.vid_lex[i] = '\0';					//create the c style string
	t.code = AVID_T;
	return t;
}


/*
Function name	aa_func03
Purpose:		Creates an SVID_T token from the char[] input.
Error checks along the way. This is a non-retracting state.

In Parameters	char[]
Out parameters	Token
Version			1.0
Author			Tyler Brady
*/
Token aa_func03(char lexeme[]) {

	Token t;										//token to return
	int length;										//length of lexeme
	int i;											//iterator
	length = strlen(lexeme);						//assign string length
	i = 0;

	if (length > VID_LEN) {							//if the length of the SVID is greater than VID_LEN
		while (i < VID_LEN - 1) {						//loop through to the first VID_LEN-1 amount
			t.attribute.vid_lex[i] = lexeme[i];
			++i;
		}
		t.attribute.vid_lex[i++] = '$';				//then add the $ signifier
	}
	else {
		while (i < length) {						//otherwise it fits, loop through
			t.attribute.vid_lex[i] = lexeme[i];
			++i;
		}
	}

	t.attribute.vid_lex[i] = '\0';					//create the c style string
	t.code = SVID_T;
	return t;
}

/*
Function name	aa_func08
Purpose:		Takes in char[], and creates a token for float values. Uses atof() to turn
the char[] into a float. Error checks the outcome of that conversion
to make sure it fits within the bounds of the language. It is a retracting state.

In Parameters	char[]
Out parameters	Token
Version			1.0
Author			Tyler Brady
*/
Token aa_func08(char lexeme[]) {
	Token t;												//token to return
	double fpl;												//double to hold the float value of lexeme
	fpl = atof(lexeme);										//turns lexeme into double
	if ((fpl > FLT_MAX || fpl < FLT_MIN) && fpl != 0) {		//checks if outside range
		if (strlen(lexeme) > ERR_LEN) {						//error checking to see if lexeme is greater than ERR_LEN
			int i;											//iterator
			for (i = 0; i < ERR_LEN - 3; ++i) {				//loop through copying the chars up to ERR_LEN - 3
				t.attribute.err_lex[i] = lexeme[i];
			}
			t.attribute.err_lex[i++] = '.';					//we reserved the last 4 spaces to add the stylized .'s
			t.attribute.err_lex[i++] = '.';
			t.attribute.err_lex[i++] = '.';
			t.attribute.err_lex[i] = '\0';					//and to create the c style string
		}
		else {												//otherwise we can copy it in
			strcpy(t.attribute.err_lex, lexeme);
			t.attribute.err_lex[strlen(lexeme)] = '\0';

		}

		t.code = ERR_T;
		return t;
	}

	t.attribute.flt_value = fpl;
	t.code = FPL_T;
	return t;
}


/*
Function name	aa_func05
Purpose:		THis function returns a DIL_T token, for decimal constants.
Uses atof() to convert the char[] to a double.
We then check its bounds, and down cast it to an int. If errors are found, returns
an ERR_T token. It is a retracting state.

In Parameters	char[]
Out parameters	Token
Version			1.0
Author			Tyler Brady
*/
Token aa_func05(char lexeme[]) {
	Token t;
	double i;
	i = atof(lexeme);		//holding int in a bigger data type in case of overflow

	if (i > SHRT_MAX || i < SHRT_MIN) {					//if int is outside range
		if (strlen(lexeme) > ERR_LEN) {					//if the lexeme is greater than ERR_LEN
			int it;										//iterator
			for (it = 0; it < ERR_LEN - 3; ++it) {		//loop through 
				t.attribute.err_lex[it] = lexeme[it];
			}
			t.attribute.err_lex[it++] = '.';
			t.attribute.err_lex[it++] = '.';
			t.attribute.err_lex[it++] = '.';
			t.attribute.err_lex[it] = '\0';				//create c style string

		}
		else {											//otherwise we can copy it in
			strcpy(t.attribute.err_lex, lexeme);
			t.attribute.err_lex[strlen(lexeme)] = '\0';
		}

		t.code = ERR_T;
		return t;
	}

	t.attribute.int_value = (int)i;
	t.code = INL_T;
	return t;
}

/*
Function name	aa_func11
Purpose:		THis function returns a HIL_T token, for hexadecimal constants.
Uses atolh() to convert the char[] to a long.
We then check its bounds, and down cast it to an int. If errors are found, returns
an ERR_T token. Is is a retracting state.

In Parameters	char[]
Out parameters	Token
Version			1.0
Author			Tyler Brady
*/
Token aa_func11(char lexeme[]) {
	Token t;										//token to return
	long hex;										//decimal representation of hex

	hex = atolh(lexeme);							//converts string hex to long
	if (hex > SHRT_MAX || hex < SHRT_MIN) {			//if hex is outside range
		if (strlen(lexeme) > ERR_LEN) {				//if lexeme is larger than err_len
			int i;									//iterator
			for (i = 0; i < ERR_LEN - 3; ++i) {		//loop through, copying to ERR_LEN-3
				t.attribute.err_lex[i] = lexeme[i];
			}
			t.attribute.err_lex[i++] = '.';
			t.attribute.err_lex[i++] = '.';
			t.attribute.err_lex[i++] = '.';
			t.attribute.err_lex[i] = '\0';
		}
		else {											//otherwise we can copy it in
			strcpy(t.attribute.err_lex, lexeme);
			t.attribute.err_lex[strlen(lexeme)] = '\0';
		}

		t.code = ERR_T;
		return t;
	}

	t.attribute.int_value = (int)hex;
	t.code = INL_T;
	return t;
}

/*
Function name	aa_func12
Purpose:		THis function returns a ERR_T token, for any error.
It is non-retracting state.

In Parameters	char[]
Out parameters	Token
Version			1.0
Author			Tyler Brady
*/
Token aa_func12(char lexeme[]) {
	Token t;									//token to return
	if (strlen(lexeme) > ERR_LEN) {				//if lexeme is larger than err_len
		int i;									//iterator
		for (i = 0; i < ERR_LEN - 3; ++i) {		//loop through, copying to ERR_LEN-3
			t.attribute.err_lex[i] = lexeme[i];
		}
		t.attribute.err_lex[i++] = '.';
		t.attribute.err_lex[i++] = '.';
		t.attribute.err_lex[i++] = '.';
		t.attribute.err_lex[i] = '\0';
	}
	else {										//otherwise we can copy it in
		strcpy(t.attribute.err_lex, lexeme);
		t.attribute.err_lex[strlen(lexeme)] = '\0';
	}

	t.code = ERR_T;
	return t;
}



//THE FUNCTION CONVERTS AN ASCII STRING
//REPRESENTING AN HEXADECIMAL INTEGER CONSTANT TO INTEGER VALUE
long atolh(char * lexeme) {
	long hex;								//soon to be hex value
	hex = (long)strtol(lexeme, NULL, 0);	//converts hex value in string to a long
	return hex;								//return the converted (long)hex
}


int iskeyword(char* kw_lexeme) {

	if (kw_lexeme == NULL) {
		return -1;
	}

	int i;	//iterator

	for (i = 0; i < KWT_SIZE; ++i) {					//loops through the KW_TABLE
		if (strcmp(kw_table[i], kw_lexeme) == 0) {		//if string is equal to an item in the kw table
			return i;									//we return the keyword's place
		}
	}

	return -1;											//otherwise we return -1
}




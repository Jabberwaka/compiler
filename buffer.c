/**********************************************
/	Author					Tyler Brady
/	Student Number			040 852 457
/	Assignment Number		1
/	File Name				buffer.c
/	CST8152_010				Compilers
/	Lab Section				013
/	Professor				Svillen Ranev
/	Assignment Due			Feb 9th 2018
/	Assignment Submitted	Feb 9th 2018
/	Purpose					To create a buffer and associated functions.
************************************************/

#include "buffer.h"
 

/*
Function name	b_allocate
Purpose:		to create and initialize the buffer structure according to the specified parameters

In Parameters	short, char, char
Out parameters	returns a pointer to the created buffer
Version			1.0
Author			Tyler Brady
*/
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode) {

	//checks to make sure init_capacity is a number that can be assigned to the buffer
	if (init_capacity < 0 || init_capacity > SHRT_MAX - 1) {
		//printf("\nInitialized Capacity is outside of range, cannot initialize buffer\n");
		return NULL;
	}


	pBuffer buffer;	//the created buffer pointer
	buffer = calloc(1, sizeof(Buffer)); //creates a single buffer structure
	if (!buffer) {	//if the calloc fails
		return NULL;
	}

	/*The following switch case checks for any correct case
	//in which the buffer's parameters may be set.
	//If a correct case is not found, the previously malloc'd
	//buffer is freed and a void pointer is returned.
	//Otherwise, the parameters are set*/
	switch (o_mode) {
	case 'f':
		if (init_capacity == 0) { //checks if the capacity works for the fixed mode
			return NULL;
		}
		buffer->mode = 0;
		buffer->inc_factor = 0;
		break;

	case 'a':
		if (inc_factor != 0) { //checks that the inc factor isn't 0
			buffer->mode = 1;
			buffer->inc_factor = inc_factor;
		}
		else { //if the inc_factor is 0, 
			buffer->mode = 0;
			buffer->inc_factor = 0;
		}
		break;

	case 'm':
		if (inc_factor > 0 && inc_factor <= 100) { //makes sure inc_factor is within bounds for multiplicative mode
			buffer->mode = -1;
			buffer->inc_factor = inc_factor;
		}
		else if (inc_factor == 0){ //if the inc_factor is 0, 
			buffer->mode = 0;
			buffer->inc_factor = 0;
		}
		else { // if the inc_factor is above 100 however, the buffer will not be created
			return NULL;
		}
		break;


	default:
		free(buffer);
		return NULL;
	}

	buffer->cb_head = malloc(sizeof(char)*(init_capacity)); //since it passed the above checks, we can malloc for capacity
	if (!buffer->cb_head) { // if it was not malloc'd properly
		free(buffer);		// free the created buffer
		return NULL;
	}

	buffer->capacity = init_capacity;
	return buffer;
}

/*
Function name	b_addc
Purpose:		to add a character to the buffer. this function handles	
				what happenes if the buffer is full and needs to expand.

In Parameters	pBuffer, char
Out parameters	returns a pointer to the created buffer
Version			1.0
Author			Tyler Brady
*/
pBuffer b_addc(pBuffer const pBD, char symbol) {

	if (pBD == NULL) {
		return NULL;
	}

	//sets r flag, to be checked again later
	pBD->r_flag = 0;

	//if the last position is further up or equal to addc, there is space to add a char
	if (pBD->capacity - 1 >= pBD->addc_offset) {
		pBD->cb_head[pBD->addc_offset] = symbol;
		++pBD->addc_offset;
		return pBD;
	}

	if (pBD->mode == 0) {
		return NULL;
	}

	if (pBD->mode == 1) {
		void * mem = pBD->cb_head; //to later check if the array moves when realloc'd
		char * placeHold; //to protect the data, we're going to make a placeholder for the buffer
		short cap = pBD->capacity; //in case of problems, we may need to re-set the capacity to its original value
		int i; //iterator for loop

		//before any adjustment, we need to see if capacity is the max value
		if (pBD->capacity + (unsigned char)pBD->inc_factor > SHRT_MAX-1) {
			return NULL;
		}

		//creating a placeholder for the buffer's data in case of any failure to realloc
		placeHold = malloc(sizeof(char)*pBD->capacity);
		for (i = 0; i < pBD->capacity; ++i) {
			placeHold[i] = pBD->cb_head[i];
		}

		//if the capacity was zero
		if (pBD->capacity == 0) {
			pBD->cb_head = realloc(pBD->cb_head, (sizeof(char)*(unsigned char)(pBD->inc_factor)));
			pBD->capacity = (unsigned char)(pBD->inc_factor);
		}
		//otherwise, we can adjust the char array size
		else {
			pBD->cb_head = realloc(pBD->cb_head, (sizeof(char)*(pBD->capacity + (unsigned char)pBD->inc_factor)));
			pBD->capacity += (unsigned char)pBD->inc_factor;
		}

		//if the realloc fails, we've saved the data, and place it back into the buffer
		if (!pBD->cb_head) {
			pBD->cb_head = placeHold;
			pBD->capacity = cap;
			return NULL;
		}
		//if it was realloced succesfully, we can free the safegaurded buffer copy
		free(placeHold);
		pBD->cb_head[pBD->addc_offset] = symbol;
		++pBD->addc_offset;

		//checks to see if the memory has moved
		if (mem != pBD->cb_head) {
			pBD->r_flag = SET_R_FLAG;
		}

		return pBD;
	}

	if (pBD->mode == -1) {
		void * mem = pBD->cb_head; //to later check if the array moves when realloc'd
		char * placeHold = 0; //to protect the data, we're going to make a placeholder for the buffer
		short cap = pBD->capacity; //in case of problems, we may need to re-set the capacity to its original value
		int i; //iterator for loop


		/*the following is a legible re-creation of what the statement below is doing
		int available = ((SHRT_MAX ) - pBD->capacity);
		double division = ((double)pBD->inc_factor / 100);
		double newInc = available * division;
		int newCap = pBD->capacity + newInc;*/

		int newCap = pBD->capacity + ((SHRT_MAX -1)-pBD->capacity)*((double)pBD->inc_factor / 100);

		if (newCap >= SHRT_MAX - 1 || newCap == pBD->capacity) { //if already as big as it can be
			return NULL;
		}

		//a placeholder for the data in the buffer in case of errors
		placeHold = malloc(sizeof(char)*pBD->capacity);
		for (i = 0; i < pBD->capacity; ++i) {
			placeHold[i] = pBD->cb_head[i];
		}

		//if the capacity was zero
		if (pBD->capacity == 0) {
			pBD->cb_head = realloc(pBD->cb_head, (sizeof(char)*newCap));
			pBD->capacity = newCap;
		}
		else {
			//otherwise, we can adjust the char array size
			pBD->cb_head = realloc(pBD->cb_head, (sizeof(char)*(newCap)));
			pBD->capacity = (short)newCap;
		}

		//if the realloc fails, we've saved the data, and place it back into the buffer
		if (!pBD->cb_head) {
			pBD->cb_head = placeHold;
			pBD->capacity = cap;
			return NULL;
		}

		//if it was realloced succesfully, we can free the safegaurded buffer copy
		free(placeHold);
		pBD->cb_head[pBD->addc_offset] = symbol;
		++pBD->addc_offset;

		//checks to see if the buffer moved position
		if (mem != pBD->cb_head) {
			pBD->r_flag = SET_R_FLAG;
		}

		return pBD;
	}

	return NULL;
}
/*
Function name	b_clear
Purpose:		re-sets the appropriate parameters to make buffer appear empty

In Parameters	pBuffer
Out parameters	returns an int for pass or fail
Version			1.0
Author			Tyler Brady
*/
int b_clear(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	pBD->addc_offset = 0;
	pBD->eob = 0;
	return F_PASS0;
}
/*
Function name	b_free
Purpose:		frees memory associated with the buffer

In Parameters	pBuffer
Out parameters	void
Version			1.0
Author			Tyler Brady
*/
void b_free(Buffer * const pBD) {

	if (!pBD) { //if the pointer passed is null
		return;
	}

	free(pBD->cb_head);
	free(pBD);
}
/*
Function name	b_isfull
Purpose:		checks wether the buffer is full

In Parameters	pBuffer
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int b_isfull(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	if (!pBD->cb_head) {	//if the function is not initialized
		return RT_FAIL1;
	}

	if (pBD->addc_offset == pBD->capacity) {	//if the buffer is full it returns 1
		return F_PASS1;
	}

	return F_PASS0;
}
/*
Function name	b_limit
Purpose:		returns the limit of the buffer struct

In Parameters	pBuffer
Out parameters	short
Version			1.0
Author			Tyler Brady
*/
short b_limit(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}
		//just return offset
	return (sizeof(char)*(pBD->addc_offset));
}
/*
Function name	b_capacity
Purpose:		returns the capacity of the buffer

In Parameters	pBuffer
Out parameters	short
Version			1.0
Author			Tyler Brady
*/
short b_capacity(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	return pBD->capacity;
}
/*
Function name	b_mark
Purpose:		sets markc_offset and returns markc_offset

In Parameters	pBuffer, short
Out parameters	short
Version			1.0
Author			Tyler Brady
*/
short b_mark(pBuffer const pBD, short mark) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	if (mark >= 0 && mark <= pBD->addc_offset) { //makes sure mark is within the proper constrains 
		pBD->markc_offset = mark;
		return pBD->markc_offset;
	}

	return RT_FAIL1;
}
/*
Function name	b_mode
Purpose:		returns the mode of the buffer

In Parameters	pBuffer
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int b_mode(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL2;
	}

	return pBD->mode;	
}
/*
Function name	b_incfactor
Purpose:		returns non-negative value of inc_factor

In Parameters	pBuffer
Out parameters	size_t
Version			1.0
Author			Tyler Brady
*/
size_t b_incfactor(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return (size_t)RT_FAIL1;
	}

	//stores the signed char into an unsigned datatype
	size_t inc = (unsigned char)pBD->inc_factor;
	return inc;
}
/*
Function name	b_load
Purpose:		reads through the FILE, and takes character by character into
				the buffer. when feof is detected, it exits.

In Parameters	FILE *, pBuffer
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int b_load(FILE * const fi, Buffer * const pBD) {
	if (!pBD || !fi) { //if the pointer passed is null
		return RT_FAIL1;
	}

	int i = 0;	//iterator
	char c;		//holds the char to be input

	while (1) {
		c = (char)fgetc(fi);	//character is recieved

		if (feof(fi)) {
			break;
		}

		if (b_addc(pBD, c) != NULL) { //if the char goes in do nothing
			++i;
		}
		else { //if it doesn't go in send out an error
			return LOAD_FAIL;
		}
	}
	//returns number of characters added to buffer
	return  i;
}
/*
Function name	b_isempty
Purpose:		tells you if the buffer is empty

In Parameters	pBuffer
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int b_isempty(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	if (pBD->addc_offset == 0) { //if the buffer is empty returns 1
		return F_PASS1;
	}

	return F_PASS0;
}
/*
Function name	b_eob
Purpose:		returns the end of buffer flag

In Parameters	pBuffer
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int b_eob(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	return pBD->eob;
}
/*
Function name	b_getc
Purpose:		returns the character found at getc_offset and checks if getc_offset
				is at the end of the buffer, if it is, it sets the eob flag.

In Parameters	pBuffer
Out parameters	char
Version			1.0
Author			Tyler Brady
*/
char b_getc(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL2;
	}

	short getc;	//stores the getc_offset that we will return upon completion
	getc = pBD->getc_offset;

	//when getc and addc offset = one another, it sets the eob to 1 and returns -1
	if (pBD->getc_offset == pBD->addc_offset) {
		pBD->eob = 1;
		return F_PASSN;
	}
	else {
		pBD->eob = 0;
	}

	++pBD->getc_offset;
	return pBD->cb_head[getc];
}
/*
Function name	b_print
Purpose:		prints the buffers contents to the screen

In Parameters	pBuffer
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int b_print(Buffer * const pBD) {
	int i = 0; //counter and iterator
	char c;		//holds the character from b_getc

	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	if (pBD->addc_offset == 0) {	//if addc_offset is 0, the buffer is empty
		printf("Empty buffer.\n");
		return F_PASS0;
	}


	 while (b_eob(pBD) == 0) {	//while we havent reached the end of the buffer
		c = b_getc(pBD);		//need to make sure the passed char isn't the eob
		if (b_eob(pBD) == 0) { //so we store it in c, check it, and print it to the screen
			printf("%c", c);
			++i;
		}	
	}

	printf("\n");
	return i;
}
/*
Function name	b_compact
Purpose:		re-sizes the buffer to its current limit + 1

In Parameters	pBuffer, char
Out parameters	pBuffer
Version			1.0
Author			Tyler Brady
*/
Buffer * b_compact(Buffer * const pBD, char symbol) {
	if (!pBD) { //if the pointer passed is null
		return NULL;
	}

	if (pBD->addc_offset == SHRT_MAX) { //if buffer is already at its seams
		return NULL;
	}

	char * placeHold;	//to protect the data, we're going to make a placeholder for the buffer
	int i;				//iterator for loop

	//copy the buffer in case of errors, can get our info back
	placeHold = malloc(sizeof(char)*pBD->addc_offset);
	for (i = 0; i < pBD->addc_offset; ++i) {
		placeHold[i] = pBD->cb_head[i];
	}

	//reallocs to the calculated size
	pBD->cb_head = realloc(pBD->cb_head, sizeof(char)*(pBD->addc_offset + 1));
	//checks to see that the realloc was succesful, if not we place placeholder back into buffer
	if (!pBD->cb_head) {
		pBD->cb_head = placeHold;
		return NULL;
	}
	else {	//otherwise we free the placehold
		free(placeHold);
	}

	//adjust the buffer's parameters and add char
	pBD->r_flag = SET_R_FLAG;
	pBD->capacity = pBD->addc_offset + 1;
	pBD->cb_head[pBD->addc_offset] = symbol;
	++pBD->addc_offset;

	return pBD;
}
/*
Function name	b_rflag
Purpose:		returns the rflag of the buffer

In Parameters	pBuffer
Out parameters	char
Version			1.0
Author			Tyler Brady
*/
char b_rflag(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	return pBD->r_flag;
}
/*
Function name	b_capacity
Purpose:		rewinds the getc_offset by 1, and then returns the getc_offset

In Parameters	pBuffer
Out parameters	short
Version			1.0
Author			Tyler Brady
*/
short b_retract(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	if (pBD->getc_offset == 0) {	//we cannot retract when getc_offset is at 0
		return RT_FAIL1;
	}

	--pBD->getc_offset;	
	return pBD->getc_offset;
}
/*
Function name	b_reset
Purpose:		resets the buffer's markc and getc offsets to re-read through	
				the buffer.

In Parameters	pBuffer
Out parameters	short
Version			1.0
Author			Tyler Brady
*/
short b_reset(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	pBD->getc_offset = pBD->markc_offset;
	return pBD->getc_offset;
}
/*
Function name	b_getcoffset
Purpose:		returns getc_offset of buffer

In Parameters	pBuffer
Out parameters	short
Version			1.0
Author			Tyler Brady
*/
short b_getcoffset(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}

	return pBD->getc_offset;
}
/*
Function name	b_capacity
Purpose:		rewinds the parameters associated with reading through the buffer to 0.

In Parameters	pBuffer
Out parameters	int
Version			1.0
Author			Tyler Brady
*/
int b_rewind(Buffer * const pBD) {
	if (!pBD) { //if the pointer passed is null
		return RT_FAIL1;
	}	

	//adjusts appropriate parameters
	pBD->eob = 0;
	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	return F_PASS0;
}
/*
Function name	b_location
Purpose:		returns the char address at loc_offset

In Parameters	pBuffer, short
Out parameters	char *
Version			1.0
Author			Tyler Brady
*/
char * b_location(Buffer * const pBD, short loc_offset) {
	if (!pBD) { //if the pointer passed is null
		return NULL;
	}
	//if loc_offset is outside the limit or negative, returns NULL
	//this might need to include 0 for loc_offset
	if (loc_offset >= pBD->addc_offset || loc_offset < 0) {
		return NULL;
	}

	return &pBD->cb_head[loc_offset];
}



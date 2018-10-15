/**********************************************
/	Author					Tyler Brady
/	Student Number			040 852 457
/	Assignment Number		3
/	File Name				parser.h
/	CST8152_010				Compilers
/	Lab Section				013
/	Professor				Svillen Ranev
/	Assignment Due			April 20th 2018
/	Assignment Submitted	April 21st 2018
/	Purpose					PARSER, uses the transformed grammar to make sure that all tokens
							are in the correct order to be grammatically correct.
************************************************/

#pragma once
//parser.h

#include "token.h"
#include "buffer.h"
#include "symboltable.h"

extern char* kw_table[];

#define NO_ATTR -1
#define ELSE 0
#define FALSE 1
#define IF 2
#define PLATYPUS 3
#define READ 4
#define REPEAT 5
#define THEN 6
#define TRUE 7
#define WHILE 8
#define WRITE 9

void match(int, int);
void parser(Buffer*);
void syn_eh(int);
void syn_printe(void);
void gen_incode(char*);
void program(void);
void opt_statements(void);
void statements(void);
void statement(void);
void statements_prime(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void pre_condition(void);
void input_statement(void);
void variable_list(void);
void variable_identifier(void);
void variable_list_prime(void);
void output_statement(void);
void opt_variable_list(void);
void output_list(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expression_prime(void);
void primary_arithmetic_expression(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expression_prime(void);
void unary_operators(void);
void mult_operators(void);
void string_expression(void);
void primary_string_expression(void);
void string_expression_prime(void);
void conditional_expression(void);
void logical_or_expression(void);
void logical_and_expression(void);
void logical_or_expression_prime(void);
void logical_and_expression_prime(void);
void relational_expression(void);
void primary_a_relational(void);
void primary_s_relational(void);
void comparison_operators(void);


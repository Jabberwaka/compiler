/**********************************************
/	Author					Tyler Brady
/	Student Number			040 852 457
/	Assignment Number		3
/	File Name				parser.c
/	CST8152_010				Compilers
/	Lab Section				013
/	Professor				Svillen Ranev
/	Assignment Due			March 23rd 2018
/	Assignment Submitted	March 23rd 2018
/	Purpose					PARSER, uses the transformed grammar to make sure that all tokens
							are in the correct order to be grammatically correct.
************************************************/
#include "parser.h"

static Buffer* sc_buf;
static Token lookahead;
extern Buffer* str_LTBL;
extern Token malar_next_token(Buffer* sc_buf);
extern int line;
int synerrno = 0;


/*
Function name	parser
Purpose:		Starts the parsing process

In Parameters	Buffer*
Out parameters	Void
Version			1.0
Author			Svillen Ranev
*/
void parser(Buffer * in_buf) {
	sc_buf = in_buf;
	lookahead = malar_next_token(sc_buf);
	program(); match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}


/*
Function name	match
Purpose:		Makes sure the current token matches the token code/attribute in parameters

In Parameters	int, int
Out parameters	Void
Version			1.0
Author			Tyler Brady
*/
void match(int pr_token_code, int pr_token_attribute) {

	if (lookahead.code != pr_token_code) {
		syn_eh(pr_token_code);
		return;
	}

	switch (pr_token_code) {
	case KW_T:
		if (lookahead.attribute.kwt_idx == pr_token_attribute) {
			break;
		}
		else {
			syn_eh(pr_token_code);
			return;
		}
	case ART_OP_T:
		if (lookahead.attribute.arr_op == pr_token_attribute) {
			break;
		}
		else {
			syn_eh(pr_token_code);
			return;
		}
	case REL_OP_T:
		if (lookahead.attribute.rel_op == pr_token_attribute) {
			break;
		}
		else {
			syn_eh(pr_token_code);
			return;
		}
	case LOG_OP_T:
		if (lookahead.attribute.log_op == pr_token_attribute) {
			break;
		}
		else {
			syn_eh(pr_token_code);
			return;
		}
	default:
		break;
	}

	if (lookahead.code == SEOF_T) {
		return;
	}
	else {
		lookahead = malar_next_token(sc_buf);
	}

	if (lookahead.code == ERR_T) {
		syn_printe();
		lookahead = malar_next_token(sc_buf);
		++synerrno;
		return;
	}
}

/*
Function name	syn_eh
Purpose:		When errors are found, it advances to the the sync_token_code
				to sync back up to what the parser understands to be not an error.

In Parameters	int
Out parameters	Void
Version			1.0
Author			Tyler Brady
*/
void syn_eh(int sync_token_code) {
	syn_printe();
	++synerrno;
	while (lookahead.code != sync_token_code) {

		if ((lookahead.code == SEOF_T) && (sync_token_code != SEOF_T)) {
			exit(synerrno);
		}
		lookahead = malar_next_token(sc_buf);
	}

	if ((lookahead.code == SEOF_T) && (sync_token_code == SEOF_T)) {
		return;
	}

	lookahead = malar_next_token(sc_buf);
}

/* error printing function for Assignment 3 (Parser), W18 */
void syn_printe() {
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("NA\n");
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T:/* SVID_T    3  String Variable identifier token */
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		printf("%s\n", b_location(str_LTBL, t.attribute.str_offset));
		break;

	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n");
		break;

	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;

	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n");
		break;

	case KW_T: /*     16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;

	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/

 /*
 Function name	gen_incode
 Purpose:		Used to show when a statement or expression is parsed

 In Parameters	char*
 Out parameters	Void
 Version		1.0
 Author			Tyler Brady
 */
void gen_incode(char * direction) {
	printf("%s\n", direction);
}

/*
first set = PLATYPUS
	<program>->
		PLATYPUS { <opt_statements> }
*/
void program() {
	match(KW_T, PLATYPUS);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}


/*
first set = first(statements), e
	<opt_statements>->
		<statements> | e
*/
void opt_statements() {
	switch (lookahead.code) {
		case AVID_T:
		case SVID_T:
			statements();
			break;
		case KW_T:      //when the keywords isnt in the first set of keywords
			if ((lookahead.attribute.kwt_idx != ELSE) && (lookahead.attribute.kwt_idx != PLATYPUS)
				&& (lookahead.attribute.kwt_idx != REPEAT) && (lookahead.attribute.kwt_idx != THEN)
				&& (lookahead.attribute.kwt_idx != TRUE) && (lookahead.attribute.kwt_idx != FALSE)) {
				statements();
				break;
			}
		default:
			gen_incode("PLATY: Opt_statements parsed");
	}
}

/*
first set = first(statement)
	<statements>->
		<statement><statements_prime>
*/
void statements() {
	statement();
	statements_prime();
}

/*
first set = AVID_T, SVID_T, IF, WHILE, READ, WRITE
	<statement>->
		<assignment_statement> | <selection_statement> | <iteration_statement> | <input_statement> | <output_statement>
*/
void statement() {

	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		assignment_statement();
		break;
	case KW_T:
		if (lookahead.attribute.kwt_idx == IF) {
			selection_statement();
			break;
		}
		if (lookahead.attribute.kwt_idx == WHILE) {
			iteration_statement();
			break;
		}
		if (lookahead.attribute.kwt_idx == READ) {
			input_statement();
			break;
		}
		if (lookahead.attribute.kwt_idx == WRITE) {
			output_statement();
			break;
		}
	default:
		syn_printe();
	}
}

/*
first set = first(statement), e
	<statement><statements_prime> | e
*/
void statements_prime() {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		statement();
		statements_prime();
		break;
	case KW_T:
		if (lookahead.attribute.kwt_idx == IF) {
			statement();
			statements_prime();
			break;
		}
		if (lookahead.attribute.kwt_idx == WHILE) {
			statement();
			statements_prime();
			break;
		}
		if (lookahead.attribute.kwt_idx == READ) {
			statement();
			statements_prime();
			break;
		}
		if (lookahead.attribute.kwt_idx == WRITE) {
			statement();
			statements_prime();
			break;
		}
	default:
		break;  //empty
	}
}

/*
first set = first(assignment_expression)
	<assignment_statement>->
		<assignment_expression> ;
*/
void assignment_statement() {
		assignment_expression();
		match(EOS_T, NO_ATTR);
		gen_incode("PLATY: Assignment statement parsed");
}

/*
first set = AVID_T, SVID_T
	<assignment_expression>->
		AVID_T = <arithmetic_expression> | SVID_T = <string_expression>
*/
void assignment_expression() {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		arithmetic_expression();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed");
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		string_expression();
		gen_incode("PLATY: Assignment expression (string) parsed");
		break;
	default:
		syn_printe();
	}
}


/*
first set = first(primary_string_expression)
	<string_expression>->
		<primary_string_expression><string_expression_prime>
*/
void string_expression() {
	primary_string_expression();
	string_expression_prime();
	gen_incode("PLATY: String expression parsed");
}

/*
first set = STR_T, SVID_T
	<primary_string_expression>->
		STR_T | SVID_T
*/
void primary_string_expression() {
	switch (lookahead.code) {
	case STR_T:
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Primary string expression parsed");
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		gen_incode("PLATY: Primary string expression parsed");
		break;
	default:    //errror
		syn_printe();
	}
}

/*
first set = #, e
	<string_expression_prime>->
		, <primary_string_expression><string_expression_prime> | e
*/
void string_expression_prime() {
	switch (lookahead.code) {
	case SCC_OP_T:
		match(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		string_expression_prime();
		break;
	default:    //empty
		break;
	}
}

/*
first set = IF
	<selection_statement>->
		IF <pre-condition> ( <conditional_expression> ) THEN { <opt_statements> } ELSE { <opt_statements> } ;
*/
void selection_statement() {
	match(KW_T, IF);
	pre_condition();
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, THEN); //
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(KW_T, ELSE);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Selection statement parsed");
}

/*
first set = TRUE, FALSE
	<pre_condition>->
		TRUE | FALSE
*/
void pre_condition() {
	switch (lookahead.attribute.kwt_idx) {
		case TRUE:
			match(KW_T, TRUE);
			break;
		case FALSE:
			match(KW_T, FALSE);
			break;
		default:
			syn_printe();
			break;
	}
}

/*
first set = WHILE
	<iteration_statement>->
		WHILE <pre_condition> ( <conditional_expression> ) REPEAT { <statements> } ;
*/
void iteration_statement() {
	match(KW_T, WHILE);
	pre_condition();
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, REPEAT);
	match(LBR_T, NO_ATTR);
	statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Iteration statement parsed");
}

/*
first set = READ
	<input_statement>->
		READ ( <variable_list> ) ;
*/
void input_statement() {
	match(KW_T, READ);
	match(LPR_T, NO_ATTR);
	variable_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Input statement parsed");
}

/*
first set = first(variable_identifier)
	<variable_list>->
		<variable_identifier><variable_list_prime>
*/
void variable_list() {
	variable_identifier();
	variable_list_prime();
	gen_incode("PLATY: Variable list parsed");
}

/*
first set = AVID_T, SVID_T
	<variable_identifier>->
		AVID_T | SVID_T
*/
void variable_identifier() {

	switch (lookahead.code) {
		case AVID_T:
			match(AVID_T, NO_ATTR);
			break;
		case SVID_T:
			match(SVID_T, NO_ATTR);
			break;
		default:
			syn_printe();
	}
}

/*
first set = , e
	<variable_list_prime>->
		, <variable_identifier><variable_list_prime>
*/
void variable_list_prime() {
	switch (lookahead.code) {
	case COM_T:
		match(COM_T, NO_ATTR);
		variable_identifier();
		variable_list_prime();
	default:    //empty
		break;
	}
}

/*
first set = WRITE
	<output_statement>->
		WRITE ( <output_list> ) ;
*/
void output_statement() {
	match(KW_T, WRITE);
	match(LPR_T, NO_ATTR);
	output_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");
}

/*
first set = AVID_T, SVID_T, STR_T, e
	<output_list>->
		<opt_variable_list> | <STR_T> | e
*/
void output_list() {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		opt_variable_list();
		break;
	case STR_T:
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed");
		break;
	default: //empty
		gen_incode("PLATY: Output list (empty) parsed");
	}
}

/*
first set = AVID_T, SVID_T, e
	<opt_variable_list>->
		<variable_list> | e
*/
void opt_variable_list() {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		variable_list();
	default: //empty
		break;
	}
}

/*
first set = first(logical_or_expression)
	<conditional_expression>->
		<logical_or_expression>
*/
void conditional_expression() {
	logical_or_expression();
	gen_incode("PLATY: Conditional expression parsed");
}

/*
first set = first(logical_and_expression)
	<logical_or_expression>->
		<logical_and_expression><logical_or_expression_prime>
*/
void logical_or_expression() {
	logical_and_expression();
	logical_or_expression_prime();
}

/*
first set = first(relational_expression)
	<logical_and_expression>->
		<relational_expression><logical_and_expression_prime>
*/
void logical_and_expression() {
	relational_expression();
	logical_and_expression_prime();
}

/*
first set = .OR., e
	<logical_or_expression_prime>->
		.OR. <logical_and_expression><logical_or_expression_prime> | e
*/
void logical_or_expression_prime() {
	switch (lookahead.code) {
	case LOG_OP_T:
		if (lookahead.attribute.log_op == OR) {
			match(LOG_OP_T, OR);
			logical_and_expression();
			logical_or_expression_prime();	
			gen_incode("PLATY: Logical OR expression parsed");
		}
	default: //empty
		break;
	}
}

/*
first set = .AND., e
	<logical_and_expression_prime>->
		.AND. <relational_expression><logical_and_expression_prime> | e
*/
void logical_and_expression_prime() {

	switch (lookahead.code) {
	case LOG_OP_T:
		if (lookahead.attribute.log_op == AND) {
			match(LOG_OP_T, AND);
			relational_expression();
			logical_and_expression_prime();
			gen_incode("PLATY: Logical AND expression parsed");
		}
		default: //empty
			break;
	}
}

/*
first set = first(primary_a_relational), first(primary_s_relational)
	<relational_expression>->
		<primary_a_relational><comparison_operators><primary_a_relational> 
		| <primary_s_relational><comparison_operators><primary_s_relational>
*/
void relational_expression() {
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T:
		primary_a_relational();
		comparison_operators();
		primary_a_relational();
		gen_incode("PLATY: Relational expression parsed");
		break;
	case STR_T:
	case SVID_T:
		primary_s_relational();
		comparison_operators();
		primary_s_relational();
		gen_incode("PLATY: Relational expression parsed");
		break;
	default: //errpor
		syn_printe();
		gen_incode("PLATY: Relational expression parsed");
	}
}

/*
first set = SVID_T, STR_T
	<primary_s_relational>->
		SVID_T | STR_T
*/
void primary_s_relational() {
	switch (lookahead.code) {
	case SVID_T:
		match(SVID_T, NO_ATTR);
		gen_incode("PLATY: Primary string expression parsed");
		gen_incode("PLATY: Primary s_relational expression parsed");
		break;
	case STR_T:
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Primary string expression parsed");
		gen_incode("PLATY: Primary s_relational expression parsed");
		break;
	default: //error
		syn_printe();
	}
}

/*
first set = AVID_T, FPL_T, INL_T
	<primary_a_relational>->
		AVID_T | FPL_T | INL_T
*/
void primary_a_relational() {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		gen_incode("PLATY: Primary a_relational expression parsed");
		break;
	case FPL_T:
		match(FPL_T, NO_ATTR);
		gen_incode("PLATY: Primary a_relational expression parsed");
		break;
	case INL_T:
		match(INL_T, NO_ATTR);
		gen_incode("PLATY: Primary a_relational expression parsed");
		break;
	default:
		syn_printe();
		gen_incode("PLATY: Primary a_relational expression parsed");
	}
}

/*
first set = <> , ==, >, <
	<comparison_operators>->
		<> | == | < | >
*/
void comparison_operators() {
	switch (lookahead.attribute.rel_op) {
	case EQ:
		match(REL_OP_T, EQ);
		break;
	case NE:
		match(REL_OP_T, NE);
		break;
	case GT:
		match(REL_OP_T, GT);
		break;
	case LT:
		match(REL_OP_T, LT);
		break;
	default:
		syn_printe();
	}
}

/*
first set = first(unary_arithmetic_expression), first(additive_arithmetic_expression)
	<arithmetic_expression>->
		<unary_arithmetic_expression> | <additive_arithmetic_expression>
*/
void arithmetic_expression() {

	switch (lookahead.code) {
	case ART_OP_T:
		unary_arithmetic_expression();
		gen_incode("PLATY: Arithmetic expression parsed");
		break;
	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T:
		additive_arithmetic_expression();
		gen_incode("PLATY: Arithmetic expression parsed");
		break;
	default:
		syn_printe();
	}
}

/*
first set = first(unary_operators)
	<unary_arithmetic_expression>->
		<unary_operators><primary_arithmetic_expression>
*/
void unary_arithmetic_expression() {
	unary_operators();
	primary_arithmetic_expression();
	gen_incode("PLATY: Unary arithmetic expression parsed");
}

/*
first set = +, -
	<unary_operators>->
		+ | -
*/
void unary_operators() {
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.arr_op == PLUS) {
			match(ART_OP_T, PLUS);
			break;
		}
		else {
			match(ART_OP_T, MINUS);
			break;
		}
	default:
		syn_printe();
	}
}

/*
first set = *, /
	<mult_operators>->
		/ | *
*/
void mult_operators() {
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.arr_op == MULT) {
			match(ART_OP_T, MULT);
			break;
		}
		else {
			match(ART_OP_T, DIV);
			break;
		}
	default:
		syn_printe();
	}
}

/*
first set = first(multiplicative_arithmetic_expression)
	<additive_arithmetic_expression>->
		<multiplicative_arithmetic_expression><additive_arithmetic_expression_prime>
*/
void additive_arithmetic_expression() {
	multiplicative_arithmetic_expression();
	additive_arithmetic_expression_prime();
}

/*
first set = first(unary_operators)
	<additive_arithmetic_expression_prime>->
		<unary_operators><multiplicative_arithmetic_expression><additive_arithmetic_expression_prime>
*/
void additive_arithmetic_expression_prime() {
	switch (lookahead.code) {
	case ART_OP_T:
		unary_operators();
		multiplicative_arithmetic_expression();
		additive_arithmetic_expression_prime();
		gen_incode("PLATY: Additive arithmetic expression parsed");
	default: //empty
		break;
	}
}

/*
first set = first(primary_arithmetic_expression)
	<multiplicative_arithmetic_expression>->
		<primary_arithmetic_expression><multiplicative_arithmetic_expression_prime>	
*/
void multiplicative_arithmetic_expression() {
	primary_arithmetic_expression();
	multiplicative_arithmetic_expression_prime();
}

/*
first set = /, *, e
	<multiplicative_arithmetic_expression_prime>->
		<mult_operators><primary_arithmetic_expression><multiplicative_arithmetic_expression_prime> | e
*/
void multiplicative_arithmetic_expression_prime() {
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.arr_op == MULT || lookahead.attribute.arr_op == DIV) {
			mult_operators();
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_prime();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
		}
	default:
		break;  //empty
	}
}

/*
first set = AVID_T, FPL_T, INL_T, (
	<primary_arithmetic_expression>->
		AVID_T | FPL_T | INL_T | ( <arithmetic_expression> )
*/
void primary_arithmetic_expression() {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		gen_incode("PLATY: Primary arithmetic expression parsed");
		break;
	case FPL_T:
		match(FPL_T, NO_ATTR);
		gen_incode("PLATY: Primary arithmetic expression parsed");
		break;
	case INL_T:
		match(INL_T, NO_ATTR);
		gen_incode("PLATY: Primary arithmetic expression parsed");
		break;
	case LPR_T:
		match(LPR_T, NO_ATTR);
		arithmetic_expression();
		match(RPR_T, NO_ATTR);
		gen_incode("PLATY: Primary arithmetic expression parsed");

		break;
	default:
		syn_printe();
	}
}




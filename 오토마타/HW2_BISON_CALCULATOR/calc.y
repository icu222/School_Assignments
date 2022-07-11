%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#define YYSTYPE double

void yyerror(char* s);
int yylex(void);

double result = 0.0;

%}


%token NUM
%left '+' '-' '/' '%' 
%right '^'

%%

line: 
   	 | line form
 	 ;
form: 	'\n'
    	| expr '\n'		{ result = $1 ; printf("result : %f \n", result); }
   	 ;
expr: 	term			{ $$ = $1; }
   	 | expr '+' term		{ $$ = $1 + $3; }
   	 | expr '-' term		{ $$ = $1 - $3; }
	 ;
term:	 power			{ $$ = $1; }
   	 | term '*' power		{ $$ = $1 * $3; }
   	 | term '/' power		{ if($3 ==0){ yyerror("can't divide by 0"); return 0;} $$ = $1 / $3; }
	 | term '%' power		{ $$ = fmod($1, $3); }
	 | term '#' power		{ $$ = (int)( $1 / $3 ); }
	 ;
power: 	factor			{ $$ = $1; }
  	 | power '^' power 		{ $$ = pow($1, $3); }
   	 ;
factor:	 NUM			{ $$ = $1; }
	 | '-' NUM 		{ $$ = -$2; }
	 | '_' 			{ $$ = result; }
	 | '(' expr ')' 		{ $$ = $2; }

%%

int yylex(){

	int c;

	while ((c = getchar()) == ' '||c == '\t') {} 

	if (isdigit(c)) {
		ungetc(c, stdin);
		scanf("%lf", &yylval);
		return NUM;
	}

	if (c == '$'){ 
		printf("END Calculator");
		return 0;
	}

	return c; 
}

void yyerror(char* s) {
	printf("ERROR : %s \n", s);
}

int main() { 
	yyparse();
	return 0;
}
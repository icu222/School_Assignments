#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>



/* Define Token with number for categorizing */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 2
#define VTYPE 3
#define S_INTEGER 4
#define S_CHAR 5
#define TRUE 6
#define FALSE 7
#define LIT_STR 8
#define IDENT 9
#define WHILE 10
#define IF 11
#define ELSE 12
#define CLASS 13
#define RETURN 14
#define OPERATOR 15
#define ASSIGN 16
#define COMPARE 17
#define TERMINATE 18
#define LSB 19
#define RSB 20
#define LCB 21
#define RCB 22
#define LPAREN 23
#define RPAREN 24

/*============== Symbol table=================*/
typedef struct
{
	char Token[20]; // length of token name
	char input[50]; // length of input
} st;

st SymbolTable[100];
int stableLen = 0; //the index of symbolTable

/*================== Lexeme ===================*/
char lexeme[100]; // one word for lex
int lexLen = 0;	  // the length of lex

char nextChar; // one character
int charClass = -1; // the token value which involved in the character 
// initial value is -1 so we can clssify the '-'

int nextToken;		 //	the token name which the word lex involved in
char token_name[20]; // the storage for token name

/*================ related to I/O File ==========*/
FILE *fp = NULL;		 //the file variable
char inputString[100];	 // statement
char fileDirectory[100]; // output file name size

int inStrLen = 0; // the length of input stream
int inStrPos = 0; // the index of input stream

int inputLine = 0;	 // the line of input stream
int final_index = 0; // the final index of input stream

/*============related to lexcial Analyzer================*/
void addCharToLexeme();	 // store one character to lexeme stream
void getCharFromInput(); // get one character from the input stream
void getNonBlank();		 // remove white space
int lexicalAnalyzer();	 // the function for analyze lexeme

/* ========= related to etc functions ===============*/
void findSTRLEN(char *str);	   // find the inStrLen variable
void clearString(char *, int); // make all element value to zero
void PutLexInSTable();		   // store one lex token to symbol table which already over lexical analyzed
void StoreDataToFILE(char *);  // send by file stream with stored value in symbol table
void error(char *);			   // prints an error statement
void findLocation(char *);	   // add input text file name to fileDirectory array

/* ========= the Logic for defined functions ===============*/
/* find the inStrLen variable */
void findSTRLEN(char *str)
{
	while (inputString[inStrLen] != 0)
	{
		inStrLen++;
	}
}

/* store one lex token to symbol table which already over lexical analyzed */
void PutLexInSTable()
{

	strncpy(SymbolTable[stableLen].Token, token_name, sizeof(token_name));
	SymbolTable[stableLen].Token[19] = 0;
	strncpy(SymbolTable[stableLen++].input, lexeme, sizeof(char) * (lexLen + 1));
}

/* add input text file name to fileDirectory array*/
void findLocation(char *str)
{
	int r = 0;
	int t = strlen(str);

	do
	{
		t--;
	} while (str[t] != '.');

	for (; r < t; r++)
		fileDirectory[r] = str[r];

	fileDirectory[r] = 0;
}

/* send by file stream with stored value in symbol table */
void StoreDataToFILE(char *str)
{

	char *result_txt = "_output.txt";
	strcat(str, result_txt);

	fp = fopen(str, "w");
	char outputline[100];

	for (int r = 0; r < stableLen; r++)
	{
		clearString(outputline, 100);
		outputline[0] = '<';
		strcat(outputline, SymbolTable[r].Token);
		strcat(outputline, ",");
		strcat(outputline, SymbolTable[r].input);
		strcat(outputline, ">\n");

		// result outputline : <Token,value>
		fputs(outputline, fp);
	}
	fclose(fp);
}

/* make strings all element value to zero */
void clearString(char *str, int len)
{

	int y = 0;
	for (; y < len; y++)
	{
		str[y] = 0;
	}
}

// char class = the automata for digit
void digit_dfa()
{
	// if input letter is 0 and it is the first.    ex) input: this case of zero>>0434234525000
	// just like the announcement, we define it as s_int, < s_int,0 >
	if (nextChar == '0' && lexLen == 0) {
		
		addCharToLexeme();

		nextToken = S_INTEGER;
		getCharFromInput();
		return;
	}

	do
	{
		addCharToLexeme();
		getCharFromInput();
		// if lexeme is 0N... (N is digit ) ex : 09878 >> error!
			
	} while (charClass == DIGIT);
	nextToken = S_INTEGER;
}

// char class = the automata for alphabet
void letter_dfa()
{

	while (charClass != UNKNOWN)
	{
		addCharToLexeme();
		getCharFromInput();
	}

	if (strcmp(lexeme, "int") == 0 || strcmp(lexeme, "char") == 0 || strcmp(lexeme, "String") == 0 || strcmp(lexeme, "boolean") == 0)
	{
		nextToken = VTYPE;
	}
	else if (strcmp(lexeme, "if") == 0)
		nextToken = IF;
	else if (strcmp(lexeme, "else") == 0)
		nextToken = ELSE;
	else if (strcmp(lexeme, "while") == 0)
		nextToken = WHILE;
	else if (strcmp(lexeme, "class") == 0)
		nextToken = CLASS;
	else if (strcmp(lexeme, "return") == 0)
		nextToken = RETURN;

	else
	{
		nextToken = IDENT;
	}
}

// char class = the automata for etc
void unknown_dfa()
{

	switch (nextChar)
	{
	case ')':
		nextToken = RPAREN;
		addCharToLexeme();
		getCharFromInput();
		break;
	case '(':
		nextToken = LPAREN;
		addCharToLexeme();
		getCharFromInput();
		break;
	case '}':
		nextToken = RCB;
		addCharToLexeme();
		getCharFromInput();
		break;
	case '{':
		nextToken = LCB;
		addCharToLexeme();
		getCharFromInput();
		break;
	case ']':
		nextToken = RSB;
		addCharToLexeme();
		getCharFromInput();
		break;
	case '[':
		nextToken = LSB;
		addCharToLexeme();
		getCharFromInput();
		break;

	case '>':
		addCharToLexeme();
		getCharFromInput();
		if (nextChar == '=')
		{
			addCharToLexeme();
			getCharFromInput();
			nextToken = COMPARE;
		}
		else if (charClass == DIGIT || charClass == LETTER)
		{
			nextToken = COMPARE;
		}
		else
		{
			// error    such as  >! error
			error("Incorrect characters were written after >");
		}
		break;

	case '<':
		addCharToLexeme();
		getCharFromInput();
		if (nextChar == '=')
		{
			addCharToLexeme();
			getCharFromInput();
			nextToken = COMPARE;
		}
		else if (charClass == DIGIT || charClass == LETTER)
		{
			nextToken = COMPARE;
		}
		else
		{
			// error    such as <> error
			error("Incorrect characters were written after <");
		}
		break;

	case '!':
		addCharToLexeme();
		getCharFromInput();
		if (nextChar == '=')
		{
			addCharToLexeme();
			getCharFromInput();
			nextToken = COMPARE;
		}
		else
		{
			// error    such as !a !4 errors
			error("Incorrect characters were written after !");
		}
		break;

	case '=':
		addCharToLexeme();
		getCharFromInput();

		if (nextChar == '=')
		{
			addCharToLexeme();
			getCharFromInput();
			nextToken = COMPARE;
		}

		else
			nextToken = ASSIGN;

		break;

	case ';':
		nextToken = TERMINATE;
		addCharToLexeme();
		getCharFromInput();
		
		break;

	case '+':
		nextToken = OPERATOR;
		addCharToLexeme();
		getCharFromInput();
		break;
	case '-':
			nextToken = OPERATOR;
			addCharToLexeme();
			getCharFromInput();
		//}
		break;
	case '/':
		nextToken = OPERATOR;
		addCharToLexeme();
		getCharFromInput();
		break;
	case '*':
		nextToken = OPERATOR;
		addCharToLexeme();
		getCharFromInput();
		break;

	case '\"':
		getCharFromInput();
		while (nextChar != '\"')
		{
			if (lexLen > 97) {
				error("There is no <\"> in the String");
			}
			addCharToLexeme();
			getCharFromInput();
		}
		getCharFromInput();
		nextToken = LIT_STR;
		break;

	case '\'':
		getCharFromInput();
		if (nextChar == '\'')
		{
			nextToken = S_CHAR;
			getCharFromInput();
		}
		else
		{
			addCharToLexeme();
			nextToken = S_CHAR;
			getCharFromInput();
			if (nextChar != '\'')
			{
				error("The character should end with ' and ' !!!!!");
			}
			getCharFromInput();
		}
		break;
	default:
		break;
	}
}

/* store one character from Input string to lexeme string */
void addCharToLexeme()
{
	if (lexLen <= 98)
	{
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = 0;
	}
	else
		error("Error - lexeme is too long \n");
}

/* get one character from the input string */
void getCharFromInput()
{

	nextChar = inputString[inStrPos++];

	if (isalpha(nextChar) || nextChar == '_')
		charClass = LETTER;

	// ahead of '-', if that lexemes Class is Digit/Ident/,,, : ex: fish(Letter) - ,  3454(Digit) -  ,,,,
	else if (nextChar == '-' && (nextToken == IDENT || nextToken == IF || nextToken == ELSE || nextToken == WHILE|| nextToken == VTYPE || nextToken == RETURN || nextToken == S_INTEGER))
		charClass = UNKNOWN;// '-' is OPERATOR 

	// ahead of '-', if that lexemes Token is OPERATOR : ex: +(operator) - 
	else if (nextChar == '-' && ( nextToken == OPERATOR || nextToken == COMPARE))
		charClass = DIGIT;// '-' is in Signed Integer

	// ahead of '-', if that charactors class is UNKNOWN but not a operator:  "dsad"(unknown) -
	else if (nextChar == '-' && charClass == UNKNOWN )
		charClass = UNKNOWN;// '-' is OPERATOR 

	else if (isdigit(nextChar))
		charClass = DIGIT;
	else
		charClass = UNKNOWN;

}

/* Remove white spaces */
void getNonBlank()
{
	while (isspace(nextChar))
	{
		getCharFromInput();
	}
}

/* change catergorized token number to string */
void Token_to_String(int tk, char *name)
{
	switch (tk)
	{

	case 0:
		strncpy(name, "LETTER", sizeof("LETTER"));
		break;
	case 1:
		strncpy(name, "DIGIT", sizeof("DIGIT"));
		break;
	case 2:
		strncpy(name, "UNKNOWN", sizeof("UNKNOWN"));
		break;
	case 3:
		strncpy(name, "VTYPE", sizeof("VTYPE"));
		break;
	case 4:
		strncpy(name, "S_INTEGER", sizeof("S_INTEGER"));
		break;
	case 5:
		strncpy(name, "S_CHAR", sizeof("S_CHAR"));
		break;
	case 6:
		strncpy(name, "TRUE", sizeof("TRUE"));
		break;
	case 7:
		strncpy(name, "FALSE", sizeof("FALSE"));
		break;
	case 8:
		strncpy(name, "LIT_STR", sizeof("LIT_STR"));
		break;
	case 9:
		strncpy(name, "IDENT", sizeof("IDENT"));
		break;
	case 10:
		strncpy(name, "WHILE", sizeof("WHILE"));
		break;
	case 11:
		strncpy(name, "IF", sizeof("IF"));
		break;
	case 12:
		strncpy(name, "ELSE", sizeof("ELSE"));
		break;
	case 13:
		strncpy(name, "CLASS", sizeof("CLASS"));
		break;
	case 14:
		strncpy(name, "RETURN", sizeof("RETURN"));
		break;
	case 15:
		strncpy(name, "OPERATOR", sizeof("OPERATOR"));
		break;
	case 16:
		strncpy(name, "ASSIGN", sizeof("ASSIGN"));
		break;
	case 17:
		strncpy(name, "COMPARE", sizeof("COMPARE"));
		break;
	case 18:
		strncpy(name, "TERMINATE", sizeof("TERMINATE"));
		break;
	case 19:
		strncpy(name, "LSB", sizeof("LSB"));
		break;
	case 20:
		strncpy(name, "RSB", sizeof("RSB"));
		break;
	case 21:
		strncpy(name, "LCB", sizeof("LCB"));
		break;
	case 22:
		strncpy(name, "RCB", sizeof("RCB"));
		break;
	case 23:
		strncpy(name, "LAPREN", sizeof("LAPREN"));
		break;
	case 24:
		strncpy(name, "RPAREN", sizeof("RPAREN"));
		break;

	default:
		break;
	}
}

/* the function for analyze lexeme */
int lexicalAnalyzer()
{

	lexLen = 0;
	getNonBlank(); // remove white space

	switch (charClass)
	{

	case LETTER:
		letter_dfa();
		break;

	case DIGIT:
		digit_dfa();
		break;

	case UNKNOWN:
		unknown_dfa();
		break;

		/* EOF */
	case EOF: // if the end of the file
		nextToken = EOF;
		lexeme[0] = 'E';
		lexeme[1] = 'O';
		lexeme[2] = 'F';
		lexeme[3] = 0;
		break;
	}
	Token_to_String(nextToken, token_name);

	PutLexInSTable();

	clearString(lexeme, lexLen);

	return nextToken;
}

/* Function that prints an error statement */
void error(char *str)
{
	printf("ERROR DETECTED : Line %d , %s \n", inputLine, str);
	printf("EXIT PROGRAM\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	findLocation(argv[1]);

	fp = fopen(argv[1], "r");

	while (fgets(inputString, sizeof(inputString), fp) != NULL)
	{

		inStrPos = 0;
		inStrLen = 0;
		inputLine++;

		findSTRLEN(inputString);
		getCharFromInput();
		inStrLen--;

		while (inStrPos < inStrLen)
		{
				lexicalAnalyzer();
		}

		final_index = --inStrLen;

		/* Error if comma ends alone last */
		if (inputString[final_index] == '\'')
			{
			if (inputString[final_index - 2] != '\'')
				{
			error("The input stream cannot be finished with ' alone!!!  ");
				}
			}

		/* execute one more time for recognize end of the particular token. */
		if (inputString[final_index] == ';' || inputString[final_index] == '-' || inputString[final_index] == '+' || inputString[final_index] == '*' || inputString[final_index] == '/')
			{
				lexicalAnalyzer();
			}

	}

			/* execute one more time for the last line to recognize end of the particular token. */
		if (inputString[final_index] == ';' || inputString[final_index] == '-' || inputString[final_index] == '+' || inputString[final_index] == '*' || inputString[final_index] == '/')
		{
			lexicalAnalyzer();
		}




	StoreDataToFILE(fileDirectory);

	printf("END Lexical Analyzer. Check the output.txt \n");

	return 0;
}
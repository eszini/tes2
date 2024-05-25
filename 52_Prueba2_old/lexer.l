%{

#include "y.tab.h"
#include <stdio.h>

#undef YY_INPUT
#define YY_INPUT(b,r,s) readInputForLexer(b,&r,s)

int 		readInputForLexer();
extern	void	grama_trk(char *,char *);

extern	int	gc_pal_e;
extern	char	*yy_ptr;



int 		scode;



%}


%%


[a-zA-Z]+_NEG	{	
			/* token */
			scode=201;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return NEG;
		}

[a-zA-Z]+_CNEG	{	
			/* token */
			scode=202;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return CNEG;
		}

[a-zA-Z]+_ENDNEG {
			/* token */
			scode=203;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return ENDNEG;
		}

[a-zA-Z]+_TNEG	{
			/* token */
			scode=204;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return TNEG;
		}

[a-zA-Z]+_VSE	{
			/* token */
			scode=205;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return VSE;
		}

[a-zA-Z]+_OBS	{
			/* token */
			scode=206;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return OBS;
		}

[a-zA-Z]+_DET	{
			/* token */
			scode=207;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return DET;
		}

[a-zA-Z]+_EVI	{
			/* token */
			scode=208;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return EVI;
		}

[a-zA-Z]+_VIS	{
			/* token */
			scode=209;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return VIS;
		}

[a-zA-Z]+_IDE	{
			/* token */
			scode=210;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return IDE;
		}

._PTO		{
			/* token */
			scode=301;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return PTO;
		}

,_COM		{
			/* token */
			scode=302;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return COM;
		}

;_PTC		{	/* token   */
			scode=303;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return PTC;
		}


\.		{	/* Separador tipo 3 - fin de linea */
			scode=103;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return FNL;
		}

[a-zA-Z]+ 	{ 	/* texto ... OJO aca estaba el strcpy a yy_ptr ... que rompia todo */
			scode=104;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return TXT;
		}

[0-9.]+		{	/* numero */
			scode=105;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return NUM;
		}


[\(\-\+\)\:]+	{	/* caracteres especiales */
			scode=106;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return CES;
		}

[\t ]+		{	/* ignorar blancos  */
			scode=107;
			printf ("(lex)  %03d %-15.15s  ",scode,yytext); 
			return BLA;
		}

\n		{	/* fin de linea */
			scode=401;
			printf ("(lex)  %03d %-15.15s  \n",scode,"\\n"); 
			return FNL;
		}





%%


void yyerror(char* s) 
{
	printf("error\n");
}

int yywrap() 
{
	return -1;
}




/* --- end of file --- */
/* --- end of file --- */
/* --- end of file --- */








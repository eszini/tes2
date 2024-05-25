%{

#include "y.tab.h"
#include <stdio.h>

#undef YY_INPUT
#define YY_INPUT(b,r,s) readInputForLexer(b,&r,s)

int 		readInputForLexer();

extern	int	l_flag;
extern	char	*yy_ptr;

int 		scode;
int		m_lex(int, int, char *);

#if 0
void		grama_trk(char *);
extern	int	gc_pal_e;
#endif


%}


%%


[a-zA-Z]+_NEG		{	/* token */
				m_lex(0,201,yytext);                     
				return NEG;             
			}

[a-zA-Z]+_CNEG		{	/* token */
				m_lex(0,202,yytext);                     
				return CNEG;            
			}

[a-zA-Z]+_ENDNEG 	{	/* token */
				m_lex(0,203,yytext);    
				return ENDNEG;          
			}

[a-zA-Z]+_TNEG		{	/* token */
				m_lex(0,204,yytext);      
				return TNEG;            
			}

[a-zA-Z]+_VSE		{	/* token */
				m_lex(0,205,yytext);
				return VSE;
			}

[a-zA-Z]+_OBS		{	/* token */
				m_lex(0,206,yytext);
				return OBS;
			}

[a-zA-Z]+_DET		{	/* token */
				m_lex(0,207,yytext);
				return DET;
			}

[a-zA-Z]+_EVI		{	/* token */
				m_lex(0,208,yytext);
				return EVI;
			}

[a-zA-Z]+_VIS		{	/* token */
				m_lex(0,209,yytext);
				return VIS;
			}

[a-zA-Z]+_IDE		{	/* token */
				m_lex(0,210,yytext);
				return IDE;
			}

._PTO			{	/* token */
				m_lex(0,301,yytext);
				return PTO;
			}

,_COM			{	/* token */
				m_lex(0,302,yytext);
				return COM;
			}

;_PTC			{	/* token   */
				m_lex(0,303,yytext);
				return PTC;
			}


\.			{	/* Separador tipo 3 - fin de linea */
				m_lex(0,103,yytext);
				return FNL;
			}

[a-zA-Z]+ 		{	/* texto */
				strcpy(yy_ptr,yytext);  
				m_lex(0,104,yytext);
				return TXT;
			}

[0-9.]+			{	/* numero */
				m_lex(0,105,yytext);
				return NUM;
			}


[\(\-\+\)\:]+		{	/* caracteres especiales */
				m_lex(0,106,yytext);
				return CES;
			}

[\t ]+			{	/* ignorar blancos ?? */
				m_lex(0,107,yytext);
				return BLA;
			}

\n			{	/* fin de linea */
				m_lex(0,401,"\\n");
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

int	m_lex(p1,p2,s3)
int	p1;
int	p2;
char	*s3;
{
	printf ("(lex)  %03d %s \n",p2,s3); 
}


/* --- end of file --- */
/* --- end of file --- */
/* --- end of file --- */








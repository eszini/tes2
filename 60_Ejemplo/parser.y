%{

#include <stdio.h>
#include <string.h>


extern void	yyerror(char* s);
extern int	yylex();
extern int	readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);

extern	int	gramatik(char *,int);

extern	int	gc_si;
extern	int	gc_no;
extern  int	v_flag;
extern	int	gc_pal_e;
extern	int	f_cneg,f_sneg;

extern	char	*spp;

extern	char	*yy_ptr;

char		tkn[64];


int		m_yacc(int,int,char *);

%}

%token		NEG ENDNEG CNEG TNEG VSE OBS DET EVI VIS IDE PTO COM PTC FNL TXT NUM CES BLA 

%%


linea:          s_oracion
	                          { m_yacc(0,0,"linea: s_oracion");         }
	      | s_oracion FNL          
	                          { m_yacc(0,0,"linea: s_oracion FNL");     }
	      | c_oracion FNL         
	                          { m_yacc(0,0,"linea: c_oracion FNL");     }
               ;


s_oracion:      oracion_cn PTO
	                          { m_yacc(1,0,"s_ora: oracion_cn PTO"); f_cneg=1;    }
	      | oracion_sn PTO
	                          { m_yacc(0,0,"s_ora: oracion_sn PTO"); f_sneg=1;    }
	       ;

 

c_oracion:	s_oracion s_oracion
	                          { m_yacc(0,0,"c_ora: s_ora s_ora");       }
	      | c_oracion s_oracion
	                          { m_yacc(0,0,"c_ora: c_ora s_ora");       }
	       ;



       
oracion_sn:	frase
	                          { m_yacc(0,0,"or_sn: frase      ");     }
	       ;

oracion_cn:	frase NEG 
	                          { m_yacc(0,0,"or_cn: frase NEG  ");     }
	      | NEG frase
	                          { m_yacc(0,0,"or_cn: NEG frase  ");     }
	      | frase NEG frase
	                          { m_yacc(0,0,"or_cn: frase NEG frase"); }
	       ;



frase:		elemento
	                          { m_yacc(0,0,"frase: elemento   ");       }
	      | frase elemento
	                          { m_yacc(0,0,"frase: frase elem ");       }
	       ;


     


elemento:	TXT
	                          { m_yacc(0,TXT,"eleme: TXT   ");             }
	      | NUM
				  { m_yacc(0,NUM,"eleme: NUM   ");             }
              | CES
				  { m_yacc(0,NUM,"eleme: CES   ");             }
              | BLA
				  { m_yacc(0,NUM,"eleme: BLA   ");             }
              | COM
				  { m_yacc(0,NUM,"eleme: COMA  ");             }
              | PTC
				  { m_yacc(0,NUM,"eleme: PTC   ");             }
              | ENDNEG
				  { m_yacc(0,NUM,"eleme: ENDNEG");             }
              | CNEG
				  { m_yacc(0,NUM,"eleme: CNEG  ");             }
              | TNEG
				  { m_yacc(0,NUM,"eleme: TNEG  ");             }
              | OBS
				  { m_yacc(0,NUM,"eleme: OBS   ");             }
	      | DET
				  { m_yacc(0,NUM,"eleme: DET   ");             }
	      | EVI
				  { m_yacc(0,NUM,"eleme: EVI   ");             }
	      | VIS
				  { m_yacc(0,NUM,"eleme: VIS   ");             }
	      | IDE
				  { m_yacc(0,NUM,"eleme: IDE   ");             }
              | VSE
				  { m_yacc(0,NUM,"eleme: VSE   ");             }
                ;



%%


int	m_yacc(p1,p2,s3)
int	p1;
int	p2;
char	*s3;
{
	int	f1,t2;

	f1=0;
	strcpy (tkn,yy_ptr);
	f1 = gramatik(tkn,p1);

	if (v_flag > 0 )
	{
		if (p2 == TXT)
		{
			if (f1)
				printf ("%s(yac), %s           (%s)  !!! \n",spp,s3,tkn);
			else
				printf ("%s(yac), %s           (%s)     \n",spp,s3,tkn);
		}
		else
			printf ("%s(yac), %s \n",spp,s3);
	} 
}




%{


#include <stdio.h>
#include <string.h>


extern void yyerror(char* s);
extern int yylex();
extern int readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);

extern	void grama_trk(char *,char *);
extern	int  check(char *, char *);

int	pru_yacc(int);

extern	int gc_si;
extern	int gc_no;
extern  int v_flag;
extern	int gc_pal_e;


extern	char *spp;

extern	char *yy_ptr;

char	tkn[64];
static  char	nivel[64];



%}

%token		NEG ENDNEG CNEG TNEG VSE OBS DET EVI VIS IDE PTO COM PTC FNL TXT NUM CES BLA 


%%

linea:          s_oracion
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),line   : s_oracion         \n",spp);
                                    }
                                  }

	      | s_oracion FNL          
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),line   : s_oracion FNL     \n",spp);
                                    }
                                  }
	      | c_oracion FNL         
                                  { if (v_flag > 0 ) 
				    {	
                                    	printf  ("%s(yac),line   : c_oracion FNL     \n",spp);   
                                    }
                                  }
               ;


s_oracion:      oracion_cn PTO
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),s_ora  : oracion_cn PTO    \n",spp);
                                    }
                                  }
	      | oracion_sn PTO
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),s_ora  : oracion_sn PTO    \n",spp);
                                    }
                                  }
	       ;

 

c_oracion:	s_oracion s_oracion
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),c_ora  : s_ora s_ora       \n",spp);
                                    }
                                  }

	      | c_oracion s_oracion
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),C_ora  : c_ora s_ora       \n",spp);
                                    }
                                  }
	       ;





       
oracion_sn:	frase
	                          { if (v_flag > 0 ) 
	                            {	
                                  	gc_no++;
					printf  ("%s(yac),ora_sn : frase             \n",spp);
                                    }
                                  }
	       ;

oracion_cn:	frase NEG
	                          { if (v_flag > 0 ) 
	                            {	
                                  	gc_si++;
					printf  ("%s(yac),ora_cn : frase NEG         \n",spp);
                                    }
                                  }
	      | NEG frase
	                          { if (v_flag > 0 ) 
	                            {	
                                  	gc_si++;
					printf  ("%s(yac),ora_cn : NEG frase         \n",spp);
                                    }
                                  }
	      | frase NEG frase
	                          { if (v_flag > 0 ) 
	                            {	
                                  	gc_si++;
					printf  ("%s(yac),ora_cn : frase NEG frase   \n",spp);
                                    }
                                  }
	       ;



frase:		elemento
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),frase  : elemento          \n",spp);
                                    }
                                  }
	      | frase elemento
	                          { if (v_flag > 0 ) 
	                            {	
					printf  ("%s(yac),frase  : frase elemento    \n",spp);
                                    }
                                  }
	       ;


     


elemento:	TXT               { if (v_flag > 0 ) 
				    {	strcpy(tkn,yy_ptr);
					printf  ("(yac),TXT    :     (%s)     \n",tkn);     
				    }
				  }
	      | NUM               { if (v_flag > 0 ) printf  ("(yac),NUM    :              \n");     }
              | CES               { if (v_flag > 0 ) printf  ("(yac),CES    :              \n");     }
              | BLA               { if (v_flag > 0 ) printf  ("(yac),BLA    :              \n");     }
              | COM               { if (v_flag > 0 ) printf  ("(yac),COMA   :              \n");     }
              | PTC               { if (v_flag > 0 ) printf  ("(yac),PTC    :              \n");     }
              | ENDNEG            { if (v_flag > 0 ) printf  ("(yac),ENDNEG :              \n");     }
              | CNEG              { if (v_flag > 0 ) printf  ("(yac),CNEG   :              \n");     }
              | TNEG              { if (v_flag > 0 ) printf  ("(yac),TNEG   :              \n");     }
              | OBS               { if (v_flag > 0 ) printf  ("(yac),OBS    :              \n");     }
	      | DET               { if (v_flag > 0 ) printf  ("(yac),DET    :              \n");     }
	      | EVI               { if (v_flag > 0 ) printf  ("(yac),EVI    :              \n");     }
	      | VIS               { if (v_flag > 0 ) printf  ("(yac),VIS    :              \n");     }
	      | IDE               { if (v_flag > 0 ) printf  ("(yac),IDE    :              \n");     }
              | VSE               { if (v_flag > 0 ) printf  ("(yac),VSE    :              \n");     }
                ;



%%



int	pru_yacc(x)
int	x;
{
	if (x == 1)
		strcpy (nivel,"LIN");
	if (x == 2)
		strcpy (nivel,"ORA");
	if (x == 3)
		strcpy (nivel,"FRA");
	if (x == 4)
		strcpy (nivel,"TER");

}



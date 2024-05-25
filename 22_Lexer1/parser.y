%{


#include <stdio.h>
extern void yyerror(char* s);
extern int yylex();
extern int readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);


extern	int gc_si;
extern	int gc_no;
extern  int v_flag;

extern	char *spp;

extern	int	global_count;


%}

%token		NEG ENDNEG CNEG TNEG VSE OBS DET EVI VIS IDE PTO COM PTC FNL TXT NUM CES BLA 


%%

sentencia:      frase             { if (v_flag > 0 ) 
	                            {	printf  ("%s(yac),sent   : frase        \n",spp);
					printf  ("global count   : %04d         \n",global_count);
                                    }
                                  }
	      | frase FNL         { if (v_flag > 0 ) 
                                    {	printf  ("%s(yac),sent   : frase, FNL   \n",spp);   
					printf  ("global count   : %04d         \n",global_count);
                                    }
                                  }
               ;

frase:          elemento          { if (v_flag > 0 ) 
				    {	printf  ("%s(yac),fras   : elemento     \n",spp);    
					printf  ("global count   : %04d         \n",global_count);
				    }
				  }

              | frase elemento    { if (v_flag > 0 ) 
                                    {	printf  ("%s(yac),fras   : fras, element\n",spp);
					printf  ("global count   : %04d         \n",global_count);
                                    }
                                  }
               ;


elemento:	TXT               { if (v_flag > 0 ) printf  ("(yac),TXT    :              \n");     }
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
              | PTO               { if (v_flag > 0 ) printf  ("(yac),PTO    :              \n");     }
              | NEG               { if (v_flag > 0 ) printf  ("(yac),NEG    :              \n");     }
              | VSE               { if (v_flag > 0 ) printf  ("(yac),VSE    :              \n");     }
                ;

%%




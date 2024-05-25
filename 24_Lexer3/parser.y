%{


#include <stdio.h>
extern void yyerror(char* s);
extern int yylex();
extern int readInputForLexer(char* buffer,int *numBytesRead,int maxBytesToRead);


extern	int gc_si;
extern	int gc_no;
extern  int v_flag;

extern	char *spp;



%}

%token		NEG ENDNEG CNEG TNEG VSE OBS DET EVI VIS IDE PTO COM PTC FNL TXT NUM CES BLA 


%%

linea:          oracion             
	                          { if (v_flag > 0 ) 
	                            {	printf  ("%s(yac),line   : oracion        \n",spp);
                                    }
                                  }
	      | oracion FNL         
                                  { if (v_flag > 0 ) 
                                    {	printf  ("%s(yac),line   : oracion, FNL   \n",spp);   
                                    }
                                  }
               ;

oracion:        frase
              | oracion frase
               ;
       
frase:
                fra_cneg
                                  { gc_si++;
                                  }
              | fra_sneg
                                  { gc_no++;
                                  }
                ;

fra_cneg:
	        NEG
                ;
     
fra_sneg:
                elemento          
                                  { if (v_flag > 0 ) 
                                    {	printf  ("%s(yac),fras   : elemento     \n",spp);    
                                    }
                                  }
              | fra_sneg elemento    
                                  { if (v_flag > 0 ) 
                                    {	printf  ("%s(yac),fras   : fras, element\n",spp);
                                    }
                                  }
              | fra_sneg PTO    
                                  { if (v_flag > 0 ) 
                                    {	printf  ("%s(yac),fras   : fras, element PTO\n",spp);
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
              | VSE               { if (v_flag > 0 ) printf  ("(yac),VSE    :              \n");     }
                ;



%%




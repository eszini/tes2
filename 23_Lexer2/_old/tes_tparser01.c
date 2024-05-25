/*
 * tes_tparser01.c
 *
 * Programa simple de prueba
 * para utilizar lexx y yacc sobre un string en memoria
 *
 *
 */

#include <stdio.h>
#include <string.h>

int yyparse();
int readInputForLexer( char *buffer, int *numBytesRead, int maxBytesToRead );

static int globalReadOffset;



int	gc_si;
int	gc_no;

int	v_flag;
char   *spp      = "                            ";


/*
 * 	Estructuras para guardar el texto a parsear
 *
 */

char 	*globalInputText;
char	buffer[1024];

FILE	*finp;

int main(argc,argv) 
int argc;
char **argv;
{
	int	tok,f1,i,j;

	gc_si = 0;
	gc_no = 0;

	v_flag = 1;


	globalInputText = buffer;
	strcpy (buffer,"A B NO C D VE F G.");

	finp = fopen (argv[1],"r");


	i=0;
	while (fgets(buffer,512,finp))
	{
		printf ("(tes_tparser01) lin: %d \n%s\n",i,buffer);
		i++;

	    	/*Analyze the string*/
    		globalReadOffset = 0;
    		tok=yyparse();

		printf ("(tes_tparser01) tok: %d\n\n",tok);
	}

	printf ("----\n\nSI: %3d\nNO: %3d\n\n",gc_si,gc_no);

    return 0;
}









int readInputForLexer( char *buffer, int *numBytesRead, int maxBytesToRead ) {
    int numBytesToRead = maxBytesToRead;
    int bytesRemaining = strlen(globalInputText)-globalReadOffset;
    int i;
    if ( numBytesToRead > bytesRemaining ) { numBytesToRead = bytesRemaining; }
    for ( i = 0; i < numBytesToRead; i++ ) {
        buffer[i] = globalInputText[globalReadOffset+i];
    }
    *numBytesRead = numBytesToRead;
    globalReadOffset += numBytesToRead;
    return 0;
}



/* --- end of file --- */
/* --- end of file --- */
/* --- end of file --- */


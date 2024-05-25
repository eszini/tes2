/*
 *	2018_09_03
 *
 *	tes_filtro03  
 *
 * 
 *	filtro1	f_input  f_output   [OPTIONS]	
 *		
 *		f_input :	(inp)  archivo con texto original a limpiar
 *		f_output:	(out)  archivo nuevo con texto limpio
 *
 *		OPTIONS	:
 *				-v	verbose
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define	FMAXL	64
#define BMAXL	4096


FILE	*finp,*fout;
char	nfinp[FMAXL],nfout[FMAXL];
char	buffer1[BMAXL],buffer2[BMAXL];
int	cant_lin;

char	segmento[4][BMAXL];
char	*marcas[3] = { "  TAB1  ", "  TAB2  ", "  TAB3  " };

int verbose_flag;

static void uso(const char * progn)
{
	fprintf (stderr, 	"uso: %s  f_input f_output [OPTIONS]    ( max file len : %d ) \n\n"
				"f_input    :   (inp) Archivo con texto a limpiar \n"
				"f_output   :   (out) Archivo con texto limpio \n"
				"                -v     verbose\n",
				progn,FMAXL);
	exit(1);
}


	
char	*minusc(char *s);

int	proceso_filtrar();
int	limpiar_marca1();
int	error();


int main(int argc, char ** argv)
{

	int i1,i2,i3,ix;
	int a;
	int minarg;

	a=0;
	minarg=0;
	verbose_flag=0;


	while ( ++a < argc )
	{

		if (a == 1)
		{
			if (strlen(argv[a]) > FMAXL || argv[a][0] == '-' )
				uso(argv[0]);

			strcpy(nfinp,argv[a]);
			minarg++;
		}

		if (a == 2)
		{
			if (strlen(argv[a]) > FMAXL || argv[a][0] == '-' )
				uso(argv[0]);

			strcpy(nfout,argv[a]);
			minarg++;
		}


		if ( a > 2 )
		{
			if (argv[a][0] != '-' )
				uso(argv[0]);
			
			switch ( argv[a][1] )
			{

				case 'v':
					verbose_flag=1;
					break;


				default:
					uso(argv[0]);
					break;
			}
		}

	}


	if ( minarg != 2)
		uso(argv[0]);




	if ((finp=fopen(nfinp,"r")) == NULL)	
	{	printf ("\nfallo %s",nfinp);
		exit(0);
	}

	if ((fout=fopen(nfout,"w")) == NULL)	
	{	printf ("\nfallo %s",nfout);
		exit(0);
	}


	proceso_filtrar();


	/* cierro files */
	fclose(finp);
	fclose(fout);


	printf ("cant de lineas contadas %d \n", cant_lin );

		
}

int	proceso_filtrar()
{

	cant_lin=0;

	while ( fgets(buffer1,BMAXL,finp ) )
	{
		limpiar_marca1();

		cant_lin++;
		fprintf (fout,"%s",buffer1);
	}
}



int	limpiar_marca1()
{
	int	p1,p2,p3,p4;
	int	f1,f2,f3,f4;
	int	m1;


	p1=0;
	p2=0;
	m1=0;

	/* Busco primer segmento           */
	while ( p1 < BMAXL - strlen(marcas[m1])  &&  strncmp(buffer1+p1,marcas[m1],strlen(marcas[m1]))  )
		p1++;

	if ( p1 == BMAXL - strlen(marcas[m1]) )
	{	error("101");
	}

	strncpy(segmento[0],buffer1+p2,p1-p2);
	segmento[0][p1-p2]=0;
	p2 = p1 + strlen(marcas[m1]);


	/* Busco segundo segmento           */
	m1=1;

	while ( p1 < BMAXL - strlen(marcas[m1])  &&  strncmp(buffer1+p1,marcas[m1],strlen(marcas[m1]))  )
		p1++;

	if ( p1 == BMAXL - strlen(marcas[m1]) )
	{	error("102");
	}

	strncpy(segmento[1],buffer1+p2,p1-p2);
	segmento[1][p1-p2]=0;
	p2 = p1 + strlen(marcas[m1]);


	/* Busco tercer segmento           */
	m1=2;

	while ( p1 < BMAXL - strlen(marcas[m1])  &&  strncmp(buffer1+p1,marcas[m1],strlen(marcas[m1]))  )
		p1++;

	if ( p1 == BMAXL - strlen(marcas[m1]) )
	{	error("103");
	}

	strncpy(segmento[2],buffer1+p2,p1-p2);
	segmento[2][p1-p2]=0;
	p2 = p1 + strlen(marcas[m1]);

	/* Busco cuarto segmento */


	printf ("cuarto segmento \n");
	printf ("buffer1     %s\n",buffer1);
	printf ("p2          %d\n",p2);
	printf ("buffer1+p2  %s\n", buffer1+p2);


	strcpy(segmento[3],buffer1+p2);
	segmento[3][strlen(segmento[3])-1]=0;

#if 0
	if (segmento[3][strlen(segmento[3])-1] == 13 || segmento[3][strlen(segmento[3])-1] == 10 )
		segmento[3][strlen(segmento[3])-1]=0;
#endif


	/* limito a 3 caracteres la clasificacion */
	segmento[3][3]=0;


	sprintf(buffer1,"%s T1 %s T2 %s T3 %s\n",
		segmento[0],
		segmento[3],
		segmento[1],
		segmento[2]);


	if (verbose_flag)
	{

		printf ("Linea:  %d\n",cant_lin);
		printf ("segm1: >%s<\n",segmento[0]);
		printf ("segm2: >%s<\n",segmento[1]);
		printf ("segm3: >%s<\n",segmento[2]);
		printf ("segm4: >%s<\n",segmento[3]);
	}


}



int	error(char *st)
{
	printf ("Error %s  linea  %d\n",st,cant_lin);
	exit(1);
}



/* ----- end of fle ------ */
/* ----- end of fle ------ */
/* ----- end of fle ------ */
/* ----- end of fle ------ */
/* ----- end of fle ------ */


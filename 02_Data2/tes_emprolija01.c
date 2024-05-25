/*
 *	2017-05-05
 *
 *	tes_emprolija01.c  
 *	Emprolija un pequeno detalle en el corpus que molesta.
 *	Quita, en todas las lineas, los caracteres que sobran al final de las lineas,
 *	que pueden ser:  blancos ... tabs ... ctrl ^M (si vienen de windows ) ...
 *	
 *	Observacion:
 *	Todo el proceso se podria hacer sin esta pequena correccion, si el espiritu fuera
 *	"no tocar el corpus " ...
 *	pero igualmente obligaria a introducir la prevision del detalle en todos los programas,
 *	por lo cual se decidio arreglar desde el principio.
 *	De querer mantener el espiritu mencionado, habría que agregar en todos los programas que
 *	procesan las lineas del corpus, la rutina que ajusta el fin de linea.
 *	
 *
 *	Input:
 *	Archivo de corpus, con formato :
 *	nro registro TAB referencia para busqueda de negacion TAB frase TAB Affirmed | Negated | Possibility
 *
 *
 *	Output:
 *	Nuevo archivo de corpus, eliminando todos los caracteres sobrantes en el fin de linea.
 *
 *
 *	tes_verifica01 	f_input  f_output   [OPTIONS]	
 *		
 *	f_input :	(inp)  Archivo con corpus input
 *	f_output:	(out)  Archivo con corpus output
 *
 *	OPTIONS	:
 *			-v	verbose
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
char	*marcas[3] = { " TAB1 ", " TAB2 ", " TAB3 " };

int verbose_flag;

static void uso(const char * progn)
{
	fprintf (stderr, 	"uso: %s  f_input f_output [OPTIONS]    ( max file len : %d ) \n"
				"f_input    :   (inp) Archivo con texto corpus a reviar       \n"
				"f_output   :   (out) Archivo con resultados                  \n"
				"               -v    verbose                                 \n",
				progn,FMAXL);
	exit(1);
}


	
char	*minusc(char *s);

int	proceso_verificar1();
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


	proceso_verificar1();


	/* cierro files */
	fclose(finp);
	fclose(fout);
		
}

/*
 *	Affirmed
 *	Negated
 *	Possibility
 *
 */


int	proceso_verificar1()
{

	int	i,j,k,q1,q2;
	int	f1,f2,f3,fl;
	int	fa,fn,fp;
	int	qa,qn,qp;
	int	qt;

	cant_lin=0;
	q1 = q2 = 0;


	while ( fgets(buffer1,BMAXL,finp ) )
	{
		if (verbose_flag)
		{
			printf ("Reg: %6d %s\n",cant_lin,buffer1);
		}


		for ( fl=0, i=strlen(buffer1)-1; !fl && i; i-- )
		{
			if ( buffer1[i] == '\t' || buffer1[i] == ' ' || buffer1[i] == 13  || buffer1[i] == '\n' )
			{
				buffer1[i]=0;
			}
			else
			{	
				fl=1;
			}
		}



		fprintf (fout,"%s\n",buffer1);

		cant_lin++;
	}

	/* Fin de proceso .. */
	if (verbose_flag)
	{
		printf ("Cant de lineas procesadas: %6d\n\n",cant_lin);
		printf ("\n\n");
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
	strcpy(segmento[3],buffer1+p2);
	segmento[3][strlen(segmento[3])-1]=0;

	if (segmento[3][strlen(segmento[3])-1] == 13 )
		segmento[3][strlen(segmento[3])-1]=0;


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


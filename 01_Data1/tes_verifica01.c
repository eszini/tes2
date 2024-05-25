/*
 *	2017-05-05
 *
 *	tes_verifica01.c  
 *	Realiza tareas de verificacion en el archivo de corpus, 
 *	necesarias para los procesos posteriores.
 *
 *	Input:
 *	Archivo de corpus, con formato :
 *	nro registro TAB referencia para busqueda de negacion TAB frase TAB Affirmed | Negated | Possibility
 *
 *	Verifica:
 *	a) que todas las lineas tengan 3 tabs 
 *	b) que todas las lineas tengan uno de Affirmed , Negated o Possibility 
 *
 *
 *	Output:
 * 	genera un archivo con los resultados / lineas halladas con problemas ...
 *
 *
 *
 *	tes_verifica01 	f_input  f_output   [OPTIONS]	
 *		
 *	f_input :	(inp)  Archivo con corpus a revisar
 *	f_output:	(out)  Archivo con resultados 
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
				buffer1[i]='\n';
				buffer1[i+1]=0;
			}
			else
			{	
				fl=1;
			}
		}


		for ( qt=0, i=0; i < strlen(buffer1); i++ )
			if (buffer1[i] == '\t' )
				qt++;

		
		fa = fn = fp = 0;
		for ( f1=0, i=0; !f1 && i < strlen(buffer1); i++)
		{
			if (strncmp(buffer1+i,"Affirmed",8) == 0)
			{
				f1=1;
				qa++;
			}

			if (strncmp(buffer1+i,"Negated",7) == 0)
			{
				f1=1;
				qn++;
			}

			if (strncmp(buffer1+i,"Possibility",11) == 0)
			{
				f1=1;
				qp++;
			}
		}

		if (!f1)
		{
			if (verbose_flag)
			{
				printf ("SIN A/N/P : %6d %30s\n",cant_lin,buffer1);
			}

			fprintf (fout,"SIN: %s\n",buffer1);
			q1++;
		}

		if (qt != 3)
		{
			if (verbose_flag)
			{
				printf ("TAB cant  : %6d %30s\n",cant_lin,buffer1);
			}

			fprintf (fout,"TAB: %s\n",buffer1);
			q2++;

		}



		cant_lin++;
	}

	/* Fin de proceso .. */
	if (verbose_flag)
	{
		printf ("Cant de lineas procesadas: %6d\n\n",cant_lin);
		printf ("Cant de lineas SIN clasif: %6d\n",q1);
		printf ("Cant de lineas TAB incorr: %6d\n",q2);
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


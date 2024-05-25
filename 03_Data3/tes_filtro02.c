/*
 *	2017-05-05
 *
 *	filtro2  
 *	Realiza cambios en lineas de corpus para futuros procesos
 *
 * 	parte de lineas con formato:
 * 	<codigo> "TAB1" <referencia para busqueda de negacion> "TAB2" <frase> "TAB3"  afirmed | negated | posibility
 *
 * 	genera un archivo con
 * 	<frase>
 *
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
	fprintf (stderr, 	"uso: %s  f_input f_output [OPTIONS]    ( max file len : 30 ) \n\n"
				"f_input    :   (inp) Archivo con texto a limpiar \n"
				"f_output   :   (out) Archivo con texto limpio \n"
				"                -v     verbose\n",
				progn);
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


	if (verbose_flag)
	{
		printf ("cant de lineas contadas %d \n", cant_lin );
	}

		
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
	int	m1,m2;


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


	/* Cierro la sentencia con un punto, para funcionamiento de los parsers */
	if ( segmento[2][strlen(segmento[2])-1] != '.' )
	{	m2=strlen(segmento[2]);
		segmento[2][m2] = '.';
		segmento[2][m2+1]=0;

	}

	sprintf(buffer1,"%s\n",
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


#if 0

int	limpiar_marca2()
{
	int	p1,p2,p3,p4;
	int	f1,f2,f3,f4;


	p1=strlen(buffer1);


	/* Saco el ultimo string entre < > ( evito los < > intermedios .. ( los cue ) */
	while ( buffer1[p1] != '>' && p1 > 0 )
		p1--;

	if ( p1 != 0 )
	{ 
		strcpy(buffer1+p1-1,".\n");
	}

}




#endif



int	error(char *st)
{
	printf ("Error %s  linea  %d\n",st,cant_lin);
	exit(1);
}


#if 0


int	gen_lex(int cant)
{
	int	i1,i2,i3,ix,iy;
	int	index;

	char	linea[500];



	while (  fgets(buffer,200,flex) )
	{
		switch ( buffer[0] )
		{
			case '&':
				for ( i1=0 ; i1<cant; i1++ )
				{
					strcpy (linea,buffer);
					ix=iy=0;
					while (buffer[ix] )
					{
						if (buffer[ix]=='&' )
						{
							index= *(buffer+ix+4) - '0';
							strncpy ( linea+iy,tabla[i1][index],strlen(tabla[i1][index]));
							ix=ix+5;
							iy=iy+strlen(tabla[i1][index]);
							linea[iy]=0;


						}
						else
						{
							linea[iy++]=buffer[ix++];
							linea[iy]=0;
						}
	
					}	
					fprintf (fout,"%s",linea);

				}
			
				break;


			default:
				fprintf (fout,"%s",buffer);
				break;


		}

	}






}





/*
 *
 *	lectura_tabla
 *	
 *	lee la tabla de reg exp y parsea 
 *
 */

int	lectura_tabla(int *cant)
{

	int j;
	int i1,i2,i3,ix;


	j=0;
	while ( fgets(buffer,200,finp))
	{
		i1=0;
		i2=0;
		ix=0;
		
		for ( ; buffer[ix] != '\t' ; )
		{	tabla[j][0][i1]=buffer[ix];
			i1++;
			ix++;
		}
		
		for ( ; buffer[ix] == '\t' ; )
			ix++;
		
		for ( ; buffer[ix] != '\n' ; )
		{	tabla[j][1][i2]=buffer[ix];
			i2++;
			ix++;
		}


		tabla[j][0][i1]=0;
		tabla[j][1][i2]=0;

		j++;
	}



	*cant=j;
}


/*
 *
 *	gen_regexp
 *	
 *	genera reg exp para elementos de la tabla
 *
 */

int	gen_regexp(int cant)
{

	int j;
	int i1,i2,i3,ix;


	for ( ix=0 ; ix<cant; ix++ )
	{


		strcpy ( tabla[ix][2], minusc(tabla[ix][0]));


	}


}



char	*minusc(char *s)
{
	static char buffer[30];
	int i1,ix;
	
	for (ix=0; ix<strlen(s); ix++)
	{	if (s[ix] >= 'a' && s[ix] <= 'z')
			buffer[ix]=s[ix];
		if (s[ix] >= 'A' && s[ix] <= 'Z')
			buffer[ix]=s[ix]-'A'+'a';
		buffer[ix+1]=0;
	}

	return buffer;
}
	
#endif


/* ----- end of fle ------ */
/* ----- end of fle ------ */
/* ----- end of fle ------ */
/* ----- end of fle ------ */
/* ----- end of fle ------ */


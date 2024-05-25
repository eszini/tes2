/*
 *	tes_parser04.c
 *	
 *	parser - tokenizador
 *	(puede usar diccionario)
 *	mejorar conexion con yacc .... 
 *
 * 	Opciones:
 * 	-h   		forma de uso
 *
 * 	( mas ejemplos abajo )
 */


/*
 * 	Ejemplos de uso:
 *
 * 	para el ejemplo ...
 *
 *
 *
 *
 * 	f04_neg.txt               archivo de input, con texto de trabajo original, formato sentencias
 *
 *
 *
 *
 *
 *	1) Generar en salida, todos los tokens del archivo de input, sin modificar, en formato columnar ( 1 tkn x linea)
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt
 *
 * 	./cpar05 f04_neg.txt f04_neg_tkn.txt
 *
 * 	2) Idem, pero forzar al final de cada linea, el token "EOL"
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e
 *
 * 	3) Idem, forzando token "EOL" y pasando todo a minuscula
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m
 *
 * 	4) Idem, forzando token "EOL" , pasando todo a minuscula, y generando formato sentencia ( no columnar )
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m -f
 *
 * 	5) Idem anterior, con alguna informacion de salida 
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m -f -v
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m -f -v  >  ver
 * 	cat ver ... etc
 *
 * 	6) Solamente SIN -f  ...  Agrega informacion en archivo de salida para controlar funcionamiento
 *
 * 	./cpar05 f04_neg.txt f04_neg_tkn.txt -nvd=1
 *
 * 	7) Para usar con un archivo de "marcas" ( para taggear ... )  Al final de este fuente, ejemplo de marcas !!!
 *
 * 	   Atencion !!!  La busqueda en la tabla de marcas NO es case sensitivo ... considerar el uso de -m 
 *
 * 	./cpar05 f04_neg.txt f04_neg_tag.txt -mrk=f04_t1.mrk -f -m -v
 *
 * 	8) Para usar con marcas, pero salida en formato un token por fila, con tag incluido ...
 *
 * 	./cpar05 f04_neg.txt f04_neg_tag.txt -mrk=f04_t1.mrk -m -v
 *
 *
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	DEBUG 0


#define	GP_SET 1
#define	GP_GET 0


int	gp_default();
int	gp_init(int ,char **);
int	gp_test();
int	gp_print();
int	gp_parser();
int	gp_fq(int,int);
char	*gp_fp(int,int,char **);

int	gp_q_partype1=0;	/* parameter type "name" */
int	gp_q_partype2=0;	/* parameter type "-something" */
int	gp_q_partype3=0;	/* parameter type "-someoption=somename" */


int	gp_verbose=0;		/* verbose 0 no 1 si */
int	gp_help=0;		/* help 0 no 1 si */
int	gp_diccionario=0;	/* diccionario 0 no 1 si */
int	gp_minusculas=0;	/* output en minuscula 0 no 1 si */
int	gp_fsentencia=0;	/* archivo de salida en formato sentencias 0 no 1 si */
int	gp_eol=0;		/* fuerzo string EOL al final de la linea ... a veces hay . en medio de la linea */
int	gp_niveldes=0;		/* nivel de descripcion que se vuelca en archivo de salida (solo con fsentencia = 0 ) */
				/* 0 normal 1 sentencia y numero de token 2 .... agrego cosas del diccionario si se usa */
int	gp_tabmrk=0;		/* usar archivo de marcas 0 no 1 si */


char	*desde_igual(char *s);
char	*pasar_a_minusc(char *s);

int	separar_seg(char *);

int	tiene_igual(char *);
int	linea_vacia(char *);
int	tipo_char(char );
int	es_numero(char);
int	es_num_tk(char *);
int	es_word(char *);
int	es_puntuacion(char *);
int	char_demed(char);
int	mostrar(int,int,char *);
int	clasificar(int );

int	error(int);
int	uso(int);

#define	TC_EOL	0
#define	TC_BLA	1
#define	TC_CCE	2	/* algunos caracteres especiales  ',;:/' */
#define	TC_PNT	3	/* punto '.' */
#define	TC_PAA	4	/* parent '([{' */
#define TC_PAC	5	/* parent ')]}' */
#define	TC_LET	6	/* letras */
#define	TC_NUM	7	/* numero */
#define	TC_CVR	8	/* caracteres varios ' % */
#define	TC_EOS	91	/* fin de string en C ... 0 */
#define	TC_RST	99

int	parser1();

#define	MAXT	200
#define	MAXB	1024
#define	MAXF	64

char	finp[MAXF];
char	fout[MAXF];
char	fdic[MAXF];
char	fmrk[MAXF];

FILE	*hfinp;
FILE	*hfout;
FILE	*hfdic;
FILE	*hfmrk;


int	flag_caracteres;


/*	Estructuras y variables para la base de marcas */

#define	MAXK	64	/* cantidad de marcas */
#define	MAXM	20	/* tamano maximo de palabra / marca */

int	bm_load();
char	*bm_tag(char *);


char	bm_key[MAXK][MAXM];
char	bm_mrk[MAXK][MAXM];

int	q_bm;		/* cantidad de marcas en base */



/*	Estructuras y variables para base de datos (diccionario ) */



int	q_wrd;				/* palabras en diccionario */

int	bd_load();


typedef	struct	tnode	*nodeptr;
typedef	struct	tnode
{	char	*wrd;
	int	num;
	struct	tnode	*nx;
}	node;

nodeptr	npx,np1,*npa;



/*
 * definicion antigua ...
 *
 */

typedef	struct
{	struct	palabra	*nx;
	int	ty;
	char	*tx;	
}	bd_type;


bd_type	*bd_first ;











/* Estructuras para guardar segmentos de la linea
 * seg 0   nro registro
 * seg 1   aff / neg / pos 
 * seg 2   palabra de referencia
 * seg 3   frase 
 *
 */

char	segmento[4][MAXB];
char	*marcas[3] = { " T1 ", " T2 ", " T3 " };



int	q_mkc;
char	mkc[][64] = { "no/ST 3 se/ST 2 obser/SN 3 REF" };

/*
 * Reglas de clasificacion
 */
 
int	nregla;
int	fregla;

int	pres_reglas();

int	cant_reglas = 7;

char	*regla1 = " no/ST   <tk ... >  se/ST    <tk ... >  obser/SN | visua/SN | ident/SN | detec/SN | evide/SN | ven/ST | produ/SN  <tk ... >  REF " ;

char	*regla2 = " no/ST   <tk ... >  obser/SN | prese/SN <tk ... >  REF ";

char	*regla3 = " sin/ST  <tk ... >  evide/SN <tk ... >  REF ";

char	*regla4 = " no/ST   <tk ... >  se/ST    <tk ... >  obser/SN | visua/SN | ident/SN | detec/SN   <tk ... > ni/ST | compat/SN  <tk ... >  REF ";

char	*regla5 = " sin/ST  <tk ... >  obser/SN | visua/SN   <tk ... >   ni/ST  <tk ... >  REF ";

char	*regla6 = " no/ST | sin/ST  REF ";

char	*regla7 = " no/ST   <tk ... >  se/ST    <tk ... >   logra/SN  <tk ... > obser/SN | visua/SN | ident/SN | detec/SN   <tk ... >   REF ";




int	pres_reglas()
{
	int i;
	char	*regla;
	
	for (i=0; i< cant_reglas; i++)
	{	switch (i)
		{
			case 0:
				regla = regla1;
				break;
			case 1:
				regla = regla2;
				break;
			case 2:
				regla = regla3;
				break;
			case 3:
				regla = regla4;
				break;
			case 4:
				regla = regla5;
				break;
			case 5:
				regla = regla6;
				break;
			case 6:
				regla = regla7;
				break;

			default:
				printf ("error en reglas !!! \n");
				error(901);
				break;

		}
		printf ("Regla nro %2d:  \"%s\"  \n",i,regla);
	}

}


/*
 * Estadisticas
 *
 */


int	q_reg_total;
int	q_reg_clasificado0;
int	q_reg_clasificado1;




/*----------------------------------------------------------------------------
 *
 *	main
 *
 */

int	main(argc,argv)
int	argc;
char	**argv;
{

	gp_default();
	gp_init(argc,argv);
	gp_parser();
	gp_print();

	if (gp_help)
	 	uso(0);


	if (gp_verbose)
	{
		printf ("inp %s\n",finp);
		printf ("out %s\n",fout);
		printf ("\n");
	}

	
	if ( (hfinp = fopen (finp,"r")) == NULL )
	{
		error(101);
	}

	if ( (hfout = fopen (fout,"w")) == NULL )
	{
		error(102);
	}


	if (gp_diccionario)
	{
		if ( (hfdic = fopen (fdic,"r")) == NULL )
		{
			error(103);
		}


		bd_load();
	}

	if (gp_tabmrk)
	{
		if ( (hfmrk = fopen (fmrk,"r")) == NULL )
		{
			error(104);
		}

		bm_load();
	}


	parser1();


	fclose(hfinp);
	fclose(hfout);
	if (gp_diccionario)
		fclose(hfdic);
	if (gp_tabmrk)
		fclose(hfmrk);

	if (gp_verbose)
		printf ("fin proceso \n");

	if (flag_caracteres)
	{	
		printf ("Atencion: caracteres no contemplados para parser encontrados en input  \n");
		printf ("Volver a ejecutar con flag -v y revisar salida (buscar \"Caracteres\" ... ) \n");
	}

}


/*
 * 	parser1
 * 	parser de linea de texto
 *
 *
 */

	char	tk[MAXT][MAXB];

int	parser1()
{

	int	i,j,k;
	int	m1,m2,m3;
	int	f1,f2,f3;
	int	q_lin;
	int	q_tk;
	int	p1,p2,p3,p4;
	int	tclas;

	char	b1[MAXB];
	char	b2[MAXB];


	/* inicializo estadisticas */
	q_reg_total = 0;
	q_reg_clasificado0 = 0;
	q_reg_clasificado1 = 0;


	/* si encuentro caracteres no considerados para el parser, avisar al final de todo el proceso */
	flag_caracteres = 0;


	q_lin=0;
	while (fgets(b1, MAXB, hfinp) != NULL)
	{
		/* procesar solo lineas no vacias */
		if ( !linea_vacia(b1))
		{
			f1=1;

			if (gp_verbose)
			{
				printf ("%s\n\n",b1);
			}
		}


		/* descomponemos la linea de entrada en los 3 segmentos */
		if (f1)
		{
			strcpy(b2,b1);
			separar_seg(b2);
			if (gp_verbose)
			{
				printf ("Linea:  %d \n",q_lin);
				printf ("segm1: >%s<\n",segmento[0]);
				printf ("segm2: >%s<\n",segmento[1]);
				printf ("segm3: >%s<\n",segmento[2]);
				printf ("segm4: >%s<\n",segmento[3]);
				printf ("\n");
			}
		}

		p1=0;
		q_tk=0;

		while ( f1 )
		{

			/* controlamos cantidad de tokens ... */
			if (q_tk > MAXT-10)
			{	error(501);
			} 

	


			/* parseamos solo la frase */
			strcpy(b1,segmento[3]);


			/* caracter p1 del buffer */
			j=tipo_char(b1[p1]);


			switch (j)
			{
				/* otro caracter !!! */
				case TC_RST:
					flag_caracteres=1;
					if (gp_verbose)
					{
						printf ("Caracter no definido para parser: %c %d \n",b1[p1],(int)b1[p1]);
					}
					p1++; 
					break; 

				/* si por alguna causa llega un 0 de c, fin de string ...*/
				case TC_EOS:
					if (gp_verbose)
					{
						printf ("Caracter fin de string !!       : %c %d \n",b1[1],(int)b1[p1]);
					}
					f1=0;
					break; 

				/* letras */
				/* en los corpus vistos, hay muchas siglas del tipo LD4 ...
				 * que para este caso, deberia ser considerado una palabra o termino
				 */
				case TC_LET:
					p2=0;
					while ( (j=tipo_char(b1[p1])) == TC_LET || \
					        (j == TC_NUM && !char_demed(b1[p1-1])  ) )
						tk[q_tk][p2++]=b1[p1++];
					tk[q_tk][p2]=0;
					q_tk++;
					break;

				/* numeros tenemos que contemplar 3.3 o 3,3 !! */
				case TC_NUM:
					p2=0;
					while ( (j=tipo_char(b1[p1])) == TC_NUM || \
						(tipo_char(b1[p1]) == TC_PNT && tipo_char(b1[p1+1]) == TC_NUM ) || \
						( (b1[p1]) == ',' && tipo_char(b1[p1+1]) == TC_NUM ) )
					{	tk[q_tk][p2]=b1[p1];
						p1++;
						p2++;
					}
					tk[q_tk][p2]=0;
					q_tk++;
					break;

				/* blanco o tab */
				case TC_BLA:
					while ( (j=tipo_char(b1[p1])) == TC_BLA)
					       p1++;	
					break;

				/* coma */
				case TC_CCE:
					tk[q_tk][0]=b1[p1];
					tk[q_tk][1]=0;
					q_tk++;
					p1++;
					break;

				/* punto */
				case TC_PNT:
					tk[q_tk][0]=b1[p1];
					tk[q_tk][1]=0;
					q_tk++;
					p1++;
					break;

				/* parentesis abre */
				case TC_PAA:
					tk[q_tk][0]=b1[p1];
					tk[q_tk][1]=0;
					q_tk++;
					p1++;
					break;

				/* parentesis cierra */
				case TC_PAC:
					tk[q_tk][0]=b1[p1];
					tk[q_tk][1]=0;
					q_tk++;
					p1++;
					break;

				/* fin de linea */
				case TC_EOL:
					tk[q_tk][0]=b1[p1];
					tk[q_tk][1]=0;
					p1++;
					f1=0;
					break;

				/* caracteres varios */
				case TC_CVR:
					tk[q_tk][0]=b1[p1];
					tk[q_tk][1]=0;
					q_tk++;
					p1++;
					break;

				/* fin de linea */
				default:
					printf ("Default, algo salio mal  !!!\n\n");
					f1=0;
					break;
			}
		}

		/* verifico si hay que sacar output  en minusculas */
		if (gp_minusculas)
		{
			for (j=0; j< q_tk; j++)
				strcpy(tk[j],pasar_a_minusc(tk[j]));
		}

		/* si esta usando tabla de marcas ... verificar si hay que taggear */
		if (gp_tabmrk)
		{
			for (j=0; j< q_tk; j++)
				if (es_word(tk[j]) || es_puntuacion(tk[j]) )
					strcpy(tk[j],bm_tag(tk[j]));
		}


		/* hay que forzar  string EOL al final de la linea */
		if (gp_eol)
			strcpy(tk[q_tk++],"EOL");


		/* salida en formato token columnar */
		if (gp_fsentencia == 0)
		{


			/* pidio nivel de descripcion en salida ... agrego la sentencia */
			if (gp_niveldes)
				fprintf (hfout,"%s\n",b1);


			/* grabo los tokens encontrados */
			for (j=0; j< q_tk; j++)
			{
				switch (gp_niveldes)
				{

					case 0:
						fprintf (hfout,"%s\n",tk[j]);
						break;

					case 1:
						fprintf (hfout,"%3d,%s\n",j,tk[j]);
						break;

					default:
						fprintf (hfout,"%s\n",tk[j]);
						break;
				}


				if (gp_verbose)
					printf ("%3d,%s\n",j,tk[j]);

			}



#if 0
			/* agrego termino EOL para indentificar donde termino
			 * esto ya que tal vez haya un . en mitad de linea 
			 */
			if (gp_eol)
				fprintf (hfout,"%s\n","EOL");

#endif
		}



		/* salida en formato sentencia */
		if (gp_fsentencia == 1)
		{


			/* grabo los tokens encontrados */
#if 0
			fprintf (hfout,"%s\n",b1);
#endif

			for (j=0; j< q_tk; j++)
			{
#if 0
				fprintf (hfout,"%3d,%s\n",j,tk[j]);
#endif
				fprintf (hfout,"%s ",tk[j]);

				if (gp_verbose)
					printf ("%3d,%s\n",j,tk[j]);

			}

#if 0
			/* agrego termino EOL para indentificar donde termino
			 * esto ya que tal vez haya un . en mitad de linea 
			 */
			if (gp_eol)
				fprintf (hfout,"%s\n","EOL");
			else
				fprintf (hfout,"\n");
#endif


			/* se termino la linea */
			fprintf (hfout,"\n");

		}


		if (gp_niveldes)
			fprintf (hfout,"\n\n");


		if (gp_verbose)
		{
			printf ("\n");
		}


		/*
		 * En este punto tengo ...
		 * a) b1            la linea completa que cargue del archivo
		 * b) q_lin         el numero de lineas que procese hasta el momento
		 * c) q_tk          cantidad de tokens de esa linea
		 * d) tk[i]         token numero i de esa linea
		 * e) segmento[0]   nro de registro
		 * f) segmento[1]   aff neg o pos
		 * g) segmento[2]   la palabra de referencia a utilizar para ver en la negacion
		 * h) segmento[3]   la frase
		 *
		 */
		if (gp_verbose)
		{
			printf ("Resumen de la linea: \n");
			printf ("b1        : |%s|\n",b1);
			printf ("q_lin     :  %d \n",q_lin);
			printf ("q_tk      :  %d \n",q_tk);
			printf ("nro       :  %s \n",segmento[0]);
			printf ("clasific  :  %s \n",segmento[1]);
			printf ("palabra   :  %s \n",segmento[2]);
			printf ("frase     :  %s \n",segmento[3]);
			printf ("-----\n\n");
		}

		tclas = clasificar(q_tk);

		if (tclas == 0)
			q_reg_clasificado0++;
		else
			q_reg_clasificado1++;

		if (gp_verbose)
		{

			printf ("Clasificar:   %d  regla: %d  \n\n\n",tclas,nregla);
			printf ("Sentencia :  \n");
			printf ("nro       :  %s \n",segmento[0]);
			printf ("clasific  :  %s \n",segmento[1]);
			printf ("palabra   :  %s \n",segmento[2]);
			printf ("frase     :  %s \n",segmento[3]);
			printf ("\n\n\n");
			printf ("-----------------------------------------------------\n");
			printf ("\n\n\n");

		}


		/* sumo lineas */
		q_lin++;


	}

	
	if (gp_verbose)
		printf ("Cant de lineas %d\n",q_lin);


	/* Ímprimo algunas estadisticas */
	q_reg_total = q_lin;


	if (gp_verbose)
	{
		printf ("\n\n\n\n");
		printf ("Estadisticas: \n");
		printf ("Cant de registros       :  %4d\n",q_reg_total);
		printf ("Cant de reg clasif en 0 :  %4d\n",q_reg_clasificado0);
		printf ("Cant de reg clasif en 1 :  %4d\n",q_reg_clasificado1);
		printf ("\n");
	}


}


/*
 *	clasficar 
 *	Aplica tipo de gramatica "in casa"
 *	para aproximar si la frase es neg o aff 
			

	printf ("nro       :  %s \n",segmento[0]);
	printf ("clasific  :  %s \n",segmento[1]);
	printf ("palabra   :  %s \n",segmento[2]);
	printf ("frase     :  %s \n",segmento[3]);
 *
 */


int	clasificar(q_tk)
int	q_tk;
{

	int	i,j,k,l,m;
	int	n1,n2,n3,n4;
	int	f1,f2,f3;
	int	tclas;

	printf ("CLASIFICO %d toks !!!!!!!!!! \n",q_tk);





	pres_reglas();

	/* asumimos que NO clasifica */
	tclas = 0;
	nregla = 0;


	/* Regla 1 */
	/* las del tipo ...  " no  <tk .. >  se  <tk ... >  obser|visua|ident   <tk ... >  PALABRA  "   */
	printf ("\nCLA: Regla 1 \n\n");


	f1=0;
	for ( j=0; j<q_tk && !f1 && !tclas; j++)
	{

		/* no  .. se .. obser / visua / ident ... REF  */

		if (!f1 && !tclas)
		{
			printf ("CLA: comparo con : %d  %s\n",j,tk[j]);

			if (strcmp(tk[j],"no") == 0)
			{


				printf ("CLA:  pase el 'no ' \n");


				for (i=j+1,n1=0; i< q_tk && n1<1 && !tclas ; i++,n1++   )
				{
					if (strcmp(tk[i],"se") == 0 )
					{	
						for (k=i+1,n2=0, f2=0; k< q_tk && n2<1 && !tclas ; k++,n2++   )
						{
							
							printf ("CLA Entro a 4 ifs seguidos ... |%s| \n",tk[k]);


							if (!f2 && strncmp(tk[k],"obser",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"visua",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"ident",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"detec",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"evide",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"produ",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"ven",3) == 0 )
							{
								f2=1;
							}

							if (f2)
							{
								for (l=k+1, n3=0; l< q_tk && n3 < 4 && !tclas ; l++,n3++ )
								{
									if (strcmp(tk[l],segmento[2]) == 0 )
										tclas=1,nregla=1;
								}
							}
						}
					}



				}
			}
		}


	}  /* for */




	printf ("\nCLA: Regla 2 \n\n");

	for ( j=0; j<q_tk && !f1 && !tclas; j++)
	{
		/* no  .. obser / prese ... REF  */

		if (!f1 && !tclas)
		{
			printf ("CLA: comparo con : %d  %s\n",j,tk[j]);

			if (strcmp(tk[j],"no") == 0)
			{


				printf ("CLA:  pase el 'no ' \n");


				for (i=j+1,n1=0, f3=0; i< q_tk && n1<1 && !tclas ; i++,n1++   )
				{
					if (!f3 && strncmp (tk[i],"obser",5) == 0)
					{	
						f3 = 1;
					}

					if (!f3 && strncmp (tk[i],"prese",5) == 0)
					{	
						f3 = 1;
					}


					if ( f3 )
					{	
						for (k=i+1,n2=0, f2=0; k< q_tk && n2<1 && !tclas ; k++,n2++   )
						{

							if (strcmp(tk[k],segmento[2]) == 0 )
								tclas=1,nregla=2;
						}
					}



				}
			}
		}
	}





	printf ("\nCLA: Regla 3 \n\n");

	for ( j=0; j<q_tk && !f1 && !tclas; j++)
	{
		/* sin   .. edviden ... REF  */

		if (!f1 && !tclas)
		{
			printf ("CLA: comparo con : %d  %s\n",j,tk[j]);

			if (strcmp(tk[j],"sin") == 0)
			{


				printf ("CLA:  pase el 'sin ' \n");


				for (i=j+1,n1=0; i< q_tk && n1<1 && !tclas ; i++,n1++   )
				{
					if (strncmp(tk[i],"eviden",6) == 0 )
					{	
						for (k=i+1,n2=0, f2=0; k< q_tk && n2<2 && !tclas ; k++,n2++   )
						{

							if (strcmp(tk[k],segmento[2]) == 0 )
								tclas=1,nregla=3;
						}
					}

				}
			}
		}
	}




	printf ("\nCLA: Regla 4 \n\n");


	for ( j=0; j<q_tk && !f1 && !tclas; j++)
	{

		/* no  .. se .....  obser / visua / ident ...   ni ... REF  */

		if (!f1 && !tclas)
		{
			printf ("CLA: comparo con : %d  %s\n",j,tk[j]);

			if (strcmp(tk[j],"no") == 0)
			{

				printf ("CLA:  pase el 'no ' \n");


				for (i=j+1,n1=0; i< q_tk && n1<1 && !tclas ; i++,n1++   )
				{
					if (strcmp(tk[i],"se") == 0 )
					{	
						for (k=i+1,n2=0, f2=0; k< q_tk && n2<1 && !tclas ; k++,n2++   )
						{
							
							printf ("CLA Entro a 4 ifs seguidos ... |%s| \n",tk[k]);


							if (!f2 && strncmp(tk[k],"obser",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"visua",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"ident",5) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[k],"detec",5) == 0 )
							{
								f2=1;
							}

							if (f2)
							{
								for (l=k+1, n3=0, f3=0; l< q_tk && n3 < 6 && !tclas ; l++,n3++ )
								{

									if (!f3 && strncmp(tk[l],"ni",2) == 0)
									{
										f3 = 1;
									}
									if (!f3 && strncmp(tk[l],"compat",6) == 0)
									{
										f3 = 1;
									}

								
									if (f3)
									{

										for (m=l+1, n4=0; m < q_tk && n4 < 2 &&  !tclas; m++,n4++)
										{

											if (strcmp(tk[m],segmento[2]) == 0 )
												tclas=1,nregla=4;
										}

									}
								}
							}
						}
					}



				}
			}
		}


	}  /* for */




	printf ("\nCLA: Regla 5 \n\n");



	for ( j=0; j<q_tk && !f1 && !tclas; j++)
	{

		/* sin  ....  obser / visua  ...   ni ... REF  */

		if (!f1 && !tclas)
		{
			printf ("CLA: comparo con : %d  %s\n",j,tk[j]);

			if (strcmp(tk[j],"sin") == 0)
			{


				printf ("CLA:  pase el 'sin' \n");


				for (i=j+1,n1=0; i< q_tk && n1<1 && !tclas ; i++,n1++   )
				{
							
							printf ("CLA Entro a 2 ifs seguidos ... |%s| \n",tk[k]);


							if (!f2 && strncmp(tk[i],"eviden",6) == 0 )
							{
								f2=1;
							}
							if (!f2 && strncmp(tk[i],"visual",6) == 0 )
							{
								f2=1;
							}

							if (f2)
							{
								for (l=i+1, n3=0, f3=0; l< q_tk && n3 < 6 && !tclas ; l++,n3++ )
								{

									if (strcmp(tk[l],"ni") == 0)
									{

										for (m=l+1, n4=0; m < q_tk && n4 < 2 &&  !tclas; m++,n4++)
										{

											if (strcmp(tk[m],segmento[2]) == 0 )
												tclas=1,nregla=5;
										}

									}
								}
							}


				}
			}
		}


	}  /* for */




	/* Regla 6 */
	/* las del tipo ...  " no  PALABRA  "   */
	printf ("\nCLA: Regla 6 \n\n");


	for ( j=0; j<q_tk && !f1 && !tclas; j++)
	{

		/* no REF  */

		if (!f1 && !tclas)
		{
			printf ("CLA: comparo con : %d  %s\n",j,tk[j]);

			if (strcmp(tk[j],"no") == 0 || strcmp(tk[j],"sin") == 0 )
			{


				printf ("CLA:  pase el 'no/sin ' \n");


				for (i=j+1,n1=0; i< q_tk && n1<1 && !tclas ; i++,n1++   )
				{
					
					printf ("Comparando:  |%s|  |%s|   \n",tk[i],segmento[2]);
					if (strcmp(tk[i],segmento[2]) == 0 )
						tclas=1,nregla=6;

				}
			}
		}


	}  /* for */






	printf ("\nCLA: Regla 7 \n\n");


	for ( j=0; j<q_tk && !f1 && !tclas; j++)
	{

		/* no  .. se ..... logra/logrado ...  obser / visua / ident ...  REF  */

		if (!f1 && !tclas)
		{
			printf ("CLA: comparo con : %d  %s\n",j,tk[j]);

			if (strcmp(tk[j],"no") == 0)
			{

				printf ("CLA:  pase el 'no ' \n");


				for (i=j+1,n1=0; i< q_tk && n1<1 && !tclas ; i++,n1++   )
				{
					if (strcmp(tk[i],"se") == 0 )
					{	
						for (k=i+1,n2=0, f2=0; k< q_tk && n2<2 && !tclas ; k++,n2++   )
						{
							
							printf ("CLA Entro a 1 ifs seguidos ... |%s| \n",tk[k]);


							if (!f2 && strncmp(tk[k],"logra",5) == 0 )
							{
								f2=1;
							}

							if (f2)
							{
								for (l=k+1, n3=0, f3=0; l< q_tk && n3 < 4 && !tclas ; l++,n3++ )
								{

									if (!f3 && strncmp(tk[l],"ident",5) == 0)
									{
										f3 = 1;
									}
									if (!f3 && strncmp(tk[l],"obser",5) == 0)
									{
										f3 = 1;
									}
									if (!f3 && strncmp(tk[l],"detec",5) == 0)
									{
										f3 = 1;
									}

								
									if (f3)
									{

										for (m=l+1, n4=0; m < q_tk && n4 < 2 &&  !tclas; m++,n4++)
										{

											if (strcmp(tk[m],segmento[2]) == 0 )
												tclas=1,nregla=7;
										}

									}
								}
							}
						}
					}



				}
			}
		}


	}  /* for */

















	/* Devolvemos true o false segun se pudo clasificar la sentencia en positivo */
	return tclas;
}










/*
 *	separar_seg
 *	Separa en segmentos la linea de entrada
 *
 *
 */



int	separar_seg(buffer1)
char	*buffer1;
{
	int	i,j,k;
	int	p1,p2,p3,p4;
	int	f1,f2,f3,f4;
	int	m1;

	char	b3[MAXB];


	p1=0;
	p2=0;
	m1=0;

	/* Busco primer segmento           */
	while ( p1 < MAXB - strlen(marcas[m1])  &&  strncmp(buffer1+p1,marcas[m1],strlen(marcas[m1]))  )
		p1++;

	if ( p1 == MAXB - strlen(marcas[m1]) )
	{	error(701);
	}

	strncpy(segmento[0],buffer1+p2,p1-p2);
	segmento[0][p1-p2]=0;
	p2 = p1 + strlen(marcas[m1]);



	/* Busco segundo segmento           */
	m1=1;

	while ( p1 < MAXB - strlen(marcas[m1])  &&  strncmp(buffer1+p1,marcas[m1],strlen(marcas[m1]))  )
		p1++;

	if ( p1 == MAXB - strlen(marcas[m1]) )
	{	error(702);
	}

	strncpy(segmento[1],buffer1+p2,p1-p2);
	segmento[1][p1-p2]=0;
	p2 = p1 + strlen(marcas[m1]);



	/* Busco tercer segmento           */
	m1=2;

	while ( p1 < MAXB - strlen(marcas[m1])  &&  strncmp(buffer1+p1,marcas[m1],strlen(marcas[m1]))  )
		p1++;

	if ( p1 == MAXB - strlen(marcas[m1]) )
	{	error(703);
	}

	strncpy(segmento[2],buffer1+p2,p1-p2);
	segmento[2][p1-p2]=0;
	p2 = p1 + strlen(marcas[m1]);


	/* Busco cuarto segmento 
 	 * OJO: en cuarto segmento NO hago strlen ... -1 como 
	 * en los anteriors, sino ... saco el ultimo new line 
	 * y tecnicamente modifico el contenido del registro
	 */

	strcpy(segmento[3],buffer1+p2);
	segmento[3][strlen(segmento[3])]=0;

	/* ojo !!
	 * se cuelan blancos al segmentar la palabra de ref a buscar
	 * algo anda mal en el parser original ...
	 */

	for (i=strlen(segmento[2])-1; segmento[2][i] == ' '; i--)
		segmento[2][i] = 0;


	/* Pasar a minuscula la palabra buscada, para compatibilidad en las comparaciones */
	strcpy (b3,segmento[2]);
	strcpy (segmento[2],pasar_a_minusc(b3));



	/* tengo los 4 segmentos */
	if (gp_verbose)
	{
		printf ("Separar: \n");
		printf ("0: |%s|\n",segmento[0]);
		printf ("1: |%s|\n",segmento[1]);
		printf ("2: |%s|\n",segmento[2]);
		printf ("3: |%s|\n",segmento[3]);
	}

}





/*
 *	2018-04-21
 *
 *	bd_load
 *	carga la base de datos del diccionario
 *	diccio base: diccio01.txt
 *
 */

int	bd_load()
{
	char	b1[MAXB];
	int	flag;
	int	i,j,k;
	int	ql;

	/* cantidad de palabras en el diccionario */
	q_wrd = 0;

	/* ptr al contenedor de direccion del ultimo nodo de la cadena */
	npa = (nodeptr *) &np1;

	ql=0;
	while (fgets(b1,MAXB,hfdic) != NULL)
	{

		if (gp_verbose)
		{
			printf ("%3d |%s|\n",ql,b1);
		}

		for ( flag=0, j=strlen(b1); !flag && j; j--)
			if (b1[j] == '\n')
				b1[j]=0,flag=1;

		*npa = (nodeptr ) malloc ( sizeof (node));
		(**npa).wrd = ( char *) malloc(strlen(b1)+1);
		sprintf ( (**npa).wrd,"%s",b1);
		(**npa).num = q_wrd+1;
		(**npa).nx = (nodeptr) NULL;
		npa = (nodeptr *) & (*npa)->nx;


		q_wrd++;
		ql++;

	}

	if (gp_verbose)
	{
		printf ("Cant de lineas en diccionario   : %6d\n",ql);
		printf ("Cant de palabras en diccionario : %6d\n",q_wrd);
	}


	if (gp_verbose)
	{
		printf ("\n\nComprobando integridad del diccionario : \n\n");
	
		i=0;
		npa = (nodeptr *) &np1;

		while ( (*npa) != (nodeptr) NULL )
		{
			printf ("Contador ..i : %3d\n",i);
			printf ("(**npa).wrd  : |%s|\n",  (**npa).wrd );
			printf ("(**npa).num  : %3d\n", (**npa).num );
			printf ("\n");

			npa = (nodeptr *) & (*npa)->nx;
			i++;
		}

	}
}












/*
 *	bm_load
 *	carga la base de datos de marcas
 *
 */

int	bm_load()
{
	char	b1[MAXB];
	char	b2[MAXM];
	int	p1,p2;
	int	i,j,k;
	int	f1;

	f1=0;
	q_bm=0;
	k=0;

	while (fgets(b1,MAXB,hfmrk) != NULL)
	{
		if (!linea_vacia(b1)  && b1[0] != '#' )
		{
			f1=1;
		}

		if (f1)
		{
			p1=0;
			p2=0;
			k=0;

			while (b1[p1] )
			{	
				b2[k++] = b1[p1];

				if (b1[p1] == ' ' || b1[p1] == '\n')
				{	
					b2[k-1]=0;


					if (p2 == 0)
					{	strcpy(bm_key[q_bm],b2);

					}

					if (p2 == 4)
					{	strcpy(bm_mrk[q_bm],b2);

					}

					p2++;
					k=0;
				}

				p1++;

			}
		
			/* next ... */
			q_bm++;
			f1=0;


			if (q_bm >= MAXK)
				error(301);
		}


		if (gp_verbose)
		{
			printf ("%3d %s\n",q_bm,b1);
		}


	}

	if (gp_verbose)
	{
		printf ("Cant de lineas en tabla de marcas: %d\n",q_bm);
		for (j=0; j<q_bm; j++)
			printf ("%3d,%-10.10s,%s\n",j,bm_mrk[j],bm_key[j]);

		printf ("\n");

	}

}


/*
 * 	bm_tag
 *
 * 	Concatena a string, el tag correspondiente, si coincide la palabra clave
 *
 */


char	*bm_tag(s)
char	*s;
{
	static char b1[MAXB];

	char	b2[MAXM];
	int	i,j,k;
	int	f1,f2;


	strcpy(b1,s);

	f1=1;

	for ( j=0; f1 && j< q_bm; j++)
	{
		if (strcmp(s,bm_key[j]) == 0)
		{
			f1=0;
			sprintf (b2,"_%s",bm_mrk[j]);
			strcat(b1,b2);
		}
	}

	return b1;
}



/*
 *	gp_print
 *
 */

int	gp_print()
{
	if (gp_verbose)
	{
		printf ("Cant de pars ....  %d\n",gp_fq(GP_GET,0) );
		printf ("Cant de tipo 1 %d\n",gp_q_partype1);
		printf ("Cant de tipo 2 %d\n",gp_q_partype2);
		printf ("Cant de tipo 3 %d\n",gp_q_partype3);
		printf ("\n");
	}
}


/*
 *	r
 *
 */

int	gp_parser()
{

	int i;

	/* verbose on ?, excepcion con verbose, por si lo pusieron al final de la linea !!! */
	for (i=0; i < gp_fq(GP_GET,0); i++  )
		if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'v'  )
			gp_verbose = 1;


	for (i=0; i < gp_fq(GP_GET,0);  )
	{

		/* parameter type 1 ... "name" */
		if ( i &&  *( gp_fp(GP_GET,i,(char **)0) + 0) != '-')
		{
			gp_q_partype1++;

			if (gp_q_partype1 == 1)
			{
				strcpy(finp, gp_fp(GP_GET,i,(char **)0));
			}

			if (gp_q_partype1 == 2)
			{
				strcpy(fout, gp_fp(GP_GET,i,(char **)0));
			}

			if (gp_verbose)
			{
				printf ("Param tipo 1: %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* parameter type 2 ... "-something" */
		if ( i && *( gp_fp(GP_GET,i,(char **)0) + 0) == '-' && !tiene_igual( gp_fp(GP_GET,i,(char **)0) ) )
		{
			gp_q_partype2++;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'v'  )
				gp_verbose = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'h'  )
				gp_help = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'm'  )
				gp_minusculas = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'f'  )
				gp_fsentencia = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'e'  )
				gp_eol = 1;

			if (gp_verbose)
			{
				printf ("Param tipo 2: %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* parameter type 3 ... "-someoption=somename" */
		if ( i && *( gp_fp(GP_GET,i,(char **)0) + 0) == '-' && tiene_igual( gp_fp(GP_GET,i,(char **)0) ) )
		{
			gp_q_partype3++;

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"inp",3) )
					strcpy(finp,desde_igual( gp_fp(GP_GET,i,(char **)0)));

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"out",3) )
					strcpy(fout,desde_igual( gp_fp(GP_GET,i,(char **)0)));

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"dic",3) )
				{	gp_diccionario=1;
					strcpy(fdic,desde_igual( gp_fp(GP_GET,i,(char **)0)));
				}

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"mrk",3) )
				{	gp_tabmrk=1;
					strcpy(fmrk,desde_igual( gp_fp(GP_GET,i,(char **)0)));
				}

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"nvd",3) )
					gp_niveldes = *desde_igual( gp_fp(GP_GET,i,(char **)0)) - '0';



			if (gp_verbose)
			{
				printf ("Param tipo 3: %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* program name */
		if (i==0)
		{	
			if (gp_verbose)
			{
				printf ("Name:  %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* next ... */
		i++;
	}

	/* si no pone parametros ... mandar a uso */
	if ( gp_q_partype1 + gp_q_partype2 + gp_q_partype3 == 0 )
		gp_help = 1;

	/* si no indico ningun  archivo ... mandar a uso */
	if ( gp_q_partype1 + gp_q_partype3 == 0 )
		gp_help = 1;

} 



/*
 *	tiene_igual
 *
 */


int	tiene_igual(s)
char	*s;
{
	int i,flag;

	for (i=0, flag=0; i< strlen(s) && !flag; i++ )
		if ( *(s+i) == '=')
			flag=1;

	return flag;
}



/*
 *	desde_igual
 *
 */


char	*desde_igual(s)
char	*s;
{
	int i,flag;

	for (i=0, flag=0; i< strlen(s) && !flag; i++ )
		if ( *(s+i) == '=')
			flag=1;

	return s+i;
}




/*
 *	gp_init
 *
 */

int	gp_init(vpar_q,vpar_p)
int	vpar_q;
char	**vpar_p;
{
	int i;

	gp_fq(GP_SET,vpar_q);
	gp_fp(GP_SET,0,vpar_p);


}


/*
 *	gp_fp
 *
 */

char	*gp_fp(action,offset,value)
int	action;
int	offset;
char	**value;
{
	static char **gp_value;

	if (action == GP_SET)
	{	
		gp_value=value;
	}

	return 	*(gp_value+offset);
}



/*
 *	gp_fq(action,value)
 *	
 *	int action
 *	int value
 *	
 *	value:
 *	on fist call, value is a number (typically argc )
 *	after first call, value can be any number, and is not used
 *
 *	returns:
 *	value
 */

int	gp_fq(action,value)
int	action;
int	value;
{
	static int gp_value=0;

	if (action == GP_SET)
	{	
		gp_value=value;
	}

	return gp_value;
}


int	gp_test()
{
	int i;

	printf ("%d\n",gp_fq(GP_GET,0) );
	for (i=0; i< gp_fq(GP_GET,0); i++)
	{
		printf ("%d %s\n",i,gp_fp(GP_GET,i,(char **)0)  );
	}
			
}

int	linea_vacia(s)
char	*s;
{
	int i,flag;

	i=0;
	flag=1;

	for (i=0; flag && i< strlen(s); i++)
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' )
			flag=0;

	return flag;
}


int	error(value)
int	value;
{
	printf ("Error: %d\n",value);
	printf ("usar -h para help\n");
	exit(0);
}

/*
 *	0	fin de linea
 *	1	blanco o tab
 *	2	otra cosa ( x ahora ... )
 *
 */

int	tipo_char(c)
char	c;
{
	int x;

	x=TC_RST;

	if (c == '\n' )
		x = TC_EOL;

	if (c == ' ' || c == '\t' )
		x = TC_BLA;

	if (c == ',' || c == ';' || c == ':' || c == '-' || c == '/' )
		x = TC_CCE;

	if (c == '.' )
		x = TC_PNT;

	if (c == '(' || c == '[' || c == '{' )
		x = TC_PAA;

	if (c == ')' || c == ']' || c == '}' )
		x = TC_PAC;

	if (c >= 'a' && c <= 'z' || c>= 'A' && c <= 'Z')
		x = TC_LET;

	if (c >= '0' && c <= '9' )
		x = TC_NUM;

	if (c == '\'' || c == '%' )
		x = TC_CVR;

	if (c == 0 )
		x = TC_EOS;

	return x;
}

int	es_numero(c)
char	c;
{
	int x;

	x=0;
	if ( c >= '0' && c <= '9' )
		x=1;

	return x;
}


/*
 * 	es_num_tk
 * 	devuelve true si string esta compuesto solo de numeros
 *
 */

int	es_num_tk(s)
char	*s;
{
	int i,x;

	x=1;
	for (i=0; x && i<strlen(s); i++)
	{	if (s[i] < '0' || s[i] > '9' )
			x=0;

	}

	return x;
}








int	es_puntuacion(s)
char	*s;
{
	int i,x;

	x=1;

	for (i=0; x && i<strlen(s); i++)
	{	if (s[i] != '.' && s[i] != ',' && s[i] != ':' && s[i] != ';' )
			x=0;
	}

	return x;

}



int	es_word(s)
char	*s;
{
	int i,x;

	x=1;

	for (i=0; x && i<strlen(s); i++)
	{	if (s[i] < 'A' || s[i] > 'z' || ( s[i] > 'Z' && s[i] < 'a')  )
			x=0;
	}

	return x;

}



int	char_demed(c)
char	c;
{
	int	x;

	x=0;
	if ( c == 'e' || c == 'E' || c == 'x' || c == 'X' )
		x=1;

	return x;
}


int	mostrar(t_case,n_token,s_token)
int	t_case;
int	n_token;
char	*s_token;
{

	if (gp_verbose)
	{
		printf ("t: %3d (%2d)   %s \n", n_token,t_case,s_token);
	}
}


int	uso(x)
int	x;
{
	printf ("Usage: \n");
	printf ("inp_file out_file                   inp_file: texto input  out_file: archivo de salida\n");
	printf ("-inp=inp_file -out=out_file         inp_file: texto input  out_file: archivo de salida\n");
	printf ("-h                                  help                                              \n");
	printf ("-v                                  verbose ... muestra cierta informacion de proceso \n");
	printf ("-m                                  salida a archivo de output en minuscula           \n");
	printf ("-f                                  salida a archivo de output en formato sentencia   \n");
	printf ("-e                                  fuerza string \"EOL\" al final de cada sentencia  \n");
	printf ("-nvd=N                              Nivel de descripcion en archivo de salida (solo sin -f) \n");
	printf ("                                    0 Sin descripcion  \n");
	printf ("                                    1 Agrega la sentencia y numero de token \n");
	printf ("                                    2 Datos del diccionario (tags) \n");


	exit(x);
}


/*
 * 	gp_default
 *
 * 	inicializa parametros de funcionamiento default
 *
 */

int	gp_default()
{
	gp_help=0;
	gp_verbose=0;
	gp_diccionario=0;
	gp_minusculas=0;
	gp_fsentencia=0;
	gp_eol=0;
	gp_niveldes=0;
	gp_tabmrk=0;
}




/*
 * 	pasar_a_minusc
 *
 *
 */

char	*pasar_a_minusc(s)
char	*s;
{
	static char b[MAXB];

	int i,j,k;

	strcpy(b,s);

	for (i=0; i<MAXB && b[i]; i++)
	{
		if ( b[i] >= 'A' && b[i] <= 'Z' )
			b[i] += 32;
	}

	return b;
}


/*
 * 	Archivo de marcas
 *
 * 	ejemplo !!
 *
 */



#if 0

no no RN 0.999297 NEG

ni ni CC 0.834853 CNEG

pero pero CC 0.999902 ENDNEG

sin sin SP 1 TNEG

se ser VMI000 1 VSE

observar observar VMIP3S0 0.989241 OBS
observa observar VMIP3S0 0.989241 OBS
observada observar VMP00SF 1 OBS
observadas observar VMP00PF 1 OBS
observadose observadose VMSP3S0 1 OBS
observan observar VMIP3P0 1 OBS
observando observar VMG0000 1 OBS
observandosse observandosse NCMS000 1 OBS
observaron observar VMIS3P0 1 OBS
observo observar VMIP1S0 1 OBS

detectar detectar VMP00SM 1 DET
detectado detectar VMP00SM 1 DET
detectaron detectar VMIS3P0 1 DET
detecto detectar VMIP1S0 1 DET

evidenciar evidencia NCFS000 0.614458 EVI
evidencia evidencia NCFS000 0.614458 EVI
evidencia evidenciar VMIP3S0 0.373494 EVI
evidencias evidencia NCFP000 0.989726 EVI

visualizar visualizar VMIP3S0 0.989236 VIS
visualiza visualizar VMIP3S0 0.989236 VIS
visualizacin visualizacin NCMS000 0.677562 VIS
visualizacion visualizacion NCFS000 1 VIS
visualizan visualizar VMIP3P0 1 VIS
visualizando visualizar VMG0000 1 VIS
visualizar visualizar VMN0000 1 VIS
visualizo visualizar VMIP1S0 1 VIS

identificar identificar VMN0000 1 IDE
identifica identificar VMIP3S0 0.989241 IDE
identifican identificar VMIP3P0 1 IDE
identificando identificar VMG0000 1 IDE

#endif


/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */



#if 0



/*
 * 	clang02.c
 *
 *	Pruebas de:
 *
 *	- typedef combinado con struct
 *	- punteros a struct
 *	- puntero a puntero de struct
 *	- armado de lista encadenada de structs con palabras para diccionario
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef	struct	tnode	*nodeptr;
typedef	struct	tnode
{	char	*wrd;
	int	num;
	struct	tnode	*nx;
}	node;


nodeptr	npx,np1,*npa;

#define	MAXF	64
#define	MAXB	1024


char	finp[MAXF];
FILE	*hfinp;


int	error(int);

int	p1();
int	p2();
int	p3();
int	p4();


int	main()
{
	int	i,j,k;

	p1();
	p2();
	p3();
	p4();

}

int	p4()
{

	int	i,j,flag,q_lin;
	char	b1[MAXB];

	strcpy(finp,"diccio01.txt");

	if ( (hfinp = fopen (finp,"r")) == NULL )
	{
		error(101);
	}

	npa = (nodeptr *) &np1;

	q_lin=0;
	while (fgets(b1, MAXB, hfinp) != NULL)
	{
		for ( flag=0, j=strlen(b1); !flag && j; j--)
			if (b1[j] == '\n')
				b1[j]=0,flag=1;

		*npa = (nodeptr ) malloc ( sizeof (node));
		(**npa).wrd = ( char *) malloc(strlen(b1)+1);
		sprintf ( (**npa).wrd,"%s",b1);
		(**npa).num = q_lin+1;
		(**npa).nx = (nodeptr) NULL;
		npa = (nodeptr *) & (*npa)->nx;

		q_lin++;

	}

	fclose (hfinp);

	printf ("Lei %d\n",q_lin);

	i=0;
	npa = (nodeptr *) &np1;

	while ( (*npa) != (nodeptr) NULL )
	{
		printf ("Contador ..i : %3d\n",i);
		printf ("(**npa).wrd  : %s\n",  (**npa).wrd );
		printf ("(**npa).num  : %3d\n", (**npa).num );
		printf ("\n");

		npa = (nodeptr *) & (*npa)->nx;
		i++;
	}
}

int	p3()
{

	int	i,j,k;

	npa = (nodeptr *) &np1;

	for (i=0; i<10; i++)
	{
		if ( ( *npa = (nodeptr ) malloc ( sizeof (node)) ) == NULL )
			error(201);
		(**npa).wrd = ( char *) malloc(10);
		sprintf ( (**npa).wrd,"%3d",i);
		(**npa).num = i;
		(**npa).nx = (nodeptr) NULL;
		npa = (nodeptr *) & (*npa)->nx;
	}


	i=0;
	npa = (nodeptr *) &np1;

	while ( (*npa) != (nodeptr) NULL )
	{
		printf ("i        : %3d\n",i);
		printf ("           %s\n",  (**npa).wrd );
		printf ("           %3d\n", (**npa).num );
		printf ("\n");

		npa = (nodeptr *) & (*npa)->nx;
		i++;

	}


}



int	p1()
{

	np1 = (nodeptr) malloc ( sizeof (node));
	(*np1).wrd = ( char *) malloc (10);
	sprintf ( (*np1).wrd,"%3d",0);
	(*np1).num = 0;
	(*np1).nx = (nodeptr) NULL;

	printf ("%s\n", (*np1).wrd );
	printf ("%3d\n",(*np1).num );
	printf ("%d\n", (*np1).nx  );	

}

int	p2()
{
	int	i,*ip;

	ip = &i;

	*ip = 10;

	printf ("i    : %3d\n",i);
	printf ("*ip  : %3d\n",*ip);
}




int	error(value)
int	value;
{
	printf ("Error: %d\n",value);
	exit(0);
}


/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */




#endif







#if 0


/*
 *	diccio01.c
 *	
 *	parser - tokenizador
 *	
 * 	implementa busqueda en diccionario
 *
 * 	Opciones:
 * 	-h   		forma de uso
 *
 * 	( mas ejemplos abajo )
 */


/*
 * 	Ejemplos de uso:
 *
 * 	para el ejemplo ...
 *
 *
 *
 *
 * 	f04_neg.txt               archivo de input, con texto de trabajo original, formato sentencias
 *
 *
 *
 *
 *
 *	1) Generar en salida, todos los tokens del archivo de input, sin modificar, en formato columnar ( 1 tkn x linea)
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt
 *
 * 	./cpar05 f04_neg.txt f04_neg_tkn.txt
 *
 * 	2) Idem, pero forzar al final de cada linea, el token "EOL"
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e
 *
 * 	3) Idem, forzando token "EOL" y pasando todo a minuscula
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m
 *
 * 	4) Idem, forzando token "EOL" , pasando todo a minuscula, y generando formato sentencia ( no columnar )
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m -f
 *
 * 	5) Idem anterior, con alguna informacion de salida 
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m -f -v
 *
 * 	./cpar05 -inp=f04_neg.txt -out=f04_neg_tkn.txt -e -m -f -v  >  ver
 * 	cat ver ... etc
 *
 * 	6) Solamente SIN -f  ...  Agrega informacion en archivo de salida para controlar funcionamiento
 *
 * 	./cpar05 f04_neg.txt f04_neg_tkn.txt -nvd=1
 *
 * 	7) Para usar con un archivo de "marcas" ( para taggear ... )  Al final de este fuente, ejemplo de marcas !!!
 *
 * 	   Atencion !!!  La busqueda en la tabla de marcas NO es case sensitivo ... considerar el uso de -m 
 *
 * 	./cpar05 f04_neg.txt f04_neg_tag.txt -mrk=f04_t1.mrk -f -m -v
 *
 * 	8) Para usar con marcas, pero salida en formato un token por fila, con tag incluido ...
 *
 * 	./cpar05 f04_neg.txt f04_neg_tag.txt -mrk=f04_t1.mrk -m -v
 *
¨*	
 *	9) -s   skip la primera linea del archivo (cuando tiene los headers de los campos )
 *
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	DEBUG 0


#define	GP_SET 1
#define	GP_GET 0


int	gp_default();
int	gp_init(int ,char **);
int	gp_test();
int	gp_print();
int	gp_parser();
int	gp_fq(int,int);
char	*gp_fp(int,int,char **);

int	gp_q_partype1=0;	/* parameter type "name" */
int	gp_q_partype2=0;	/* parameter type "-something" */
int	gp_q_partype3=0;	/* parameter type "-someoption=somename" */


int	gp_verbose=0;		/* verbose 0 no 1 si */
int	gp_help=0;		/* help 0 no 1 si */
int	gp_diccionario=0;	/* diccionario 0 no 1 si */
int	gp_minusculas=0;	/* output en minuscula 0 no 1 si */
int	gp_fsentencia=0;	/* archivo de salida en formato sentencias 0 no 1 si */
int	gp_eol=0;		/* fuerzo string EOL al final de la linea ... a veces hay . en medio de la linea */
int	gp_niveldes=0;		/* nivel de descripcion que se vuelca en archivo de salida (solo con fsentencia = 0 ) */
				/* 0 normal 1 sentencia y numero de token 2 .... agrego cosas del diccionario si se usa */
int	gp_tabmrk=0;		/* usar archivo de marcas 0 no 1 si */
int	gp_skip=0;		/* skip primera fila del archivo 0 no 1 si ( para header con descripcion de campos ) */


char	*desde_igual(char *s);
char	*pasar_a_minusc(char *s);

int	tiene_igual(char *);
int	linea_vacia(char *);
int	tipo_char(char );
int	es_numero(char);
int	es_num_tk(char *);
int	es_word(char *);
int	es_puntuacion(char *);
int	char_demed(char);
int	mostrar(int,int,char *);

int	error(int);
int	uso(int);

#define	TC_EOL	0	/* fin de linea en unix ... ascii 10 '\n' */
#define TC_PCO  1	/* separador .. ver en tipo_char ... puede ser , o ; */
#define	TC_BLA	2	/* blanco */
#define	TC_CCE	3	/* algunos caracteres especiales  ',:/' */
#define	TC_PNT	4	/* punto '.' */
#define	TC_PAA	5	/* parent '([{' */
#define TC_PAC	6	/* parent ')]}' */
#define	TC_LET	7	/* letras */
#define	TC_NUM	8	/* numero */
#define TC_DAP  9	/* apostrofes dobles */
#define TC_RET  10	/* CR asci 13  '\r' */
#define TC_EOS	11	/* eod of string */
#define	TC_ARR	12	/* arroba @ */
#define	TC_RST	99	/* todo lo demas */

int	parser1();

#define	MAXT	400
#define	MAXB	2560
#define	MAXF	30
#define	MAXN	128

char	finp[MAXF];
char	fout[MAXF];
char	fou2[MAXF];
char	fou3[MAXF];
char	fdic[MAXF];
char	fmrk[MAXF];

FILE	*hfinp;
FILE	*hfout;
FILE	*hfou2;
FILE	*hfou3;
FILE	*hfdic;
FILE	*hfmrk;


int	flag_caracteres;


/*	Estructuras y variables para la base de marcas */

#define	MAXK	64	/* cantidad de marcas */
#define	MAXM	20	/* tamano maximo de palabra / marca */

int	bm_load();
char	*bm_tag(char *);


char	bm_key[MAXK][MAXM];
char	bm_mrk[MAXK][MAXM];

int	q_bm;		/* cantidad de marcas en base */



/*	Estructuras y variables para base de datos */

int	bd_load();

typedef	struct
{	struct	palabra	*nx;
	int	ty;
	char	*tx;	
}	bd_type;


bd_type	*bd_first ;

/*
 *	Estructuras para almacenar tokens
 *
 */


int	q_tk;
char	tk[MAXT][MAXB];
char	tk_key[MAXN];




/*	Estructuras para manejo de errores */
/*
 * 	Error codes
 *
 * 	Entre 101 y 199		Errores en ingreso de datos ... agrega nnnn -help 
 *
 */

char	pgm_name[MAXN];




/*
 *	estructuras para manejo de analisis
 *
 *	Prototipo de Analizador ...
 *	
 *	Como buscar ...
 *	- una palabra  (movistar)
 *	- una frase    (mercado libre) ... despues afecta que aparezca mercado o libre solo .. !!! 
 *                     y las variantes "mercadolibre" ...
 *	- un elemento de lista (por ejemplo una "sucursal" .. y verificar que esta en una tabla 
 *	- un modismo   por ejemplo, humor ... malo, choto, bueno, normal
 *	- un reclamo   
 *
 */


/*	Motivo   ... */


/*	Cliente interno ... */

int	wc01,wcf01,wtc01;		/* movistar  */
int	wc02,wcf02,wtc02;		/* mercado libre  */
int	wc03,wcf03,wtc03;		/* banco patagonia   */
int	wc04,wcf04,wtc04;		/* banco galica  */


/*	Producto ...        */

int	wp01,wpf01,wtp01;		/* celular  */
int	wp02,wpf02,wtp02;		/* tarjeta */
int	wp03,wpf03,wtp03;		/* pasaporte */
int	wp04,wpf04,wtp04;		/* partida nacimiento */

/*	Sucursal ...        */

int	ws01,wsf01,wts01;		/* menciona suc ... despues hay que rastrearla !  */
int	ws02,wsf02,wts02;		/*   */
int	ws03,wsf03,wts03;		/*   */
int	ws04,wsf04,wts04;		/*   */

/*	Humor ...        */

int	wh01,whf01,wth01;		/* normal  */
int	wh02,whf02,wth02;		/* malo  */
int	wh03,whf03,wth03;		/* agresivo  */
int	wh04,whf04,wth04;		/*  */

/*	Motivo ...        */

int	wm01,wmf01,wtm01;		/* consulta */
int	wm02,wmf02,wtm02;		/* reclamo x atraso en la entrega   */
int	wm03,wmf03,wtm03;		/*  */
int	wm04,wmf04,wtm04;		/*  */





/*
 *	main
 *
 */

int	main(argc,argv)
int	argc;
char	**argv;
{

	strcpy(pgm_name,argv[0]);


	gp_default();

	gp_init(argc,argv);

	gp_parser();

	gp_print();

	if (gp_help)
	 	uso(0);


	if (gp_verbose)
	{
		printf ("inp %s\n",finp);
		printf ("out %s\n",fout);
		printf ("ou2 %s\n",fou2);
		printf ("ou3 %s\n",fou3);
		printf ("\n");
	}

	
	if ( (hfinp = fopen (finp,"r")) == NULL )
	{
		error(101);
	}

	if ( (hfout = fopen (fout,"w")) == NULL )
	{
		error(102);
	}

	if ( (hfou2 = fopen (fou2,"w")) == NULL )
	{
		error(106);
	}

	if ( (hfou3 = fopen (fou3,"w")) == NULL )
	{
		error(107);
	}


	if (gp_diccionario)
	{
		if ( (hfdic = fopen (fdic,"r")) == NULL )
		{
			error(103);
		}


		bd_load();
	}

	if (gp_tabmrk)
	{
		if ( (hfmrk = fopen (fmrk,"r")) == NULL )
		{
			error(104);
		}

		bm_load();
	}

	if (gp_verbose)
		printf ("voy a parser \n");

	parser1();


	fclose(hfinp);
	fclose(hfout);
	fclose(hfou2);
	fclose(hfou3);
	if (gp_diccionario)
		fclose(hfdic);
	if (gp_tabmrk)
		fclose(hfmrk);

	if (gp_verbose)
		printf ("fin proceso \n");

	if (flag_caracteres)
	{	
		printf ("Atencion: caracteres no contemplados para parser encontrados en input  \n");
		printf ("Volver a ejecutar con flag -v y revisar salida (buscar \"Caracteres\" ... ) \n");
	}

}


/*
 * 	parser2
 *	Analisis de comentario
 *	
 *	Paso 1 ...
 *	Busco "clientes internos"
 *
 *	A este punto tengo:
 *	tk_key     es la clave del registro que estoy procesando
 *	q_tk       cantidad de tokens 
 *	tk[q_tk]   vector de tokens ..
 *
 */



int	parser2()
{
	int	i,j,k,lm;
	int	q1,q2;
	int	m1,m2,m3;
	int	f1,f2,f3,f4;
	int	q_lin;
	int	p1,p2,p3,p4;
	int	fg_abierto;

	char	cc;
	char	b0[MAXB];
	char	b1[MAXB];
	char	b2[MAXB];

	/* entro */
	if (gp_verbose)
	{	printf ("Ingreso a parser2  \n");
		printf ("Clave: %s\n",tk_key);
		printf ("Q tok: %d\n",q_tk);
		for (j=0; j<q_tk; j++)
		{
			printf ("%d %s\n",j,tk[j]);

		}

	}


	/* Contadores de ... */
	wc01=0; wcf01=1;
	wc02=0; wcf02=1;
	wc03=0; wcf03=1;
	wc04=0; wcf04=1;

	wp01=0; wpf01=1;
	wp02=0; wpf02=1;
	wp03=0; wpf03=1;
	wp04=0; wpf04=1;

	ws01=0; wsf01=1;
	ws02=0; wsf02=1;
	ws03=0; wsf03=1;
	ws04=0; wsf04=1;

	wh01=0; whf01=1;
	wh02=0; whf02=1;
	wh03=0; whf03=1;
	wh04=0; whf04=1;

	wm01=0; wmf01=1;
	wm02=0; wmf02=1;
	wm03=0; wmf03=1;
	wm04=0; wmf04=1;


	/* for principal */
	for (j=0; j< q_tk; j++)
	{

		/* Cliente interno  ... */

		if (wcf01)
		{	if (strncmp(tk[j],"movistar",8)     == 0 )
			{	wc01=1;
				wcf01=0;
			}
		}

		if (wcf02)
		{	if (strncmp(tk[j],"mercadolibre",10)     == 0 )
			{	wc02=2;
				wcf02=0;
			}
		}

		if (wcf02)
		{	if (strncmp(tk[j],"mercado",7)     == 0 )
			{	wc02=1;
			}

			if (strncmp(tk[j],"libre",5) == 0 && wc02 )
			{	wc02=2;
				wcf02=0;
			}
		}


		if (wcf03)
		{	if (strncmp(tk[j],"bancopatagonia",10)     == 0 )
			{	wc03=2;
				wcf03=0;
			}
		}

		/* ojo ... no es lo mismo banco patagonia ... que banco que tiene mi viejo en patagonia ... */
		if (wcf03)
		{	if (strncmp(tk[j],"banco",5)     == 0 )
			{	wc03=1;
			}

			if (strncmp(tk[j],"patagonia",8) == 0 && wc03 )
			{	wc03=2;
				wcf03=0;
			}
		}

		if (wcf04)
		{	if (strncmp(tk[j],"banco",5)     == 0 )
			{	wc04=1;
			}

			if (strncmp(tk[j],"galicia",6) == 0 && wc04 )
			{	wc04=2;
				wcf04=0;
			}
		}


		/* Productos ... */

		if (wpf01)
		{	if (strncmp(tk[j],"celular",7)   == 0 )
			{	wp01=1;
				wpf01=0;
			}
		}

		if (wpf02)
		{	if (strncmp(tk[j],"tarjeta",7)   == 0 )
			{	wp02=1;
				wpf02=0;
			}
		}

		if (wpf03)
		{	if (strncmp(tk[j],"licencia",8)   == 0 )
			{	wp03=1;
				wpf03=0;
			}
		}

		if (wpf04)
		{	if (strncmp(tk[j],"pasaporte",8)   == 0 )
			{	wp04=1;
				wpf04=0;
			}
		}



		/* Sucursal */
		if (wsf01)
		{	if (strcmp(tk[j],"suc")   == 0 )
			{	ws01=1;
				wsf01=0;
			}

			if (strcmp(tk[j],"sucursal")   == 0 )
			{	ws01=1;
				wsf01=0;
			}
		}

		

		/* Humor ... */


		/* en general, la palabra comunicar, esta ligada a la imposibilidad de comunicarse */
		
		if (whf02)
		{

			if (strncmp(tk[j],"comunicarse",9) == 0 )
			{	wh02 = 1;
				whf02 = 0;
			}

			if (strncmp(tk[j],"verguenza",9) == 0 )
			{	wh02 = 1;
				whf02 = 0;
			}

			if (strncmp(tk[j],"atiende",7) == 0 )
			{	wh02 = 1;
				whf02 = 0;
			}

			if (strncmp(tk[j],"mala",4) == 0 )
			{	wh02 = 1;
				whf02 = 0;
			}

			if (strncmp(tk[j],"noticias",4) == 0 )
			{	wh02 = 1;
				whf02 = 0;
			}

		}
		
		if (whf03)
		{

			if (strcmp(tk[j],"mierda") == 0 )
			{	wh03 = 1;
				whf03 = 0;
			}

			if (strncmp(tk[j],"enojad",6) == 0 )
			{	wh03 = 1;
				whf03 = 0;
			}


		}
		


		/* Motivo ... */
		/* Disyuntiva ... tambien lo hacemos "excluyente " ? o sea ... la suma de los motivos .. debe dar 
		¨* igual al total de registros ...
		 */

		/* Consulta */
		/* quisiera ... saber , con digamos ... en un total de 5  tokens seguidos ... */
		if (wmf01)
		{
			if (strncmp(tk[j],"quisiera",8) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<6; i++,k++   )
				{
					if (strcmp(tk[i],"saber") == 0 )
					{	wm01 = 1;
						wmf01 = 0;
					}
				}
			}

			if (strncmp(tk[j],"gustaria",8) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<6; i++,k++   )
				{
					if (strcmp(tk[i],"saber") == 0 )
					{	wm01 = 1;
						wmf01 = 0;
					}
				}
			}

			if (strncmp(tk[j],"necesito",8) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<6; i++,k++   )
				{
					if (strcmp(tk[i],"saber") == 0 )
					{	wm01 = 1;
						wmf01 = 0;
					}
				}
			}

			if (strncmp(tk[j],"querria",6) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<4; i++,k++   )
				{
					if (strcmp(tk[i],"saber") == 0 )
					{	wm01 = 1;
						wmf01 = 0;
					}
				}
			}

		}


		/* Reclamo porque no le llego */
		/* no ... llego   / llegado / recibi   ... */
		if (wmf02)
		{
			if (strcmp(tk[j],"no") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strcmp(tk[i],"llego") == 0 )
					{	wm02 = 1;
						wmf02 = 0;
					}
					if (strcmp(tk[i],"llegado") == 0 )
					{	wm02 = 1;
						wmf02 = 0;
					}
					if (strcmp(tk[i],"recibi") == 0 )
					{	wm02 = 1;
						wmf02 = 0;
					}
				}
			}

			if (strcmp(tk[j],"sigo") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strcmp(tk[i],"esperando") == 0 )
					{	wm02 = 1;
						wmf02 = 0;
					}
				}
			}

			if (strcmp(tk[j],"imposible") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strcmp(tk[i],"recibirlo") == 0 )
					{	wm02 = 1;
						wmf02 = 0;
					}
				}
			}

		}


		/* Reclamo porque no se puede comunicar  */
		/* no  ... puedo / necesito / imposible ... comunicar / llamar   ... */
		if (wmf03)
		{
			if (strcmp(tk[j],"imposible") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}

				}
			}

			if (strcmp(tk[j],"puedo") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strcmp(tk[j],"necesito") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strcmp(tk[j],"trato") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strcmp(tk[j],"quiero") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strncmp(tk[j],"tratado",5) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strncmp(tk[j],"intentando",6) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strcmp(tk[j],"pude") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strncmp(tk[j],"tratado",5) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strcmp(tk[j],"logro") == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

			if (strncmp(tk[j],"hora",4) == 0)
			{
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}
				}
			}

		}



		/* reclamo por la pagina web */
		if (wmf04)
		{
			if (strcmp(tk[j],"pagina") == 0)
			{

				wm04=1;

#if 0
				for (i=j+1,k=0; i< q_tk && k<5; i++,k++   )
				{
					if (strncmp(tk[i],"comunicar",8) == 0 )
					{	wm03 = 1;
						wmf03 = 0;
					}

				}

#endif
			}

		}







	}
	/* for principal */





	/* clientes ... */

	if ( wc01 == 1 )
		wtc01++;

	if ( wc02 == 2 )
	{
		wc02 == 1;
		wtc02++;
	}

	if ( wc03 == 2 )
	{
		wc03 == 1;
		wtc03++;
	}


	if ( wc04 == 1 )
		wtc04++;




	/* productos */

	if ( wp01 == 1)
		wtp01++;

	if ( wp02 == 1)
		wtp02++;

	if ( wp03 == 1)
		wtp03++;

	if ( wp04 == 1)
		wtp04++;



	/* sucursales */

	
	if (ws01 == 1)
		wts01++;

	/* humor */
	/* humor es particular ... las columnas son excluyentes ! */

	wh01 = 1;

	if (wh02 == 1 && wh03 == 0)
	{
		wh01 = 0;
		wh02 = 1;
	}

	if (wh03 == 1 )
	{
		wh01 = 0;
		wh02 = 0;
	}

	if ( wh01 == 1)
		wth01++;

	if ( wh02 == 1)
		wth02++;

	if ( wh03 == 1)
		wth03++;



	/* motivo*/

	/* consulta   */
	if (wm01 == 1)
		wtm01++;

	/* reclamo por encomienda no llegada en tiempo  */
	if (wm02 == 1)
		wtm02++;


	/* reclamo por imposibilidad de comunicarse   */
	if (wm03 == 1)
		wtm03++;

	/* reclamo por la pagina    */
	if (wm04 == 1)
		wtm04++;




	fprintf (hfou3,"%7s %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
		tk_key,wc01,wc02,wc03,wc04,wp01,wp02,wp03,wp04,ws01,ws02,ws03,ws04,wh01,wh02,wh03,wh04,wm01,wm02,wm03,wm04);



}



/*	end of parser2    */
/*	end of parser2    */
/*	end of parser2    */
/*	end of parser2    */
/*	end of parser2    */
/*	end of parser2    */




/*
 *	parser1
 *
 *	parser general de la linea
 *
 */



int	parser1()
{

	int	i,j,k,lm;
	int	q1,q2;
	int	m1,m2,m3;
	int	f1,f2,f3,f4;
	int	q_lin;
	int	p1,p2,p3,p4;
	int	fg_abierto;

	char	cc;
	char	b0[MAXB];
	char	b1[MAXB];
	char	b2[MAXB];

	/* entro */
	if (gp_verbose)
		printf ("Ingreso a parser 1  \n");


	/* si encuentro caracteres no considerados para el parser, avisar al final de todo el proceso */
	flag_caracteres = 0;


	/* determinar si salteo primera linea */
	f4=0;
	if (gp_skip)
	{
		f4=1;
		fgets(b1, MAXB, hfinp);
	}

	/* contadores globales de ocurrencias */
	wtc01=0;
	wtc02=0;
	wtc03=0;
	wtc04=0;

	wtp01=0;
	wtp02=0;
	wtp03=0;
	wtp04=0;

	wts01=0;
	wts02=0;
	wts03=0;
	wts04=0;

	wth01=0;
	wth02=0;
	wth03=0;
	wth04=0;

	wtm01=0;
	wtm02=0;
	wtm03=0;
	wtm04=0;



	p4=0;

	q_lin=0;
	while (fgets(b1, MAXB, hfinp) != NULL)
	{
		/* procesar solo lineas no vacias */
		if ( !linea_vacia(b1))
		{
			f1=1;

			if (gp_verbose)
			{
				printf (">>%s<<\n",b1);
			}
		}
		else
		{
			if (gp_verbose)
			{
				printf ("Linea vacia %d\n\n",q_lin);
			}
		}



		/* revision que entre pares de apostrofes (delimitadores de algunos campos ) NO hay ; ... separador */
		f2=0;

		fg_abierto=0;

		for (k=0 ; k < strlen(b1) ; k++)
		{
			if (tipo_char(b1[k]) == TC_DAP )
			{	
				if (fg_abierto)
					fg_abierto = 0;
				else
					fg_abierto = 1;
			}

			if (tipo_char(b1[k]) == TC_PCO && fg_abierto  )
				b1[k] = ',';

		}
				

		/* p1 pointer en la linea , q_tK nro de token , p2 pointer de posicion en el token nro q_tk */
		p1=0;
		p2=0;
		q_tk=0;

		while ( f1 )
		{

			/* controlamos cantidad de tokens ... */
			if (q_tk > MAXT-10)
			{	error(501);
			} 

			j=tipo_char(b1[p1]);


			/* ATENCION: Este es el parser que separa campos del registro */
			switch (j)
			{
				/* caracteres extranos */
				case TC_RST:		
					printf ("CARACTER extrano en parser  %d %d (%c)\n",j,b1[p1],b1[p1]);
					p1++;
					break;

				/* otro caracter !!! */
				case TC_BLA:		
				case TC_CCE:		
				case TC_PAA:		
				case TC_PAC:		
				case TC_LET:		
				case TC_NUM:		
				case TC_DAP:
				case TC_RET:
				case TC_PNT:
					tk[q_tk][p2]=b1[p1];
					p1++; 
					p2++;
					break; 

#if 0
				case TC_PNT:		
					tk[q_tk][p2]=0;
					p2=0;
					q_tk++;

					tk[q_tk][p2]=b1[p1];
					p1++;
					p2++;
					break;

				case TC_PNT:		
					tk[q_tk][p2]=0;
					p1++;
					p2=0;
					q_tk++;
					break;
#endif


				/* separador  */
				/* esquema de campos separador por separador */
				case TC_PCO:
					tk[q_tk][p2]=0;
					p1++;
					p2=0;
					q_tk++;
					break;

				/* fin de linea */
				case TC_EOL:
					tk[q_tk][p2]=0;
					p1++;
					p2=0;
					q_tk++;
					f1=0;
					break;

#if 0
				/* apostrofes ... que encierran strings conteniendo el separador */
				case TC_DAP:
					tk[q_tk][p2]=b1[p1];
					p1++; 
					p2++;
					
					q2=0;
					while (b1[p1] != TC_DAP )
					{
						printf ("entro a loop %d\n",q2);
						q2++;
						if (q2 > MAXB )
							error (502);
						
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;
					}
					break; 
#endif



				default:
					printf ("Default, algo salio mal >>%d<< !!!\n\n",j);
					f1=0;
					break;
			}
		}

		/* verifico si hay que sacar output  en minusculas */
		if (gp_minusculas)
		{
			for (j=0; j< q_tk; j++)
				strcpy(tk[j],pasar_a_minusc(tk[j]));
		}

		/* si esta usando tabla de marcas ... verificar si hay que taggear */
		if (gp_tabmrk)
		{
			for (j=0; j< q_tk; j++)
				if (es_word(tk[j]) || es_puntuacion(tk[j]) )
					strcpy(tk[j],bm_tag(tk[j]));
		}


		/* hay que forzar  string EOL al final de la linea */
		if (gp_eol)
			strcpy(tk[q_tk++],"EOL");


		/* salida en formato token columnar */
		if (gp_fsentencia == 0)
		{


			/* pidio nivel de descripcion en salida ... agrego la sentencia */
			if (gp_verbose)
				fprintf (hfout,"Columnas (token: %d) :\n",q_tk);

			/* pidio nivel de descripcion en salida ... agrego la sentencia */
			if (gp_niveldes)
				fprintf (hfout,"%s\n",b1);


			/* grabo los tokens encontrados */
			for (j=0; j< q_tk; j++)
			{
				switch (gp_niveldes)
				{

					case 0:
						fprintf (hfout,"%s\n",tk[j]);
						break;

					case 1:
						fprintf (hfout,"%3d,%s\n",j,tk[j]);
						break;

					default:
						fprintf (hfout,"%s\n",tk[j]);
						break;
				}


				if (gp_verbose)
					printf ("%3d,%s\n",j,tk[j]);

			}



#if 0
			/* agrego termino EOL para indentificar donde termino
			 * esto ya que tal vez haya un . en mitad de linea 
			 */
			if (gp_eol)
				fprintf (hfout,"%s\n","EOL");

#endif
		}



		/* salida en formato sentencia */
		if (gp_fsentencia == 1)
		{


			/* grabo los tokens encontrados */
#if 0
			fprintf (hfout,"%s\n",b1);
#endif
			if (p4 == 0)
			{	p4=q_tk;
			}
			else
				if (p4 != q_tk )
				{	printf ("CANTIDAD de tokens diferente !! lin %d todos %d  este  %d  \n",q_lin,p4,q_tk);
				}

			for (j=0; j< q_tk; j++)
			{
#if 0
				fprintf (hfout,"%3d,%s\n",j,tk[j]);
#endif
				if (j != q_tk -1 )
					fprintf (hfout,"%s;",tk[j]);
				else
					fprintf (hfout,"%s",tk[j]);
				

				if (gp_verbose)
					printf ("%3d,%s\n",j,tk[j]);

			}

#if 0
			/* agrego termino EOL para indentificar donde termino
			 * esto ya que tal vez haya un . en mitad de linea 
			 */
			if (gp_eol)
				fprintf (hfout,"%s\n","EOL");
			else
				fprintf (hfout,"\n");
#endif


			/* se termino la linea */
			fprintf (hfout,"\n");

		}


		if (gp_niveldes)
			fprintf (hfout,"\n\n");


		if (gp_verbose)
		{
			printf ("\n");
		}

		/* 
		 * Termine de parsear campos a nivel registro,
		 * Ahora parseo solo el campo con comentarios del usuairo
		 * y genero resultado en archivo separado
		 *
		 * Utilizo las mismas estructuras para parsear todo el registro
		 * ya que a este punto, ya grabe y no necesito mas los campos
		 */


		/* copio la clave del registro */	
		strcpy(tk_key,tk[0]);

		/* copio el string, sin los apostrofes que delimitan el campo */
		strcpy(b1,tk[1]+1);
		q1=strlen(b1);
		b1[q1-1]=0;

		q_tk = 0;
		p1=0;
		p2=0;

		f3=1;

		if (gp_verbose)
		{
			printf ("VOY a 2do parser con \n>>%s<<\n\n",b1);
		}

		while ( f3)
		{
			if ( q_tk > MAXT - 10 )
				error(315);

			cc = b1[p1];
			j = tipo_char(cc);

#if 0
			if (gp_verbose)
			{	printf ("TIPO_CHAR %d  char %d  (%c) q_tk: %d  tok0: %s\n",j,cc,cc,q_tk,tk[0]);
			}

#endif
			/* ATENCION: parser del comentario del usuarios */
			switch (j)
			{
				/* caracteres extranos */
				case TC_RST:		
				case TC_RET:
				case TC_EOL:
					printf ("CARACTER %d\n",j);
					p1++;
					break;

				/* apostrofes .. */
				case TC_DAP:
					while (tipo_char(b1[p1]) == TC_DAP )
					{	
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;
					}

					tk[q_tk][p2]=0;
					p2=0;
					q_tk++;
					break; 

				case TC_PNT:		
					while ( tipo_char( b1[p1] ) == TC_PNT )
					{	
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;

					}

					tk[q_tk][p2]=0;
					p2=0;
					q_tk++;
					break;


				case TC_PAA:		
				case TC_PAC:
					while ( tipo_char( b1[p1] ) == TC_PAA || tipo_char( b1[p1] ) == TC_PAC )
					{	
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;

					}

					tk[q_tk][p2]=0;
					p2=0;
					q_tk++;
					break;



				/* otros caracteres !!! */
				case TC_CCE:
				case TC_PCO:
					while ( tipo_char( b1[p1] ) == TC_PCO || tipo_char( b1[p1] ) == TC_CCE )
					{	
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;

					}

					tk[q_tk][p2]=0;
					p2=0;
					q_tk++;
					break;

				/* numeros */
				case TC_NUM:		
					while ( tipo_char( b1[p1] ) == TC_NUM )
					{	
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;

					}

					tk[q_tk][p2]=0;
					p2=0;
					q_tk++;
					break;

				case TC_LET:		
					while ( tipo_char( b1[p1] ) == TC_LET )
					{	
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;

					}

					tk[q_tk][p2]=0;
					p2=0;
					q_tk++;
					break;

				case TC_BLA:		
					while ( tipo_char( b1[p1] ) == TC_BLA )
					{	
						tk[q_tk][p2]=b1[p1];
						p1++; 
						p2++;

					}

					/*
					 * atencion ...
					 * no tiene sentido guardar el blanco como token
					 *
					 */

					strcpy(tk[q_tk]," ");
					/* tk[q_tk][p2]=0; */
					p2=0;
					/* q_tk++; */
					break;

				/* fin de linea */
				case TC_EOS:
					if (gp_verbose)
					{	printf ("SWITCH x %d q_tk: %d  tok0: %s\n",j,q_tk,tk[0]);
					}
					f3=0;
					break;


				default:
					printf ("DEFAULT, algo salio mal >>%d<< !!!\n\n",j);
					f3=0;
					break;
			}

		}

		if (gp_verbose)
		{	printf ("PARS2 Cant de lineas %d\n",q_tk);
			printf ("tok 0: %s\n",tk[0]);
		}

		for (j=0; j<q_tk; j++)
		{	
			fprintf (hfou2,"|%s| \n",tk[j] );
		}
	
		/*
		 * A este punto, tengo en tk[] y q_tk, el comentario separado en tokens
		 * q_tk    cantidad de tokens
		 * tk[]    vector de tokens
		 * tk_key  clave del comentario (primer campo del registro)
		 *
		 */
	
	
		if (gp_verbose)
		{
			for (j=0; j < q_tk; j++)
				printf ("CCLF:%s:%s\n",tk_key,tk[j]);
	
		}
	

		/* llamo distintos parsers para buscar variables de analisis */
		parser2();


		/* sumo lineas */
		q_lin++;
	}

	
	if (gp_verbose)
		printf ("Cant de lineas %d\n",q_lin);


	if (gp_verbose)
	{
		printf ("totales CLiente:   %5d %5d %5d %5d\n",wtc01,wtc02,wtc03,wtc04);
		printf ("totales Product:   %5d %5d %5d %5d\n",wtp01,wtp02,wtp03,wtp04);
		printf ("totales Sucursa:   %5d %5d %5d %5d\n",wts01,wts02,wts03,wts04);
		printf ("totales Humor  :   %5d %5d %5d %5d\n",wth01,wth02,wth03,wth04);
		printf ("totales Motivo :   %5d %5d %5d %5d\n",wtm01,wtm02,wtm03,wtm04);

	}




}


/*
 *	bd_load
 *	carga la base de datos del diccionario
 *
 */

int	bd_load()
{
	char	b1[MAXB];
	int	i,j,k;
	int	ql;

	ql=0;
	while (fgets(b1,MAXB,hfdic) != NULL)
	{
		ql++;

		if (gp_verbose)
		{
			printf ("%3d %s\n",ql,b1);
		}

	}

	if (gp_verbose)
	{
		printf ("Cant de lineas en diccionario: %d\n",ql);
	}

}



/*
 *	bm_load
 *	carga la base de datos de marcas
 *
 */

int	bm_load()
{
	char	b1[MAXB];
	char	b2[MAXM];
	int	p1,p2;
	int	i,j,k;
	int	f1;

	f1=0;
	q_bm=0;
	k=0;

	while (fgets(b1,MAXB,hfmrk) != NULL)
	{
		if (!linea_vacia(b1)  && b1[0] != '#' )
		{
			f1=1;
		}

		if (f1)
		{
			p1=0;
			p2=0;
			k=0;

			while (b1[p1] )
			{	
				b2[k++] = b1[p1];

				if (b1[p1] == ' ' || b1[p1] == '\n')
				{	
					b2[k-1]=0;


					if (p2 == 0)
					{	strcpy(bm_key[q_bm],b2);

					}

					if (p2 == 4)
					{	strcpy(bm_mrk[q_bm],b2);

					}

					p2++;
					k=0;
				}

				p1++;

			}
		
			/* next ... */
			q_bm++;
			f1=0;


			if (q_bm >= MAXK)
				error(301);
		}


		if (gp_verbose)
		{
			printf ("%3d %s\n",q_bm,b1);
		}


	}

	if (gp_verbose)
	{
		printf ("Cant de lineas en tabla de marcas: %d\n",q_bm);
		for (j=0; j<q_bm; j++)
			printf ("%3d,%-10.10s,%s\n",j,bm_mrk[j],bm_key[j]);

		printf ("\n");

	}

}


/*
 * 	bm_tag
 *
 * 	Concatena a string, el tag correspondiente, si coincide la palabra clave
 *
 */


char	*bm_tag(s)
char	*s;
{
	static char b1[MAXB];

	char	b2[MAXM];
	int	i,j,k;
	int	f1,f2,f3;


	strcpy(b1,s);

	f1=1;

	for ( j=0; f1 && j< q_bm; j++)
	{
		if (strcmp(s,bm_key[j]) == 0)
		{
			f1=0;
			sprintf (b2,"_%s",bm_mrk[j]);
			strcat(b1,b2);
		}
	}

	return b1;
}



/*
 *	gp_print
 *
 */

int	gp_print()
{
	if (gp_verbose)
	{
		printf ("Cant de pars ....  %d\n",gp_fq(GP_GET,0) );
		printf ("Cant de tipo 1 %d\n",gp_q_partype1);
		printf ("Cant de tipo 2 %d\n",gp_q_partype2);
		printf ("Cant de tipo 3 %d\n",gp_q_partype3);
		printf ("\n");
	}
}


/*
 *	r
 *
 */

int	gp_parser()
{

	int i;

	/* verbose on ?, excepcion con verbose, por si lo pusieron al final de la linea !!! */
	for (i=0; i < gp_fq(GP_GET,0); i++  )
		if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'v'  )
			gp_verbose = 1;


	for (i=0; i < gp_fq(GP_GET,0);  )
	{

		/* parameter type 1 ... "name" */
		if ( i &&  *( gp_fp(GP_GET,i,(char **)0) + 0) != '-')
		{
			gp_q_partype1++;

			if (gp_q_partype1 == 1)
			{
				strcpy(finp, gp_fp(GP_GET,i,(char **)0));
			}

			if (gp_q_partype1 == 2)
			{
				strcpy(fout, gp_fp(GP_GET,i,(char **)0));
			}

			if (gp_q_partype1 == 3)
			{
				strcpy(fou2, gp_fp(GP_GET,i,(char **)0));
			}

			if (gp_q_partype1 == 4)
			{
				strcpy(fou3, gp_fp(GP_GET,i,(char **)0));
			}

			if (gp_verbose)
			{
				printf ("Param tipo 1: %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* parameter type 2 ... "-something" */
		if ( i && *( gp_fp(GP_GET,i,(char **)0) + 0) == '-' && !tiene_igual( gp_fp(GP_GET,i,(char **)0) ) )
		{
			gp_q_partype2++;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'v'  )
				gp_verbose = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'h'  )
				gp_help = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'm'  )
				gp_minusculas = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'f'  )
				gp_fsentencia = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 'e'  )
				gp_eol = 1;

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 's'  )
				gp_skip = 1;

			if (gp_verbose)
			{
				printf ("Param tipo 2: %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* parameter type 3 ... "-someoption=somename" */
		if ( i && *( gp_fp(GP_GET,i,(char **)0) + 0) == '-' && tiene_igual( gp_fp(GP_GET,i,(char **)0) ) )
		{
			gp_q_partype3++;

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"inp",3) )
					strcpy(finp,desde_igual( gp_fp(GP_GET,i,(char **)0)));

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"out",3) )
					strcpy(fout,desde_igual( gp_fp(GP_GET,i,(char **)0)));

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"dic",3) )
				{	gp_diccionario=1;
					strcpy(fdic,desde_igual( gp_fp(GP_GET,i,(char **)0)));
				}

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"mrk",3) )
				{	gp_tabmrk=1;
					strcpy(fmrk,desde_igual( gp_fp(GP_GET,i,(char **)0)));
				}

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"nvd",3) )
					gp_niveldes = *desde_igual( gp_fp(GP_GET,i,(char **)0)) - '0';



			if (gp_verbose)
			{
				printf ("Param tipo 3: %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* program name */
		if (i==0)
		{	
			if (gp_verbose)
			{
				printf ("Name:  %s\n", gp_fp(GP_GET,i,(char **)0 ) );
			}
		}

		/* next ... */
		i++;
	}

	/* si no pone parametros ... mandar a uso */
	if ( gp_q_partype1 + gp_q_partype2 + gp_q_partype3 == 0 )
		gp_help = 1;

	/* si no indico ningun  archivo ... mandar a uso */
	if ( gp_q_partype1 + gp_q_partype3 == 0 )
		gp_help = 1;

} 



/*
 *	tiene_igual
 *
 */


int	tiene_igual(s)
char	*s;
{
	int i,flag;

	for (i=0, flag=0; i< strlen(s) && !flag; i++ )
		if ( *(s+i) == '=')
			flag=1;

	return flag;
}



/*
 *	desde_igual
 *
 */


char	*desde_igual(s)
char	*s;
{
	int i,flag;

	for (i=0, flag=0; i< strlen(s) && !flag; i++ )
		if ( *(s+i) == '=')
			flag=1;

	return s+i;
}




/*
 *	gp_init
 *
 */

int	gp_init(vpar_q,vpar_p)
int	vpar_q;
char	**vpar_p;
{
	int i;

	gp_fq(GP_SET,vpar_q);
	gp_fp(GP_SET,0,vpar_p);
}


/*
 *	gp_fp
 *
 */

char	*gp_fp(action,offset,value)
int	action;
int	offset;
char	**value;
{
	static char **gp_value;

	if (action == GP_SET)
	{	
		gp_value=value;
	}

	return 	*(gp_value+offset);
}



/*
 *	gp_fq(action,value)
 *	
 *	int action
 *	int value
 *	
 *	value:
 *	on fist call, value is a number (typically argc )
 *	after first call, value can be any number, and is not used
 *
 *	returns:
 *	value
 */

int	gp_fq(action,value)
int	action;
int	value;
{
	static int gp_value=0;

	if (action == GP_SET)
	{	
		gp_value=value;
	}

	return gp_value;
}


int	gp_test()
{
	int i;

	printf ("%d\n",gp_fq(GP_GET,0) );
	for (i=0; i< gp_fq(GP_GET,0); i++)
	{
		printf ("%d %s\n",i,gp_fp(GP_GET,i,(char **)0)  );
	}
			
}

int	linea_vacia(s)
char	*s;
{
	int i,q,flag;

	i=0;
	flag=1;

	q=strlen(s);

	for (i=0; flag && i< q; i++)
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' )
			flag=0;

	return flag;
}


int	error(value)
int	value;
{
	if (gp_verbose)
		printf ("Entre a error \n");

	printf ("[%s] Error: %d\n",pgm_name,value);
	if (value >= 101 && value <= 199 )
		printf ("%s -help \n",pgm_name);
	exit(0);
}

/*
 *	tipo_char
 *	verificar el funcionamiento para "separador" ....
 *
 */

int	tipo_char(c)
char	c;
{
	int x;

	x=TC_RST;

	if (c == '\n' )
		x = TC_EOL;

	if (c == '\r' )
		x = TC_RET;

	if (c == ' ' || c == '\t' )
		x = TC_BLA;

	/* atencion ... si se usa separador , o ; */
	if (c == ',' )
		x = TC_PCO;

	if (c == ',' || c == ';' || c == ':' || c == '-' || c == '/' )
		x = TC_CCE;

	if (c == '.' )
		x = TC_PNT;

	if (c == '(' || c == '[' || c == '{' )
		x = TC_PAA;

	if (c == ')' || c == ']' || c == '}' )
		x = TC_PAC;

	if (c >= 'a' && c <= 'z' || c>= 'A' && c <= 'Z' || c == '@' )
		x = TC_LET;

	if (c >= '0' && c <= '9' )
		x = TC_NUM;

	if (c == '\"' )
		x = TC_DAP;

	if ( c == 0 )
		x = TC_EOS;

	if (c == ';' )
		x = TC_PCO;

#if 0
	if (c == '@' )
		x = TC_ARR;
#endif


	return x;
}

int	es_numero(c)
char	c;
{
	int x;

	x=0;
	if ( c >= '0' && c <= '9' )
		x=1;

	return x;
}


/*
 * 	es_num_tk
 * 	devuelve true si string esta compuesto solo de numeros
 *
 */

int	es_num_tk(s)
char	*s;
{
	int i,x;

	x=1;
	for (i=0; x && i<strlen(s); i++)
	{	if (s[i] < '0' || s[i] > '9' )
			x=0;

	}

	return x;
}








int	es_puntuacion(s)
char	*s;
{
	int i,x;

	x=1;

	for (i=0; x && i<strlen(s); i++)
	{	if (s[i] != '.' && s[i] != ',' && s[i] != ':' && s[i] != ';' )
			x=0;
	}

	return x;

}



int	es_word(s)
char	*s;
{
	int i,x;

	x=1;

	for (i=0; x && i<strlen(s); i++)
	{	if (s[i] < 'A' || s[i] > 'z' || ( s[i] > 'Z' && s[i] < 'a')  )
			x=0;
	}

	return x;

}



int	char_demed(c)
char	c;
{
	int	x;

	x=0;
	if ( c == 'e' || c == 'E' || c == 'x' || c == 'X' )
		x=1;

	return x;
}


int	mostrar(t_case,n_token,s_token)
int	t_case;
int	n_token;
char	*s_token;
{

	if (gp_verbose)
	{
		printf ("t: %3d (%2d)   %s \n", n_token,t_case,s_token);
	}
}


int	uso(x)
int	x;
{
	printf ("Usage: \n");
	printf ("inp_file out_file                   inp_file: texto input  out_file: archivo de salida\n");
	printf ("-inp=inp_file -out=out_file         inp_file: texto input  out_file: archivo de salida\n");
	printf ("-h                                  help                                              \n");
	printf ("-v                                  verbose ... muestra cierta informacion de proceso \n");
	printf ("-m                                  salida a archivo de output en minuscula           \n");
	printf ("-f                                  salida a archivo de output en formato sentencia   \n");
	printf ("-e                                  fuerza string \"EOL\" al final de cada sentencia  \n");
	printf ("-nvd=N                              Nivel de descripcion en archivo de salida (solo sin -f) \n");
	printf ("                                    0 Sin descripcion  \n");
	printf ("                                    1 Agrega la sentencia y numero de token \n");
	printf ("                                    2 Datos del diccionario (tags) \n");


	exit(x);
}


/*
 * 	gp_default
 *
 * 	inicializa parametros de funcionamiento default
 *
 */

int	gp_default()
{
	gp_help=0;
	gp_verbose=0;
	gp_diccionario=0;
	gp_minusculas=0;
	gp_fsentencia=0;
	gp_eol=0;
	gp_niveldes=0;
	gp_tabmrk=0;
	gp_skip=0;
}




/*
 * 	pasar_a_minusc
 *
 *
 */

char	*pasar_a_minusc(s)
char	*s;
{
	static char b[MAXB];

	int i,j,k;

	strcpy(b,s);

	for (i=0; i<MAXB && b[i]; i++)
	{
		if ( b[i] >= 'A' && b[i] <= 'Z' )
			b[i] += 32;
	}

	return b;
}


/*
 * 	Archivo de marcas
 *
 * 	ejemplo !!
 *
 */



#if 0

no no RN 0.999297 NEG

ni ni CC 0.834853 CNEG

pero pero CC 0.999902 ENDNEG

sin sin SP 1 TNEG

se ser VMI000 1 VSE

observar observar VMIP3S0 0.989241 OBS
observa observar VMIP3S0 0.989241 OBS
observada observar VMP00SF 1 OBS
observadas observar VMP00PF 1 OBS
observadose observadose VMSP3S0 1 OBS
observan observar VMIP3P0 1 OBS
observando observar VMG0000 1 OBS
observandosse observandosse NCMS000 1 OBS
observaron observar VMIS3P0 1 OBS
observo observar VMIP1S0 1 OBS

detectar detectar VMP00SM 1 DET
detectado detectar VMP00SM 1 DET
detectaron detectar VMIS3P0 1 DET
detecto detectar VMIP1S0 1 DET

evidenciar evidencia NCFS000 0.614458 EVI
evidencia evidencia NCFS000 0.614458 EVI
evidencia evidenciar VMIP3S0 0.373494 EVI
evidencias evidencia NCFP000 0.989726 EVI

visualizar visualizar VMIP3S0 0.989236 VIS
visualiza visualizar VMIP3S0 0.989236 VIS
visualizacin visualizacin NCMS000 0.677562 VIS
visualizacion visualizacion NCFS000 1 VIS
visualizan visualizar VMIP3P0 1 VIS
visualizando visualizar VMG0000 1 VIS
visualizar visualizar VMN0000 1 VIS
visualizo visualizar VMIP1S0 1 VIS

identificar identificar VMN0000 1 IDE
identifica identificar VMIP3S0 0.989241 IDE
identifican identificar VMIP3P0 1 IDE
identificando identificar VMG0000 1 IDE

#endif


/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */



#endif




#if 0


/*
 * diccio01.txt
 * diccionario contact center
 *
 */

a
aa
aaa
aaber
aadi
aaj
aar
abad
abadie
abajo
abandone
abaonar
abaratar
abarca
abastecimiento
abb
abba
abc
abdulrahman
abel
aber
aberiguo
aberse
aberturas
abia
abian
abierta
abiertas
abierto
abiertode
abiertos
abisan
abisar
abismal
abl
aboerto
abogada
abogado
abogados
abollada
abollado
abollar
abona
abonaba
abonada
abonado
abonados
abonan
abonando
abonar
abonaria
abonarian
abonarla
abonarlo
abonarse
abonas
abone
abonen
abono
about
abr
abra
abrany
abre
abren
abri
abria
abriendo
abrieron
abril
abrimos
abrio
abrir
abrira
abrirle
abrirlo
abro
abrosetti
absoluta
absolutamente
absolutamnete
absolutos
absurdamente
abuela
abuscar
abuse
abusen
abusivo
abuso
ac
aca
acaba
acaban
acabo
acacias
acarrea
acaso
acassuso
acasuso
acasusso
acbqgdflkedxbtkymdht
acc
accecible
acceda
accedan
acceder
accedi
accediendo
accedio
accedo
accelo
accesar
accesible
accesibles
acceso
accesorio
accesorios
accidente
accion
accionar
accionare
acciones
accuones
ace
acebedo
aceguraron
aceite
aceleren
acelrar
acentado
acentar
acepta
aceptaban
aceptada
aceptado
aceptan
aceptar
aceptarlo
acepten
acepto
acer
aceracrme
acerandose
acerca
acercado
acercan
acercando
acercandome
acercandose
acercar
acercara
acercaran
acercare
acercaria
acercarian
acercarles
acercarme
acercarmelo
acercarnos
acercaron
acercarse
acerce
acerco
aceres
aceriguar
acerlo
aceros
acerque
acerquen
acevedo
acha
achega
acia
ackron
aclar
aclara
aclaraba
aclaracion
aclarado
aclaramos
aclaran
aclarando
aclarar
aclarare
aclararle
aclararles
aclararme
aclararon
aclare
aclaren
aclarenme
aclaro
acolchado
acomodada
acomodar
acompana
acompanando
acondicionado
aconsejo
acontecido
acopanados
acoradar
acordada
acordado
acordamos
acordando
acordandome
acordar
acordaron
acorde
acordes
acordo
acortados
acosta
acostumbrada
acostumbrado
acotada
acotado
acoyte
acqua
acredita
acreditacion
acreditar
acredite
acredito
acrilico
acseda
acta
actas
actitud
actitudes
activa
activacion
activar
activen
actividad
actividades
activo
acto
actos
actuaciones
actual
actuales
actualice
actualicen
actualidad
actualiza
actualizacion
actualizaciones
actualizada
actualizadas
actualizado
actualizados
actualizan
actualizar
actualizarla
actualizaron
actualizarse
actualizen
actualizo
actualmente
actuan
actulizaron
actuo
acudan
acudi
acudieron
acudio
acudir
acudire
acudo
acuerda
acuerde
acuerdo
acuna
acurdir
acusa
acusan
acusar
acuse
acuses
ad
ada
adaptable
adaptarlo
adapten
adapto
adara
addidas
adecco
adeco
adecuada
adecuado
adelante
adelia
adelina
ademas
adentro
aderezos
adesivo
adherir
adherirme
adherirnos
adhesiva
adicional
adicionales
adicionalmente
adidas
adidasgacias
adidasy
adieren
adinde
adivinando
adivinar
adivinen
adjunta
adjuntado
adjuntamos
adjuntando
adjuntar
adjuntarlas
adjuntarlo
adjuntaron
adjuntas
adjunto
adm
admicion
administracion
administrativa
administrativas
administrativo
administrativos
admiracion
admision
admisiones
admistrativo
admitida
admitido
admitidos
admitio
adolfo
adomicilio
adonde
adosar
adquiera
adquierido
adquiero
adquiri
adquiridas
adquirido
adquiridos
adquirio
adquirir
adquiriri
adquirirlo
adrian
adriana
adrogue
aduana
aduce
aducen
aduciendo
adulterado
adulto
adultos
advance
advertir
advierte
ae
aer
aerea
aereo
aero
aeronautico
aeronaval
aeroparque
aeropuerto
aeropueto
aesperar
af
afa
afashop
afecta
afecto
afiliacion
afiliados
afilio
afines
afip
afirma
afirmando
afirmar
afirmativa
afirmativo
afjkhasjkdfasgfasdf
afrecerme
afrontar
after
aftership
aftershipping
afuera
ag
agarrar
agarren
agaurda
age
agena
agencia
agencias
agenciero
agendar
agende
agente
agentes
agfzb
agiardo
agil
agiles
agilice
agilicen
agilizan
agilizar
agina
agip
ago
agostina
agosto
agota
agradece
agradeceira
agradecemos
agradecer
agradecere
agradeceremos
agradeceria
agradecida
agradecido
agradecidos
agradeciendo
agradecreia
agradecria
agradesco
agradesoco
agradezco
agrado
agrale
agravio
agrdeceria
agrega
agregado
agregando
agregar
agregarlo
agregaron
agrego
agregue
agreguen
agresivamente
agricola
agricultura
agro
agroalimentarios
agronomia
agropecuaria
agrupacion
agrupate
agua
aguada
aguado
aguaerdo
aguagracias
aguantarlo
aguaray
aguardamos
aguardando
aguardar
aguardarme
aguarde
aguardo
aguer
aguero
aguila
aguilar
aguilares
aguilera
aguirre
aguna
agusrdo
agustin
agustina
ah
ahace
aher
ahi
ahigracias
ahira
ahn
aho
ahoa
ahor
ahora
ahorrar
ahun
ahy
ai
aies
aigue
ailen
ailin
aimogasta
aiqidjzlgs
air
airaudo
aire
aireador
aires
airestendria
ais
aislado
aixa
aj
ajena
ajenas
ajeno
ajo
ajustar
akdaelfbddcqnccus
akguno
aky
al
ala
alabamas
alaburar
alacena
alambre
alamos
alan
alarcon
alas
alba
albana
albaniles
albano
albaran
albarello
albarellos
albarez
albarino
albarracin
albbasierpe@gmail
alberdi
alberti
alberto
alborada
albornoz
alcaire
alcance
alcances
alcanza
alcanzaba
alcanzada
alcanzar
alcanzaran
alcaraz
alcira
alcocer
alcohol
alcolchados
alcorta
aldana
aldao
aldea
alderetes
aldo
aleatoriamente
aledanas
aledanos
alegan
alegando
alegar
alejada
alejandra
alejandro
alejo
alem
alemania
alerta
alesio
alexander
alexandra
alexis
alfombra
alfombras
alfonsin
alfonsina
alfonso
alfredo
algo
alguan
alguie
alguien
alguin
alguirn
algun
alguna
algunas
alguno
algunos
algunpunto
alho
alianza
alias
alicia
aliexpress
alimentacion
alimenticios
alina
alisos
aljibe
alla
allagracias
allan
allanada
allanado
allegado
allegar
allen
allende
aller
alli
allyson
alma
almacen
almacenado
almada
almafuerte
almagro
almerares
almirante
almohadas
almohadones
almparecer
almuerzo
alojado
alonso
alquien
alquilaba
alquilan
alquilar
alrededor
alrededores
alsina
alta
altas
alte
alternativa
alternativas
alternativo
alto
altos
altura
aluminio
alumna
alumnos
alv
alvadocumento
alvarenga
alvares
alvarez
alvear
alvez
alzette
am
ama
amabilidad
amable
amablemente
amables
amado
amalejo
amalia
amarilla
amas
amazon
amba
ambas
ambientes
ambito
ambos
ambroseti
ambrosetti
ambrosio
ambrosti
ambul
ameghino
ameldi@gmail
amenabar
amenacen
amenazaba
amenazando
amendola
amentamos
ameooo
america
american
americas
amex
ami
amicone
amiga
amigable
amigo
amigos
aminoacido
amionetas
amiun
amoladoraskill
amonestar
amor
amores
amparo
ampliamente
ampliar
amplio
amrtin
amtatock
an
ana
anabel
anabela
anabella
anac
anahi
anali
analia
analitico
analiticos
analizar
analogico
anastasia
anatuya
ancho
anchorena
anchoris
anchox
anciano
anclada
and
anda
andaba
andaban
andacollo
andalgala
andan
andar
andaron
andas
ande
andecam
andes
andie
andino
ando
andrade
andre
andrea
andreani
andrejuk
andres
andresito
andreza
andriani
android
anduvieron
anduvo
anegada
anellirena@mailcan
anelo
anexar
angel
angela
angeli
angelina
angelis
angelita
angerphoit
angostura
angrisani
anguil
angular
angustia
anibal
anillos
anima
animo
anna
ano
anoche
anos
anotacion
anotada
anotando
anotar
anotaron
anote
anoto
anriquez
anses
ansias
ansiosamente
ansioso
ante
antecedentes
antedicho
antelacion
antemano
antencion
antender
anteponer
anteponiendo
anterior
anteriores
anterioridad
anteriorme
anteriormente
antes
antesgracias
anti
anticipacion
anticipando
anticipar
antienden
antigua
antiguedad
antiguo
antiguos
antiguosd
antina
antipanico
anto
antoja
anton
antonella
antonia
antonieta
antonini
antonio
ants
antune
anulaba
anulada
anular
anularlo
anularon
anuncia
anuncian
anunciando
anunciandose
anunciaron
anuncio
anviar
aokulasic
aoo
apagado
apago
apara
aparatito
aparato
aparce
apare
aparece
apareceb
aparecen
aparecer
aparecere
aparecia
aparecian
apareciendo
apareciera
aparecieron
aparecio
aparecion
aparecioque
apareciron
aparenta
aparente
aparentemente
aparesca
aparevio
aparezca
aparezcaan
aparezcan
aparicio
aparicion
apariencia
apartado
apartamento
aparte
apartir
apba
apellida
apellido
apem
apenas
apercibimiento
apersonar
apersonarme
apersonaron
apersone
apersono
apertura
apetece
api
aplaudieran
aplausosque
aplicacion
aplicar
aplicativo
aplio
apliquen
apmision
apoderada
apoderadacon
apoderado
apoderados
apoderar
apolinario
aporta
aporte
aporximadamente
apostando
apostillado
apostillados
apostillar
apostillarlo
apostille
apostoles
aposyilla
apoximada
apoyo
app
approx
apps
apr
apra
aprarece
apreciare
aprecio
aprendan
aprender
apretado
aprezcan
aprobacion
aprobado
aprobaron
apropiada
aproposito
aprosimadamente
aprovechar
aprovecho
aprox
aproxi
aproxima
aproximada
aproximadamente
aproximado
aproximarme
aproxmadamente
apto
aptos
apuntada
apunto
apurado
apuran
apurar
apurarlo
apuren
apuro
apwu
aq
aqcuerdo
aquello
aquellos
aquete
aqui
ar
ara
arabel
araceli
aragones
aran
aranceles
arandano
arandanos
arandelitas
aranguren
aranzuzu
araoz
arata
araya
arbol
archivo
archivos
arcos
arcubic
ardid
arduino
area
areas
areaticket
areco
aremitente
arenales
arenas
arenaza
arequito
arevalo
arfiel
arfield
arg
arganaraz
argenta
argentiba
argentina
argentinas
argentino
argentinos
argerich
arguello
argumentan
argumentando
aria
ariadna
arian
ariana
ariano
arias
ariculo
ariel
arijon
aril
arimex
ariobamba
aristibulo
aristobulo
ariticulo
arl
armada
armado
arman
armando
armar
armaron
arme
armella
armstrong
arnaldo
arnet
aromos
arq
arrancada
arrebatarme
arrechea
arrecife
arrecifes
arredo
arredondo
arregalr
arreglaba
arreglado
arreglamos
arreglando
arreglar
arregle
arreglen
arreglenlo
arreglo
arrepiento
arriba
arribado
arribados
arribando
arribar
arribe
arribo
arriesgarme
arrivo
arroba
arroja
arrojan
arrojar
arrow
arroyito
arroyo
arroyos
arruinada
arruinaron
arruino
arsfield
art
arta
arte
arteaga
artes
artesanal
articular
articulo
articulos
artidas
artigas
artiko
arto
arts
arturo
as
asa
asaber
asaltado
asaron
ascasubi
ascensor
asciende
ascooo
ascoooooo
ase
asecte
asegirarse
asegura
asegurada
asegurado
aseguran
asegurando
asegurandome
asegurar
asegurarlo
asegurarme
aseguraron
asegurarse
aseguren
aseguro
asemas
asembor
asen
asentada
asentado
asentar
aser
aserlo
asesor
asesorado
asesoramiento
asesorar
asesorarme
asesorarnos
asesorarse
asesoras
asesore
asesoren
asesoria
asfaltada
asfalto
asg
asi
asia
asiduamente
asiduo
asiganar
asignacion
asignaciones
asignada
asignado
asignar
asignaron
asignatura
asigne
asignen
asigno
asimismo
asinamiento
asiq
asique
asirevisar
asista
asiste
asisten
asistencia
asistido
asistie
asistieron
asistir
asistiran
asistirme
asl
aslele
aso
asoc
asociacion
asociadas
asociados
asociar
asociarme
asomamos
asombra
asp
aspectos
aspidpieza
aspiradora
aspnumero
aspxdatab
aspxnumbertracking
aspxqjblnfbsud
aspxqjeoup
asselborn
asta
astudillo
asu
asucarsal
asuncion
asunto
asuntos
asy
asz
at
atada
ataque
atardecer
atarjeta
atascado
atee
ateinda
ateinedem
aten
atenc
atencio
atencion
atencionalclientebue@dhl
atencison
atendamos
atende
atendedor
atendemos
atenden
atender
atenderan
atenderlas
atenderle
atenderlo
atenderme
atendi
atendia
atendian
atendible
atendida
atendidas
atendido
atendidos
atendiendo
atendieran
atendieron
atendiese
atendimos
atendio
atendiso
atener
atenta
atentamente
atentamete
atentando
atentas
atente
atento
atentos
ather
atienda
atiendaa
atiendad
atiendan
atiende
atienden
atiendenen
atiendennn
atiendes
atiendo
atiene
atienen
atiente
atienza
atineden
atlantica
atraer
atrajeron
atras
atrasada
atrasado
atrasados
atrasan
atrasar
atrasarse
atrasme
atraso
atraves
atravesando
atravesar
atravez
atrevidos
atrienden
atril
atrio
att
atte
atuemde
au
auahqa
aucirsal
aucursal
audiencia
audifonos
audiocenter
auditoria
aue
auer
augusto
aumasque
aumenta
aun
aunque
auqneu
auricular
auriculares
ausencia
ausentaba
ausentarme
ausente
ausento
australia
autegestion
autenticado
authier
auto
autoadhesiva
autochap
autocity
autogestion
autogestionable
autogestionar
automatica
automaticamente
automatico
automotor
automotores
automovilismo
autonoma
autopista
autorice
autoricen
autoridades
autorizacion
autorizada
autorizadas
autorizado
autorizan
autorizandolo
autorizar
autorizarlo
autorizarme
autorizen
autorizo
autos
auxiliadora
auxiliar
av
avalcado@yahoo
avalos
avance
avancen
avances
avanida
avantrip
avanza
avanzado
avanzar
avanzo
avda
ave
aveces
avelino
avellaneda
avenida
aver
avergue
averiaguar
averias
averiguado
averiguando
averiguar
averiguarlo
averiguarlogracias
averiguarme
averiguarnos
averigue
averiguen
averiguo
averio
averme
avery
avia
aviacion
avian
avidaran
avila
aviles
avilla
avinsenme
avio
avion
avisa
avisaba
avisaban
avisada
avisado
avisados
avisamdome
avisan
avisando
avisandome
avisar
avisara
avisaran
avisarle
avisarles
avisarme
avisaron
avise
avisen
avisenemen
avisenle
avisenme
aviso
avisos
avivada
avizo
avlares
avor
axel
axion
ay
aya
ayacucho
ayahuma
ayala
ayde
ayelen
ayeln
ayer
ayerq
ayjz
aylagas
ayohuma
ayre
ayres
ayuda
ayudado
ayudame
ayudan
ayudar
ayudaran
ayudarespero
ayudarian
ayudarme
ayudarvtengo
ayudasen
ayude
ayudeme
ayuden
ayudenme
ayudennos
azamor
azcuenaga
azucar
azul
azulejos
azzarello
b
ba
baas
babuchitas
bacacay
bacha
bachiller
bacle
bad
badia
baez
bafle
bafles
bags
bagues
bah
bahia
baigorria
bailecito
bailez
bainorte
baires
baiska
baja
bajaba
bajada
bajando
bajar
bajara
bajarlo
bajarnos
baje
bajen
bajjo
bajo
balacarce
balaguer
balance
balbi
balbin
balcarce
baldini
baldunciel
balleste
ballester
balnearia
balneario
balor
balvanera
balvin
bamco
bancalari
bancanria
bancaria
bancarias
bancario
bancarios
banco
bancogalicia
bancos
banda
bandeja
bandera
banderalo
banegas
banelco
banfield
banfieldcuyo
banitory
bank
banking
bano
banuito
bar
baradero
barado
barato
barbara
barbaridad
barbeito
barboza
barbuscio
barca
barcelona
barcena
barci
barda
bariloche
bario
barni
baron
barquitos
barra
barracas
barrancas
barranqueras
barras
barreal
barrere
barrio
barrionuevo
barrios
barro
barros
bartio
bartolme
bartolome
bas
basail
base
basicamente
basico
basile
basilio
bassano
bassi
basta
bastante
bastantes
bastaria
basto
basualdo
basura
bateria
batilana
batista
bato
baun
bautismo
bautista
bb
bc
bco
bcod
bcslr
bd
be
beatriz
beauty
beba
bebe
bebedero
bebes
beccar
bederman
beel
begonia
being
beiro
bel
belaustegui
belen
belgrano
belizan
bell
bella
bello
belotti
beltran
belviso
belzunce
benardoni
benavidez
bendahan
bendiciones
bendiga
bendita
bendito
beneficio
beneficios
benitez
benito
bentarcon
bentivoglio
benz
berabevu
berasategui
berazategui
berciano
berenice
beresnak
bergamni
berguenza
beriso
berisso
berlin
berlingo
bermejo
bermudas
bermudez
bernachi
bernacki
bernal
bernarda
bernardez
bernardino
bernardis
bernardo
bernasconi
berrio
berrotaran
berruezo
berthet
bertie
bertin
bertollo
beruti
berutti
beso
besos
betarte
betina
bettina
bettini
bexacta
bfoot
bgh
bgui
bh
bhi
biagini
bianca
biblioteca
biblos
bicecleta
bici
bicibleta
bicicleta
bicicletas
bicileta
bicocca
bidon
bidones
bien
biene
bienen
biennnnnnnnnnnnnnn
bienos
bijouterie
bike
bikini
bikinis
billetera
billeteras
billinghurst
binimelis
bino
bioheuris
biopsiados
biorgen
birn
birome
bis
bissutti
black
blaistein
blanc
blanca
blancia
blanco
blanes
blanquear
blanquearla
blanqueria
blas
blegrano
blister
bllaponagat
blnca
block
bloqueado
blos
blue
blumenage
bmmujfj
bnos
bo
boating
boatti
bobis
boca
bocas
bocashop
bocayuva
bocina
boda
bodrio
body
boedo
bogado
bohemia
bohle
boisi
boiteux
bokura
bola
bolante
bolestas
boleta
boletas
boleto
bolilla
bolivar
bollini
bolonia
bolsa
bolsas
bolsillo
bolsin
bolsines
bolsita
bolso
bolson
boludeando
boludear
boludeces
boludendo
boludo
boludos
bom
bonaccorsi
bonacossa
bonacossamarcelo@gmail
bonaduci
bonafede
bonar
bonfigli
bonificacion
bonifique
bonifiquen
bonofique
bonus
bonzi
book
bootstrap
boquilla
bordes
bordon
borges
borra
borrar
borre
bosch
bosh
bosi
bosin
bosques
bossinga
bota
botas
botella
botellas
botines
botiquin
boton
botones
bottger
bouchard
boulevar
boulevard
boulevares
boulogne
bouquet
bouquez
bourg
bourgo
boutique
boutros
bovina
bowls
boxeo
boxer
boyando
boyle
bozzone
bph
bpn
bqnbc
bracchi
bragado
bragg
braian
braicovich
bramed
brandlive
brandsen
brasil
brasilera
brasilia
bravo
brebedad
brenan
brenas
brenda
breton
breve
brevedad
brian
bridaron
brinda
brindado
brindan
brindando
brindandome
brindar
brindaran
brindarles
brindarme
brindarnos
brindaron
brinde
brinden
brindo
brinkmann
brir
bristol
britez
britos
brizuela
bro
broach
brochero
broer
brollo
brolun
broma
bronca
bronquitis
brossard
brown
brrio
brukt
brunaga
bruno
brusa
brusco
brutalidad
brutos
bs
bsas
bsj
bsmata
bsnta
bsszdt
buacar
bucando
bucci
buchardo
budge
bue
buelta
bueltas
buen
buena
buenaa
buenaaa
buenas
buenasqueria
buenasvtardes
buendia
buenis
buenisimo
buennas
bueno
buenoa
buenoas
buenod
buenos
buenosbdias
buenosdias
buey
bufano
buho
buj
bulevar
bulevares
bullto
bulnes
bulonera
bulto
bultos
bunas
bune
bunen
bunero
bunny
bunos
buo
buodo
buratovich
burbujas
burd
burgos
burla
burlan
burlaron
burocracia
burocratassss
burro
bursaco
burzaco
busac
busacr
busacrlo
busar
busca
buscaba
buscado
buscador
buscan
buscanco
buscando
buscar
buscare
buscarla
buscarlas
buscarlo
buscarlogracias
buscarlos
buscas
buscasr
busco
buso
busque
busqueda
busquedas
busquen
bussolini
bustamante
busto
bustos
but
buta
butacas
butacauno
buwcador
buwnas
buzo
buzon
buzos
bv
bvoc
bvpiez
by
bynnon
byon
c
ca
caa
caamano
cab
caba
caballito
caballo
cabe
cabecera
cabello
cabeza
cabiendo
cabildo
cabin
cable
cablido
cabo
caboto
cabral
cabrales
cabrera
cac
cacarana
caceres
cachari
cache
cachorro
cacique
cada
cadena
cadete
cadeteria
caduco
cae
cafayate
cafe
cafetera
cafital
cagada
cagan
cagar
cagaron
cagarse
cago
caguen
cah
cahncari
caida
caido
caiga
caja
cajaraville
cajas
cajita
cajones
cajs
caka
calabresi
calada
calafate
calamuchita
calchaqui
calco
calcula
calculaba
calculador
calculan
calcular
calcule
calculo
calculos
calderon
calducci
cale
calefactor
calefactores
calefones
calel
calera
cales
caleta
caleufu
calfulqueo
calidad
caliente
calificacion
calificaciones
calificado
calificar
calificarme
california
calingasta
call
calla
callao
callcenter
calle
callende
calles
callo
calor
calvo
calzada
calzado
calzados
cama
camacua
camano
camara
camaras
camarlinghi
camarones
camastro
cambia
cambiaban
cambiado
cambian
cambiando
cambiar
cambiaran
cambiaremos
cambiaria
cambiarla
cambiarlo
cambiarnos
cambiaron
cambiarse
cambie
cambien
cambienlos
cambio
cambios
camelia
camila
camilla
camilo
caminando
caminar
camino
caminoo
camion
camioneros
camiones
camioneta
camionetas
camisetas
cammilleri
camoga
campagnaro
campagnat
campana
campanas
campanino
campanita
campara
campera
camperas
campichuelo
campillo
camping
campo
campodinico
campos
campus
camuso
can
canada
canadon
canal
canalejas
canales
canalicen
canalizar
canals
cancel
cancela
cancelaba
cancelacion
cancelada
cancelado
cancelan
cancelar
cancelaron
cancele
cancelen
cancelo
cancha
candela
candelaria
candida
candidato
cane
caneda
cangallo
cangie
canning
cano
cansada
cansado
cansafp
cansamos
cansando
cansaron
canse
canso
canta
cantar
cante
cantidad
cantidades
cantilo
canto
canuelas
caos
cap
capaces
capacidad
capacidades
capacitacion
capacitar
capacitarnos
capas
capaz
capial
capif
capilar
capilla
capita
capital
capitaloca
capitan
capo
capot
caprichoso
capristo
capsulas
captal
capturas
caputal
car
cara
carabajal
caraballo
caracciolo
caracter
caracteres
caracteristicas
caracteriza
caracterizaba
caradurez
carafour
carajo
carajoooooooo
carajos
caram
carapachay
cararcter
caras
carballal
carballo
carbone
carcarana
carcasa
carcasas
card
cardales
cardena
cardenal
cardenales
cardenosa
cardiologia
cardozo
carece
carecen
carente
carfur
carga
cargada
cargadas
cargadir
cargado
cargador
cargados
cargan
cargando
cargar
cargarian
cargarlo
cargarlos
cargaron
cargarse
cargas
cargo
cargos
cargue
carguen
carhue
carisimo
carla
carlos
carlota
carmag
carmel
carmela
carmen
carmencito
carmensa
carmona
carnet
carnets
caro
carolina
caroya
carpa
carpeta
carpetas
carpinteria
carranza
carre
carrefour
carrefuor
carrefur
carreta
carriego
carrier
carril
carrito
carrizo
carroza
carrusel
carsa
carta
cartada
cartagena
cartas
carte
cartel
cartelera
carteleria
carteles
cartelito
cartelli
carter
cartera
carteras
carterita
cartero
carteros
cartertero
cartier
carton
cartoncito
cartones
carullo
cas
casa
casacuberta
casali
casamiento
casandra
casanova
casares
casarme
casarrubios
casas
casay
casero
caseros
casi
casiello
casilda
casilla
casillero
casino
casita
caso
casos
casse
castano
castelar
castellano
castellanos
castelli
castells
castex
castigo
castilla
castillo
castro
casualidad
casualmente
casuarinas
casulalidad
catalano
catalina
catalogaron
catalogo
catamarca
catan
catedral
categoria
categorias
caterina
catonas
catozzi
catre
catriel
catulo
caudal
caula
causa
causando
causaria
causaron
causas
cavagna
cavallin
cavia
cavibles
caye
cayenato
cayendo
cayetano
cazaux
cazon
cazorla
cba
cbj
cbkytbpbg
cbu
cbvx
cc
cco
ccy
cd
cdad
cdb
cdba
cdc
cdd
cde
cdf
cdg
cdigo
cdnmwldv
cdo
cdqo
cds
cdu
cdy
ce
ceballos
cebey
cecana
cecilia
cecilia@graphictex
ceclamo
cedasayrxyajiifdqczpfqu
cede
cedulas
ceh
ceibas
ceibo
ceibos
cel
celeridad
celeste
celestes
celia
celina
cellini
cellular
celso
celu
celular
celulares
celumar
celurar
cemana
cementerio
cen
cenco
cencosud
cencosur
centenario
centenarios
centeno
center
centerario
centimetros
central
centrar
centre
centrico
centro
cenular
ceparo
cepeda
cepillo
ceramica
ceramicas
ceramicos
cerca
cercana
cercanaque
cercanas
cercania
cercano
cerda
ceres
ceritificada
cero
ceros
cerrada
cerradas
cerrado
cerrados
cerramos
cerrando
cerrar
cerrarla
cerraron
cerrarse
cerredada
cerri
cerrito
cerro
cerroti
certera
certero
certeza
certezas
certificacion
certificaciones
certificada
certificadaa
certificadas
certificado
certificados
certifican
cervantes
cerveza
cervicio
cervino
cesar
cetificada
cetrogar
cetrograr
cetrohogar
cevallos
cezmfuwoyjzy
cf
cfapital
cfc
cfviyl
cfx
cgglmegbyv
cgo
cgt
ch
cha
chabas
chaca
chacabuco
chacarita
chaco
chacon
chacra
chajari
chaleco
chalten
chamical
champagnat
chamuyo
chanar
chanares
chance
changomas
chantas
chantre
chapapi
chaparro
charadai
charata
charcas
charlando
charlar
charlone
charrua
chas
chascomus
chata
chateando
chavarria
chavez
chazarreta
checkiando
chelini
chemes
chepes
cheqeuar
cheque
chequeaba
chequeamos
chequean
chequeando
chequear
chequearlo
chequee
chequeo
chequeras
cheques
chequie
chiapello
chic
chica
chicas
chico
chiconigabriel@hotmail
chicos
chilavert
chile
chilecito
chilotes
china
chinchirin
chingo
chingolo
chino
chinomayorista
chiosa
chip
chipre
chips
chiquita
chiquitas
chiquito
chiriboga
chiripa
chiste
chiuito
chivilcoy
chochecito
chocolate
chocolates
chocopart
choel
choele
chofer
choferes
chomicz
choque
choques
chorroaein
chorros
chos
chris
christian
chrone
chubut
ci
cia
ciano
cibtestaxion
cidigo
ciego
cielos
ciencias
ciendo
cierra
cierran
cierre
cierta
ciertas
cierto
ciertos
cierva
cifani
cilek
cilindro
cima
cimientos
cin
cinc
cinco
cindi
cinimos
cinta
cintas
cintestan
cinthya
cintia
cinto
cintra
cintura
cio
cipoletti
cipolleti
cipolletti
circ
circuito
circulacion
circulo
circunsc
circunstancia
circunstancias
circunvalacion
cirigliano
cirugia
cirvalan
cisneros
cistina
cita
citacion
citada
citadina
citado
citados
citaron
cito
city
ciudad
ciudadania
ciudadano
ciudadela
ciudades
ciudadevita
civil
civile
cjenta
ck
cla
clara
claramente
claretiana
clarg
claridad
clarificar
clarisa
clarisimo
claro
claros
clase
clases
clasica
clasico
claudia
claudio
clavada
clavado
clave
clavero
clavo
claypole
clemente
clemira
clic
click
clickear
clie
clienta
clientas
cliente
clientes
clik
clikiar
clima
climaticas
clinca
clinica
clmonicarme
clorinda
clte
club
cm
cmndte
cmo
cmp
cms
cmte
cmts
cmx
cn
cndo
cnea
cnel
cnochaert
cnottbyuwhjscod
cnpaxrurgthezk
cnsulta
cnsulto
co
coassolo
cobarn
cobertura
cobos
cobra
cobraba
cobrado
cobrale
cobramos
cobran
cobrando
cobrandome
cobranza
cobranzas
cobrar
cobraran
cobrarian
cobrarlo
cobrarme
cobraro
cobraron
cobre
cobren
cobro
cobros
cobtrareembolso
coca
cocacola
cochabamba
cochecito
cocilobo
cocina
cocot
cocu
cocuzza
cod
code
codifo
codig
codiga
codigo
codigobierno
codigocde
codigos
codigp
codo
coffe
cofigo
coherencia
coherente
cohinciden
coihues
coincide
coinciden
coincidia
coincidir
coiron
cola
colaboracion
colaboradores
colaboren
colapsada
colapsado
colas
colastine
colchon
colchones
colchonetas
colecctora
colectivo
colectora
colega
colegiale
colegiales
colegio
colegios
colello
colgada
colgado
colgar
colgo
colibri
colja
coll
collar
collections
collon
colmada
colman
colmo
colo
colocado
colocan
colocar
colocarlo
colocarlos
colocaron
coloco
colombia
colombres
colon
colonia
colonial
coloque
coloqueo
color
colorada
colorado
colorin
colsulta
colsultar
columna
com
comafi
comallo
comandante
comarca
combinar
combo
combustibles
comedor
comenarios
comence
comenta
comentado
comentamos
comentan
comentando
comentandome
comentar
comentario
comentarios
comentarle
comentarles
comentaron
comentas
comente
comenten
comento
comenzado
comenzando
comenzar
comenzara
comenzare
comenzo
comer
comerce
comercia
comercial
comercial@maria
comerciales
comercializa
comercializacion
comercializados
comercializar
comercializo
comerciante
comercio
comercios
comerse
cometer
cometi
cometida
cometieron
comfirmar
comic
comicar
comicarme
comicilio
comience
comienda
comienza
comienzo
comienzos
comincarme
cominicame
cominicar
cominicarme
cominicarse
cominique
cominxar
comisaria
comisiones
comisionista
comite
comiunicarme
comjkicarme
coml
commerce
communicarme
comnicarme
comniocarme
comnpre
como
comodidad
comodo
comodoro
comol
comonucacion
comopre
comora
comore
comos
companeros
compania
companias
compara
comparacion
compensacion
compensar
competencia
competencias
competente
competir
competitiva
complaceria
complegisa
complejo
completa
completado
completamente
completar
complete
completo
complica
complicacion
complicada
complicado
complicados
complican
complicando
complicar
complicaria
complicaron
componente
composiciones
compra
comprada
compradas
comprado
comprador
compradora
compradores
comprados
compragamer
compragammer
compralo
compramos
comprar
comprarlo
comprarme
compraron
compras
compratuticket
compre
comprencion
comprende
comprender
comprendi
comprendida
comprendo
compresor
compro
comprobable
comprobado
comprobante
comprobantes
comprobar
comprobrante
comprometerse
comprometida
comprometieron
comprometio
compromiso
compromisos
comprovante
comprq
compruebo
compu
compuesto
compugamer
compulsar
compulsiva
compulsivashowroom
compumundo
compuna
computadora
computo
comre
comsultar
comubicarme
comuiniquen
comumicarles
comumincarme
comun
comuna
comuncarle
comuncarme
comunciarme
comuniarme
comunica
comunicaba
comunicabamos
comunicaban
comunicacion
comunicacional
comunicaciones
comunicada
comunicado
comunicamos
comunican
comunicando
comunicandome
comunicar
comunicara
comunicaran
comunicarce
comunicare
comunicaremos
comunicarian
comunicarle
comunicarles
comunicarme
comunicarmelo
comunicarnos
comunicaron
comunicarse
comunicarte
comunicase
comunicate
comunicbam
comunico
comunicque
comunidad
comunikensen
comunion
comunique
comuniqueme
comuniquen
comuniquence
comuniquenese
comuniquenme
comuniquense
comuniquensen
comuniquese
comuniquiense
comunucarme
comunycon
con
conaultar
conb
conbinada
concedidas
concejal
concep
concepcion
concepto
concertado
concesionaria
concesionario
concha
conchudos
conciencia
concierto
conciertode
concluciones
conclusion
concluyeron
concluyo
concordar
concordia
concordo
concreta
concretado
concretamos
concretar
concretarse
concretas
concrete
concreto
conctactarse
concultar
concurra
concurre
concurri
concurrido
concurrieron
concurrio
concurrir
concurriran
concurso
condarco
conde
condial
condicciones
condicion
condicionante
condiciones
condigan
condimentos
condominio
condor
condori
conducir
conductor
condulta
conectada
conectar
conectarlo
conectarme
conectarse
conecten
conel
conenna
conentaron
conexion
confeccionado
confecha
confia
confiaba
confiable
confiables
confiado
confiando
confianza
confiar
confidencial
confie
configurar
confimarme
confio
confirma
confirmaban
confirmacion
confirmada
confirmado
confirmados
confirmamos
confirman
confirmando
confirmandome
confirmar
confirmarian
confirmarlo
confirmarm
confirmarme
confirmaron
confirme
confirmeme
confirmen
confirmo
conflicto
conformar
conforme
conformidad
conformo
confort
confrontacion
confronte
confronto
confuciones
confundan
confunde
confundi
confundido
confundieron
confundimos
confundio
confundirse
confusa
confusion
confuso
congelado
congelar
congestionada
congestionado
congreso
congreto
conjunto
conlleva
conmigo
conmuniquen
connection
connumero
cono
conoce
conocemos
conocen
conocer
conocia
conocian
conocida
conocidas
conocido
conocidos
conocieran
conocimiento
conocimientos
conozca
conozcan
conozco
conpre
conscientes
consecion
consecuencia
consecuencias
consecutiva
consecutivo
consegui
conseguido
conseguir
conseguiran
conseguirlo
consejo
consertar
conserva
conservan
conservantes
conserven
considerable
consideracion
considerado
considerando
considere
consideren
considero
consigan
consigna
consignada
consignado
consigno
consigo
consiltarle
consiste
consistia
conslta
consola
consorcio
consta
constaba
constacia
constan
constancia
constancias
constante
constantemente
constantinidis
constanza
constar
constatado
constatando
constatar
constate
conste
constetan
constitucion
constituye
constituyentes
construccion
constructiva
consuklta
consulado
consular
consularizada
consulta
consultaaaa
consultado
consultaestadosolicitudes
consultamos
consultan
consultando
consultandoles
consultar
consultarle
consultarles
consultarlo
consultaro
consultaron
consultarte
consultas
consultatracking
consulte
consulto
consultora
consultorio
consultorios
consultrales
consumidor
consumidora
consumidores
consumo
consuta
consutlarlo
contaba
contaban
contable
contact
contacta
contactaba
contactaban
contactacion
contactado
contactamdo
contactame
contactando
contactandome
contactandose
contactar
contactara
contactaran
contactarce
contactarlos
contactarme
contactarnos
contactaron
contactarse
contactarte
contacte
contacten
contactenme
contactenos
contactense
contacterce
contacto
contactos
contactrme
contactrse
contadora
contamos
contancia
contando
contar
contario
contarles
contarlos
contaron
conte
conteaten
contemplar
contendria
contenia
contenido
conteniendo
content
contenta
contentan
contentos
contenua
contest
contesta
contestaba
contestacion
contestado
contestador
contestadora
contestan
contestando
contestandome
contestar
contestarme
contestaron
conteste
contesteme
contesten
contestenme
contesto
contiene
contienen
contigo
contiguos
continua
continuacion
continuamente
continuan
continuar
continue
continuo
contra
contractualmente
contradice
contradictoria
contraentrega
contraer
contramano
contrana
contrarebolso
contrareembolso
contrareenbolso
contrariamente
contrario
contrarrembolso
contrasena
contrata
contratacion
contrataciones
contratada
contratado
contratan
contratando
contratar
contrataran
contratarlo
contratarlos
contrate
contraten
contratiempo
contratiempos
contratista
contrato
contreras
contribuir
control
controla
controladora
controlando
controlar
controle
controlen
controlo
controrario
conulta
conunico
conustedes
conv
convalidacion
convalidar
convalidarlo
convencerme
convencimiento
convengamos
convenida
conveniente
convenio
convenios
convenir
conversacion
conversar
convertidor
convertir
convertirme
conviene
convocatorias
coodine
coooolmoooo
coop
coopsar
coordiales
coordibar
coordina
coordinacion
coordinado
coordinadora
coordinadra
coordinamos
coordinando
coordinar
coordinarlo
coordine
coordinemos
coordino
cooresponda
coorporativo
copa
copago
copia
copiapo
copias
copio
coppel
coppie
cor
corazon
corboda
cordial
cordiale
cordiales
cordialmente
cordillera
cordinado
cordinar
cordine
cordoba
cordobaii
cordobas
coreeo
corespondencia
coria
corientes
corijan
cornejo
cornelio
cornudos
coronado
coronda
coronel
coronil
corpa
corporacion
corporate
corporativa
corporativas
corporativo
corporativos
corpporativa
corral
corre
correa
correccion
correcciones
correcta
correctamente
correctio
correction
correctivas
correctivos
correcto
correctos
corredor
corregi
corregida
corregido
corregir
corregirla
corregirlo
correjidor
corremos
corren
correo
correos
correpondencia
correpondiente
correpsondientes
corres
corresponda
correspondan
corresponde
correspondedeja
correspondeeee
correspondeeeee
corresponden
correspondenci
correspondencia
correspondencias
corresponder
corresponderia
correspondia
correspondian
correspondiente
correspondientes
corresponsal
corresta
corria
corrida
corrido
corridos
corriendo
corriente
corrientes
corrigan
corrigen
corrigiendo
corrigieron
corrija
corrijan
corrijo
corrio
corro
corrobar
corroboraban
corroborado
corroboramos
corroborar
corroborarlo
corrobore
corroboren
corroboro
corroborsr
corrrido
corta
cortaban
cortada
cortado
cortados
cortan
cortando
cortandome
cortar
cortaremos
cortarles
cortarme
cortaron
corte
corten
cortes
cortesia
cortez
cortina
cortinas
corto
cortos
cortsn
corzuela
cosa
cosas
coser
cosme
cosmetica
cosmetico
cosmeticos
cosmetologia
cosquin
cossettini
costa
costaba
costar
costaria
costarme
costo
costogracias
costola
costos
costosa
costoso
costumbre
cosultar
cotactar
coticen
cotices
cotizacion
cotizado
cotizan
cotizar
cotizarme
cotto
coumentacion
country
couriers
covertor
coyhaique
coz
cp
cpaital
cpc
cpdigo
cpmo
cpn
cpuwthni
cr
craneo
cras
crb
crdito
creaciones
creada
creamos
crean
creando
crear
crearlo
crearme
crecer
creciendo
crecimiento
cred
credencial
credenciales
credial
credibilidad
credit
credita
credito
creditos
creditovisa
creditp
cree
creedicoop
creedito
creemos
creen
creer
creeria
crei
crema
cremona
creo
crescimiento
cresito
crespo
crespones
criquet
cristal
cristian
cristiania
cristin
cristina
cristo
cristobal
criterio
critica
criticas
croacia
crovara
crr
cruce
cruces
cruz
cruzados
cruzan
cruzar
cruzo
csha
cskt
cta
ctal
cte
ctes
cto
cu
cua
cuabdo
cuadno
cuadra
cuadrados
cuadras
cuadro
cuadros
cual
cuales
cualidad
cualq
cualqier
cualquier
cualquiera
cualquiser
cuan
cuand
cuandk
cuando
cuandolo
cuano
cuanod
cuanta
cuantas
cuanto
cuantos
cuantp
cuanyo
cuarta
cuartel
cuarto
cuatia
cuato
cuatro
cuatros
cuba
cubicos
cubiertas
cubierto
cubiertos
cubre
cubrir
cucha
cuelga
cuelgan
cuellito
cuenca
cuendo
cuensta
cuenta
cuentan
cuentas
cuenten
cuento
cuernos
cuerpo
cuerpos
cuesta
cuestan
cuestion
cuestiones
cuetna
cueva
cufalis
cuidad
cuidado
cuidar
cuidarlo
cuidd
cuiden
cuil
cuip
cuit
cullen
culo
culpa
culpables
culpan
culpen
culpina
cumpla
cumplan
cumple
cumpleanos
cumplen
cumpli
cumplida
cumplido
cumplidos
cumpliendo
cumplieron
cumplimentar
cumplimiento
cumplio
cumplir
cumplirian
cumplirla
cumplirme
cumplirse
cumunicacion
cumunicar
cumuniquese
cuna
cunado
cundo
cunha
cunik
cunoliey
cunsulta
cunsultar
cuotas
cupon
cura
curcio
curi
curiculum
curiosisimo
curriculo
curriculum
curso
cursos
curuzu
cusndo
custodia
custodiada
custodiado
customer
cutral
cuya
cuyas
cuyo
cuyos
cuzco
cv
cxarlos
cynthia
cynthia@rehavita
d
da
daba
daban
daber
dada
dado
dadone
dados
dafiti
dafitti
daiana
dainformacion
daireaux
dalonzo
dam
damaris
damian
damonte
damos
dan
danada
danado
danar
danderfer
dando
dandole
dandoles
dandome
daniel
daniela
danieli
danina
dano
danos
danubio
danvers
dar
daran
dardo
daria
darian
darin
dario
darioa
darjeen
darle
darles
darlo
darme
darmela
darnos
daron
darquier
darragueira
darse
darte
darwin
das
dasdasdsad
dastoli
datandhome
datasoft
dato
datos
datosdesde
davico
david
davis
daytona
db
dbfaq
dbito
dbvcz
dcia
dciio
dd
dde
ddnbdb
de
deade
deafuera
deagraciadamente
dealextreme
dean
dear
deba
debajo
debe
debemos
deben
deber
debera
deberaia
deberan
debere
deberes
deberi
deberia
deberiamos
deberian
debi
debia
debiamos
debian
debibo
debidamente
debidi
debido
debiendo
debiera
debieramos
debieran
debieron
debio
debiten
debito
debo
debora
deboto
debque
debria
debrian
debtro
deby
decae
decaido
deceado
decear
decearia
decencia
decente
deceo
decepcion
decepcionado
decepcionante
decepcionaron
decepciono
decia
decian
decias
decidi
decidido
decidieran
decido
deciendo
decima
decime
decir
decirle
decirles
decirme
decirte
decis
decision
declaracion
declarada
declarado
declarados
declarando
declarar
declare
deco
decodificadores
decon
decoracion
decorativos
decredito
decuendo
dede
dedica
dedicacion
dedicada
dedicamos
dedico
dedos
deduzco
deesde
deesperar
defecto
defectuoso
defensa
defensoria
defenza
deficiencia
deficient
deficiente
deficitario
definen
definir
definitiva
definitivamente
defounier
defraudacion
defuncion
degue
deguia
deheza
deia
deicbc
deipenao
dej
deja
dejaba
dejaban
dejado
dejados
dejamos
dejan
dejando
dejandome
dejar
dejara
dejaram
dejaran
dejareon
dejaria
dejarian
dejarin
dejarla
dejarle
dejarles
dejarlo
dejarlos
dejarme
dejarmela
dejarmelo
dejarnos
dejaro
dejaron
dejarte
deje
dejear
dejen
dejenla
dejenlo
dejenme
dejensela
dejo
dejrme
dejulio
del
dela
delante
delas
delay
delegacion
delegar
delfina
delgado
delicada
delicadas
delicadeza
delicados
delincuente
delincuentes
delito
delivered
delivery
dell
della
dellepiane
delmismo
delpadre
delseguimiento
delta
dem
demaciado
demanda
demandando
demandar
demandare
demando
demarco
demaria
demas
demasiada
demasiadas
demasiado
demasiados
demian
demmler
demoliciones
demolieron
demora
demoraba
demorada
demorado
demorados
demoragracias
demoran
demorando
demorar
demorara
demoraran
demoraria
demoraron
demoras
demore
demoren
demoro
demorsria
demostraba
demostrando
demostrar
demuestra
demuestre
den
denim
denise
denme
denomina
dentor
dentro
denuevo
denuncia
denunciar
denunciare
denunciarlos
denuncias
denuncie
denuncio
dep
depaqueteria
departamanto
departamento
departamentos
departamneto
depende
dependencia
dependia
dependiendo
dependiente
deplorable
deplorablen
deportiva
deportivo
depositar
depositarla
depositarlo
depositaron
deposite
deposito
depository
depositos
deprestamos
deprorable
deps
depto
deptob
deptos
dereccin
dereccion
derecha
derecho
derechos
deribar
deribaron
deriva
derivada
derivado
derivados
derivan
derivar
derivaran
derivarlo
derivarme
derivaron
derive
deriven
derivo
derme
derqui
des
desactualizada
desafio
desafios
desagradable
desagrado
desapaarecio
desaparecen
desaparecer
desaparecida
desaparecido
desaparecio
desaprensiva
desarmable
desarmada
desarmados
desarrollado
desarrollador
desarrollados
desarrollamos
desarrollando
desarrolle
desarrollo
desaste
desastre
desastrosa
desastroso
desastrozo
desbe
descarado
descarga
descargando
descargar
descargarme
descargo
descargue
descaro
descartados
descifrar
descirme
descolgado
descompuso
desconectado
desconectan
desconfiaba
desconfiguro
desconfio
desconforme
desconformidad
desconoce
desconocemos
desconocen
desconocer
desconocida
desconocido
desconociendo
desconocimiento
desconocio
desconosco
desconozco
desconsiderados
descontando
descontar
descontento
descripcion
descubro
descuento
descuido
desd
desda
desde
desdel
desdice
dese
desea
deseaba
deseada
deseado
desean
deseando
desear
desearia
deseastre
desee
desempenaba
desempenar
desempleo
desencoja
desencontrarnos
desencuentros
desenvolvimiento
deseo
desepcionada
desepcionado
desepsionan
deseria
deses
desesperacion
desesperada
desesperado
desestima
desestime
desestimen
desestimenlo
desgracia
desia
desidia
designado
designe
desilusion
desinteligencia
desiree
desistan
desisti
desitino
desktop
deslindo
desmarca
desmedido
desntro
deso
desocupar
desocupen
desp
despacha
despachaban
despachada
despachadas
despachado
despachadores
despachados
despachamos
despachan
despachando
despachandola
despachandolo
despachante
despachar
despacharemos
despacharla
despacharlo
despacharlos
despacharmelo
despacharon
despache
despachen
despacho
despachos
desparcharlos
desparecio
despchar
despedido
despedir
despegar
despegue
despejar
despenaderos
desperdiciados
desperfectos
desperte
despidieron
despido
despierta
despierto
desplegable
despliega
despliego
desprenderse
desprestigien
despretigian
desprevenido
desprolija
desprolijidad
desprolijo
despues
despures
destacar
destaco
destiempo
destinada
destinado
destinados
destinario
destinarios
destinataria
destinatario
destinatariono
destinatarios
destino
destinos
destintario
destrato
destribuirla
destrozada
destrozadas
destrozados
destrozaron
destruccion
destrudion
destruyo
desuscribirme
desvian
desviar
desvinculacion
desvio
detalla
detallaban
detallada
detalladamente
detallado
detallamos
detallan
detallando
detallar
detallarme
detallarnos
detalle
detalleenviore
detalles
detallo
detecta
detecto
detener
detengan
detenido
detergente
deterioro
determinad
determinada
determinado
determinados
determinando
determinar
detinatario
detino
detras
detuvo
deuda
deudas
deuna
deutsche
deveria
devescovi
devilucion
devincenzi
devito
devlver
devo
devolcion
devolucion
devoluciones
devolver
devolvera
devolveran
devolverian
devolverla
devolverle
devolverlo
devolverlos
devolverme
devolvi
devolvia
devolvian
devolviendo
devolvieron
devolvimos
devolvio
devolviron
devota
devoto
devuekven
devuelta
devueltas
devuelto
devueltos
devuelva
devuelvan
devuelvanla
devuelvanme
devuelvansela
devuelve
devuelven
devuelvo
devuevan
dewlypsn
df
dfdsfsdfdf
dfgfdgf
dgnal
dhl
di
dia
diaa
diad
diadisculpe
diaestoy
diag
diagonal
diagramacion
diala
diamante
diamela
diametro
dian
diana
dianecesito
diaquisiera
diariamente
diario
diarios
dias
diasperi
diasqueria
diasquisiera
diasserian
diassoy
diasu
diatancia
diaz
dibiembre
dibujitos
dibujo
dic
diccionario
dice
dicecq
dicem
dicen
dicencanta
dicendome
dicese
dicha
dichas
dicho
dichona
dichos
diciedo
diciembre
diciendo
diciendome
dicindome
dicr
dicta
dictarse
dicumentacion
didactico
diden
diduch
diego
dienro
diera
dieran
dieron
dietario
dietarios
diez
diferencia
diferenciada
diferenciar
diferenciarse
diferencias
diferenes
diferente
diferentes
difernetes
dificil
dificilmente
dificulta
dificultades
dificultosa
dificultoso
difiero
difusion
diga
digame
digan
diganle
diganme
diganmen
digas
digeron
digieron
digital
digite
digito
digitos
digna
dignan
dignar
dignaron
digne
dignen
digo
diiron
dij
dijanme
dije
dijeon
dijera
dijeran
dijern
dijero
dijerom
dijeron
dijieron
dijo
dilatando
diligenciado
diligenciar
dillon
dimension
dimensiones
dimicilio
dinamica
dinde
dinero
dio
dionisio
dios
dique
dir
dirc
dirccion
dire
direaccion
direcccion
direcci
direcciin
direccio
direccioin
direccion
direcciona
direccionaba
direccionar
direccionen
direcciones
direccionhola
direccionlq
direccon
direcdireccion
direcion
directa
directamente
directo
directora
directorio
directos
directv
direcxionn
direjon
dirfeccion
diria
dirian
dirige
dirigen
dirigi
dirigia
dirigida
dirigidas
dirigido
dirigiendo
dirigiendose
dirigiera
dirigieron
dirigimos
dirigio
dirigir
dirigire
dirigirlo
dirigirme
dirigirse
dirigo
dirija
dirijan
dirije
diriji
dirijido
dirijo
diro
dirreccion
dirrecion
dis
discapacidad
discapacitada
discapacitado
discepolo
disco
discografica
discomforme
disconforme
disconformes
disconformidad
discos
discrepancia
discriminacion
discriminada
disculpa
disculpado
disculpar
disculpas
disculpe
disculpen
dise
disenada
disenar
disenene
diseno
dises
disfrute
disgustada
disgustado
disgusto
disitntas
dislac
disney
disociada
disoosicion
dispacitada
disparate
dispensada
dispocicion
dispone
disponemos
disponen
disponer
disponga
dispongan
dispongo
disponia
disponibilidad
disponible
disponibles
disponiendo
disposicion
dispositivo
dispositivos
disppnibles
dispuesta
dispuesto
disputarse
distal
distancia
distancias
distante
distibucion
distincion
distingo
distingue
distinta
distintas
distinto
distintos
distraidos
distribuccion
distribucion
distribuciuon
distribuicion
distribuidor
distribuidora
distribuidores
distribuidos
distribuimos
distribuir
distribuya
distribuye
distribuyen
distribuyendo
distrillantas
distrito
diversas
diversos
divertida
dividida
divisores
divisoria
divisorias
divorcio
djanikian
djjeron
dl
dlio
dn
dnd
dni
dnia
dnim
dnitendria
do
doa
dobde
dobladas
dobladoras
doblar
doble
doblo
doc
docena
docencia
docente
docentes
docuemnto
document
documentacion
documento
documentos
documentqcion
documrnto
dodigo
dodrias
doinde
dolares
dolor
dolore
dolores
dom
domcilio
domestica
domic
domiciilio
domiciilo
domicilia
domiciliada
domiciliado
domiciliaria
domiciliio
domicilil
domicilio
domicilioespero
domiciliogracias
domiciliooo
domicilios
domicilioy
domicillio
domicilo
domicio
domiciolio
domiclio
domiclios
domigo
domiilio
domilcio
domilicio
domilio
domincilio
domingo
domingos
dominico
dominio
domocilio
domucilio
don
donalisio
donarse
donato
dond
donde
dondequiera
donjde
donolo
dontorcuato
doonde
dora
doradas
dorado
dorrego
dorso
dos
dosbultos
dot
douclot
doutw
dove
doy
dpmicilio
dpto
dpupbl
dpusefezxamyzul
dr
dra
dream
dres
dreyer
drifter
driofar
drl
drofar
drogo
drogueria
drone
drugstore
ds
dsd
dsdsdf
dsfsdf
dsp
dsps
dss
dstoy
dt
dti
dto
du
duarte
ducen
ducha
ducursal
duda
dudas
dude
duden
dudo
dudoso
duena
duenas
dueno
duenos
duggan
duhalde
dumontt
duplex
duplicado
duplicar
dura
duracion
durante
durar
durectamente
duro
dvm
dvpq
dvuelvan
dw
dye
dz
dziewa
dzmrlrpitiuyqwpholteiizistf
dzukdg
e
ea
eaa
eana
eaperando
eaperarlos
eata
eate
ebcalada
ebeling
ec
ecargado
ecarta
ecatepec
ecbc
eces
echan
echar
echeverri
echeverria
echo
ecnuentra
eco
ecoclima
ecoderm
ecomico
ecomienda
ecomiendas
ecominda
ecommerce
ecomunicarme
econienda
economica
economicamente
economico
ecooeste
ecoplan
ecosol
ecotrans
ecribo
ect
ecuador
ed
edaci
edad
edgar
edgardo
ediba
edicion
edificio
edificios
edifico
edio
edith
editorial
edo
edpero
edtoy
eduardo
educacion
educada
educado
educados
educativo
educativos
ee
eeee
eel
eencuentra
eenvios
eesta
eeuu
efectiva
efectivamente
efectivizacion
efectivizamos
efectivizan
efectivizar
efectivo
efectivos
efecto
efectos
efectuada
efectuado
efectuan
efectuando
efectuar
efectuara
efectuarian
efectuaron
efectuarse
efectue
efectuen
efectuo
eficacia
eficaz
eficiencia
eficiente
eficientes
efm
efono
ego
egresados
eguiazu
eguimiento
eh
eichenberger
einstein
ej
eje
ejecutiva
ejecutivo
ejemplar
ejemplares
ejemplo
ejercicios
ejercito
ejido
el
ela
elaboracion
elaborar
elcano
eldia
eldorado
eleccion
elect
electrica
electricidad
electrico
electrodomestico
electroingenieria
electronica
electronicas
electronico
electronicos
electronics
elegante
elegi
elegible
elegida
elegido
elegimos
elegir
eleji
elejir
elemento
elementos
elemplo
elena
elepant
eleuterio
elevado
elevando
elevar
elevare
elflein
eliana
elias
elida
eligio
elijio
elijo
elisa
elisabet
elizabet
elizabeth
elizalde
elk
ell
ella
ellas
ellis
ello
ellocalcomercialdonde
ellos
elm
elmdia
elmismo
elmpaqueta
eln
elndni
elnro
elntelefono
elnumero
eloisa
elos
elpaquete
elsa
elsagalarza
elseguimiento
elsegundo
eltiempo
elvira
em
ema
email
emails
emanuel
emas
emb
embajada
embalada
embaladas
embalado
embalados
embalaje
embalan
embalar
embalarla
embalarlo
embalarlos
embalaron
embale
embalo
embalse
embarazada
embarazo
embargo
embiado
embole
embolsadas
embustera
embutir
emcomienda
eme
emergencia
emergencias
emeterio
emilia
emiliano
emilio
emilse
emision
emisor
emisora
emite
emiti
emitida
emitidas
emitido
emitio
emitir
emitiran
emma
emmanuel
emoleado
emoresa
empacada
empacados
empapado
empapelados
empaque
empaquetadora
empaquetamiento
empaquetan
empaquetando
empaquetar
empaquetarlos
empatica
empece
empeora
empeorado
empezamos
empezando
empezar
empezo
empiecen
empieza
empleada
empleadas
empleado
empleador
empleados
emplearla
empleda
empleo
empprea
empreaa
empreado
emprendimiento
emprendo
empresa
empresaria
empresarial
empresas
empreza
emresa
emrpesa
emvia
emvian
emviar
emviaron
emvie
emvio
en
enacom
enbala
enbargo
enbien
enbio
enbla
enbuenos
encalada
encamino
encantado
encantaria
encarecidamente
encarga
encargaba
encargada
encargado
encargados
encargan
encargando
encargar
encargarian
encargarme
encarge
encargo
encargue
encausar
encendedor
encentra
encerio
encerrada
encerrado
encia
enciar
enciaron
encie
encientyra
encima
encinas
encio
encla
encntraba
encomie
encomiedas
encomien
encomienda
encomiendadesde
encomiendaque
encomiendas
encomiendes
encomiendo
encominda
encomindas
encomineda
enconomicos
encontado
encontar
encontaron
encontra
encontraba
encontrabamos
encontraban
encontrada
encontrado
encontramos
encontrando
encontrandome
encontrandose
encontrans
encontrar
encontrara
encontraran
encontrarce
encontrare
encontraria
encontrarla
encontrarlo
encontrarlos
encontrarme
encontrarn
encontraron
encontrarse
encontrarte
encontrarton
encontras
encontrasron
encontre
encontro
enconttraron
encotraba
encuantra
encuentea
encuento
encuentra
encuentran
encuentras
encuentre
encuentren
encuentro
encuetra
encuntra
ende
endeudo
endless
endonde
endovascular
ene
enecima
energia
enero
enetrega
enetregado
enferma
enfermo
enfrente
enfurecida
enganado
england
engorrosa
engreso
eniaran
eniaron
enio
enlace
enlinea
enlugar
enn
ennla
ennvio
eno
enojada
enojado
enojados
enojo
enomienda
enomiendas
enorme
enormemente
enpresa
enre
enrealidad
enrega
enregarme
enrique
enrollar
enseguida
ensena
ensenada
ensima
ensintada
ensobradas
ensobrado
ensobrar
ensta
ensuc
ensuenos
entas
ente
entega
entegara
entendemos
entender
entenderan
entendi
entendible
entendido
entendiendo
entendimos
entendio
entera
enteramos
enterar
enterare
enteraria
enterarme
enteras
entere
enteregado
entero
enteros
entidad
entidades
entiemdo
entienda
entiendan
entiende
entienden
entiendo
entonces
entons
entorno
entorpece
entra
entraba
entrada
entradaa
entradas
entradasfan
entradasmuchas
entradga
entrado
entraga
entragado
entragan
entragar
entragaron
entrando
entrar
entrara
entraria
entraron
entras
entre
entreado
entredas
entredicho
entrega
entregaba
entregaban
entregada
entregadas
entregado
entregadocomo
entregadores
entregados
entregagado
entregala
entregalo
entregamos
entregan
entregando
entregandolo
entregann
entregar
entregara
entregaran
entregare
entregaremos
entregaria
entregarian
entregarla
entregarlas
entregarle
entregarlo
entregarlos
entregarme
entregarmela
entregarmelo
entregaron
entregarony
entregarse
entregarselas
entregarselo
entregarte
entregas
entregasemos
entregen
entregenla
entrego
entregue
entreguedo
entreguem
entreguen
entreguenlo
entreguenme
entregues
entren
entrenador
entrenguen
entrepapelo
entrerios
entres
entrevista
entrga
entrgado
entrgando
entrgar
entrgarme
entro
entrrgado
enttregarlo
entube
enumeradas
envadio
envar
envasa
envasados
envase
envases
enven
envi
envia
enviaba
enviaban
enviada
enviadaaguardo
enviadacdesde
enviadas
enviado
enviadodesde
enviados
enviafon
enviame
enviamos
envian
enviando
enviandolo
enviandome
enviandose
enviaq
enviar
enviara
enviaramos
enviaran
enviare
enviarel
enviaria
enviarian
enviarion
enviarla
enviarlao
enviarlas
enviarle
enviarles
enviarlo
enviarlos
enviarme
enviarmela
enviarmelo
enviarnos
enviarnun
enviaron
enviarondesde
enviarse
enviarselo
enviasen
enviaton
enviava
enviavan
envida
envidado
envie
envieme
enviemos
envien
envienla
envienlo
envienme
envienmelas
envieron
enviio
envil
envio
envioas
enviode
envioi
envion
envioni
envioo
enviopack
envioporque
envios
enviouna
envir
envisr
enviue
envoi
envoltorio
envoltura
envolver
envolvi
envomienda
envontraron
envuelta
envuelto
envuo
enzima
enzimatico
enzo
eo
eon
eoo
eotapgp
eov
ep
epack
epacksoporte@oca
epak
epaktrackingweb
epero
episodio
epoca
epresa
epuyen
equibocados
equiboco
equipamiento
equipo
equipos
equivoca
equivocacion
equivocada
equivocadamente
equivocado
equivocados
equivocamos
equivocando
equivocaron
equivoco
equivoque
er
era
eramos
eran
erbes
erede
erewrewrfwwff
ergueza
erica
erika
ernesto
ero
erra
errada
erran
errecart
erronea
erroneaaa
erroneamente
erroneo
erroneos
erronia
error
errores
ers
es
esa
esadasdasdasd
esas
escala
escalada
escalante
escalera
escaleras
escandalo
escaneada
escaneado
escaneando
escaneo
escaneos
escape
escasa
escel
esch
escha
escirbir
esclarecer
esclavos
escobar
escolar
escondido
escontraron
escrachar
escracharlos
escri
escriba
escriban
escribania
escribano
escribe
escriben
escribi
escribia
escribieron
escribimos
escribio
escribir
escribirle
escribirles
escribirlo
escribo
escrita
escrito
escritorio
escritos
escritura
escucha
escuchado
escuchamos
escuchando
escuchar
escuche
escuchenme
escucho
escuela
escuelaintento
escuelas
esducada
ese
eseele
esel
eseliana
esencial
esero
eshop
esito
eskabe
esmeralda
eso
esoerando
esoero
esos
esoy
esp
espacio
espacios
espana
espantoso
esparcido
especial
especiales
especializada
especialmente
especie
especifica
especificaba
especificaciones
especificada
especificado
especificamente
especifican
especificando
especificar
especificarme
especificaron
especificcion
especifico
especificos
especifique
espectaculo
espectativa
espejo
espeleta
espera
esperaba
esperaban
esperabndo
esperada
esperado
esperamos
esperan
esperanba
esperando
esperandoe
esperandola
esperandolo
esperandolos
esperanza
esperanzas
esperaod
esperar
esperara
esperare
esperariamos
esperarla
esperarlo
esperarlos
esperarlosuna
esperarme
esperaron
esperas
espere
esperedando
esperen
esperguin
esperndo
espero
espersndo
espesificacion
espesor
espetando
espinola
espinosa
espinoza
esplicacion
espoille
espor
esporadicamente
esposa
esposo
esprando
espresspak
esprrando
espuma
esq
esque
esquel
esqueleto
esquina
esquinas
esquivocaron
esra
esribimos
ess
esste
est
esta
estaba
estabamos
estaban
establece
establecen
establecer
establecida
establecido
establecidos
establecimiento
estacion
estacionadas
estacionado
estacionamiento
estacionar
estaciones
estada
estadio
estado
estados
estafa
estafada
estafado
estafadores
estafando
estafar
estafaron
estafeta
estafo
estai
estamos
estampilla
estampillado
estampillas
estan
estancada
estancado
estancias
estancionado
estanco
estandar
estando
estani
estanislao
estante
estanteria
estantes
estao
estar
estara
estaran
estare
estaremos
estaria
estariamos
estarian
estas
estaste
estatus
estava
estaxen
estba
estban
estdo
este
estebab
esteban
estedes
estefania
estela
estella
esten
estepa
ester
estero
estevez
estevia
esther
estiamados
estiamdos
estilo
estima
estimaba
estimada
estimadas
estimado
estimados
estimadxs
estiman
estimar
estimate
estimativa
estimativo
estimdos
estimo
estimulado
estinatario
estio
estipulaba
estipulada
estipulado
estipulados
estipularon
estive
esto
estoespero
estoi
estomba
estopense
estore
estos
estou
estoy
estra
estrabiado
estrada
estraviado
estravio
estrecho
estrega
estrella
estrenar
estresante
estria
estribos
estructura
ests
estsba
estube
estuche
estudiando
estudiante
estudiar
estudie
estudio
estudios
estufa
estufas
estuimados
estupida
estupido
estuve
estuviera
estuvieran
estuvieron
estuviese
estuvimos
estuvo
esunyae
esurgente
esw
esye
esz
eta
etandby
etapa
etc
etchepareborda
etcj
eterna
etica
eticos
etienden
etiqueta
etiquetado
etiquetan
etiquetar
etiquetaron
etiquetas
etiquete
etire
etrega
etregado
etregar
etrego
etruria
etsa
etsamos
etsba
ettnfufau
etx
eu
eue
eugenia
eugenio
eun
euna
euromayor
europa
eva
evaluando
evaluar
evangelina
evaristo
eveling
evelyn
evento
eventos
eventualmente
ever
eviaje
evidente
evidentemente
evio
evion
evita
evitando
evitandonos
evitar
evitarlo
evite
eviten
evkmyqsyhz
evr
ewapero
ewlfeh
ewrwerwerewrewewrewr
ex
exacta
exactamente
exactitud
exacto
exageracion
exagerado
exagerando
exaltacion
examen
examenes
excede
excedido
excedieron
excedio
excedo
excel
excelente
excelentemente
excepcion
excepcional
excepto
exceptuan
excesiva
excesivo
exceso
excitosamente
exclusiva
exclusivamente
exclusivo
excusa
excusan
excusas
exiate
exigen
exigencia
exigi
exigia
exigidos
exigiendo
exigo
exijo
exista
existe
existen
existente
existesi
existia
existian
existiera
existio
existir
existira
existo
exito
exliquen
exo
expandir
expectativa
expectativas
expediente
expedir
expeditivo
expensas
experiencia
experiencias
experimente
experincia
experta
expess
expide
expired
explica
explicaban
explicacion
explicaciones
explicado
explican
explicando
explicar
explicarle
explicarme
explicaron
explicitamente
explicito
explico
explique
expliquen
explorer
explotados
expocision
exponer
exponiendo
exportacion
exportacion@molinochacabuco
exportadores
exposicion
expres
expresa
expresada
expresado
expresamente
expresar
expresarle
expresarles
exprese
expreso
exprespak
express
expresspack
expresspak
expte
expuesta
expuesto
extana
extender
extendiendo
extendieron
extendio
extensa
extensivo
extensos
exterior
exteriores
external
externo
extiendan
extitud
extra
extracto
extraer
extraido
extrajero
extrana
extranaeltiempo
extranas
extrangero
extranjero
extrano
extras
extraviadas
extraviado
extraviados
extravian
extraviaron
extravie
extravio
extrema
extremadamente
extremo
eyer
ezeiza
ezequiel
ezpeleta
ezte
f
fa
fabian
fabor
fabrica
fabricio
fabrico
face
facebook
facha
facil
facilidad
facilitan
facilitar
facilitarian
facilitarme
facilitaron
facilite
faciliten
facilito
facilmente
fact
factible
factura
facturacion
facturado
facturas
facultad
facundo
fader
fae
faedda
fagano
fagnano
faircloth
fajardo
falabella
falacia
falacias
falaz
falco
falcon
falda
falencias
falla
fallado
fallando
fallar
fallaron
fallas
falle
fallecido
fallida
fallidas
fallido
fallidos
fallla
fallo
falsa
falso
falsos
falta
faltaba
faltaban
faltade
faltan
faltando
faltandome
faltante
faltantes
faltar
faltara
faltare
faltaria
falte
falten
falto
falucho
falzo
famacity
famailla
familia
familiar
familiares
familyguard
fan
fanny
fanpage
fantasma
fanti
faovr
fap
far
farelluk
farias
farinas
farmaceutico
farmacia
farmacity
faro
farrace
farran
fascina
fast
fastidio
fatima
fato
faussats
faustino
fav
favaloro
favaro
favir
favoooooorrrr
favoorr
favopr
favor
favorable
favorablemente
favorables
favores
favorrr
favorrrr
favro
favultad
fax
fazem
fazzini
fb
fc
fcias
fcio
fcm
fco
fcubhdmmdbz
fdsfdsff
fe
febrero
fecha
fechada
fechano
fechas
feche
fed
fedeeral
fedeico
federacion
federal
federico
federigi
fedex
fedrigo
fehaciente
fehacientemente
feliciano
felicitaciones
felicitenme
felicito
felipe
feliu
felix
feliz
femenina
femenino
fenix
feo
fera
ferdinando
feriado
feriadoo
feriados
fernadez
fernanda
fernandez
fernando
fernendo
ferrari
ferrarotti
ferre
ferrer
ferrere
ferreteria
ferreyra
ferroviario
ferrum
festejare
festejo
fez
ffdfdggdfg
ffecha
ffg
fgeller@gellerabogados
fhuo
fi
fia
fiambala
fias
fiat
fibra
fibras
fibrofacil
ficha
fidedigna
fidedigno
fidelizarlo
fidelli
fiden
fiel
fieron
fierro
fifa
fig
figgura
figuera
figueroa
figura
figuraba
figurabs
figuramos
figuran
figurando
figurar
figuras
figure
figuren
figuritas
figuro
fija
fijado
fijando
fijandome
fijar
fijarme
fijaron
fijarse
fijas
fije
fijen
fijense
fijo
fila
file
filial
filiar
filippi
filippone
film
filmacion
filmadora
filmico
fin
fina
final
finales
finalizada
finalizado
finalizar
finalmente
financiera
finca
fincas
find
finde
fines
finfe
finishing
finjan
finochietto
fiorella
fiorino
fiorito
firma
firmaba
firmacomo
firmada
firmado
firmapaz
firmar
firmara
firmas
firmat
firme
firmen
firmes
firmo
fiscal
fiscales
fiscalizacion
fisica
fisicamente
fisicas
fisico
fisicos
fito
fittipaldi
fjsgqnytypjdxl
flash
flaubert
flavia
fleming
flete
fletero
fleteros
fletes
fleury
fleurypat
fleurypat@yahoo
flexibles
flia
fliar
flore
florecio
florencia
florencio
floreria
flores
florespara
floresta
florida
flota
flotantes
flrorida
fls
fluctua
fluida
fluvial
flv
fm
fma
fn
foja
fojeu
folletos
fomav
fonavi
fondo
fono
fonseca
fontan
fontana
footy
ford
forenses
forest
forestal
forma
formaba
formacion
formacon
formade
formal
formales
formalice
formalidades
formalizar
formar
formas
formato
formenme
formosa
formulario
formularios
formularon
foro
forraje
forros
fort
forte
fortuna
fortunas
forzado
forzosamente
fot
foto
fotocopia
fotocopiadora
fotocopias
fotocopiia
fotografica
fotografico
fotos
fotosprint
fotter
foussats
fox
fpzmgpw
fq
frabajo
fraccion
fraccionado
fracias
fracil
fractura
fragil
fragiles
fragueiro
francamente
frances
francesca
francia
francisca
francisco
franck
franco
franisco
franja
franjas
franquicias
fransisco
fras
frasada
frasco
frascos
frase
frasquitos
fraudulento
fravega
fray
frealizarlo
frecuencia
frei
frena
frenada
frenado
frenar
frende
freno
frenos
frente
frescos
fresnos
fretes
frias
frio
fritas
from
frontera
fructuosa
frusta
frustrada
frustrado
frustrante
frustro
fruta
frutos
fsa
ft
fte
fu
fual
fue
fuego
fuejo
fuen
fuente
fuentealba
fuera
fuerok
fueron
fuerte
fuerza
fuese
fuesen
fugurando
fui
fuimos
fuisicamente
full
fullticket
fumuso
funcion
funciona
funcionaba
funcional
funcionalidad
funcionamiento
funcionamos
funcionan
funcionando
funcionar
funcione
funciones
funciono
funda
fundacion
fundadores
fundamentacion
fundamental
fundamentalmente
fundan
fundicion
fundida
fundido
fundir
fune
funes
furgon
furiosa
futbol
futon
futura
futuro
futuros
fvor
fvpschskccfpusmtkgbokwbpyri
fvr
fw
fwd
g
gabarino
gabarret
gabinete
gabriel
gabriela
gabrielli
gaceta
gacias
gafas
gagliardi
galarza
galassi
galaxy
galdoz
galeano
galeno
galeria
galicia
galicias
galina
gallardo
gallego
gallegos
galletto
gallo
galvan
galvez
gamer
gana
ganadera
ganaderia
ganancias
ganas
gandhi
gandolfo
gane
gangi
gano
gaona
garadesco
garage
garantia
garantiza
garantizan
garat
garay
garbarino
garcas
garcha
garcia
garcias
gardel
gardey
garibaldi
gariboldi
garin
garita
garrahan
garre
garrett
garrido
gas
gasparini
gasset
gastado
gastamos
gastando
gastar
gaste
gasto
gaston
gastos
gastronomia
gastronomicas
gatica
gattemeyer
gattoni
gaucho
gauchos
gauna
gavila
gay
gb
gba
gbg
gcaba
gcba
gchu
gcias
gciass
gcs
gdlcv
gdor
ge
geek
geese
gel
gell
gema
gemaelisabet@gmail
gendarmeria
gendarmes
genera
generada
generado
generador
generados
general
generales
generalmente
generan
generando
generandonos
generar
generarlo
generarme
generaron
generas
genere
generen
genero
gengo
genial
gente
gentea
gentecomo
gentil
gentilmente
gentr
geo
geolocalizacion
geopaqueteria
geored
georgina
geraldy
gerardo
gerencia
gerente
gerez
geriatrica
gerli
german
geronimo
gerosa
gervasoni
ges
gesel
gesell
gesellcuando
geselloca
gessel
gessell
gestion
gestiona
gestionada
gestionado
gestionando
gestionar
gestionara
gestionarlo
gestione
gestionen
gestiones
gestiono
gestor
gestora
gestoria
getente
getting
gg
gh
gherente
ghia
gia
giagetto
gian
giana
giannuzzi
giardino
gigante
gigena
giglio
gil
giles
gimanasio
gimena
gimenez
gimnacia
gimnasio
gina
gino
giordano
giorgina
girada
girando
gisel
gisela
giuliana
giuliano
gjwidgr
gl
gladis
glaston
glc
glew
gllen
global
glucemia
glucosa
gm
gmail
gmmfgt
gmnikodmkg
gms
gmtzomwfncmmvdwevgalcwytlq
gnc
gnecco
gni
go
gob
gobernador
gobetti
gobierno
god
godoy
goernador
goiri
gold
golf
golondrinas
golpe
golpea
golpeado
golpean
golpeando
golpear
golpearan
golpearon
golpee
golpeo
goma
gomas
gome
gomeria
gomez
gomita
gon
gonnet
gonzales
gonzalez
gonzalo
goo
google
gopro
gordillo
gorina
gorizia
gorras
gorriti
gorrrito
gostaria
gota
gottifredi
gov
gowland
goy
goya
goycoa
goyena
gp
gpat
gps
gr
graba
grabaciones
grabado
grabador
grabando
grac
gracia
graciaa
graciaas
graciad
gracias
graciase
graciasel
graciass
graciasss
graciasy
graciaz
graciela
gracis
gracisas
graciss
gracjas
gracuas
graficas
grafico
graham
graias
graicas
gral
gramos
gran
granadero
granaderos
grand
grande
grandes
graneros
grano
grappa
gras
grasias
grata
gratificacion
gratis
grato
gratuitamente
gratuito
graty
grave
gravedad
gravias
gravismo
grcias
greco
gredito
gregoratto
gregores
gregoria
gregorio
greso
grido
griffith
griglio
grilla
grimaldi
grimoldi
gripe
gris
griselda
grisino
grision
gritandome
grms
groba
groppo
grosera
grosor
grossi
grosso
group
grover
grs
grupo
grxs
gs
gss
gt
gua
guachos
guadalupe
gualeguay
gualeguaychu
guanilo
guantes
guapa
guarda
guardaban
guardada
guardado
guardan
guardar
guardarlo
guardarme
guarden
guardia
guardias
guasada
guastavino
guaymallen
guedas
guede
guelet
guelfo
guemes
guerendiain
guernica
guerra
gui
guia
guiarlos
guiarme
guias
guido
guilherme
guillen
guillermo
guillon
guiraldes
guitarra
gurruchaga
gusman
gusta
gustaria
gustariua
gustarua
gustatia
gustavo
gustavotorrigiani@molinochacabuco
gusto
gustria
gutierrez
gutirrez
guzman
guzzi
gvjgteafz
gw
gxias
gya
gym
h
ha
haash
haba
habana
habc
haber
haberes
haberla
haberlas
haberle
haberlo
haberlos
haberme
habermelo
habernos
haberse
haberte
habia
habiamos
habian
habias
habido
habiendo
habiendome
habiendose
habiertos
habil
habiles
habilita
habilitacion
habilitaciones
habilitada
habilitadas
habilitado
habilitados
habilitar
habiliten
habilito
habitantes
habitual
habitualmente
habiua
habla
hablaba
hablado
hablamos
hablan
hablando
hablar
hablare
hablarlo
hablaron
hablat
hable
hablen
hablo
hablrar
habra
habran
habren
habria
habrian
habrir
habverte
hacd
hacdner
hace
haceder
hacem
hacemos
hacemosmi
hacen
hacer
hacerca
hacercar
hacercarme
haceres
hacergracias
hacerl
hacerla
hacerle
hacerles
hacerlo
hacerlogracias
hacerlos
hacerme
hacermelo
hacernos
hacerque
hacersaludos
hacerse
hacersigo
haces
haci
hacia
hacian
haciend
haciendo
haciendole
haciendolo
haciendome
haciendose
haciento
hacirndo
hacr
hact
hadta
hae
haedo
haer
haga
hagamos
hagan
haganle
haganlo
haganse
hagansen
hago
hagonpara
hagooooo
haidar
haiti
halgo
halies
halla
hallan
hallar
hallaron
hallarse
hallo
han
hanpasado
hantes
haqgo
haquin
hara
haran
harden
hare
haremos
haria
hariamos
harian
harina
harta
hartando
harte
harto
has
hascia
hasco
haser
hast
hasta
have
haver
hay
haya
hayamos
hayan
hayar
hayas
hblar
hbtdana
hc
hcbc
hcbs
hce
hcer
hd
hdmp
hdp
hdsajkhdsahdsajkl
hdvqcpvavg
he
hebreo
hecha
hechar
hechas
hecho
hechos
hector
heit
heladera
helguera
heller
helsinki
helvecia
hemodinamia
hemos
hemoso
hems
henandez
henderson
heras
heredia
herman
hermana
hermano
hermanos
hermosa
hermoso
hernan
hernandez
hernando
herramienta
herramientas
herrera
herrrrrmosa
hes
hevres
hew
hf
hg
hgfkogvhy
hgghdfghfhgdfhfdhdfhhgghdfghfhgdfhfdhdfhhgghdfghfhgdfhfdhdfh
hgzzdc
hhola
hhw
hi
hice
hicevuna
hiciceron
hiciera
hicieron
hicimos
hidalgo
hidratacion
hielo
hiero
hierro
higiene
higienico
higuain
hiho
hija
hijas
hiji
hijo
hijos
hilario
hildt
himba
hinck
hino
hinojo
hipoacusica
hipolito
hipotecario
hira
hirqueta
hirschhorn
hise
hiso
historia
historial
historialestados
historicamente
historico
hiy
hize
hizo
hla
hlwtzdzy
hna
hoa
hoal
hobbys
hogar
hoja
hojas
hola
holaa
holaaa
holae
holales
holanda
holaqueria
holaquiero
holas
holasim
holbuenos
holi
holowaty
hols
holza
hombre
home
homebanking
homogeneo
hondo
hong
honkytonk
honorable
honorarios
hoola
hor
hora
horacio
horan
horaria
horarias
horario
horariohace
horariom
horarios
horas
horca
horiario
horitas
horizontal
horizonte
hornero
horno
hornos
horqueta
horrible
horriblemente
horribles
horrios
horrores
hospedado
hospital
hot
hotel
hotmail
hotsale
howard
hoy
hoyo
hoyvxq
hoz
hpy
hq
hqce
hr
hras
hrenuk
hroas
hrs
hs
hsbc
hsbs
hscb
htmltracking
htnumero
htp
http
https
huanguelen
huarpes
huawei
hubicado
hubico
hubiera
hubieramos
hubieran
hubieron
hubiese
hubiesen
hubo
hudson
huergo
huerjpkoksp
huevito
huevos
hugo
huidobro
huinca
huincul
hulluman
humahuaca
humano
humanos
humberto
humo
humor
hurgando
hurlingam
hurlingham
hurlinghan
hurry
hurto
hussain
hvt
hwtn
hwyj
hya
i
ia
iaf
iair
iakuia
iara
ias
iba
ibaceta
iban
ibanez
ibarlucea
ibarra
ibarreta
ibas
iberlucea
ibero
ibicuy
ibira
ibm
ibone
ica
icbc
icbi
ice
icomienda
ics
id
ida
idas
idea
ideal
idear
idearon
idenficada
identica
identicas
identidad
identidada
identificacion
identificada
identificado
identificador
identificar
identificarlo
identificatorio
identifique
idiota
ido
idtramite
ient
ierm
ifnormacion
iformacion
ifyvoacha
igaltex
iglesia
iglesias
ignacio
ignorado
ignorantes
ignoro
igual
igualdad
iguales
igualmente
iguazu
ii
iibb
iii
iioca
ileana
ilegible
ilogico
iluminacion
iluminada
iluminicent@gmail
ilusion
ilustrativo
imagen
imagena
imagenes
imaginan
imagine
imagino
imail
iman
imbeciles
imcompleto
imei
imformacion
imformarme
immhhgdeoimuqdxypd
impaciencia
impaciente
impacto
impagables
impar
imparcial
impecable
impedida
impedimentos
impericia
imperio
imperiosa
imperiosamente
impida
impide
impiden
impidieron
impirtante
implatacion
implementacion
implementada
implementando
implementar
implementarla
implementarlo
implemetar
implica
implican
implique
implorar
impone
importa
importacion
importadora
importadores
importados
importan
importancia
importania
importante
importantes
importar
importe
importede
importes
imposibilidad
imposibilita
imposibilitada
imposibilitadas
imposible
imposibles
imposicion
impotencia
imppsible
imprenta
impresa
impresas
imprescindible
impresentable
impresentables
impresincible
impresion
impreso
impresora
impresoras
impresos
impresso
imprevisto
imprima
imprime
imprimieron
imprimii
imprimio
imprimir
imprtancia
impsible
impuesto
impuestos
impunidad
impuntualidad
impusieron
in
ina
inaccesible
inaceptable
inactiva
inadecuada
inalambrica
inalambrico
inalambricos
inaudito
inaugurar
inc
incan
incansablemente
incapaces
incapacidad
incertidumbre
inclan
inclemencias
incluia
incluida
incluido
incluir
incluirlos
incluirnos
inclusive
incluso
incluye
incluyen
incluyendo
incluyo
incognita
incoherente
incomoda
incomodidad
incomodo
incomodos
incompetencia
incompetente
incompetentes
incompleta
incompleto
incompletos
incomprensible
incomunicado
incomveniente
inconcordancia
inconforme
inconformidad
inconrrecto
inconsultables
inconveniente
inconvenientes
inconveniete
incorporacion
incorporan
incorporar
incorrecta
incorrectamente
incorrectas
incorrecto
incorrectos
incoveniente
incovenientes
increible
incumimiento
incumplan
incumple
incumpliendo
incumplimiento
incumplimientos
incurrir
indagar
indarte
indebidamente
indecifrable
indefinidamente
indefinido
indentidad
indentificacion
indentifico
independecia
independencia
indescifrable
indias
indica
indicaba
indicaban
indicacion
indicaciones
indicada
indicadme
indicado
indicados
indican
indicando
indicandome
indicaque
indicar
indicaran
indicarle
indicarles
indicarme
indicarnos
indicaron
indiciaron
indico
indigna
indignacion
indignada
indignadisima
indignado
indignante
indio
indios
indique
indiquen
indiquenme
indiquenos
indispensable
individuo
individuos
indole
inducarme
indumentaria
induplack
industria
industrial
industriales
industrializados
inecistente
ineficacia
ineficiencia
ineficiente
ineficientes
inentendible
ineptitud
inepto
ineptos
ines
inesacto
inesperado
inesperados
inexacto
inexistene
inexistente
inexplicable
inf
infantes
infantil
infectologia
inferior
infernal
infierno
infinidad
infinitamente
infirmando
infirmar
inflable
inflado
inflamables
influencia
influye
info
infomacion
infomaron
infomracion
infomro
inforamcion
inforar
informa
informaba
informaban
informacio
informacion
informaciones
informada
informado
informados
informales
informamos
informamrme
informan
informando
informandome
informanos
informanque
informar
informara
informaran
informaremos
informarian
informarle
informarles
informarlo
informarme
informaron
informatica
informativo
informcion
informe
informen
informenme
informenos
informes
informmacion
informo
infornaran
infornen
infoscg@hotmail
infraccion
infructuosa
ing
ingenieria
ingeniero
ingenieros
ingenio
ingesar
ingirmacion
ingles
ingormar
ingreasar
ingresa
ingresada
ingresado
ingresamos
ingresan
ingresando
ingresandolo
ingresar
ingresarlo
ingresaron
ingrese
ingreso
ingresos
ingrid
inhabilitada
inhabilitado
inhouse
inib
inicar
inicia
iniciado
inicial
inicialmente
iniciando
iniciar
iniciara
iniciare
inicie
inicio
ininterrumpidamente
inistnqzkcfdwnoffxzooszbdp
injustificada
inmediata
inmediatamente
inmediatez
inmediato
inmensamente
inminente
inmovilizado
inmueble
innecesaria
innecesario
innovacion
innumerable
innumerables
ino
inodoro
inodoros
inoperancia
inoperante
inoperantes
inoxidables
inpedimentos
inposible
inprimir
inquietud
inrresponsable
insalvable
insatisfaccion
insatisfactoria
insatisfecha
insatisfecho
inscriba
inscribi
inscribir
inscribirme
inscripcion
inscripta
inscripto
inscriptos
inscrito
insertar
inservible
inservibles
inside
insignificante
insinuo
insiste
insistencia
insistencias
insistente
insistentemente
insistiendo
insistieron
insistio
insistir
insisto
insolente
insolentes
insolito
inst
instagram
instalable
instalacion
instalaciones
instalar
instalarlo
instancia
instancias
instante
instantes
institucion
instituto
instruccion
instrucciones
instructiva
instruirme
instrumente
instrumento
insuiz
insultando
insumos
insuperable
int
integra
integracion
integrada
integral
integrando
integrante
integrar
integrarla
intencion
intenciones
intendente
inteno
intenta
intentado
intentadome
intentamos
intentan
intentando
intentanso
intentanto
intentar
intentara
intentaran
intentare
intentarian
intentaron
intente
intenten
intento
intenton
intentos
intentr
inteprovincial
interactivo
intercambio
interceder
intercomunicador
interes
interesa
interesada
interesado
interesados
interesan
interesaria
intereso
interezado
interior
interiorizar
interios
intermediarios
intermedio
interna
internaacional
internac
internacion
internacional
internacionales
internada
internado
internal
internar
internaron
international
internet
interno
internos
interponer
interpretar
interpreto
interrumpida
interrumpieron
intervencion
intervencionista
intervenga
intetar
inteto
intimar
intimo
intransitable
intransitables
intratable
intres
introducir
introducirlo
introduje
introduzco
intuitiva
inumeras
inunda
inundacion
inundada
inundado
inutel
inutil
inutiles
invalida
invalido
invariablemente
inventando
inventaron
inversion
inverti
invertido
invertidos
invertir
investigacion
investigar
investigue
investiguen
inviaron
invirtio
invitacion
invitaciones
involucrada
involuntaria
ioa
ipoca
ipona
iproda
iqdopgus
ir
ira
iran
ire
iria
irian
iriarte
iribarren
irigaray
irigoyen
iris
irizar
irla
irlo
irma
irme
ironia
ironica
ironicamente
irregular
irregularidades
irreponsabiildad
irrespetuosa
irrespetuoso
irrespetuosos
irresponsabilidad
irresponsabilidades
irresponsable
irresponsables
irresponzabilidad
irresponzable
irrisorio
irsa
is
isabek
isabel
isabella
ise
isic
isidro
isidrooca
isieron
isimos
isla
islas
islenos
isletas
ismael
ismxgdwnpotdkvahaib
isnardi
iso
isola
isquierda
israel
istedes
it
italia
italiana
italiano
itati
itau
itaua
itauargentina
itaud
itauen
itauesta
item
items
iten
itinerario
iturrieta
ituzaingo
ituzaningo
ituziango
iu
iunfructuosos
iv
iva
ivan
ivana
iveco
ivo
izan
izav
izquierda
j
ja
jacarandaes
jacen
jachal
jackeline
jackie
jaguel
jaime
jajaja
jajajajajaj
jamas
jan
janeiro
janet
japon
japones
jara
jarabe
jaramillo
jardin
jarla
jaula
javier
javierd
jazmin
jazmines
jb
jbl
jc
jcelectronica
jcpaz
jd
jdk
jdru
jean
jeans
jefe
jeiji
jeje
jerarquico
jerarquicos
jeremias
jeronimo
jesica
jessica
jesuitas
jesus
jeta
jetpaq
jeunesse
jhonson
jimena
jimenez
jm
jmc
jmkoujvdkm
jna
jnunca
jo
joakobendahan@gmail
joaquin
joda
joden
joder
joel
jofre
johan
johnson
jonas
jonatan
jonathan
jorge
jornada
jornadas
jose
joshua
josue
joven
joyas
jrbarni@gmail
jse
jstcc
jtlkmzck
juam
juan
juana
juanita
juanm
juarez
jubilac
jubilacion
jubilada
jubilado
jubilados
jubilo
judicial
judiciales
judicialmente
judith
juega
juegan
juego
juegos
jueguen
jueves
jufre
jugada
jugando
jugar
jugo
jugos
juguete
jugueteria
jujuy
jul
juleriaque
julia
juliaduelli
julian
juliana
julieta
julio
julissa
jun
juncal
junin
junio
junior
junkn
junta
juntan
juntarme
juntas
junto
juntos
junturas
juose
juramento
jurgens
juridiccion
juridico
juries
jurisdiccion
juscar
jusgado
just
justa
justamente
justapetra
justicia
justificacion
justificar
justificativo
justifico
justina
justiniano
justo
justos
juxwnggdbsdv
juzgado
jvtm
jwp
jx
jy
k
ka
kag
kandil
kangoo
kansai
karen
karina
karting
katherine
kay
kc
kcwcv
kd
ke
kedo
keep
keila
kennedy
kennen
keratin
keratina
keria
kes
kevin
keyla
kfhncmgpxgtiqtlvc
kfvv
kg
kged
kggracias
kgrs
kgs
kh
kiero
kiissnervalentina@gmail
kilaje
kilo
kilogramo
kilogramos
kilometraje
kilometros
kilos
kiogramos
kiosco
kiosko
kirchner
kisera
kisiera
kit
kitchenaid
kl
klei
kluber
km
kms
kn
kndependencia
ko
koch
kochan
kodak
kohen
kong
korn
kos
koury
kp
kped
krause
krausen
kromacolor
krqvhjdscbkqtwmewgyzugkfhwtcfbvnsw
ks
ksa
kve
kvgmb
kz
l
la
laa
labarden
labiales
labor
laborable
laboral
laborales
laboralmente
laboratorio
laboratorios
laborde
laboulaye
labrador
laburan
laburando
laburar
labure
laburo
lacattiva
lacroze
lactea
lademora
lado
lados
ladran
ladrillos
ladrones
lady
ladyfit
laenvian
laenvin
laferrere
laferte
lafuent
lafuente
lagacetashop
lage
lagos
laguna
lagunas
lahitte
lamadrid
lambare
lamentaban
lamentable
lamentablemente
lamentamos
lamentan
lamentando
lamento
lami
lamina
lampagina
lamparas
lan
lancioni
langelotti
lansucursal
lanus
lanzando
lanzar
lanzarla
lapachos
lapagina
lapiz
laprida
laps
lapso
lapueden
lara
larco
larga
largamente
largas
largo
largos
larguisimo
laroque
larran
larrazabal
larrea
larreo
larroque
larrosa
larroude
las
laser
lastima
lastra
lasucursal
latarjeta
latas
latera
lateral
laterza
latitud
latitular
latorre
laudani
lauquen
laura
lauragutierrezbello@gmail
lava
lavalle
lavalleja
lavan
lavarden
lavarle
lavarropa
lavarropas
lavezzari
lavorales
laya
lazzari
lbk
lbr
lcd
ld
lda
lde
ldhvzzjwg
le
lea
leal
lean
leandro
leaozlaa
lebensohn
leche
led
ledesma
ledezma
lee
leen
leer
leerlos
leernos
leg
lega
legajo
legal
legales
legalidad
legalizacion
legalizada
legalizadas
legalizar
legalmente
legar
legislacion
lego
legue
leguizamon
lei
leido
lejana
lejisimo
lejisimos
lejos
lekuubguw
lel
lelgo
leloir
lema
lemos
lenah
lenceria
lenos
lenta
lente
lentes
lentitud
lento
leo
leon
leonardo
leonel
leones
leonesa
leonor
lera
lerma
les
leticia
letis
letra
letrado
letras
leucemia
leufu
levantan
levantar
levantare
levante
levaran
levemente
ley
leyenda
leyendo
leyton
lezcano
lg
lh
li
lia
libano
libera
liberado
liberan
libercam
liberen
libertad
libertador
libre
libreria
libres
librito
libro
libros
lic
licencia
licencias
liceo
licitacion
licitar
lida
lidia
lienan
lifestyle
ligados
lighting
lila
lilen
liliana
lilus
lima
limita
limitacion
limitada
limitado
limite
limites
limpien
limpieza
lin
linch
lincoln
linda
lindor
line
linea
lineamiento
lineamientos
lineas
liners
linier
liniers
linio
link
linkedin
linniers
lino
linterna
lio
lionel
liotti
lipartiti
liquidacion
liquidaciones
liquidadores
lis
lisandro
lista
listadas
listado
listo
listos
lite
literal
literalmente
lithg
litin
litro
litros
little
live
livepass
liveslow
liveticket
liviana
liviano
living
livio
lizaso
ljadgcydcsxh
lkj
ll
lla
llacer
llagada
llagado
llagan
llagar
llagara
llagaron
llama
llamaba
llamada
llamadas
llamado
llamados
llamafo
llamame
llamamos
llaman
llamandi
llamando
llamandolos
llamandonos
llamar
llamara
llamaran
llamare
llamaren
llamares
llamaria
llamarian
llamarles
llamarlo
llamarlos
llamarm
llamarme
llamaron
llamarse
llamas
llamda
llamdas
llame
llameme
llamemos
llamen
llamenme
llamenmr
llames
llamo
llanes
llanta
llantas
lleco
lleegar
lleevando
llega
llegaba
llegaban
llegada
llegado
llegadoel
llegan
llegando
llegandogracias
llegandome
llegango
llegar
llegara
llegaramos
llegaran
llegare
llegargracias
llegari
llegaria
llegarian
llegarme
llegaron
llegarva
llegas
llegaso
llegat
llegen
llegi
llego
llegoa
llegodel
llegomi
llegoy
llegra
llegue
lleguel
lleguemos
lleguen
llegur
llehar
lleho
lleme
llena
llenada
llenado
llenar
llenas
llendo
llene
llenos
lleog
llerena
lleva
llevaba
llevaban
llevada
llevado
llevados
llevamos
llevan
llevando
llevar
llevaran
llevarian
llevarla
llevarlas
llevarles
llevarlo
llevarme
llevaron
llevarse
lleve
llevemos
lleven
llevo
llevomi
llevos
llgo
lllamando
lllegamuchas
lllego
llovia
llovio
llueve
lluvia
lluvioso
lmuchenik@oca
ln
lna
lnforme
lo
loa
loas
loayza
loberia
lobos
lobtrjo
loca
locaclidad
local
locales
localidad
localidades
localiza
localizable
localizacion
localizado
localizan
localizar
localizarla
localizarlo
localizarlos
localizarme
localizo
locas
locker
lockers
loco
locomoverme
locontrario
locos
locura
locutorio
logar
logear
logica
logicamente
logico
logisitco
logistic
logistica
logistico
logistics
logo
logor
logos
logra
logrado
logran
logrando
lograr
lograrla
lograrlo
lograron
logras
logre
logren
logro
loinforme
loli
lolos
lom
loma
lomas
lombardi
lombardini
lomitas
longboard
longchamps
longitud
longobucco
lonja
lonpuedo
lonque
lopez
lopez@grupoabans
loquear
lor
lora
lorca
lorecibio
lorecibo
lorena
lorenso
lorenzo
loreto
loria
los
loscaligaris
losdos
losso
lote
lourdes
loza
lp
lpg
lpl
lpm
lq
ls
lsgobpfcn
lso
lts
lu
lubicada
lubrication
luca
lucas
luccerini
lucero
luces
lucho
lucia
luciana
luciani
luciano
lucila
lucrecia
ludmila
luduena
luego
lues
lugano
lugar
lugares
lugones
luiggi
luis
luisa
luisina
lujan
lujo
lules
lulu
lun
luna
lunes
lupia
lupinos
lupita
luque
luro
lushka
luxemburgo
luz
luzuriaga
lvbpkuufrhiruxngukfhdhc
lvph
lwwryou
lxs
lynch
lyrpj
m
ma
maa
maas
mabel
mabres
macacha
macarena
macetas
maceteros
macia
macias
maciel
maciela
mackenna
macri
macro
macron
madam
madariaga
made
madera
maderas
madero
madre
madrid
madrugada
madry
madryn
maestra
maestro
mafiosos
magaldi
magali
magarinos
magdalena
magdalenas
maggio
maggiolo
magicos
magnanelli
magnasco
magnenat
magni
magnolias
magui
mahatma
mail
mailroom
mails
mailto
maio
maipu
maira
maiten
maiz
maiztegui
maj
make
mal
mala
malabia
malacari
malal
malargue
malas
malasia
malaver
maldita
maldito
maldonado
maleducada
maleducado
malentendido
malestar
malibu
malisima
malisimo
malisimos
mall
malla
mallea
mallorca
malo
malos
malsenido
maltratada
maltratadores
maltratados
maltratando
maltraten
maltrato
maltratos
malv
malvinas
malvinaspesimo
mama
mamamia
mamberti
mamnera
man
manana
mananamuchas
mananas
manchacolores
mancilla
mancuernas
manda
mandaba
mandaban
mandada
mandadas
mandadero
mandado
mandaeon
mandame
mandamelo
mandamos
mandan
mandando
mandandolo
mandandonos
mandar
mandaran
mandaria
mandarian
mandarin
mandarla
mandarle
mandarles
mandarlo
mandarlos
mandarme
mandarmela
mandaron
mandarpara
mandarse
mandasen
mandatario
mandaton
mandavan
mande
mandelo
manden
mandenme
mandenos
mando
mandqr
maneja
manejamos
manejan
manejando
manejar
manejarme
manejarnos
maneje
manejen
manejo
manera
maneras
manga
mangaka
mangold
mangos
maniaco
manifestar
manifestare
manifestarles
manifeste
manifesto
manifiesta
manifiestan
manifiesten
manifiesto
maniobras
maniqui
manmelodia
manna
mano
manos
manseo
mansilla
manso
manta
mantenga
mantengan
mantengo
mantenia
manteniendo
mantenimiento
mantiene
mantienen
mantra
manualidades
manuel
manufactura
many
manz
manza
manzana
manzanar
mapa
mapaches
mapas
mapita
maps
maq
maquete
maquina
maquinas
maquinchao
mar
mara
marabini
maracaibo
maravillas
marca
marcaba
marcada
marcadamente
marcadas
marcado
marcan
marcando
marcapasos
marcar
marcaron
marcela
marcelina
marcelino
marcelo
marcha
marchas
marciales
marco
marconetto
marconi
marcos
mardel
marearon
marengo
margarita
maria
mariana
mariangel
mariano
mariarosviar
marias
maricel
marido
mariel
mariela
marin
marina
marino
mario
maris
marisa
mariscal
marisol
maritimo
market
marketing
marlee
marlen
marmol
maro
maroni
marque
marquez
marron
marroquineria
mart
marta
marte
martelli
martello
martes
martha
marti
martin
martina
martindale
martines
martinez
maruja
mary
marykay
marzo
mas
masaje
masajeador
masajes
masari
maschwitz
masculina
masculino
mascwitz
maso
masomenos
massa
massaroni
master
mastercad
mastercar
mastercard
mastercat
masterccard
masters
mastes
mastescard
mastias
mat
mataderos
matafuegos
matanza
mate
mateo
matera
material
materiales
materias
materna
mates
matezi
matheu
mathey
mati
matias
matienzo
matilde
matinordberg@gmail
matriculados
matrimonio
matthews
maure
mauricio
max
maxi
maxihogar
maxikiosco
maxima
maxime
maximiliano
maximo
maximos
maxiquiosco
may
mayo
mayome
mayor
mayores
mayoria
mayorista
mayoristas
mayormente
mayra
mazaira
mazxini
mazza
mb
mbk
mc
mcuhas
md
mda
mdgfwuwb
mdicen
mdp
mdq
mds
mdz
me
meavisaron
mecado
mecadoenvios
mecanismo
mechita
mecking
meda
medejaron
media
mediacion
mediadora
mediados
mediana
mediano
mediante
medias
medica
medicacion
medical
medicame
medicamento
medicamentos
medicar
medice
medicen
medici
medicina
medicion
medico
medicos
medida
medidas
medieron
medife
medina
medio
mediocentro
mediocre
mediocridad
mediodia
medios
medir
medrando
medrano
medvedeva
mee
mega
megafrio
megallantas
megatone
megattone
meil
mejia
mejor
mejora
mejorar
mejorarla
mejoren
melaentreguen
melanie
melbourne
melero
melina
melincue
melipal
melisa
melissa
melivilo
melo
memandan
membretada
membrete
mena
menaje
mencion
mencionada
mencionadas
mencionado
mencionados
mencionan
mencionar
mencionaron
mencione
menciono
mendez
mendiza
mendosa
mendoza
mendozase
menendez
menghi
menor
menores
menos
menosponen
mensahe
mensaje
mensajede
mensajeo
mensajepididiendome
mensajeria
mensajerias
mensajero
mensajes
mensajescontesten
mensajesdesde
mensajito
mensale
mensual
mensuales
mensualmente
mensurarse
menta
mente
mentir
mentira
mentiraaaaa
mentiraestoy
mentiras
mentiroso
mentirosos
mentirrrrrrrr
menu
menucos
mer
meraderia
merc
mercadera
mercaderia
mercaderias
mercadi
mercado
mercadoenvio
mercadoenvios
mercadolibre
mercadolubre
mercadopago
mercados
mercafdo
mercafo
mercalibre
mercantiles
merced
mercedes
mercedez
mercomax
merecemos
merecen
meregalli
merezco
merlo
mero
mery
mes
mesa
mesada
mesaje
mesanje
mesas
mese
meses
meseta
mesita
mespor
messenger
metal
metam
metan
metanse
metansela
metanselo
metegol
meten
meter
meterle
meti
metido
metiendome
meto
metodo
metodologia
metodos
metro
metrogas
metros
mexico
meyer
meza
mezclaron
mezza
mhhu
mhjf
mi
mia
mias
micaela
micasa
miccoli
michael
michas
michelini
mico
micro
microcentro
microemprendimiento
microfono
microfonos
microlending
microondas
micros
mid
mide
miden
midiendo
midma
mieando
miedo
miel
miembro
mienta
mientan
miente
mienten
mientiendole
miento
mientras
mienvio
mienvioy
mier
mierc
miercoes
miercoles
mierda
miglioli
mignola
migo
migraciones
miguel
migueletes
miguez
mil
mila
milagros
milberg
milena
miles
militar
millan
miller
milll
millonecima
millones
milones
mima
min
mina
minaglia
minchiotti
mindomicilio
mine
minetti
mini
miniaturas
minicuotas
minima
minimamente
minimo
minimos
ministerio
ministrio
minitos
minitramp
mino
minoristas
minpaquete
minseguimiento
mint
mintiendo
mintieron
mintio
minuto
minutod
minutos
minuwrecibo
mio
mios
mipaque
mira
mirabello
mirador
miramar
miramontes
miran
miranda
mirando
mirar
mirason
mire
miren
miriam
mirian
mirko
mirna
miro
mirta
mirtha
mis
mision
misione
misiones
misiva
misivas
mism
misma
mismas
mismo
mismos
miso
missiones
mistral
mitad
mitarjeta
mitchell
mitre
miux
mj
mje
mjes
mji
mjooinifjug
mjs
mk
mki
mkj
ml
mla
mlibre
mlnumero
mlvqatojnjxtdpb
mlvqatojxcz
mlvqatojxidkm
mlvqatojyts
mly
mm
mma
mmandando
mmmm
mna
mnn
mnonte
mo
mobil
mobile
mobiliario
mochila
mochilas
mocoreta
mod
modalidad
modalidades
modapormayor
modela
modelo
modem
modernos
modestos
modficaron
modifica
modificacion
modificaciones
modificada
modificadas
modificado
modificados
modifican
modificando
modificar
modificarla
modificarlo
modificaron
modifico
modifique
modifiquen
modistas
modo
modos
modulo
modulos
moi
moises
moix
mojar
mojo
molde
moldes
molesta
molestando
molestar
molestaria
molestaron
moleste
molestia
molestias
molesto
molina
molinari
moline
moliner
molino
molinochacabuco
molinos
molles
mombre
momentaneamente
momento
momentos
mon
monachesi
moneda
monente
monetario
monete
monica
monicaaleslingiardi@hotmail
monina
monitor
monitoreoenvio
monitores
monkeyargentina
monoblock
monoblok
monocromatica
monotributista
monotributo
monoxido
monroe
monrteros
mons
monsenor
monserrat
monster
monstersound
montanari
montando
montaneses
montar
monte
monteagudo
montecarlo
montecaseros
montella
montero
monteros
montes
monteserrat
montevideo
monti
montiel
monto
monton
montserrat
montura
monzon
mora
morada
moradores
morales
moran
morante
morelli
morena
moreno
morenoya
morete
moreto
mori
moritan
moro
moron
moros
morse
morteros
mosconi
moscoso
moscu
mosquera
mosquitero
mosso
mostarles
mosteiro
mostraba
mostrador
mostrando
mostrar
mostrarles
mostraron
mostrarse
mostre
mostro
mot
motcor
motibo
motiva
motivo
motivos
moto
motocicleta
motomensajeria
moton
motoquero
motor
motora
motorola
motos
motosierra
mototola
motovehiculo
motriz
mountain
mouras
move
mover
moverme
movernos
moverse
movi
movido
movie
moviemientos
moviendo
movientos
moviera
movieron
movies
movil
moviles
movilidad
movilisarme
movilizarme
movilizarse
movimiento
movimientos
movio
movioooo
movista
movistar
moviste
movsitar
movstar
moyano
mp
mr
mrdrano
msj
msje
msjs
mt
mto
mtras
mtrs
mts
mucama
mucas
mucgas
mucha
muchaa
muchacho
muchad
muchas
muchaz
muchenik
muchisima
muchisimas
muchisimo
muchisimop
muchisimos
mucho
muchos
muchosimas
muchss
mucjas
mudado
mudamos
mudanza
mudar
mudarse
mude
mudo
mueble
muebleria
muebles
muere
muero
muestra
muestran
muestras
mueva
muevan
mueve
mueven
muevo
mugica
mujer
muletas
multa
multiples
mumero
mumeros
mundial
mundo
muneca
munecas
muni
municipal
municipalidad
municipio
munirme
muniz
munoz
munro
muratura
musica
musical
musimundo
musiquita
musuquita
musventradas
muten
mutual
mutuales
mutuo
muuuuchas
muuuy
muy
mw
mwvcfch
my
mychas
mysql
mytts
mz
mza
mzna
n
na
naar
nac
nacer
naci
nacida
nacido
nacimiento
nacion
nacional
nacionales
nacionalidad
naciones
nada
nadaa
nadaron
nadia
nadie
nadiee
nadieeeee
nadies
nadir
nadue
nadya
naembe
naftera
nagy
nahir
nahuel
naibi
nailon
naim
naldo
nancy
naomi
napal
naranja
naranjas
nas
nasa
naschel
natahena
natali
natalia
natalio
natura
natural
naty
nautica
nava
navaja
navarro
navegador
navegadores
navegar
navidad
nay
nazar
nazca
nbd
nbsf
ncesito
ncoeezyajsngwqa
ncontraba
nda
nde
ndeseguimiento
ndf
ndi
ndie
ne
nebulizador
nec
necdesitamos
nececito
neceditaria
neceisto
neceito
necesaria
necesariamente
necesarias
necesario
necesarios
necesidad
necesidades
necesido
necesiraria
necesit
necesita
necesitaba
necesitabamos
necesitad
necesitamos
necesitan
necesitando
necesitar
necesitara
necesitari
necesitaria
necesitariamos
necesitarlo
necesitas
necesite
necesiti
necesito
necesitoconsuotar
necesitp
necesotaria
necesoti
necestio
necestito
necesto
necios
necito
neco
necochea
necrsito
necsito
necxusbaires
nedia
nedisur
neecsito
neesito
nefasto
nefi
negando
negar
negativa
negativo
negliencia
negligencia
nego
negocio
negocion
negra
negras
negro
nehuen
neira
nel
nelida
nellensen
nelli
nelly
nelson
nena
nenas
nendoza
nenvio
neo
neojo
nerd
nerviosa
nervo
nescesito
nesecitamos
nesecitaria
nesecito
nesesito
nestor
net
netshoes
netshop
neumatico
neumaticos
neumonia
neuquen
nevada
nevesito
new
newberi
newbery
newcito
nformavion
nfs
nguia
ni
niall
nic
nicaragua
nicolas
nicole
niegan
nieto
nieva
nieve
nigun
niguno
nike
nikon
nilda
ninca
ninel
ningna
ningum
ningun
ninguna
ninguno
ningunos
nino
ninos
ninugna
nisiquiera
nisuta
nivel
niveles
nivoli
njo
nk
nl
nme
nmi
nnguna
nnumero
no
noble
noblex
nobre
nobtiene
noce
nocesito
noche
nochecita
noches
nocnoc
noe
noelia
noemi
noentregado
nogales
nogoya
noha
noificacion
nollego
nomas
nombrada
nombre
nombres
nombrw
nome
nomenclatura
nomento
nomme
nones
nonfunciona
nongun
nonpuedo
nooo
noooooo
nora
norauto
norberto
nordelta
norlog
norma
normabeatrizbelizan
normal
normalea
normalmente
normas
normativa
norpatagonica
norte
norteoca
northern
northfield
nos
nose
noseda
nosotros
not
nota
notado
notando
notar
notarial
notas
notbook
note
notebook
noticas
noticia
noticias
notifica
notificaban
notificacion
notificaciones
notificada
notificado
notificados
notifican
notificando
notificandome
notificar
notificarles
notificarlos
notificarme
notificaron
notificiacion
notifico
notifique
notifiquen
notifiquenme
noto
notocias
notorial
notorio
noutbut
nov
novatium
novato
novedad
novedades
novia
noviembre
novio
novoa
npaa
nqn
nr
nrcesito
nro
nron
nros
nseg
nseguimiento
nstra
nsulte
ntrega
nube
nuca
nucci
nuebe
nuec
nuemero
nuemro
nuequen
nuero
nuestra
nuestras
nuestro
nuestros
nuev
nueva
nuevamebte
nuevamente
nuevamentegracias
nuevamento
nuevas
nuevcamente
nueve
nueves
nuevo
nuevos
nuhp
nulas
nulero
nulo
num
numancia
number
nume
numeor
numer
numeracion
numeraciones
numeradas
numerito
numerl
numerlo
numero
numeros
numerosas
numro
nun
nuna
nunaca
nunca
nuncs
nuneracion
nunero
nunez
nunguna
nunk
nunka
nunva
nurmeo
nurstros
nuscar
nustra
nustro
nutricional
nutrilon
nuumero
nv
nva
nviar
nvio
nw
nxm
nyae
nylon
nz
o
oa
oagina
oara
oaso
obelisco
obera
oberst
objetivo
objeto
objetos
oble
oblea
obleas
obliga
obligaba
obligacion
obligada
obligado
obligandonos
obligar
obligaron
obligatoriamente
obliguen
obra
obraplus
obrar
obras
obrigada
obsequio
obsequios
observa
observacion
observan
observando
observar
observe
observo
obsoleto
obstaculizo
obstante
obteber
obtenemos
obtener
obtenerla
obtenerlo
obtengo
obteniendo
obtienen
obtuve
obtuvieron
obtuvo
obviamente
obvio
oc
oca
ocacionados
ocaciones
ocaenvios@oca
ocaepak
ocaepaknet
ocageo
ocala
ocamobi
ocampo
ocampos
ocapak
ocapostal
ocasa
ocasion
ocasiona
ocasionadas
ocasionado
ocasionados
ocasionando
ocasionaron
ocasiones
ocasiono
ocaweb
ocawebconsulta@oca
ocawebconsultas
ocdrfpwe
ocho
octavio
octubre
ocupa
ocupaados
ocupada
ocupadas
ocupado
ocupados
ocupan
ocupe
ocupen
ocupo
ocurre
ocurrida
ocurrido
ocurriendo
ocurrio
ocurrir
ocurrira
odioooooo
odlozcboyohhwqmm
oenyqm
oeptrackingweb
oest
oeste
oestes
oev
of
ofelia
ofender
ofendidos
ofertas
office
oficia
oficial
oficiales
ofician
oficina
oficinas
oficio
oficios
ofrece
ofrecemos
ofrecen
ofrecer
ofreceremos
ofrecerle
ofrecerles
ofrecerlo
ofrecerme
ofrecernos
ofrecia
ofrecido
ofreciendo
ofrecieron
ofrecimiento
ofrefemos
ofrezcan
ofrezco
ofuzco
ogar
ogrjye
oh
ohad
ohcepa
ohiggins
oidos
oil
ojala
ojchvm
ojeda
ojete
ok
okey
ola
olaa
olaguer
olanta
olavarria
olazabal
olbtuve
oldecop
olea
olegario
olga
oliden
oliva
olivares
olivera
oliveros
olivetta
olivia
olivos
ollantay
olmedo
olmos
olo
olocar
olta
olvarria
olvidaron
olvide
olvido
omar
ombu
ombus
ome
omiculio
omiti
omitio
omitivo
omm
omnibus
omq
on
ona
oncativo
once
oncologico
onda
onde
one
onlie
onlina
online
oo
oordinar
ooviviendas
op
opcin
opcion
opciones
opdea
open
opencart
opensport
opera
operacion
operaciones
operada
operador
operadora
operadoras
operadores
operaitva
operamos
operando
operar
operardor
operario
operarios
operativa
operativas
operativos
operatoria
operatorias
opinion
oportuna
oportunamente
oportunidad
oportunidaddes
oportunidades
oportuno
oprtunidadesque
opte
opticas
optimice
optimicen
optivas
opto
opwshcocg
or
oram
oran
ord
orde
orden
ordenes
order
ordered
orellana
organice
organismo
organismos
organizacion
organizados
organizar
organize
organizo
organo
orienta
orientacion
orientales
orientar
orientarme
origel
origen
original
originales
originalmente
origino
origone
orimer
orione
oriundo
orlandi
ormq
ornela
ornella
oro
oroplata
orqueste
orrible
orrrrrto
ortega
ortiz
orto
ortopedia
ortopedico
ortuzar
orwleg
os
oscar
oscoema
oscuros
osdepym
osdepyn
osea
osecac
osfaccing@gmail
osito
oso
osquete
osvaldo
otaku
otazo
otono
otorgada
otorgado
otorgan
otorgaron
otos
otra
otras
otro
otros
otrro
otto
ouee
out
ova
ovarios
ovbiamente
overhard
ovidio
ovuoeyde
owrsonalmen
oxeducation
oyk
p
pa
paa
paada
paaron
pabellon
pablina
pablo
pablolucero
pabloterrazzino@hotmail
pac
pachame
pacheco
paciencia
paciente
pack
packaging
packetes
pacta
pactada
pactadas
pactado
pactados
pactamo
pactamos
pactan
pactando
pactar
pactara
pactarenvio
pactarlos
pactaron
pacte
pacto
padel
padilla
padre
padres
padua
paez
pag
paga
pagaba
pagada
pagadera
pagado
pagamiento
pagamos
pagan
pagando
pagano
pagar
pagara
pagaria
pagarles
pagarlo
pagarm
pagaron
pagarse
page
paget
pagina
paginas
pagkna
pago
pagos
pagquete
pague
pagueademas
paguen
pagues
paguina
paguqte
pahud
paini
pairone
pais
paises
paiva
pajase
pajina
pak
pakete
palabra
palabras
palacio
palacios
palavecino
palermo
palet
paleta
palier
palmas
palmero
palmieri
palmira
palo
paloma
palomar
palomeque
palopoli
palpa
palta
pamela
pami
pampa
pan
panales
panama
panamericana
panasonic
panel
paniagua
panico
pantalla
pantalon
pantalones
panter
pantol
panuelos
paola
paotal
papa
papael
papaginoaapp
papampa
papas
papazzi
papel
papelera
papeleria
papeles
papelito
papelon
paptxeqxinibog
paq
paqete
paqetes
paqjete
paque
paquee
paquequte
paquere
paquerte
paquet
paqueta
paquete
paquetedesde
paquetee
paqueteia
paqueteqaa
paqueter
paquetera
paqueteria
paqueteriaa
paqueterias
paqueterna
paquetero
paqueterua
paquetes
paquetesaludos
paquetesoy
paquetito
paqueto
paquetria
paquetw
paquina
paqute
paqutede
paquwete
par
para
paraba
parada
paradero
parado
parados
paragolpe
paragolpes
paragonia
paraguay
paraisos
paralela
paralelas
paralizado
paralizo
parametros
paran
parana
paranaoca
parar
pararlo
parasitos
parate
parc
parcel
parches
pardo
pare
parea
parec
parece
parecen
parecer
pareceria
parecia
parecido
pareciera
parecio
parecion
pareda
paredes
pareja
parejas
parentesis
parera
pares
paricular
pariente
pario
paris
park
parlante
parlantes
parner
parnert
paro
paros
parovincia
parq
parque
parquetes
parqur
parra
parral
parravicini
parrilla
parroco
partamos
parte
parten
partes
participacion
participando
participar
particular
particulares
particularmente
partida
partidas
partido
partie
partieron
partio
partir
partner
partyplace
pas
pasa
pasaaaa
pasaba
pasaban
pasada
pasadan
pasadas
pasado
pasados
pasaje
pasajes
pasamos
pasan
pasando
pasaos
pasaporte
pasaportes
pasar
pasara
pasaran
pasare
pasaria
pasarian
pasarla
pasarles
pasarlo
pasarme
pasarn
pasarnos
pasaro
pasaron
pasarte
pasas
pasasen
pasasr
pascanas
pascual
pascuale
pasdan
pase
paseando
pasear
pasen
pasenle
pasenlo
pasenme
paseo
paserle
pasi
pasillo
pasillos
paso
pasoron
pasos
pasrian
pass
passadore
passrian
pasta
pastor
pata
patagones
patagonia
patente
paternal
patetica
patetico
pateticos
patgonia
patines
patineta
patio
patoco
patologia
patos
patria
patricia
patricio
patriciofleury
patricios
patrimonio
patrocinadores
patrocinio
patron
pauete
paula
paulafernandezcra@yahoo
paulina
pauperrima
pauperrimo
pauqete
pauquete
pausado
pauta
pautada
pautado
pautar
pautas
pavese
pavo
pavon
pavone
pawuete
paypal
payro
paysandu
payu
paz
pazienza
pazls
pazo
pb
pba
pc
pca
pci
pcia
pco
pcomunicarnos
pd
pde
pdf
pdo
pdoer
peatonal
peatones
peceras
pecom
pedales
pedazos
pedcido
pedebernade
pedernera
pedi
pedian
pediatria
pedida
pedidas
pedidio
pedido
pedidos
pedidosi
pedidoun
pedidoy
pedimos
pediod
pediodo
pedir
pediria
pedirle
pedirles
pedirlo
pedirme
pediso
peditrica
pedo
pedrito
pedro
peeo
pefido
pegada
pegadas
pegado
pegados
pegar
pegarla
pegarlas
pegarle
pegarme
pegaron
pegue
peguen
pehuajo
pehuen
pehuenia
pelada
pelea
peleando
pelegrini
peliconi
pelicono
peligro
peligrosa
peligroso
pellegrini
pellets
pelo
pelota
pelotas
pelotuda
pelotudez
pelotudo
pelotudos
peluche
peluqueria
peluquerias
pena
penal
pendieente
pendiente
pendientes
pending
penitenciario
penjak
pensaba
pensado
pensamiento
pensamos
pensando
pensar
pense
peo
peoducto
peor
peores
pepiri
pepsico
pequena
pequeno
pequenos
pequete
per
pera
peralta
peran
perazzini
percate
perder
perdere
perderlas
perderse
perdes
perdi
perdices
perdida
perdidas
perdido
perdiendo
perdiento
perdieron
perdiguero
perdimos
perdio
perdir
perdon
perdonarme
perdonen
perdriel
perea
pereda
pereira
pereyra
pereyro
perez
perfecta
perfectamente
perfectas
perfecto
perfil
perfume
perfumeria
perfumes
pergamino
peri
pericia
perico
periodicamente
periodicidad
periodicos
periodista
periodistas
periodo
perito
perjudica
perjudicada
perjudicado
perjudican
perjudicando
perjudicandome
perjudicar
perjudico
perjudiquen
perjuicio
perjuicios
perla
permanece
permanecer
permanecera
permanecia
permanecido
permanecio
permanente
permanentemente
permanentes
permanesca
permanezca
permiso
permisos
permita
permitan
permite
permiten
permitido
permitieron
permitio
permitirme
permito
pernuzzi
pernuzzijm@hotmail
pero
peroa
perola
peron
perone
peronismo
peror
perozzi
perri
perrone
perros
persianas
persona
personal
personalemente
personales
personalizacion
personalizada
personalizadas
personalizados
personalmente
personas
pertece
pertence
pertenece
pertenecer
pertenecia
perteneciente
pertenencias
pertinente
pertinentes
peru
pesa
pesaba
pesadilla
pesan
pesando
pesar
pesara
pesaria
pesaron
pesas
pese
pesima
pesimas
pesimo
pesimos
peso
pesoa
pesos
pesrsonales
pestana
pestanas
peter
peticionado
peticiones
petrel
petriella
petro
petrobras
petroleo
petropack
peudo
peugeot
pfzlbko
pgs
philips
phoit
phone
php
pi
piano
piap
pibe
pica
picco
pichi
pickit
pico
picun
pida
pidan
pide
piden
pideron
pidiendo
pidiendole
pidieran
pidieron
pidio
pido
pidria
pidrian
piedad
piede
pieden
piedra
piedrabuena
piedras
piedrasos
piens
piensan
pienso
pier
pierda
pierdan
pierde
pierdo
piero
pierre
pies
pieza
piezas
pigue
pila
pilar
pilares
pilas
pileta
piloni
piloto
pilotos
pimientero
pin
pinamar
pinar
pinceles
pindaro
pinedo
pineiro
pineyro
pinilla
pino
pinos
pinto
pintos
pintura
pinturas
pinturerias
pio
pionera
piovano
piquete
piquetes
pir
piran
pirane
pirelli
piro
pirque
pis
pisani
pisible
pisiquiatrico
piso
pisos
pista
pistarini
pistola
pita
pitagoras
pitiaranda@hotmail
pizarro
pizza
pj
pje
placa
placard
placas
plan
planat
plancha
planchas
planchetta
planeada
planeado
planeamiento
planean
planes
planeta
planificado
planilla
planillas
planta
plantada
plantado
plantados
plantas
plantea
plantear
planteo
plantilla
plantillas
planton
plasma
plastica
plastico
plasticos
plata
plataforma
plataformas
platanos
platas
plateado
platinium
platinum
plato
platos
plats
play
playa
plaza
plazo
plazos
plc
please
plegable
pleno
ploli
plottier
plp
plta
plumerillo
plus
pm
pmcdvcvvekzvziztoipmqsskrdwpa
pnagina
pnaq
pnfvguoyrnrrnqyynrxzzjll
po
poatal
poblema
poblet
pobres
poca
pocas
poco
pocos
podamos
pode
podemos
poder
podera
poderla
podes
podesta
podia
podiamos
podian
podible
podiblemente
podido
podieron
podiran
podiras
podra
podran
podras
podre
podremos
podria
podriamos
podrian
podriantes
podrias
podrida
podrido
podrinas
poe
point
polasek
polema
poli
policia
policial
policias
polirubro
politica
politicas
poliza
pollacchi
polonia
polvo
polvorines
poman
pomera
pomgan
pompeya
ponce
pone
ponemos
ponen
poner
ponerla
ponerle
ponerlo
ponerme
ponernos
ponerse
pones
ponga
pongamos
pongan
ponganse
pongansen
pongo
ponia
poniendo
pontevedra
ponzo
poo
poque
poquito
por
poraqui
porblemas
porcel
porcelanato
porchedda
pordrian
porductos
poreso
porf
porfa
porfabor
porfavir
porfavlr
porfavor
porian
pornta
poronga
porq
porqeu
porqie
porque
porqueria
porqur
porrini
port
portabebes
portador
portal
portas
portas@gmail
portatil
portena
porteno
portera
porteria
portero
portigliatti
porto
porton
portugal
portugues
poruqe
posada
posadas
pose
posean
posee
poseen
poseer
posen
poseo
posibilidad
posibilidades
posible
posiblemente
posibles
posiblidad
posicion
posicionar
posido
positiva
positivo
posponiendo
pospusieron
pospuso
posse
possible
post
postak
postal
postalcertificada
postales
posteeiormente
postergaciones
postergando
postergandome
postergarme
postergaron
posterguen
posterior
posterioridad
posteriormente
postilla
postulacion
postularme
postulen
posubilidad
pot
potenciar
potque
potr
potrillo
pozo
ppa
ppder
ppdo
ppio
ppkdycwjgt
pporq
pporque
ppr
pprque
ppuedo
pq
pqc
pqn
pqo
pqra
pqs
pqsb
pque
pquete
pquetes
pra
practica
practicamente
practicar
practicas
practicatengo
practico
practicuna
prado
pralong
prde
prdido
prdiendo
prdir
prducto
pre
preatendedor
precaucion
precauciones
precentar
precio
precios
precioso
precisa
precisaba
precisamente
precisando
precisaria
precision
preciso
prediger
predisposicion
preducto
prefectura
prefer
preferencial
preferenciales
preferente
preferentemente
preferiblemente
preferido
preferiria
prefiero
prefuntar
pregaunta
pregramada
pregunta
preguntaba
preguntan
preguntando
preguntandoles
preguntandome
preguntar
preguntarle
preguntarles
preguntarme
preguntaron
preguntas
pregunte
pregunten
pregunto
preimpresos
premet
premia
premier
premio
premios
prenda
prendarios
prendas
prensa
prensentaron
prenumerada
preocupa
preocupacion
preocupada
preocupado
preocupados
preocupando
preocupar
preocuparamos
preocuparme
preocupase
preopacion
preoxupado
prepactado
prepaga
prepagadas
preparado
preparar
preparo
prepo
prepotente
prepotentemente
prepotentes
preprogramacion
preprograman
pres
presa
presciso
presencia
presenta
presentable
presentacion
presentado
presentamos
presentan
presentando
presentandola
presentar
presentara
presentare
presentarla
presentarlo
presentarme
presentaron
presentarse
presentas
presente
presentes
presento
presidencia
presidente
presio
presion
presionar
prespuesto
pressente
prestacion
prestaciones
prestada
prestado
prestadora
prestamista
prestamo
prestamos
prestan
prestando
prestar
prestaron
preste
presten
prestigio
prestigiosa
prestigioso
presumo
presupuestado
presupuestar
presupuesten
presupuesto
presupuestos
pretende
pretenden
pretenderan
pretendo
preupuesto
prevencion
previa
previamente
preview
previo
previos
prevista
previsto
previstos
pricipios
priducto
prieto
prima
primaria
primarias
primario
primas
prime
primer
primera
primeramente
primeras
primero
primeros
primo
primordial
principal
principalmente
principio
principios
pringles
printing
prioridad
prioritaria
prioritario
priscila
privacidad
privada
privado
privados
privandome
privarme
privincia
pro
proactiva
probable
probablemente
probado
probe
probema
proble
problema
problemas
problematico
problemita
procasa
procede
procedednte
procedeer
procedemos
proceden
procedencia
procedente
proceder
procedere
procederemos
procedi
procedido
procedimient
procedimiento
procedio
procedo
proceguir
procesada
procesado
procesadora
procesamiento
procesando
procesandose
procesar
procesarlo
procesen
proceso
procesos
procincia
procrso
prod
prodcutio
prodcuto
prodicto
prodrian
produccion
produce
produciendo
productivo
producto
productoen
productoo
productora
productos
productro
produjo
produto
prof
profesional
profesionales
profesionalismo
profesor
profucto
profundidad
profundizar
profx
progamar
programa
programaban
programacion
programada
programadas
programado
programador
programadores
programados
programando
programar
programarlo
programaron
programarse
programas
programdo
programe
programo
progreso
prolongado
promedio
promesa
promesas
prometan
prometen
prometido
prometieron
promocion
promocionales
promocionan
promociones
pronta
prontamente
prontas
prontisima
prontitud
pronto
proovedor
propducto
propia
propias
propiedad
propietaria
propietario
propietarios
propio
propios
proponen
proponerme
proponga
proponiendo
proporciona
proporcionada
proporcionado
proporcionados
proporcional
proporcionar
proporcionarle
proporcionaron
proporcione
proporciono
proposito
propuesta
propuesto
prorbe
prorrogar
prosecretaria
proseguir
prosegur
prosianiuk
prosigo
prosigue
protagoniza
proteccion
protegidos
protocolo
prov
provean
proveedor
proveedora
proveedores
proveen
proveer
proveerdor
proveerlo
proveerme
provencred
provenia
proveniente
provenientes
proveyo
proviene
provincia
provincial
provincias
provinie
provision
provisionalmente
provisto
provoca
provocando
prox
proxima
proximada
proximamente
proximas
proximo
proximos
proyectada
proyecto
proyectos
prsima
prudencial
prueba
pruebas
pruebo
prune
prup
ps
psar
psen
psicologo
psra
pss
pst
ptal
pte
pu
pua
puan
publica
publicacion
publicaciones
publicada
publicado
publicados
publicar
publicare
publicidad
publico
publicos
puccinelli
puch
pucheta
pude
puden
pudes
pudiendo
pudiera
pudieran
pudieron
pudiesen
pudimos
pudo
pudrirse
pudu
pueblito
pueblo
pueblos
pueda
puedan
puedar
puedas
puede
pueden
puedes
puedi
puedieron
puedo
puedocrealizarcel
puefo
puente
puentes
puera
puerta
puertas
puerto
pues
puesa
pueso
puesta
puesto
puestos
pueyrredon
pueza
puff
puffs
pugliese
pujato
pujol
pulgadas
pulse
pulseras
puma
punado
punilla
puno
punta
punto
puntos
puntoss
puntual
puntualidad
pupi
pura
puros
puse
pusera
pusieran
pusieron
pusimos
puso
puta
putas
puto
putos
pvc
pveo
pwb
px
pxwnbvqkd
pyedo
pyme
pymes
q
qaa
qaaa
qahzkav
qdo
qe
qede
qeria
qeso
qeu
qhe
qhola
qie
qieria
qiero
qisiera
qiwria
qje
qjeria
qjuiero
qm
qno
qqa
qque
qr
qro
qsean
qsk
qsoft
qtdtki
qu
qua
quaglia
quality
quatro
qubo
qud
que
quebesta
quebhaya
quebme
quebrachal
quebrada
queda
quedaba
quedado
quedamos
quedan
quedando
quedandome
quedar
quedara
quedare
quedaria
quedarme
quedarnos
quedaron
quedarse
quede
queden
quedo
quedp
quee
queens
queestapasandocon
quein
queiro
queja
quejando
quejandome
quejandose
quejar
quejarme
quejarse
quejas
queje
quejo
quellegue
quemado
queme
quemesaliocarisimo
queno
quenunca
quepasar
quera
queramos
queremos
querer
queres
queri
queria
queriamos
querian
querias
querida
querido
queriendo
queriendome
queriq
queris
quermos
querria
ques
quesignifica
quesucursal
quetenga
quetia
queya
quezada
qui
quia
quiaca
quiciera
quidiera
quidrl
quiebra
quiebren
quieero
quien
quienes
quieo
quier
quiera
quieran
quiere
quieren
quieres
quieria
quieriera
quiero
quiesiera
quieto
quiisera
quijadas
quilmes
quilombo
quilombos
quimica
quimicos
quimili
quimioterapia
quince
quinceeeeeeeeeeeeeeeeeeeeeeeee
quincenal
quino
quinta
quintana
quintino
quinto
quipildor
quiren
quiro
quiroga
quirurgica
quise
quisera
quiseron
quisiea
quisiera
quisieramoa
quisieramos
quisieran
quisieras
quisieron
quisisera
quisiwra
quiso
quisoera
quisuera
quita
quitilipi
quito
quiusiera
quiza
quizas
quiziera
quizo
qun
qur
qurbina
quria
qusiera
quusiera
quw
qwewqe
qwue
qye
qyeria
qzl
r
ra
rachazos
racional
raclamo
rada
radaelli
radiador
radica
radicacion
radicar
radio
rafael
rafaela
rafel
raffo
ragone
raider
raido
raiz
raja
ralizan
ralizar
ralo
ramal
ramallo
ramela
ramirez
ramiro
ramon
ramona
ramos
ranchos
ranciari
rancul
ranelagh
ranga
rango
rani
ranquil
ranser
rapida
rapidamente
rapidez
rapidisimo
rapido
rapidos
rapien
rapipago
rappallini
raquel
rar
rara
raro
raros
ras
rascan
rascar
rascarse
raspantiflia@gmail
rastra
rastrea
rastreador
rastreando
rastrear
rastrearla
rastrearlas
rastrearlo
rastree
rastreen
rastreo
rastros
ratificar
ratifico
ratirarla
ratito
rato
ratona
ratos
ratrear
rauch
raul
rava
ravignani
rawson
razon
razones
rcia
rcp
rdboo
rdc
rdg
rdios
re
reabrir
reactivar
reagendar
reagendo
real
realemente
realemnte
reales
realice
realicee
realicemos
realicen
realices
realico
realicwe
realidad
realisado
realise
realiza
realizaba
realizaban
realizada
realizadas
realizado
realizados
realizamos
realizan
realizando
realizar
realizara
realizaran
realizarbelnseguimientobde
realizare
realizaria
realizarian
realizarla
realizarle
realizarlo
realizarom
realizaron
realizarse
realize
realizo
reallizar
realmente
realozar
reanudara
reapuesta
reauisitos
rebbok
rebirlo
rebizo
rebotada
rebotado
rebotando
reboto
rebuscada
rebuscarsela
recalcada
recalde
recambio
recanalizar
recargo
recarte
recebi
recebir
recebirlo
recein
receiving
recepcion
recepcion@novotecargentina
recepciona
recepcionada
recepcionado
recepcionar
recepcionarlo
recepcionaron
recepcione
recepcionista
receptor
receptora
receptoria
recerptoria
recetas
rech
rechace
rechacen
rechase
rechaso
rechazaban
rechazada
rechazado
rechazar
rechazarlo
rechazaron
rechaze
rechazo
reciba
reciban
recibe
reciben
reciberespacio
recibes
recibi
recibia
recibian
recibida
recibidas
recibido
recibidor
recibidos
recibiendo
recibiendolo
recibiera
recibierlo
recibieron
recibimiento
recibimos
recibio
recibir
recibira
recibiraar
recibiran
recibire
recibiria
recibirl
recibirla
recibirlas
recibirlo
recibirloo
recibirlos
recibiy
recibo
recibr
recicibr
reciciran
reciden
recidido
recido
recien
reciente
recientemente
recini
recinir
recio
recir
recistencia
recital
recivbr
recivi
recivido
recivir
recivo
recklame
reclama
reclamado
reclaman
reclamando
reclamar
reclamaria
reclamarlo
reclamaron
reclame
reclamo
reclamos
reclamso
reclomo
recobi
recoger
recogerlo
recogida
recogido
recojan
recojer
recojerlo
recoleccion
recolecta
recolectar
recoleta
recomendable
recomendare
recomendaria
recomiendo
reconcha
reconciliar
reconducir
reconoce
reconocer
reconocida
reconozca
reconquista
recontra
recoordinacion
recoordinaran
recorda
recordamos
recordandole
recordar
recorde
recordinar
recorrido
recpcion
recpcionado
recta
rectificacion
rectificar
rectificaran
rectifico
recubre
recuerda
recuerdo
recupera
recuperar
recuperarla
recuperarlo
recupere
recupero
recurrente
recurrir
recurrire
recurro
recursos
red
redactada
rededor
redes
rediccionar
redireccion
redirecciona
redireccionar
redireccionarlo
redireccione
redireccionen
redirecciones
redirecciono
redirige
redirigido
redirigiendo
redirigir
redirigirlo
redpuesta
reduccion
reducciones
reducida
reducidos
reducir
ree
reebok
reebook
reebox
reeenvien
reef
reeimpresion
reembolso
reemplaza
reemplazarme
reencaminarlo
reencausarlo
reencien
reennvio
reenviado
reenvian
reenviar
reenviarian
reenviarla
reenviarle
reenviarlo
reenviarlos
reenviarmelo
reenviaron
reenviarselo
reenvie
reenvien
reenvio
reenvuarme
reeprogramado
reeprogramaron
reeprogramen
ref
refereancia
referencia
referencias
referente
referida
referidas
referido
referirme
reff
refiere
refieren
refiero
refiriendo
refleja
reflejado
reflexionar
reflotarlo
refrencian
refrendada
refrescaba
refrigeracion
refrigeradas
refrigerante
refutan
reg
regalar
regalaria
regalarle
regalo
regalopara
regarding
regiatrarme
regiatro
regimen
regimiento
regina
regio
region
regiona
regional
regionales
registered
registra
registracion
registrada
registradas
registrado
registrados
registrame
registran
registrar
registrarlo
registrarme
registrarnos
registrarse
registrarte
registras
registre
registren
registrese
registro
registros
registtrarme
regitros
regresa
regresado
regresan
regresando
regresar
regresarlo
regresaron
regrese
regresen
regreso
regsitro
reguera
regulador
regular
regularice
regularizara
regularmente
regustrarme
rehavita
rehen
reimpresion
reina
reinafe
reintegran
reintegrar
reintegraron
reintegre
reintegren
reintegro
reintentar
reintente
reitera
reiterada
reiteradamente
reiteradas
reiterados
reiterar
reiterarlo
reitero
reitrar
reitrarla
reja
rejas
relacion
relaciona
relacionada
relacionadas
relacionado
relaciones
relcionados
relef
relevante
relice
relizado
relizar
relizaron
relize
relizo
rellenos
reloj
relojes
remarcada
remarcar
reme
remedios
remera
remeras
remesa
remetente
remietente
remil
remis
remiseria
remita
remitan
remite
remiten
remitente
remitida
remitidas
remitido
remitiendo
remitieron
remitir
remitirlo
remitirse
remito
remitonro
remitos
remoto
renacimiento
renanco
renape
renaper
renata
renato
renault
rendicion
rendir
rendirlo
rene
renegando
renghini
renobacion
renoce
renom
renova
renovacion
renovatuvestidor
renove
renpre
renprogramaron
rentable
renuncia
renunciar
renviado
reoarten
reobaron
reorganice
rep
repactar
repactaron
repacte
reparacion
reparaciones
reparando
reparar
reparatoria
reparta
reparte
reparten
reparticion
repartido
repartidor
repartidora
repartidores
repartiendo
repartieron
repartio
repartir
repartirle
repartirlo
reparto
repartos
repartr
repaute
repecto
repente
repetida
repetidas
repetir
repeto
repite
repitio
repito
replanificada
replanificar
repletas
repleto
repoarto
repogramado
reponer
reporgraman
reporta
reportado
reportan
reportar
reporte
reportes
reporto
reposicion
reposo
representa
representacion
representante
representantes
representarlos
representate
represente
represento
reproduccion
reprogamado
reprogamando
reprogamar
reprogamaron
reprogaramado
reprograbama
reprograma
reprogramaba
reprogramacion
reprogramaciones
reprogramada
reprogramado
reprogramadoquiero
reprogramados
reprograman
reprogramando
reprogramar
reprogramaran
reprogramaria
reprogramarla
reprogramarlo
reprogramarmelo
reprogramaron
reprogramcion
reprograme
reprogramo
reprrogramar
reptogramado
republica
repuesta
repuestas
repuesto
repuestos
reputacion
reputatisima
requena
requeri
requerida
requerido
requeridos
requerimiento
requerimientos
requerir
requicito
requiere
requieren
requiero
requisito
requisitos
rerirar
reritar
resalto
resarcimiento
resarcir
resarsimiento
resatrearlo
rescatar
rescatarlo
rescatate
rescate
resch
rescibo
resena
resentido
reserva
reservar
reservare
reserven
reservo
resfuerzos
resgistracion
resguardado
resguardo
resibi
resibir
resibirla
resibo
reside
residencia
residencial
residentes
residiendo
residir
resido
residuos
resistencia
resivido
resivir
resivo
resolucion
resolver
resolveran
resolverlo
resolviera
resoplando
resp
respalde
respectivamente
respectivas
respectivo
respectivos
respecto
respeta
respetable
respetan
respetaron
respeten
respeto
respetos
respetusoso
respeusta
respiestas
responda
respondame
respondan
respondanme
responde
respondemos
responden
responder
respondera
responderan
responderme
responderte
responderya
respondi
respondia
respondian
respondido
respondiendo
respondiera
respondieran
respondieron
respondimos
respondio
respondiste
respondo
responfio
responsabilidad
responsabilidades
responsabiliza
responsabilizan
responsable
responsables
respopnden
respuedsta
respuesa
respuest
respuesta
respuestaaaaaa
respuestas
respuesto
respuesttas
respueta
respuetas
respuetsa
respusta
respyesta
resspuesta
resta
restante
restaria
restaurant
resto
restriccion
restricciones
resuelta
resuelto
resuelva
resuelvan
resuelve
resuelven
resuelvo
resulta
resultado
resultados
resultando
resultaque
resumen
resumenes
resumes
resumida
retencion
retener
retenerlo
retenerme
retenermelo
retenga
retengan
retenganmen
retenida
retenidas
retenido
retenidos
retiar
retiarar
retiarlo
retiene
retienen
retira
retiraba
retirada
retirado
retirados
retirafo
retirala
retiralas
retiralo
retiramos
retiran
retirando
retirandola
retirandolo
retirar
retirara
retiraran
retirararla
retirare
retiraresta
retiraria
retirarian
retirarl
retirarla
retirarlaespero
retirarlas
retirarlo
retirarlocuanto
retirarloporque
retirarlos
retirarls
retirarme
retiraro
retiraron
retirarse
retire
retiremos
retiren
retirer
retirlo
retiro
retiros
retirrian
retirsr
retirtar
retitar
retitat
retite
retorna
retornado
retornar
retornaron
retorno
retrasado
retrasados
retrasando
retrasaron
retraso
retrasos
retrogrado
retubo
retuvieron
reucaudos
reunion
reunir
reunirme
reunirnos
reunirse
revcibirlo
revean
revelado
rever
reveranda
reversible
revertido
revertir
reves
revestimiento
revetendo
revisaba
revisamos
revisan
revisando
revisar
revisarla
revisarlo
revise
revisen
revision
reviso
revista
revistas
revivir
revlamo
revlon
revueltas
reweigh
rey
reyes
rezzano
rfoffani
rg
rgb
ria
rial
ribeiro
ribotta
rica
ricardo
richterman
rickert
rico
ridiculo
riesgo
rieu
rigano
riglos
rimell
rimondino
rincon
rinconada
rindo
rio
riocuarto
rioja
rios
riquelme
riquisimos
risa
risnik
ristobal
rita
riv
riva
rivadavia
rivafavia
rivarola
rivera
rizzo
rkimy
rl
rma
rn
rne
rnpq
rnsp
ro
roach
robado
roban
robando
robaron
roberto
robles
robo
roca
rocamora
rocha
rocio
rocket
rodado
rodados
rodeitos
rodolfo
rodrigo
rodriguez
rodriquez
rogamos
rogando
rogaria
rogarles
roger
roggero
roguant
rois
roitz
roja
rojas
rojasque
roldan
rolelr
roller
roma
roman
romanello
romano
rombola
romero
romina
rompa
rompe
romper
rompiendo
rompieron
rompio
romulo
ronconi
ropa
roperos
roque
ros
rosa
rosales
rosamonte
rosana
rosario
rosariooca
rosas
rosboch
rosemberg
rosetti
rosetto
rosich
rospentek
rosquin
rossetto
rossi
rosviar
rota
rotas
roto
rotonda
rotos
rotulado
rotule
rotulo
rotura
router
rowa
rpta
rqxd
rralice
rralizar
rre
rreclame
rrhh
rrrrrre
rrservar
rs
rsario
rspuesta
rspuestal
rstancia
rstoy
rta
rtas
rte
rtirar
rtta
rtte
ru
ruatta
ruben
rubro
rueda
ruedas
ruego
ruegole
ruegoles
rufino
ruido
ruidos
ruiz
rumbo
rumipal
rural
rurales
rusia
ruta
ruter
rutina
rutinarios
ruy
rv
rvour
rw
rwdomn
rx
ryjtnnwsycghq
rz
rzapata@megatone
s
sa
saavedra
sab
sabad
sabadini
sabado
sabados
sabanas
sabattini
sabe
sabee
sabemos
saben
saber
saberb
sabercomo
sabercsi
sabercuando
saberel
saberen
saberlo
sabernsi
saberrrrr
sabes
sabet
sabia
sabiamos
sabian
sabido
sabiendo
sable
sabr
sabran
sabrer
sabria
sabrian
sabrina
sac
saca
sacado
sacan
sacandonos
sacanta
sacar
sacaran
sacarlo
sacarme
sacaron
sacate
saco
saen
saenpena
saens
saenz
saez
sagai
saker
sala
salada
saladillo
salado
salamandra
salazar
saldaria
saldo
saldos
saldra
saldria
saldrua
sale
salen
salg
salga
salgan
salgo
salguero
sali
salia
salian
salida
salido
salidos
saliendo
salieron
salimos
salinas
salio
salir
saliuo
sallal
salliquelo
salnudo
salome
salomon
salon
salsipuedes
salsria
salta
saltaba
saltan
salto
saltos
salud
saluda
saludamos
saludarles
saludis
saludo
saludoa
saludos
saludoss
salvador
salvadorarrieta@hotmail
salvar
salvat
salve
salvio
salvo
samara
sambucetti
samiento
sampacho
samsum
samsung
samuel
san
sanabria
sanatorio
sanchez
sancion
sancor
sancristobal
sancti
sandoval
sandra
saner
sanfield
sanitaria
sanitarios
sanmartin
sanmiguel
sanna
sano
sanogasta
sanrafe
sanroque
sans
santa
santafe
santaigo
santamaria
santana
santander
santi
santiago
santiaguenos
santigo
santillan
santini
santisima
santo
santojanni
santoni
santos
sanudos
sao
sap
saque
saqueada
sara
sarafield
sarah
sarahi
sarandi
saravi
saravia
sarde
sardfield
sarfiel
sarfield
sarfields
sarfierl
sarfierld
sarfil
sargento
sarmiento
sarquis
sarratea
sarsfiel
sarsfield
sarsfieldoca
sarsfielf
sarsfiled
sarzotti
sas
sastre
satisfactoria
saturada
saturadas
saturnino
sauce
sauces
sauma
save
saver
savera
saveriodiriccistore
savid
sber
scalabrini
scaramella
scarpello
schedule
schepens
schmira
schools
scodigo
scombro
scooter
scripts
scursal
sd
sdadsadasdad
sdasdasd
sdasdasdasd
sde
sdfdsfsdf
sdo
sdos
sds
se
sea
sean
sebas
sebastian
seber
sec
secaplatos
secarropas
secc
seccion
seccional
seco
secos
secretaria
secreto
sector
sectores
secuestrado
secundaria
secundario
secundarios
secursal
secwr
sede
sedes
sedronar
see
sefuimiento
seg
segado
segato
segimiento
segir
segovia
segudo
segui
seguia
seguida
seguidas
seguido
seguidor
seguidos
seguiemiento
seguiendo
seguiente
seguiento
seguiiento
seguiimiento
seguim
seguimeinto
seguimento
seguimie
seguimiemto
seguimiendo
seguimieno
seguimienot
seguimienoto
seguimient
seguimientl
seguimiento
seguimientobque
seguimientol
seguimientos
seguimientseguimiento
seguimieto
seguimiiento
seguimineto
seguiminto
seguimoento
seguimoentos
seguimos
seguimuento
seguir
seguira
seguire
seguirla
seguirlo
seguirmiento
seguiumiento
segulmiento
segumiento
segun
segund
segunda
segundo
segundos
segunfa
segunnla
segura
seguramente
seguridad
seguridas
seguro
segurola
seguros
seguuirlo
segyimiento
seimpere
seis
seisdedos
seleccion
seleccionando
seleccionar
seleccione
selecciono
selena
selene
self
sellada
sellado
sellar
sello
sellos
semaforo
semama
semana
semanal
semanales
semanalmente
semanas
seme
sememana
semi
semiento
semillero
semnaa
semnanas
semnas
sen
sena
senal
senalaba
senalan
senalando
senale
senalizacion
senalizaciones
senalizada
senalizado
senas
sencillamente
sencillo
sencomienda
sencosud
senda
sendin
sendos
senguer
senillasa
senillosa
senor
senora
senores
senorita
sensato
sensillo
sentada
sentado
sentados
senti
sentido
sentidos
sentimiento
sentimos
sentir
seoane
sep
sepa
sepan
separa
separado
separados
separar
seprit
septiembre
septima
sequimiento
ser
sera
serafin
seran
serca
sercana
sercano
serena
sergi
sergio
seria
serian
serie
seriedad
serio
serios
serlo
serme
serna
serrana
serrano
serv
server
service
servicio
servicios
servicios@oca
serviciosson
servico
servijur
servimos
servio
serviocio
servios
servir
servira
serviria
servitech
sesion
sesrvicio
set
setiembre
sets
seucursal
seuimiento
sexto
seycmgbjiplfwot
sf
sfn
sgkoortlr
sgo
sgte
shakira
sharp
shell
sherloshop
ship
shoes
shop
shopify
shoppin
shopping
shops
show
showroom
showrooms
shuclyuirozi
shunika@gmail
si
siancha
sibre
sibyl
sicursal
sido
sieguendo
siembre
siemdo
siemore
siempre
siendo
siendon
sientan
sienten
siento
sierra
sierras
siesta
siete
sifo
sifuiente
sig
siga
sigan
sigio
significa
significaba
significativo
sigo
siguavion
sigue
siguen
siguendo
siguiendo
siguiente
siguientes
siguiete
siguimiento
siguiwnte
sigwald
sijeron
silicona
silla
sillas
sillon
sillones
silva
silvana
silverio
silvia
silvina
silvio
sim
simcard
similar
similares
simisol
simoca
simon
simone
simple
simplemente
simples
simplificada
simplificarian
simplificarla
simplifiquen
simulat
sin
sincera
sinceramente
sinceros
sincronizar
sindicales
sindicalista
sindicato
singular
sini
sinientro
siniestrada
siniestrado
siniestro
sinietro
sinllego
sinllehona
sinner
sino
sinonimo
sinopec
sinque
sinserian
sinteticas
sintio
sinverguenzas
siquera
siquiera
sir
sirva
sirvan
sirvase
sirve
sirven
sistema
sistemas
sistematica
sita
sitado
site
sitio
sitios
sito
situacion
situaciones
situada
situraciopn
siucribe
siverguenzas
sivyl
sky
skyglass
skype
skyxntttc
sl
sldos
slds
sm
smart
smartcom
smartphone
smartwatch
smartway
smata
smdisalvio@gmail
smg
smith
smn
sms
smtuc
sn
sna
snacks
sneuquen
snowboard
snta
so
soap
sobejano
soberania
sobra
sobrador
sobradores
sobrandome
sobrante
sobre
sobrepasaron
sobres
socastro
socia
social
sociales
sociedad
socio
socursal
socusal
soderia
sofi
sofia
sofisticados
soft
software
soga
soi
soilan
sokl
sol
sola
solamente
solamnete
solana
solange
solano
solapas
solar
solciito
soldado
soldador
soldati
soldatti
soledad
soleil
soler
soliciitar
solicion
solicita
solicitaba
solicitada
solicitadas
solicitado
solicitados
solicitamos
solicitan
solicitando
solicitandome
solicitante
solicitar
solicitaria
solicitarle
solicitarles
solicitarlo
solicitaron
solicitarte
solicite
soliciten
solicitida
solicito
solicitud
solicotante
solidaria
solier
solis
solitante
solmesky
solo
solocito
solos
soluciionen
solucion
soluciona
solucionaba
solucionado
solucionados
solucionamos
solucionan
solucionanlo
solucionar
solucionaran
solucionarlo
solucionarme
solucionarmelo
solucionarmeno
solucionarse
solucione
solucionen
solucionenme
solucionenmelo
soluciones
soluciono
solucitaron
solucuiion
solutions
soluzione
solventan
solventar
sombras
sombrilla
somer
sommariva
sommier
somo
somos
son
sonando
sonar
sonde
sonia
sonnlos
sono
sony
soportes
sorda
soria
sorprende
sorprendida
sorprendidos
sorpresa
sorrentino
sorteo
sos
sosa
sospecho
sospechoso
sostema
soto
sound
sourdeaux
souvenirs
soy
spagnuoli
spar
spbjqwxzd
specialist
speedo
speedy
spegazzini
sperando
spinelli
spiritus
spor
sporline
sport
sportline
spr
sprt
spunc
spymex
spzghnre
sr
sra
sres
srguimiento
srl
srs
srta
srvq
ss
ssaber
ssber
ssm
ssr
st
sta
stamosa
standar
standard
star
status
ste
stefani
stefano
stella
stellato
step
stephanie
stephen
sticker
stiker
stimada
stivel
sto
stock
stocks
stoecklin
stolovitzky
store
stork
stos
stoy
studio
su
suardi
suarez
suazo
sub
subcontratista
subcursar
sube
subio
subir
subirlo
sublimadora
sublitextil
subo
suboficiales
subsanar
suc
succursal
sucecio
suceda
sucede
suceden
suceder
sucedia
sucedida
sucedido
sucedidos
sucediendo
sucediera
sucedio
suceidendome
sucesido
sucesivo
sucia
sucirsal
sucre
sucrsal
sucrsula
sucrusal
sucuarsal
sucuasal
suculentas
sucun
sucural
sucurcal
sucurdal
sucursa
sucursal
sucursala
sucursalde
sucursale
sucursalen
sucursales
sucursalesgracias
sucursalno
sucursalppr
sucursalquilmes
sucursalsalta
sucursalsino
sucursaltenemos
sucursar
sucursdal
sucursl
sucursla
sucurssal
sucurzal
sucusal
sucusales
sucusar
sud
sudamericana
sudas
suegra
suegro
suegros
sueldo
suele
suelen
suelo
sueltas
suena
suenan
suene
suerte
suertye
sueters
suficiente
sufrido
sufrio
sufrir
sufro
sugerencia
sugeri
sugerido
sugerir
sugerirles
sugiere
sugieren
sugiero
sugirieron
suguimiento
sugun
suipacha
sujeto
sukbymish
suktar
sullivan
sulma
sultar
sultarles
sulupe
suma
sumado
sumamente
suman
sumando
sumandole
sumar
sumarle
sumas
suministrada
suministrado
suministrar
suministro
sumo
sunat
sunchales
supe
supemercado
super
supera
superado
superan
superaron
superior
superiores
supermercado
supero
supervielle
superville
supervise
supervisen
supervisor
supesto
supiera
supieron
supimos
suplemento
suplementos
suplico
supo
supone
supongo
suponia
supplier
supplies
supuesta
supuestamente
supuestamento
supuestamentr
supuestamnete
supuestas
supuestmente
supuesto
supuestos
supuetamente
supuse
sur
suran
surcirsal
surcursal
surcusal
surdeasia
surge
surgentes
surgido
surgieron
surgio
surgir
surja
surmat
suroca
sursal
surucua
survusal
sus
susana
suscribe
suscribir
suscribirme
suscripta
suscripto
suscrito
susedio
susodicho
suspender
suspendi
suspendido
susques
sustancias
sustrajeron
suya
suyo
sv
svwaixaqjvep
sweater
swguimiento
swiss
swissjust
sx
sy
syrywtyjaywarcticbtcwhksnjlfnexgkfgu
systema
t
ta
taa
taaanto
tabares
tabla
tablada
tabladas
tablero
tablet
tabletas
tablets
taboada
taboga
taburete
tacaagle
tack
taco
tacos
tacrarse
tacuapi
tacuapy
tacuaras
tacuari
tae
taer
taeran
tafi
tagle
taj
tajeron
tajeta
tajo
tal
talar
talcahuano
tales
talestoy
talle
taller
talon
talones
talquiero
taluna
tamano
tamanos
tamara
tamb
tambien
tambienhago
tambo
tamone
tampoc
tampoco
tan
tanbien
tancacha
tanda
tandil
tania
tanque
tanta
tantas
tanti
tanto
tantos
tapa
tapada
tapando
tapera
tapia
tapias
tar
tarabajo
tarado
tarados
tarbado
tarcicio
tarda
tardaba
tardaban
tardan
tardando
tardanto
tardanza
tardar
tardara
tardaran
tardarestamos
tardaria
tardaron
tarde
tarden
tardes
tardescompre
tardesme
tardo
tarea
tareas
tarel
target
targeta
targetas
tarheta
tarifa
tarifaria
tarifario
tarifarios
tarifas
tarj
tarjera
tarjeta
tarjetagracias
tarjetamastercard
tarjetas
tarjetaservicio
tarjete
tarjewta
tarrabe
tarsetti
tartagal
tasaciones
tatiana
tatjetas
tax
taylor
taza
tazas
tb
tbl
tbn
tbt
tc
tcl
tdf
tdo
tdos
tdv
te
teatro
tebgi
tebgo
tec
techint
techo
teclado
tecmaco
tecnica
tecnico
tecnicos
tecnologia
tecnologicos
tecnord
teconologia
teddy
tedin
teefonicamente
teelfono
tefeno
tego
tejada
tejedor
tejer
tejeta
tejido
tekmedical
tel
tela
telas
tele
telecentro
telecom
teleefono
telef
telefanta
telefonia
telefonica
telefonicamente
telefonicas
telefonico
telefonicoel
telefonicos
telefonio
telefonista
telefono
telefonodebere
telefonono
telefonooo
telefonoooooo
telefonos
telegrama
telegramas
teleonos
telepase
teletex
televentas
television
televisor
telf
tellefono
telles
telmo
telono
tema
temaiken
temas
temgo
temia
temo
temor
temperaturas
temperley
temple
temporariamente
temprano
ten
tenagbqvpagar
tencion
tendian
tendo
tendra
tendran
tendre
tendri
tendria
tendriamos
tendrian
tendrias
tenefonos
tenemos
tenen
tener
tenerla
tenerlas
tenerlo
tenerme
tenes
tenfo
teng
tenga
tengamos
tengan
tengas
tengi
tengk
tengo
tengonq
tengoq
tenia
teniamos
tenian
teniaque
tenias
tenido
teniendo
teniente
tenis
tenog
tension
tentado
tentativa
tentria
teodoro
teoria
teoricamente
terapeutica
tercer
tercera
tercerizada
tercerizado
tercerizador
tercerizamos
tercerizar
tercero
terceros
terciario
terciarizado
tercra
terdes
teresa
teresita
tereza
tergopol
tergopoles
terjeta
terjetas
term
termas
termica
termina
terminacion
terminado
terminados
terminal
terminan
terminando
terminar
terminaron
termine
termino
termo
termotanque
terrada
terralagos
terrero
terrestre
terrible
terriblemente
terribles
territorio
territorios
terror
tesei
tessei
testaferro
tetamanti
tethis
tetirar
tetorarla
tevez
texas
texoplast
textil
textiles
texto
tey
teyssandier
tgo
thames
thank
thanks
the
thiago
thioso
this
thomae
thomas
thompson
thonet
thssiquhuxb
ti
tibia
ticket
ticketec
ticketeck
ticketek
ticketet
ticketk
ticketporta
ticketportal
tickets
tiembre
tiempo
tiempoquiero
tiempos
tien
tiend
tienda
tiendafuton
tiendas
tiende
tiendita
tiene
tienen
tienenden
tienes
tierra
tierras
tigre
tikcet
tiket
tiketec
tiketportal
tilda
tildado
tildados
tildaron
tildo
tilisarao
tilly
timbraron
timbre
timbres
time
timepo
timoteo
tinacos
tinogasta
tintina
tio
tipeo
tipie
tipo
tipos
tique
tiquet
tiquete
tira
tirada
tiradentes
tirado
tiran
tirar
tiraron
tirarse
tirne
tiro
tiscornia
titierra
titnen
titulada
tituladas
titular
titulares
titulo
titulos
tiularidad
tlb
tlc
tlefeno
tmb
tmp
tn
tner
tngo
to
tobias
toblanc
toca
tocado
tocan
tocar
tocara
tocaran
tocaria
tocarme
tocaron
toco
tod
toda
todabia
todas
todavi
todavia
todes
todi
todo
todoa
todopeluqueria
todopeluquerias
todos
todosvia
todosvlos
tofavia
togi
token
tokengggajtfw
toledo
tolerancia
tolerar
tolkar
tolosa
toloza
tom
toma
tomada
toman
tomando
tomandole
tomar
tomaran
tomare
tomarle
tomaron
tomas
tomasevich
tomate
tombesi
tomchuc
tome
tomen
tomo
tonde
tone
tonello
toninas
toniolo
toop
top
tope
topshow
toque
toquen
toques
toras
torciste
torcuato
torcuto
tordilla
toresani
torna
tornilleria
tornquist
toro
torre
torrencialmente
torres
torrigiani
torriglia
tortolero
tortora
tortuga
tortuguitas
tortura
toscanini
toscasxq
tostado
total
totales
totalidad
totalmente
totoral
totoras
totura
tovadia
toxico
toyota
tp
tpdrvteklp
traaja
trabado
trabados
trabaja
trabajaba
trabajabamos
trabajaban
trabajado
trabajador
trabajadores
trabajamos
trabajan
trabajando
trabajar
trabajaremos
trabajarlo
trabajarlos
trabajaron
trabaje
trabajen
trabajo
trabajos
traban
trabas
trabo
trace
track
trackear
trackearlo
trackeo
tracker
trackin
tracking
trackingconsult
trackingenvio
trackkk
tracvking
trades
tradicional
tradicionalmente
traduce
traducida
traductor
trae
traelo
traeme
traen
traer
traeran
traeria
traerian
traerla
traerlo
traerme
traermelo
traermelos
traes
traffic
trafic
trafico
traguieron
traia
traian
traicking
traiding
traido
traiga
traigam
traigan
traiganlo
traiganme
train
trajeran
trajeron
trajeta
trajetas
trajieron
trajo
trajobuna
trak
trakear
traking
trakking
tral
tramie
tramiresaludos
tramitacion
tramitada
tramitado
tramitados
tramitamos
tramitando
tramitar
tramitarla
tramitarlo
tramite
tramites
tramito
tramscurso
tramtites
trancado
trancas
tranferencias
tranking
tranportan
tranquila
tranquilidad
tranquillo
tranquilo
tranquilos
trans
transaccion
transacciones
transadan
transcurre
transcurren
transcurridas
transcurrido
transcurridos
transcurrieron
transcurrio
transcurrir
transcurso
transferencia
transferida
transferir
transfieran
transfieren
transit
transitada
transitar
transito
translado
transmite
transporta
transportada
transportadapor
transportadora
transportan
transportando
transportar
transportarlo
transporte
transportes
transportista
transurso
trapito
trapo
traqueo
trar
trarjeta
tras
trasero
trasladadas
trasladado
trasladar
trasladarlas
trasladarlo
trasladarme
trasladen
traslado
traslados
trasmano
traspapelo
traspasar
trasplante
trasportar
trastee
trastorno
trata
trataba
tratado
tratamiento
tratan
tratando
tratandome
tratandonde
tratandose
tratar
tratare
trataria
trataron
tratarse
tratava
trate
traten
trato
tratos
travago
travel
traves
travez
trayecto
trayectoria
trayendo
trebol
tree
treinta
trejomovich
treking
trekking
trelew
trelles
tremenda
tremendo
tren
trenque
tres
trevelin
triangulo
tribulato
tribultao
tribunales
triciclo
triciclos
trimestral
trinidad
triple
tripoli
tripulante
tristan
tristeza
triunvirato
trivulato
trjeta
trjo
trngo
tromen
tronador
troncos
tropea
tropezon
troquel
trossero
trrme
trs
truchas
truffa
truncado
trying
ts
tshg
tsingas
tsiros
tte
ttwwmtn
tu
tube
tubo
tuc
tucuma
tucuman
tuenti
tuerca
tueros
tuerto
tulipaneros
tulumpa
tumusla
tunuyan
tupo
tupungato
tuquejasuma
turbio
turcotti
turdera
turismo
turner
turno
turnos
turquoise
tus
tuve
tuviera
tuvieron
tuviesen
tuvimos
tuvo
tuyo
tuyu
tuyuti
tv
tvemart
tvo
tvpara
tvsat
tw
tyhofm
tyqjxhvnuvf
u
ua
uaa
uam
uan
ub
uba
ube
ubica
ubicacion
ubicaciopn
ubicada
ubicado
ubicados
ubican
ubicar
ubicarla
ubicarlo
ubicarlos
ubicarme
ubicaron
ubico
ubieran
ubiesen
ubique
ubiquen
ucacha
uco
ud
udaondo
udes
udine
uds
udtedes
ue
uedbhvue
uegente
uf
ufo
ugarte
ugarteche
ugo
ugrzggqewjznhkad
uhhh
uhuaia
uidz
uieri
uiero
uisiera
ulio
ulman
ultima
ultimamente
ultimas
ultimo
ultimos
ultrajada
ultrasonico
um
uma
umero
un
una
unacarta
unan
unanue
unas
unasituacion
unastarjetas
unb
unbpaquete
unca
uncelular
unibyte
unica
unicamente
unicar
unico
unicos
unidad
unidades
unidas
unidos
uniformes
unimev
union
uniquen
unir
unisex
universidad
universitario
universitarios
unmundoclick
unn
uno
unos
unq
unquillo
uns
unsolucion
uo
up
uqe
uqeria
uqx
uranga
urbano
urdinarrain
urg
urge
urgeeee
urgencia
urgenete
urgent
urgente
urgentee
urgenteeee
urgenteeeeeee
urgenteeeeeeeee
urgentemente
urgenteo
urgentes
urgentey
urguente
uriburu
uriel
url
urna
urquiza
urto
uruguay
uruguaya
uruguayo
us
usa
usada
usadas
usado
usan
usando
usar
usara
usarlo
usarse
usds
use
usetedes
ush
ushuahia
ushuaia
ushusia
uso
ustarros
ustd
ustdes
usted
ustedes
usteds
ustes
usttedes
usuahia
usual
usualmente
usuari
usuario
usuarios
usuhaia
ususario
usuuario
utds
ute
util
utilice
utilidad
utilitario
utiliza
utilizada
utilizado
utilizados
utilizamos
utilizan
utilizando
utilizar
utilizare
utilizarla
utilizarlo
utilizo
uu
uw
uwbxh
uwf
uxfr
uya
uyb
v
va
vacaciones
vacaiciones
vacantes
vacia
vacias
vacio
vacunas
vagos
vags
valcado
valcheta
valdez
valdivia
vale
valencia
valente
valentin
valentina
valenzuela
valeria
valida
validacion
validar
validez
valido
validos
valija
valla
vallan
valle
vallester
vallina
valor
valores
valorgracias
valoro
valparaiso
vam
vamos
vampa
van
vana
vanas
vanco
vandalismo
vander
vane
vanesa
vanessa
vanguardia
vanina
vanitori
vanitory
vannesa
vano
vans
vaqueros
varada
varado
varela
vargas
varia
varian
variar
varias
varillas
vario
varios
vas
vasos
vaya
vayan
vazquez
vb
vbuav
vcc
vces
vdominico
vds
ve
vea
veamos
vean
vecchi
vecchio
veces
vecesa
vecina
vecino
vecinos
vecs
vedia
veeeeeerguenzaa
vees
vega
vegas
vehiculo
vehiculos
veian
veinte
veira
velasquez
velasquezedith@gmail
velazquez
veles
velez
velezarfield
vellon
velz
vemos
vemvhjhqjpx
ven
venado
vence
vencen
vencia
vencidas
vencido
vencieron
vencimiento
vencio
vende
vendedor
vendedora
vendedores
vendedro
vendemos
venden
vender
venderia
venderlos
vendi
vendia
vendida
vendidas
vendido
vendidos
vendiendo
vendieron
vendio
vendo
vendra
vendran
vendria
vendrian
venecia
venecitas
venex
venezola
venezolano
venezuela
venga
vengan
vengo
venia
veniamos
venian
venida
venido
venir
venis
venta
ventana
ventanas
ventanilla
ventas
ventas@fitz
ventasrt
ventiluz
ventiservice
veo
vep
ver
vera
veran
verda
verdad
verdadera
verdaderamente
verdadero
verdaderos
verde
verdes
vere
vereda
vergas
vergienza
vergonzosa
vergonzoso
vergonzosodesde
vergonzozo
verguensa
verguenza
verguenzaaa
verguerza
vergueza
verif
verificacion
verificado
verificar
verificarlo
verificarse
verifico
verifique
verifiquen
verlas
verlo
verlos
vermouth
vernay
vernet
veronica
verse
versean
version
versos
ves
vesna
vestido
vestidor
vestimenta
veterinaria
vez
vezuna
vh
vhnqjizcuyflxono
vi
via
viable
viacargo
viagogo
viaja
viajan
viajando
viajante
viajar
viajaria
viaje
viajea
viajes
viajo
viale
viamonte
vias
viauno
vica
vicas
vicente
viceversa
viciaron
vicino
vicio
vicita
vicitaron
victimas
victor
victoria
victoriano
victorio
vicuna
vida
vidal
vidart
video
videos
vides
vidita
vidriada
vidriado
vidriera
vidrio
vidrios
vie
viedma
vieja
viejita
viejo
viejos
viendo
viene
vienen
vienes
vienieron
viento
vier
viera
viernes
vieron
views
vieytes
vigente
vigentes
vighi
vigilancia
vijande
vika
vilage
vilar
villa
villacorta
village
villaguay
villaloba
villalonga
villanueva
villar
villarino
villarruel
villars
villavicencio
ville
villea
villegas
vilma
vimos
vinabal
vincenzi
vinculada
vinculado
vincular
vincularlo
vine
vineron
viniendo
viniera
vinieran
vinierin
viniero
vinieron
vinierton
vinilo
vinilos
vinita
vino
vinoenvio
vinos
vio
violada
violado
violatorio
violeta
violetas
violi
violonchelo
vip
viral
viralizar
virasoro
virgen
virgilio
virginia
virrey
virreyes
virtual
virtud
virus
visa
visarme
visible
vision
visita
visitaban
visitada
visitado
visitador
visitalos
visitamos
visitan
visitando
visitandome
visitar
visitaran
visitaremos
visitaria
visitarian
visitarme
visitaron
visitas
visite
visiten
visito
viso
visperas
vista
vistaron
vistas
vistitado
visto
visual
visualicen
visualiza
visualizar
visualizarlo
visualizarlos
visualizo
vita
vitae
vital
vitv
viv
vive
viven
viveros
vivi
vivia
viviana
vivido
vividos
vivienda
viviendas
viviendo
vivimos
vivio
vivir
viviras
vivlasfotos
vivo
vivos
vizcarra
vizualizo
vjzss
vlez
vlla
vlver
vmbikinis
vmy
vndedor
vo
voces
vodnik
voila
volante
volar
volatil
volcar
volumen
voluntad
voluntario
volve
volvemos
volver
volvera
volveran
volvere
volveria
volverian
volverlo
volvernos
volverse
volvi
volvia
volvian
volviendo
volvieorn
volvieran
volvieron
volvio
volvipapelvolvioqque
vomero
vomo
vontra
vor
vos
vota
vover
voy
voz
vra
vroggero@icssa
vs
vsf
vte
vto
vtra
vuaje
vucetich
vuekvo
vueleven
vuelta
vueltas
vuelto
vuelva
vuelvan
vuelve
vuelvem
vuelven
vuelvo
vuelvoa
vuestra
vuestras
vuestro
vuestros
vueven
vv
w
wal
waldemar
walmart
walsh
walter
war
warnes
wasap
waters
watsaap
wdfdsfsdfsdf
wear
web
webb
webservice
webservices
website
wegscheider
weguer
weide
weisheim
welleda
wep
wesemann
wf
wgqqyrjsmaejhc
whasap
what
whatsapp
white
whuh
whumlgpm
wieczorek
wifdwrmdk
wilde
williams
winifreda
wippe
wish
wivana
wjhanc
wl
wn
woollands
wordpress
worm
wort
wrfr
wry
wsapp
wsec
wsta
wub
wue
www
wx
wzia
x
xbox
xcy
xeodgeia
xfa
xfabor
xfavor
xfavorrr
xgtskuzfrkmqjjzx
xi
xia
xiaomi
xii
xiv
xk
xmi
xnme
xp
xperia
xq
xqes
xque
xrq
xtf
xuando
xxiii
y
ya
yaa
yaaa
yaaaa
yaaaaaa
yaao
yab
yacanto
yae
yaguar
yalguien
yamila
yan
yanet
yanina
yanquetruz
yaque
yatay
yavino
yazmin
yb
ybno
ybya
ycmembvugk
ydiryjab
ye
yega
yegado
yegar
yego
yegua
yendo
yenifer
yerba
yesica
yfarias
yhkdxkgtyf
yl
ylo
ym
ymqm
yn
ynmee
yo
yocuamdo
yohana
yome
yop
yoshimura
you
young
yratando
yrigoyen
yrygoyen
ysale
ysnah
ysolo
ysrx
yt
yuhogzlgbsnnjzugr
yven
yya
yyo
yyy
z
zaa
zabala
zaballa
zaen
zagala
zaida
zalazar
zamboni
zambrano
zamora
zamponi
zamudio
zan
zanartu
zanchi
zapala
zapas
zapata
zapateria
zapatilas
zapatilla
zapatillas
zapatos
zapla
zar
zarate
zarza
zavaleta
zazzarino
zcaq
zcxwe
zeballos
zejaron
zejp
zelarrayan
zelkqzipgxpao
zeller
zelucash
zero
zfd
zh
zi
ziezas
zip
zipoli
zizzrciitdftslqettfktdc
zo
zoe
zola
zona
zonal
zonas
zonda
zorros
zsayqpia
zue
zumba
zuniga
zuviria
zzlbfu




#endif

/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */
/* ----------- end of file ------------- */


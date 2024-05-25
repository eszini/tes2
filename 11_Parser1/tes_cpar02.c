/*
 *	tes_cpar02.c
 *	
 *	parser - tokenizador
 *
 * 	Opciones:
 * 	-h   		forma de uso
 *
 */


/*
 * 	Ejemplos de uso:
 *
 * 	para el ejemplo ...
 *
 *
 *
 *	h01                       corpus de entrada, con el siguiente formato  ( es igual al f01 )
 *
 *      Ej:
 *
 *      1000 T1 Neg T2 liquido T3 0.9 x 0.8 x 0.85 cm No se observo liquido libre en cavidad.
 *
 *
 *
 *
 *	1) Generar en salida, todos los tokens del archivo de input, en formato columnar ( 1 tkn x linea)
 *
 * 	./tes_cpar02 -inp=h01  -out=h01_tkn
 *
 * 	./tes_cpar02  h01  h01_tkn
 *
 *
 *	2) Generar en salida, todos los tokens del archivo de input, en formato columnar ( 1 tkn x linea)
 *         con -e agrega al final de linea, el token EOL
 *
 * 	./tes_cpar02 -inp=h01  -out=h01_tkn  -e
 *
 *
 *	3) Generar en salida, todos los tokens del archivo de input, en formato columnar ( 1 tkn x linea)
 *         con -m pasa todo a minuscula
 *
 * 	./tes_cpar02 -inp=h01  -out=h01_tkn  -m
 *
 *
 *	4) Generar en salida, todos los tokens del archivo de input, en formato columnar ( 1 tkn x linea)
 *         Con -s ... separa segmentos por T1 T2 y T3 y procesa solo la frase que esta despues de T3 ...
 *
 * 	./tes_cpar02 -inp=h01  -out=h01_tkn  -s
 *
 *
 *	5) Generar en salida, todos los tokens del archivo de input, en formato "linea" (no columnar)
 *
 * 	./tes_cpar02 -inp=h01  -out=h01_tkn  -f
 *
 *
 * 	6) Idem anterior, con alguna informacion de salida 
 *
 * 	./tes_cpar02 -inp=h01  -out=h01_tkn  -s -e -m -f -v
 *
 *
 * 	7) SIN -f  ...  Agrega informacion en archivo de salida para controlar funcionamiento ( linea procesada y nro de token)
 *
 * 	./tes_cpar02 -inp=h01  -out=h01_tkn  -s -e -v -nvd=1
 *
 *
 * 	8) Para usar con un archivo de "marcas" ( para taggear ... )  Al final de este fuente, ejemplo de marcas !!!
 *
 * 	   Atencion !!!  La busqueda en la tabla de marcas NO es case sensitivo ... considerar el uso de -m 
 *
 * 	./tes_cpar02 h01 h01_tag  -mrk=f05_t1.mrk                    ( sale en formato columnar, con tags donde aplique )
 *
 * 	./tes_cpar02 h01 h01_tag  -mrk=f05_t1.mrk  -m                ( pasa todo a minuscula para eliminar el problema del case sensitive )
 *
 * 	./tes_cpar02 h01 h01_tag  -mrk=f05_t1.mrk  -s -m             ( aplica solo a la "frase" despues de marca t3 )
 *
 * 	./tes_cpar02 h01 h01_tag  -mrk=f05_t1.mrk  -s -m -nvd=1      ( agrega la frase procesada y nro de token en la salida )
 *
 * 	./tes_cpar02 h01 h01_tag  -mrk=f05_t1.mrk  -s -m -f          ( genera salida en formato frase, con tags )
 *
 * 	./tes_cpar02 h01 h01_tag  -mrk=f05_t1.mrk  -v -s -m -e       ( salida formato sentencia, con tags, en minuscula, con EOL, muestra proceso por pantalla )
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
int	gp_separa=0;		/* 0 NO separar el input por tabs ... 1 SI, separar por tabs y procesar solo la frase ... */


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
int	separar_seg(char *);

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
#define	TC_RST	99

int	parser1();

#define	MAXT	200
#define	MAXB	1024
#define	MAXF	64
#define	MAXD	16	/* buffer minimo */

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



/*	Estructuras y variables para base de datos */

int	bd_load();

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
char	marcas[3][MAXD] = { " t1 ", " t2 ", " t3 " };
int	tipo_marca=0;







/*
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


int	parser1()
{

	int	i,j,k;
	int	m1,m2,m3;
	int	f1,f2,f3;
	int	q_lin;
	int	q_tk;
	int	p1,p2,p3,p4;

	char	b1[MAXB];
	char	b2[MAXB];
	char	b3[MAXB];
	char	tk[MAXT][MAXB];

	/* si encuentro caracteres no considerados para el parser, avisar al final de todo el proceso */
	flag_caracteres = 0;


	q_lin=0;
	while (fgets(b1, MAXB, hfinp) != NULL)
	{
		/* procesar solo lineas no vacias */
		if ( !linea_vacia(b1))
		{
			f1=1;

			/* si indico que existen separadores (tab) en linea, separo los segmentos */
			if (gp_separa)
			{
				strcpy(b3,b1);
				separar_seg(b3);
				strcpy(b1,segmento[3]);
			}


			if (gp_verbose)
			{
				printf ("%s\n\n",b1);
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

			j=tipo_char(b1[p1]);


			switch (j)
			{
				/* otro caracter !!! */
				case TC_RST:
					flag_caracteres=1;
					if (gp_verbose)
					{
						printf ("Caracter no definido para parser: %c \n",b1[p1]);
					}
					p1++; 
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


		/* sumo lineas */
		q_lin++;


	}

	
	if (gp_verbose)
		printf ("Cant de lineas %d\n",q_lin);


}







/*
 * -----------------------------------------------------------------------------------
 *
 *	separar_seg
 *
 *	separa la linea del corpus en segmentos ...
 *
 * -----------------------------------------------------------------------------------
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

	/* set de marcas */
	if (tipo_marca == 0)
	{	for (i=0, !tipo_marca; i<strlen(buffer1); i++)
		{
			if (!tipo_marca && strncmp(buffer1+i," t1 ",4) == 0)
			{
				strcpy(marcas[0]," t1 ");
				strcpy(marcas[1]," t2 ");
				strcpy(marcas[2]," t3 ");
				tipo_marca = 1;
			}

			if (!tipo_marca && strncmp(buffer1+i," T1 ",4) == 0)
			{
				strcpy(marcas[0]," T1 ");
				strcpy(marcas[1]," T2 ");
				strcpy(marcas[2]," T3 ");
				tipo_marca = 2;
			}

			if (!tipo_marca && strncmp(buffer1+i," TAB1 ",6) == 0)
			{
				strcpy(marcas[0]," TAB1 ");
				strcpy(marcas[1]," TAB2 ");
				strcpy(marcas[2]," TAB3 ");
				tipo_marca = 3;
			}
		}


		if (gp_verbose)
		{
			printf ("\n");
			printf ("Tipo de marca :  %d   |%s|  |%s|  |%s|  \n\n",tipo_marca,marcas[0],marcas[1],marcas[2]);
		}
	}




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



#if 0
	/* tengo los 4 segmentos */
	if (gp_verbose)
	{
		printf ("Separar: \n");
		printf ("0: |%s|\n",segmento[0]);
		printf ("1: |%s|\n",segmento[1]);
		printf ("2: |%s|\n",segmento[2]);
		printf ("3: |%s|\n",segmento[3]);
	}
#endif

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

			if ( *( gp_fp(GP_GET,i,(char **)0) + 1) == 's'  )
				gp_separa = 1;

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
	gp_separa=0;
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






#if 0

f01:


1000 T1 Neg T2 liquido T3 0.9 x 0.8 x 0.85 cm No se observo liquido libre en cavidad.
1001 T1 Aff T2 pielonefritis cronica T3 88377 Ecografia Renovesical pre y postmiccional RD con signos de pielonefritis cronica sin dilataciones( 9 x 2.5 cm) RI con leve dilatacion pielocalicial Pelvis pre miccional 1.5 cm, postmiccional 1.6 cm, sin variaciones No se visualizan ureteres, Vejiga con escaso residuo postmiccional
1002 T1 Neg T2 adenomegalias T3 Diametro longitudinal:9.7( cm ) No se detectaron adenomegalias.
1003 T1 Neg T2 separacion T3 Conclusion: la imagen descripta podria corresponder a coleccion, pero, al no poder visualizarse claramente plano de separacion con el lobulo izquierdo tiroideo, no se puede descartar compromiso de esta glandula.
1005 T1 Neg T2 nodular T3 141730 Se observa glandula tiroides con ecoestructura heterogenea, predominantemente hipoecoica, no nodular.
1006 T1 Neg T2 adenomegalias T3 942914 Ambos nodulos tiroideos difusa y levemente heterogeneos, compatible con TLS No se observan adenomegalias en cuello Vol Tiroideo LD4.
1007 T1 Aff T2 ausencia T3 336386 implante renal aumentado de tamano, heterogeneo, sin diferenciacion  corticomedular Dl 122 mm se observa flujo arteial y venoso a nivel del hilio con ausencia de flujo en la periferia presencia de fistula A-V en tercio medio no se observan colecciones ni liquido libre
1008 T1 Neg T2 imagen T3 Psoas con fibras musculares conservadas, no se observa imagen sugerente de coleccion.
1009 T1 Neg T2 dilatacion  T3 Pelvis de diametro anteroposterior de 1.4(cm), extrarrenal No se detecto dilatacion  ureteral.
1010 T1 Aff T2 apendicitis T3 en su diametro anteroposterior, minimo liquido libre, e imagen tubular, dolorosa, no compresible con apendicolito en su interior, compatible con apendicitis aguda.

#endif










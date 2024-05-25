/*
 *	tes_cpar05.c
 *	
 *
 *	Fri Sep 28 10:08:43 -03 2018
 *
 *
 *	tokenizador
 *	parser (lex & yacc) y (nga ... negex ampliado )
 * 
 *	- maneja archivo de marcas para tagging
 *	- maneja diccionario de palabras para busqueda de errores
 *
 * 	Opciones:
 * 	-h   		forma de uso
 *
 *
 *
 */



/*
 *
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
 * 	./tes_cpar05 -inp=h01  -out=h01_tkn
 *
 * 	./tes_cpar05  h01  h01_tkn
 *
 *
 *	2) Generar en salida, todos los tokens del archivo de input, en formato columnar ( 1 tkn x linea)
 *         con -e agrega al final de linea, el token EOL
 *
 * 	./tes_cpar05 -inp=h01  -out=h01_tkn  -e
 *
 *
 *	3) Generar en salida, todos los tokens del archivo de input, en formato columnar ( 1 tkn x linea)
 *         con -m pasa todo a minuscula
 *
 * 	./tes_cpar05 -inp=h01  -out=h01_tkn  -m
 *
 *
 *	4) Generar en salida, todos los tokens del archivo de input, en formato columnar ( 1 tkn x linea)
 *         Con -s ... separa segmentos por T1 T2 y T3 y procesa solo la frase que esta despues de T3 ...
 *
 * 	./tes_cpar05 -inp=h01  -out=h01_tkn  -s
 *
 *
 *	5) Generar en salida, todos los tokens del archivo de input, en formato "linea" (no columnar)
 *
 * 	./tes_cpar05 -inp=h01  -out=h01_tkn  -f
 *
 *
 * 	6) Idem anterior, con alguna informacion de salida 
 *
 * 	./tes_cpar05 -inp=h01  -out=h01_tkn  -s -e -m -f -v
 *
 *
 * 	7) SIN -f  ...  Agrega informacion en archivo de salida para controlar funcionamiento ( linea procesada y nro de token)
 *
 * 	./tes_cpar05 -inp=h01  -out=h01_tkn  -s -e -v -nvd=1
 *
 *
 * 	8) Para usar con un archivo de "marcas" ( para taggear ... )  Al final de este fuente, ejemplo de marcas !!!
 *
 * 	   Atencion !!!  La busqueda en la tabla de marcas NO es case sensitivo ... considerar el uso de -m 
 *
 * 	./tes_cpar05 h01 h01_tag  -mrk=f05_t1.mrk                    ( sale en formato columnar, con tags donde aplique )
 *
 * 	./tes_cpar05 h01 h01_tag  -mrk=f05_t1.mrk  -m                ( pasa todo a minuscula para eliminar el problema del case sensitive )
 *
 * 	./tes_cpar05 h01 h01_tag  -mrk=f05_t1.mrk  -s -m             ( aplica solo a la "frase" despues de marca t3 )
 *
 * 	./tes_cpar05 h01 h01_tag  -mrk=f05_t1.mrk  -s -m -nvd=1      ( agrega la frase procesada y nro de token en la salida )
 *
 * 	./tes_cpar05 h01 h01_tag  -mrk=f05_t1.mrk  -s -m -f          ( genera salida en formato frase, con tags )
 *
 * 	./tes_cpar05 h01 h01_tag  -mrk=f05_t1.mrk  -v -s -m -e       ( salida formato sentencia, con tags, en minuscula, con EOL, muestra proceso por pantalla )
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
int	gp_detneg=0;		/* detectar negacion */
int	gp_tipneg=0;		/* tipo de negacion */


char	*desde_igual(char *s);
char	*pasar_a_minusc(char *s);

int	nnparse();
int	tiene_igual(char *);
int	linea_vacia(char *);
int	tipo_char(char );
int	es_numero(char);
int	es_num_tk(char *);
int	es_word(char *);
int	es_puntuacion(char *);
int	char_demed(char);
int	mostrar(int,int,char *);
int	m_estado();
int	separar_seg(char *);
int	nxt_tkn(int);

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
#define	TC_GBJ	9	/* guion bajo '_' */
#define	TC_RST	99

int	parser1();

#define	MAXV	64	/* buffer chico */
#define	MAXT	200 
#define	MAXB	1024	/* buffer grande */
#define	MAXF	64
#define	MAXD	16	/* buffer de juguete */
#define	MAXP	128	/* maximo de palabras en general */

char	finp[MAXF];
char	fout[MAXF];
char	fdic[MAXF];
char	fmrk[MAXF];
char	fgrm[MAXF];

FILE	*hfinp;
FILE	*hfout;
FILE	*hfdic;
FILE	*hfmrk;
FILE	*hfgrm;

int	flag_caracteres;


/*
 *	Estructuras y variables para tokens de las lineas procesadas
 *
 */

int	q_tk;
char	tk[MAXT][MAXB];






/*	Estructuras y variables para la base de marcas */

#define	MAXK	64	/* cantidad de marcas */
#define	MAXM	20	/* tamano maximo de palabra / marca */

int	bm_load();
char	*bm_tag(char *);


char	bm_key[MAXK][MAXM];
char	bm_mrk[MAXK][MAXM];

int	q_bm;		/* cantidad de marcas en base */




/* Estructuras para guardar segmentos de la linea
 * seg 0   nro registro
 * seg 1   aff / neg / pos 
 * seg 2   palabra de referencia
 * seg 3   frase 
 *
 */

char	segmento[4][MAXB];
char	marcas[3][10] = { " t1 ", " t2 ", " t3 " };
int	tipo_marca=0;


/*
 *	Estructuras y variables para diccionario de palabras
 *
 *
 */



int	bd_load();			/* funcion de carga de diccionario */

int	q_wrd;				/* cantidad de palabras en diccionario */

typedef	struct	tnode	*nodeptr;
typedef	struct	tnode
{	char	*wrd;
	int	num;
	struct	tnode	*nx;
}	node;

nodeptr	npx,np1,*npa;



/*
 *	Estructuras y variables para la base de gramatica nga 
 *
 */


int	bg_load();

int	q_grm;				/* cantidad de reglas en la gramatica nga */

typedef	struct	tregla	*reglaptr;
typedef	struct	tregla
{	char	*wrd;			/* puntero a la palabra */
	char	nm[4];			/* para identificar la regla */
	int	tip;			/* tipo de matching ... 1 exacto 2 parcial */
	int	num;			/* si es OR de palabras, cantidad de palabras alternativas, si es "tokens" .. cantidad a saltear */
	int	mrk;			/* spare use */
	struct	tregla	*nx;		/* proximo en la cadena (cuando es OR ) */
}	regla;

reglaptr	rpx,rp1,*rpa,*rpb;

reglaptr	tbr[20][64];		/* tabla de reglas ... 20 reglas ... 64 elementos x regla max */


/*
 *	Estructuras y variables para lex / yacc
 *
 */


int yyparse();
int readInputForLexer( char *buffer, int *numBytesRead, int maxBytesToRead );

static int globalReadOffset;

int	gc_pal_e;		/* palabra encontrada */

int	gc_si;
int	gc_no;
int	v_flag;
int	l_flag;
int	tok;
int	tok_si;
int	tok_no;

char   *spp      = "                            ";

char 	*globalInputText;
char	buffer[4096];
char	global_ref[MAXP];

char	yy_buffer[64];
char	*yy_ptr;

int	gramatik(char *,int);

int	f_cneg;			/* flag si parser encontro negacion en la linea */
int	f_sneg;			/* flag si parser NO encontro negacion en la linea */

int	f_cb;			/* flag continuo buscando ... (en el abol que arma yacc ) */
int	f_pe_lin;		/* flag palabra encontrada en linea  (en yacc ) */
int	f_pe_ora;		/* flag palabra encontrada en frase nomimal (en yacc) */




/*
 * -----------------------------------------------------------------------------------
 *
 *	main
 *
 *
 *
 * -----------------------------------------------------------------------------------
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


	if (gp_tipneg == 2)
	{
		if ( (hfgrm = fopen (fgrm,"r")) == NULL )
		{
			error(105);
		}

		bg_load();
	}



	parser1();


	fclose(hfinp);
	fclose(hfout);
	if (gp_diccionario)
		fclose(hfdic);
	if (gp_tabmrk)
		fclose(hfmrk);
	if (gp_tipneg == 2)
		fclose(hfgrm);


	if (gp_verbose)
		printf ("fin proceso \n");

	if (flag_caracteres)
	{	
		printf ("Atencion: caracteres no contemplados para parser encontrados en input  \n");
		printf ("Volver a ejecutar con flag -v y revisar salida (buscar \"Caracteres\" ... ) \n");
	}

}


/*
 * -----------------------------------------------------------------------------------
 *
 * 	parser1
 * 	parser de linea de texto
 *
 *
 * -----------------------------------------------------------------------------------
 */


int	parser1()
{

	int	i,j,k;
	int	m1,m2,m3;
	int	f1,f2,f3,f4;
	int	q_lin;
	int	p1,p2,p3,p4;

	char	b1[MAXB];
	char	b2[MAXB];
	char	b3[MAXB];

	/* init de valores para lex / yacc */
	globalInputText = buffer;
	gc_si    = 0;
	gc_no    = 0;
	v_flag   = 1;
	l_flag   = 1;
	gc_pal_e = 0;


	/* zona de intercambio entre lex y yacc */
	memset(yy_buffer,0,sizeof(yy_buffer));
	yy_ptr=yy_buffer;



	/* si encuentro caracteres no considerados para el parser, avisar al final de todo el proceso */
	flag_caracteres = 0;


	q_lin=0;
	while (fgets(b1, MAXB, hfinp) != NULL)
	{

	    /* procesar solo lineas no vacias */
	    if ( !linea_vacia(b1))
	    {
		/* blancos al final */
		for (i=strlen(b1)-1, f4=1 ; i && f4 ; i-- )
			if (b1[i] == ' ' || b1[i] == '\n' )
				b1[i]=0;
			else
				b1[i+1]='\n', b1[i+2]=0, f4=0;
		

 
		f1=1;

		if (gp_verbose)
		{
			printf ("\n");
			printf ("Linea  : %d \n\n",q_lin);
			printf ("Buffer :|%s|\n\n",b1);
		}



		/* Para el corpus de prueba inicial ... 
		 * Los campos son de forma:  nro t1 aff|neg|pos t2 palabra a buscar t3 frase  
		 *
		 * Separo todos los campos para tener en segmentos diferentes la palabra a buscar
		 * y la frase que tenemos que parsear 
		 *
		 * segm1:  nro de linea
		 * segm2:  aff neg pos
		 * segm3:  palabra de referencia ( para ver si esta en el scope de la negacion )
		 * segm4:  frase
		 *
		 *
		 */

		strcpy(b3,b1);
		separar_seg(b3);
		if (gp_verbose)
		{
			printf ("Linea  :  %d \n\n",q_lin);
			printf ("segm1  : >%s<\n",segmento[0]);
			printf ("segm2  : >%s<\n",segmento[1]);
			printf ("segm3  : >%s<\n",segmento[2]);
			printf ("segm4  : >%s<\n",segmento[3]);
			printf ("\n\n");
		}


		/* prep para analizar por tokens */
		memset(b1,0,sizeof(b1));
		strcpy(b1,segmento[3]);

		memset(global_ref,0,sizeof(global_ref));
		strcpy(global_ref,segmento[2]);


		/* prep para analizar la linea con lex / yacc */
		memset(buffer,0,sizeof(buffer));
		strcpy(buffer,segmento[3]);



		/* comienzo parser de tokens */
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

				/* guion bajo (es el join para el tag !! ) */
				case TC_GBJ:
					tk[q_tk][0]=b1[p1];
					tk[q_tk][1]=0;
					q_tk++;
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

		} /* while */


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


		/* si se pidio detectar negacion ... */
		if (gp_detneg == 1)
		{




		/*
		 * A este punto, termine de parsear la linea, y guardar todos los 
		 * tokens en vector.
		 *
		 */


		/* parseo la linea entera con lex & yacc */

		if (gp_verbose)
		{
			printf ("Parsing de gramatica ... \n");
			printf ("Palabra que busco ...   |%s|\n",global_ref);
			printf ("|%s|\n\n",buffer);

		}


		/* init de flags e indicadores para detectar negacion y pal en arbol de yacc */
		f_sneg   = 0;
		f_cneg   = 0;
		f_cb     = 1;	/* flag continuo buscando ... (en el abol que arma yacc ) */
		f_pe_lin = 0;	/* flag palabra encontrada en linea  (en yacc ) */
		f_pe_ora = 0;	/* flag palabra encontrada en frase nomimal (en yacc) */


    		globalReadOffset = 0;


		if (gp_tipneg == 1)
		{
    			tok=yyparse();
		}

		if (gp_tipneg == 2)
		{
			tok=nnparse();
		}

		if (gp_tipneg == 2 )
		{

		if (gp_verbose)
		{
			printf ("\n|%s|\n",segmento[3]);

			printf ("Resultado del parser           : %02d (%s)\n",tok,segmento[0]);
			printf ("Palabra  encontrada en linea ? : %d\n",f_pe_lin);
			printf ("Palabra  encontrada en scope ? : %d\n",f_pe_ora);

			printf ("\n\n\n - - - - - - - - - - - - \n\n\n");

		}

		}



		if (gp_tipneg == 1 )
		{

		/* resgistro para estadistica final */
		if (tok == 0)
			tok_si++;
		else
			tok_no++;

		if (f_sneg)
			gc_no++;
		if (f_cneg)
			gc_si++;

		if (f_pe_lin)
			gc_pal_e++;


		if (gp_verbose)
		{
			if (tok != 0 )
				printf ("ERROR ... No pudo parsear la lin : %d ... Sigue en prox. linea \n\n",q_lin);
		}


		if (gp_verbose)
		{
			printf ("\n\n");

			printf ("Resultado del parser           : %d\n",tok);
			printf ("Palabra  encontrada en linea ? : %d\n",f_pe_lin);
			printf ("Palabra  encontrada en scope ? : %d\n",f_pe_ora);

			printf ("\n\n\n - - - - - - - - - - - - \n\n\n");

		}


		}


		} /* if (detneg ... )


		/* 
		 * Termine todo lo que tenia que hacer con esta linea,
		 * sumo lineas 
		 *
		 */

		q_lin++;


	    } /* if ... no esta vacia la linea */

	}  /* while fgets ... */


	
	if (gp_verbose)
	{
		printf ("Cant de lineas procesadas %d\n",q_lin);
		printf ("Cant de encuentros de pal %d\n",gc_pal_e);
		printf ("\n");
		printf ("Cant de parseadas bien    %d\n",tok_si);
		printf ("Cant de parseadas mal     %d\n",tok_no);
		printf ("\n");
#if 0
		printf ("Cant de frase sin neg     %d\n",gc_no);
		printf ("Cant de frase con neg     %d\n",gc_si);
#endif
		printf ("\n\n\n");
	}


	/* parser1 */
}


/*
 * -----------------------------------------------------------------------------------
 *
 *	nnparse
 *
 *	parser tipo negex ampliado
 *
 * -----------------------------------------------------------------------------------
 */


int	nnparse()
{

	int	i,j,k;

	int	hay_mas_reglas;
	int	la_linea_sirve;
	int	regla_actual_match;
	int	ptl,ptk;
	int	ptl_ant;
	int	chances;
	int	nro_regla;
	int	sigo;
	int	alguna;
	int	condiciones;
	int	tipo;
	int	match;
	int	busco;
	int	regla_que_matcheo;
	int	f_pr;
	int	q_tk_ref;


#if 0
	if (gp_verbose)
		m_estado();
#endif

	/* aplico reglas a lista de tokens */

	hay_mas_reglas     = 1;
	la_linea_sirve     = 1;
	regla_actual_match = 1;

	match = 0;

	nro_regla = 0;

	regla_que_matcheo = 0;
	busco = 1;
	condiciones = 0;

	q_tk_ref = q_tk;


	while (  busco && hay_mas_reglas )
	{


		chances = q_tk-1 ;
		ptl = 0;
		ptk = 0;

		rpa = (reglaptr *) &tbr[nro_regla][ptk];

		printf ("Aplico Regla: |%s| \n",(**rpa).nm);

		ptl_ant = 0;

		while ( busco && la_linea_sirve )
		{
			printf ("Entre a la linea sirve     \n\n\n");

		chances = q_tk-1 ;
#if 0
			chances = 0;
#endif
			ptl = ptl_ant;	/* tengo que guardar donde quede ... */
			ptk = 0;

			regla_actual_match = 1;

			f_pr = 1;

			while ( busco && regla_actual_match )
			{
				sigo = 1;

				rpa = (reglaptr *) &tbr[nro_regla][ptk];

				tipo = (**rpa).mrk;

printf ("\nAntes del switch    nro_re: %d  tip: %d  ptk: %2d  tr: |%10.10s|  ptl: %2d  sig: %d  match: %d  cha: %d  cond:  %d  tk:  |%s|\n",
			nro_regla,tipo, ptk,(*tbr[nro_regla][ptk]).wrd,ptl, sigo,match,chances,condiciones,tk[ptl]);




				match = 0;

				switch ( tipo  )
				{
					/* es una palabra sola */
					case 1:	
						if ( (**rpa).tip == 1)
						{	if (strcmp ( (**rpa).wrd , tk[ptl] ) != 0)
								sigo = 0;  
						}
						if ( (**rpa).tip == 2)
						{	if (strncmp ( (**rpa).wrd, tk[ptl], strlen( (**rpa).wrd)  ) != 0 )
								sigo = 0;
						}

						if (sigo)
							match=1;
						break;

					/* numero de tokens intermedios posibles */  
					case 2:
						chances = (**rpa).num;
						break;

					/* es un OR de palabras alternativas */
					case 3:
						alguna = 0;

						rpb = rpa;

						for (j=0; !alguna && j< (**rpa).num; j++)
						{

	printf ("\nYYY loop case 3 ... sigo:  %d j: %2d  tkn:  |%s|  alt: |%s|  tip: %d  num:  %d   cha:  %d \n\n",
		sigo,j,tk[ptl],(**rpb).wrd,(**rpb).tip,(**rpb).num,chances);

							/* recorro la lista de palabras alternativas */
							if ( (**rpb).tip == 1)
							{	if (strcmp ( (**rpb).wrd , tk[ptl] ) == 0)
									alguna = 1;  
							}
							if ( (**rpb).tip == 2)
							{	if (strncmp ( (**rpb).wrd, tk[ptl], strlen( (**rpb).wrd) ) == 0 )
									alguna = 1;
							}

							rpb = (reglaptr *) & (*rpb)->nx;
						}

						if (alguna == 1 )
							match = 1;

						if (alguna == 0 )
							sigo = 0;

						printf ("\nCASE 3:  alguna: %d  sigo: %d  \n",alguna,sigo);

						break;

					/* es la palabra de referncia ... una .. o varias !!! (AND) */
					case 4:
						if ( strcmp(segmento[2],tk[ptl]) != 0 )
							sigo = 0; 

						if (sigo)
							match = 1;

				printf ("Entre CASE 4  seg: |%s|  tk: |%s|  ptl: %2d  ptk: %2d sigo: %d  wrd |%s| \n\n",
							segmento[2], tk[ptl], ptl,ptk,sigo, (**rpa).wrd);
						break;

				}  /* switch ... */

				if (f_pr && match == 1 )
				{	f_pr = 0;
					ptl_ant = ptl + 1;

					printf ("\nSET ptl_ant: %2d\n",ptl_ant);
				}

				if (match == 1)
					chances = 0;

				/* tengo distancia igual a 0 no hubo matchs, y se acabo la linea */
				if (chances == 0 && sigo == 0 && ptl == q_tk_ref )
					condiciones = 0;
				
				/* tengo distancia igual a 0 no hubo matchs, pero hay linea */
				if (chances == 0 && sigo == 0 && ptl < q_tk_ref )
				{	condiciones = 0;
					ptl = nxt_tkn(ptl);
				}
				
				/* tengo distancia igual a 0 y si hubo matchs y queda linea */
				if (chances == 0 && sigo == 1 && ptl < q_tk_ref )
				{
					condiciones = 1;
					ptl = nxt_tkn(ptl);
					ptk++;
					printf ("VVV1 ptk: %d\n",ptk);
				}

				/* tengo distancia igual a 0 y si hubo matchs pero no queda linea  */
				if (chances == 0 && sigo == 1 && ptl == q_tk_ref  )
				{
					condiciones = 0;
#if 0
					ptl = nxt_tkn(ptl);
					ptk++;
#endif
				}

				if (ptk == 0 && ptl == q_tk )
				{
					condiciones = 0;
					printf ("SET1 ... cond: %d\n\n",condiciones );
					regla_actual_match = 0;
					la_linea_sirve  = 0;

				}


				/* si el token suma chances continuo */
				if (tipo == 2)
				{	condiciones = 1;
					ptk++;
					printf ("VVV2 ptk: %d\n",ptk);
				}

				/* si el token no suma chances, y no sigo , restar una chance  */
				if (tipo != 2 && sigo == 0 && chances > 0)
				{
					chances--;
					condiciones = 1;
					ptl = nxt_tkn(ptl);
				}
					
				/* si el token no suma chances, y sigo, borrar chanches */
				if (tipo != 2 && sigo == 1 && chances > 0)
				{
					chances=0;
					condiciones = 1;
					ptl = nxt_tkn(ptl);
				}
					
				/* se termino la linea */
				if (tipo !=2 && chances > 0 && ptl == q_tk_ref )
				{
					condiciones = 0;
					printf ("SET2 ... cond: %d\n\n",condiciones );
					regla_actual_match = 0;
					la_linea_sirve  = 0;

				}


				/* determinar condiciones para seguir con regla actual */

				if ( condiciones == 0 )
				{
					regla_actual_match = 0;
					printf ("============>  corta !! \n");
				}

if ( tbr[nro_regla][ptk] != (reglaptr) NULL )
{
printf ("\nPie w reg act matc  nro_re: %d  tip: %d  ptk: %2d  tr: |%10.10s|  ptl: %2d  sig: %d  match: %d  cha: %d  cond:  %d  tk:  |%s| \n",
			nro_regla,tipo, ptk,(*tbr[nro_regla][ptk]).wrd,ptl, sigo,match,chances,condiciones,tk[ptl]);
}
else
{
printf ("\nPie w reg act matc  nro_re: %d  tip: %d  ptk: %2d  tr: |%10.10s|  ptl: %2d  sig: %d  match: %d  cha: %d  cond:  %d  tk:  |%s| \n",
			nro_regla,tipo, ptk,"vacio",ptl, sigo,match,chances,condiciones,tk[ptl]);
}


				/* si termino la regla y estamos en 'sigo' ... entonces tenemos match */
				if ( tbr[nro_regla][ptk] == (reglaptr) NULL )
				{
					printf ("ZZZ lleguamos al final de la regla ... \n");
					printf ("sigo: %d  match: %d  ptk: %d  cha:  %d \n\n\n",sigo,match,ptk,chances);

					regla_actual_match = 0;
					busco = 0;
					regla_que_matcheo = nro_regla + 1;
				}


			} /* while  regla_actual_match ... */


#if 0
			la_linea_sirve  = 0;
#endif

			printf ("\n\n\nSali del loop de la regla ... ptl_ant: %2d \n\n\n",ptl_ant);



		} /* while  la_linea_sirve ... */

		nro_regla++;
		if (nro_regla == q_grm)
			hay_mas_reglas = 0;

		la_linea_sirve     = 1;
		regla_actual_match = 1;



	} /* while hay_mas_reglas .. */

	


	return regla_que_matcheo;
}



/*
 * 	nxt_tkn
 *
 * 	provisorio ...
 * 	necesitamos resolver el tema de que en la lista de tokens,
 * 	aparecen los elementos de los tagg ...
 * 	y esto "entorpece" aplicar las raglas !!!
 *
 */


int	nxt_tkn(l)
int	l;
{
	int	i,j,k;
	int	new_l;
	int	f1,f2;

	new_l	= l+1;

	f1=1;

	while (f1)
	{
		f2 = 0;

		if ( strcmp( tk[new_l],"_" ) == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"NEG") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"VSE") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"TNEG") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"CNEG") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"ENDNEG") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"OBS") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"VIS") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"EVI") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"IDE") == 0 )
			f2=1;

		if ( strcmp( tk[new_l],"DET") == 0 )
			f2=1;



		if (f2)
			new_l++;
		else
			f1=0;

	}

	return new_l;
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
 * -----------------------------------------------------------------------------------
 *
 *	m_estado
 *
 *	muestra estado de tokens procesados y reglas en memoria
 *
 * -----------------------------------------------------------------------------------
 */


int	m_estado()
{

	int i,j,k,m9;


	printf ("Parsing linea con NGA \n\n");
	
	printf ("Separar: \n");
	printf ("0: |%s|\n",segmento[0]);
	printf ("1: |%s|\n",segmento[1]);
	printf ("2: |%s|\n",segmento[2]);
	printf ("3: |%s|\n",segmento[3]);
		
	printf ("\n\n");
	printf ("Cantidad de tokens ... %3d\n",q_tk);
	for (i=0; i< q_tk; i++)
		printf ("(%3d) %s\n",i,tk[i] );
	printf ("\n\n");

	printf ("Cantidad de reglas ... %3d\n",q_grm);
	for (i=0; i < q_grm; i++)
	{
		j=0;
		while ( tbr[i][j] != (reglaptr) NULL )
		{
			printf ("Reg: %2d  tk: (%2d)          tip: %3d    num: %3d   mrk:  %d |%s|\n",
				i,j,
				(*tbr[i][j]).tip, (*tbr[i][j]).num,(*tbr[i][j]).mrk,(*tbr[i][j]).wrd   );

			if ( (*tbr[i][j]).nx != (reglaptr) NULL )
			{
				rpa = (reglaptr *) &tbr[i][j];
				m9 = ( **rpa).num;

				for ( k=0; k< m9; k++)
				{	
					if (k)
					{
						printf ("                           tip: %3d    num: %3d   mrk:  %d |%s|\n",
							(**rpa).tip, (**rpa).num, (**rpa).mrk, (**rpa).wrd   );
					}

					rpa = (reglaptr *) & (*rpa)->nx;
				}
			}
			j++;

		}  /* while tbr ... */
		printf ("\n");
	}
	printf ("\n\n");

}












/*
 * -----------------------------------------------------------------------------------
 *
 *	bg_load
 *	carga la base de datos de gramatica nga
 *
 * -----------------------------------------------------------------------------------
 */

/*
 *	2018-09-27
 *
 *	bg_load
 *	carga la base de datos de la gramatica nga
 *
 */

int	bg_load()
{
	char	b1[MAXB];
	char	b2[MAXB];
	char	b3[MAXB];
	int	flag,f1,f2,f3,f4;
	int	i,j,k;
	int	p1,p2,q1,q2,q3;
	int	qtr;
	int	m9;

		/* wrd exacta, wrd parcial, barra, pipe, asterisco, mas, numero, tipo */
	int	flg_e,flg_p,flg_b,flg_pp,flg_a,flg_m,flg_num,flg_tip;

	/* cantidad de palabras en el diccionario */
	q_grm = 0;

	while (fgets(b1,MAXB,hfgrm) != NULL)
	{
		if (!linea_vacia(b1)  && b1[0] != '#' )
		{
			/* quitamos blancos, tabs y new lines ... y terminamos con una ',' */
			j=strlen(b1)-1;
			while ( j >=0 && ( b1[j] == ' ' || b1[j] == '\n' || b1[j] == '\t' ) )
				j--;
			b1[j+1]=',';
			b1[j+2]=0;

			if (gp_verbose)
			{
				printf ("%3d |%s|\n",q_grm,b1);
			}

			qtr=0;

			f1 = 1;
			p1 = 0;
			p2 = 0;

			while (f1)
			{	

				flg_e = flg_p = flg_b = flg_pp = flg_a = flg_m = flg_num = flg_tip = 0;

				while (b1[p1] != ',' )
				{
					if (b1[p1] != ' ')
					{
						b2[p2] = b1[p1];

						if (b2[p2] >= '0' && b2[p2] <= '9' )
							flg_num = (int) b2[p2] - '0';

						if (flg_b && b2[p2] == 'E')
							flg_tip = 1;
						if (flg_b && b2[p2] == 'P')
							flg_tip = 2;
							
						if (b2[p2] == '/')
							flg_b = p2;
						if (b2[p2] == '|')
							flg_pp = 1;
						if (b2[p2] == '+')
							flg_m = 3;
						if (b2[p2] == '*')
							flg_m = 4;

						p2++;
					}
					p1++;
				}
				b2[p2]=0;
				p2=0;
				

				if (gp_verbose)
				{
					printf ("Regla: %3d  token: (%3d) |%s|\n",q_grm,qtr,b2);
				}

				/* tengo regla q_grm, token qtr, txt del token b2 */
				f2 = 0;
				tbr[q_grm][qtr] = (reglaptr) malloc(sizeof(regla)) ;

				/* XXX identificar la regla !! */
				sprintf ( (*tbr[q_grm][qtr]).nm,"%3d",q_grm);

				/* es una palabra sola */
				if (!f2 && flg_b && !flg_pp )
				{
					(*tbr[q_grm][qtr]).wrd = (char *) malloc (strlen(b2)+1);
					strncpy ( (*tbr[q_grm][qtr]).wrd,b2,flg_b );
					(*tbr[q_grm][qtr]).wrd[flg_b]=0;
					(*tbr[q_grm][qtr]).tip=flg_tip;
					(*tbr[q_grm][qtr]).num=1;
					(*tbr[q_grm][qtr]).mrk=1;
					f2=1;
				}


				/* es un contador de tokens ... + es min 1 max NUM ... * es min 0 max NUM */
				if (!f2 && flg_m )
				{
					(*tbr[q_grm][qtr]).wrd = (char *) malloc (strlen(b2)+1);
					strcpy ( (*tbr[q_grm][qtr]).wrd,b2 );
					(*tbr[q_grm][qtr]).tip=flg_m;
					(*tbr[q_grm][qtr]).num=flg_num;
					(*tbr[q_grm][qtr]).mrk=2;
					f2=1;
				}


				/* es un OR de palabras !! */
				if (!f2 && flg_pp )
				{	if (b2[strlen(b2)-1] != '|')
						strcat(b2,"|");
					for ( m9=0, k=0; k<strlen(b2); k++ )
						if (b2[k] == '|')
							m9++;

					/* rpa ... puntero al primer puntero de la cadena de OR en la matriz */
					(*tbr[q_grm][qtr]).mrk=3;
					rpa = (reglaptr *) &tbr[q_grm][qtr];

					f3 = 1;
					q2 = 0;
					q3 = 0;

					while (f3)
					{
						flg_b = flg_tip = 0;
		
						while (b2[q2] != '|' )
						{
							if (b2[q2] != ' ')
							{
								b3[q3] = b2[q2];
		
								if (flg_b && b2[q2] == 'E')
									flg_tip = 1;
								if (flg_b && b2[q2] == 'P')
									flg_tip = 2;
									
								if (b2[q2] == '/')
									flg_b = q3;
		
								q3++;
							}
							q2++;
						}

						b3[flg_b]=0;
						q3=0;

						/* guardo los datos de la palabra N-sima */

						(**rpa).wrd = (char *) malloc(strlen(b3)+1);
						sprintf ( (**rpa).wrd,"%s",b3);	
						(**rpa).tip=flg_tip;
						(**rpa).num=m9;	
						(**rpa).nx = (reglaptr) NULL;
				

						q2++;
						if (b2[q2] == 0)
						{	f3=0;
						}
						else
						{
							(**rpa).nx = (reglaptr) malloc ( sizeof ( regla ));
							rpa = (reglaptr *) & (*rpa)->nx;
							
						}


					} /* while f3 .. */


					f2=1;
				}




				/* es REF */
				if (!f2)
				{
					(*tbr[q_grm][qtr]).wrd = (char *) malloc (strlen(b2)+1);
					strcpy ( (*tbr[q_grm][qtr]).wrd,b2 );
					(*tbr[q_grm][qtr]).mrk=4;
					f2=1;
				}
					



				p1++;
				if (b1[p1] == 0)
				{	f1=0;
					tbr[q_grm][qtr+1] = (reglaptr) NULL;
				}

				qtr++;
				
			} /* while f1 ... */


			/* nueva regla */
			q_grm++;

		} /* if not linea vacia .. */

	} /* while fgets ... */


	if (gp_verbose)
	{
		printf ("\n");
		printf ("Cantidad de lineas en gramatica     : %6d\n\n",q_grm);
	}

	if (gp_verbose)
	{	
		printf ("\n");
		printf ("Comprobando integridad de matriz de reglas \n\n");
		
		for (i=0; i < q_grm; i++)
		{
			j=0;
			while ( tbr[i][j] != (reglaptr) NULL )
			{
				printf ("Reg: %2d  tk: (%2d)          tip: %3d    num: %3d   mrk:  %d |%s|\n",
					i,j,
					(*tbr[i][j]).tip, (*tbr[i][j]).num,(*tbr[i][j]).mrk,(*tbr[i][j]).wrd   );


				if ( (*tbr[i][j]).nx != (reglaptr) NULL )
				{
					rpa = (reglaptr *) &tbr[i][j];
					m9 = ( **rpa).num;

					for ( k=0; k< m9; k++)
					{	
						if (k)
						{
							printf ("                           tip: %3d    num: %3d   mrk:  %d |%s|\n",
								(**rpa).tip, (**rpa).num, (**rpa).mrk, (**rpa).wrd   );
						}

						rpa = (reglaptr *) & (*rpa)->nx;
					}

				}
							
				j++;

			}  /* while tbr ... */
			
			printf ("\n");
		}

		printf ("\n\n");
	}
	
	return 0;

}







#if 0

/*
 * -----------------------------------------------------------------------------------
 *
 *	bd_load
 *	carga la base de datos del diccionario
 *
 * -----------------------------------------------------------------------------------
 */


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
	int	flag,f1;
	int	i,j,k;
	int	ql;

	/* cantidad de palabras en el diccionario */
	q_wrd = 0;

	/* ptr al contenedor de direccion del ultimo nodo de la cadena */
	npa = (nodeptr *) &np1;

	ql=0;
	while (fgets(b1,MAXB,hfdic) != NULL)
	{
		if (!linea_vacia(b1)  && b1[0] != '#' )
		{
			for ( flag=0, j=strlen(b1); !flag && j; j--)
				if (b1[j] == '\n')
					b1[j]=0,flag=1;

			if (gp_verbose)
			{
				printf ("%3d |%s|\n",ql,b1);
			}

			*npa = (nodeptr ) malloc ( sizeof (node));
			(**npa).wrd = ( char *) malloc(strlen(b1)+1);
			sprintf ( (**npa).wrd,"%s",b1);
			(**npa).num = q_wrd+1;
			(**npa).nx = (nodeptr) NULL;
			npa = (nodeptr *) & (*npa)->nx;

			q_wrd++;
			ql++;
		}
	}

	if (gp_verbose)
	{
		printf ("Cantidad de lineas en diccionario   : %6d\n",ql);
		printf ("Cantidad de palabras en diccionario : %6d\n",q_wrd);
	}


	if (gp_verbose)
	{
		printf ("\n\nComprobando integridad del diccionario : \n\n");
	
		i=0;
		npa = (nodeptr *) &np1;

		while ( (*npa) != (nodeptr) NULL )
		{
			printf ("i: %3d  (**npa).num: %3d  (**npa).wrd: |%s|\n",i,(**npa).num,(**npa).wrd);
			npa = (nodeptr *) & (*npa)->nx;
			i++;
		}
	}
}



#endif









/*
 * -----------------------------------------------------------------------------------
 *
 *	bd_load
 *	carga la base de datos del diccionario
 *
 * -----------------------------------------------------------------------------------
 */


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
	int	flag,f1;
	int	i,j,k;
	int	ql;

	/* cantidad de palabras en el diccionario */
	q_wrd = 0;

	/* ptr al contenedor de direccion del ultimo nodo de la cadena */
	npa = (nodeptr *) &np1;

	ql=0;
	while (fgets(b1,MAXB,hfdic) != NULL)
	{
		if (!linea_vacia(b1)  && b1[0] != '#' )
		{
			for ( flag=0, j=strlen(b1); !flag && j; j--)
				if (b1[j] == '\n')
					b1[j]=0,flag=1;

			if (gp_verbose)
			{
				printf ("%3d |%s|\n",ql,b1);
			}

			*npa = (nodeptr ) malloc ( sizeof (node));
			(**npa).wrd = ( char *) malloc(strlen(b1)+1);
			sprintf ( (**npa).wrd,"%s",b1);
			(**npa).num = q_wrd+1;
			(**npa).nx = (nodeptr) NULL;
			npa = (nodeptr *) & (*npa)->nx;

			q_wrd++;
			ql++;
		}
	}

	if (gp_verbose)
	{
		printf ("Cantidad de lineas en diccionario   : %6d\n",ql);
		printf ("Cantidad de palabras en diccionario : %6d\n",q_wrd);
	}


	if (gp_verbose)
	{
		printf ("\n\nComprobando integridad del diccionario : \n\n");
	
		i=0;
		npa = (nodeptr *) &np1;

		while ( (*npa) != (nodeptr) NULL )
		{
			printf ("i: %3d  (**npa).num: %3d  (**npa).wrd: |%s|\n",i,(**npa).num,(**npa).wrd);
			npa = (nodeptr *) & (*npa)->nx;
			i++;
		}
	}
}







/*
 * -----------------------------------------------------------------------------------
 *
 *	bm_load
 *	carga la base de datos de marcas
 *
 * -----------------------------------------------------------------------------------
 */

int	bm_load()
{
	char	b1[MAXB];
	char	b2[MAXM];
	int	flag;
	int	p1,p2;
	int	i,j,k;
	int	f1;

	f1=0;
	q_bm=0;
	k=0;

	while (fgets(b1,MAXB,hfmrk) != NULL)
	{
		if (gp_verbose)
		{
			printf ("%3d %s",q_bm,b1);
		}

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
	}

	if (gp_verbose)
	{
		printf ("\n");
		printf ("Cant de lineas en tabla de marcas: %d\n",q_bm);

		for (j=0; j<q_bm; j++)
			printf ("%3d,%-10.10s,%s\n",j,bm_mrk[j],bm_key[j]);

		printf ("\n");

	}

}







/*
 * -----------------------------------------------------------------------------------
 *
 * 	bm_tag
 *
 * 	Concatena a string, el tag correspondiente, si coincide la palabra clave
 *
 * -----------------------------------------------------------------------------------
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
 * -----------------------------------------------------------------------------------
 *
 *	gp_parser
 *
 *	parser general de parametros de input al programa
 * 
 *
 * -----------------------------------------------------------------------------------
 */

int	gp_parser()
{

	int i;
	char	prm[MAXV];

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

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"dng",3) )
				{	gp_detneg = 1;
					gp_tipneg = 1;
					strcpy(prm,desde_igual( gp_fp(GP_GET,i,(char **)0)));
					if (strncmp(pasar_a_minusc(prm),"lex",3) == 0)
						gp_tipneg = 1;
					if (strncmp(pasar_a_minusc(prm),"nga",3) == 0)
						gp_tipneg = 2;
				}

				if (!strncmp(gp_fp(GP_GET,i,(char **)0)+1,"grm",3) )
				{	
					strcpy(fgrm,desde_igual( gp_fp(GP_GET,i,(char **)0)));
				}



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

	/* futuro ...
	 * Hacer una gramatica para la combinacion de comandos
	 * posibles !!
	 *
	 * Por ahora, revisamos combinaciones basicas que
	 * son necesarias entre parametros ...
	 *
	 */

	/* si no pone parametros ... mandar a uso */
	if ( gp_q_partype1 + gp_q_partype2 + gp_q_partype3 == 0 )
		gp_help = 1;

	/* si no indico ningun  archivo ... mandar a uso */
	if ( gp_q_partype1 + gp_q_partype3 == 0 )
		gp_help = 1;

	/* si indico gramatica nga, debe indicar archivo de gramatica */
	if ( gp_tipneg == 2 && strlen(fgrm) < 2 )
		gp_help = 1;

} 




/*
 * -----------------------------------------------------------------------------------
 *
 *	tiene_igual
 *
 * -----------------------------------------------------------------------------------
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
 * -----------------------------------------------------------------------------------
 *
 *	desde_igual
 *
 * -----------------------------------------------------------------------------------
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
 * -----------------------------------------------------------------------------------
 *
 *	gp_init
 *
 * -----------------------------------------------------------------------------------
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
 * -----------------------------------------------------------------------------------
 *
 *	gp_fp
 *
 * -----------------------------------------------------------------------------------
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
 * -----------------------------------------------------------------------------------
 *
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
 *
 * -----------------------------------------------------------------------------------
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

/*
 * -----------------------------------------------------------------------------------
 *
 *	gp_test 
 *
 *	
 *
 * -----------------------------------------------------------------------------------
 */

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
 * -----------------------------------------------------------------------------------
 *
 *	tipo_char
 *
 *	ATENCION:
 *	Falta contemplar que hacemos con el caracter '_' !!!!
 *	Por ahora lo incluyo en letras, porque si no, me separa el tag de las palabras
 *	Si voy a parsear corpus sin tag ... lo tengo que sacar de la lista !!!
 *
 *	0	fin de linea
 *	1	blanco o tab
 *	2	otra cosa ( x ahora ... )
 *
 * -----------------------------------------------------------------------------------
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

	if (c == '_' )
		x = TC_GBJ;

	if (c == ',' || c == ';' || c == ':' || c == '-' || c == '/' )
		x = TC_CCE;

	if (c == '.' )
		x = TC_PNT;

	if (c == '(' || c == '[' || c == '{' )
		x = TC_PAA;

	if (c == ')' || c == ']' || c == '}' )
		x = TC_PAC;

#if 0
	if (c >= 'a' && c <= 'z' || c>= 'A' && c <= 'Z' || c == '_')
		x = TC_LET;
#endif
	if (c >= 'a' && c <= 'z' || c>= 'A' && c <= 'Z' )
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
	printf ("inp_file out_file                   inp_file: texto input  out_file: archivo de salida      \n");
	printf ("-inp=inp_file -out=out_file         inp_file: texto input  out_file: archivo de salida      \n");
	printf ("-h                                  help                                                    \n");
	printf ("-v                                  verbose ... muestra cierta informacion de proceso       \n");
	printf ("-m                                  salida a archivo de output en minuscula                 \n");
	printf ("-f                                  salida a archivo de output en formato sentencia         \n");
	printf ("-e                                  fuerza string \"EOL\" al final de cada sentencia        \n");
	printf ("-nvd=N       (no poner -f )         Nivel de descripcion en archivo de salida (solo sin -f) \n");
	printf ("                                    0 Sin descripcion                                       \n");
	printf ("                                    1 Agrega la sentencia y numero de token                 \n");
	printf ("-dng=lex                            detecta negacion ( con lex & yacc )                     \n");
	printf ("                                                                                            \n");
	printf ("-dng=nga     (va con -grm )         detecta negacion ( con gramatica negex ampliada)        \n");
	printf ("-grm=grm_file                       grm_file: archivo con reglas de gramatica nga           \n");
	printf ("\n\n");

	exit(x);
}


/*
 * -----------------------------------------------------------------------------------
 *
 * 	gp_default
 *
 * 	inicializa parametros de funcionamiento default
 *
 * -----------------------------------------------------------------------------------
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
	gp_detneg=0;
	gp_tipneg=0;
}




/*
 * -----------------------------------------------------------------------------------
 *
 * 	pasar_a_minusc
 *
 *
 * -----------------------------------------------------------------------------------
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
 * -----------------------------------------------------------------------------------
 *
 *	ATENCION !!
 *
 *	Aqui van las rutinas de soporte a las actividades de lex & yacc
 *
 *
 * -----------------------------------------------------------------------------------
 *
 */






/*
 * -----------------------------------------------------------------------------------
 *
 * 	readInputForLexer
 *
 *	ajusta funcionamiento del lexer para poder ler de strings
 *
 *
 * -----------------------------------------------------------------------------------
 */



int readInputForLexer( char *buffer, int *numBytesRead, int maxBytesToRead ) 
{
    int numBytesToRead = maxBytesToRead;
    int bytesRemaining = strlen(globalInputText)-globalReadOffset;

    int i;


    if ( numBytesToRead > bytesRemaining ) 
    {    
        numBytesToRead = bytesRemaining; 
    }

    for ( i = 0; i < numBytesToRead; i++ ) 
    {
        buffer[i] = globalInputText[globalReadOffset+i];
    }

    *numBytesRead = numBytesToRead;
    globalReadOffset += numBytesToRead;
    return 0;
}


/*
 * -----------------------------------------------------------------------------------
 *
 * 	gramatik
 *
 *	algunas funciones de apoyo al parser
 *
 *
 * -----------------------------------------------------------------------------------
 */

int	gramatik(s,reduccion_neg)
char	*s;
int	reduccion_neg;
{
	int	f1;

	f1=0;

	if ( strcmp(s,global_ref) == 0)
	{	
		f_pe_lin = 1;
		f1=1;
	}


	if (f_cb)
	{
		if (reduccion_neg == 1)
		{
			if (f_pe_lin)
			{
				f_pe_ora = 1;
				f_cb = 0;
			}
		}
	}

	return f1;
}





/* -------------- end of source --------------------- */
/* -------------- end of source --------------------- */
/* -------------- end of source --------------------- */
/* -------------- end of source --------------------- */

#if 0


#	
#	f05_t1.mrk
#	2017-05-28
#
#	archivo de marcas para el tagger
#
#	Se genera un archivo con las palabras que son "importantes" en la gramatica 
#	que se quiere construir.
#	
#	Para cada una de esas palabras, se genera una entrada en la tabla, con el
#	correspondiente codigo (tag) con las que la marcara el tagger.
#
#	version 1
#	usaba freeling
#
#	version 2
#	ya no hace falta freeling, pero se respeta el formato con los campos
#	que genera freeling, para compatibilidad con los programas que utilizan la tabla
#	
#
#	se genera corriendo:
#
#	./tes_cpar02 f01_neg_fra f02_neg_fra_tkn -m -v 
#	./tes_cpar02 f01_aff_fra f02_aff_fra_tkn -m -v 
#	./tes_cpar02 f01_pos_fra f02_pos_fra_tkn -m -v 
#
#	(obsoleto en version 2 )
#	luego se procesa con freeling
#
#	/usr/local/bin/analyze -f local.cfg <f04_neg_sen.txt >f04_neg_sen_fre.txt
#	/usr/local/bin/analyze -f local.cfg <f04_aff_sen.txt >f04_aff_sen_fre.txt
#	/usr/local/bin/analyze -f local.cfg <f04_pos_sen.txt >f04_pos_sen_fre.txt
#
#	se concatenan los resultados para unificar y hacer una tabla general
#
#	cat f02_*tkn | sort -u > f03_tkn
#
#	se rescata lo necesario para las marcas que se quieran ...
#
#	
#	cat f03_tkn | grep -w ^no        > f04_t1
#	cat f03_tkn | grep -w ^se       >> f04_t1
#	cat f03_tkn | grep -w ^ni       >> f04_t1
#	cat f03_tkn | grep    ^sin      >> f04_t1
#	cat f03_tkn | grep    ^pero     >> f04_t1
#	cat f03_tkn | grep     observ   >> f04_t1
#	cat f03_tkn | grep     detect   >> f04_t1
#	cat f03_tkn | grep     eviden   >> f04_t1
#	cat f03_tkn | grep     visualiz >> f04_t1
#	cat f03_tkn | grep     identif  >> f04_t1
#	cat f03_tkn | grep '^\.'        >> f04_t1
#	cat f03_tkn | grep '^,'         >> f04_t1
#	cat f03_tkn | grep '^;'         >> f04_t1
#
#
#	finalmente modificar para ajustar al formato anteriormente
#	usado despues de freeling
#
#	./tes_gmarca01 f04_t1 f04_t2 -v -f  > z3.log
#	cat f04_t1.bse f04_t2 > f05_t1.mrk
#
#	La tabla de marcas tiene este formato:
#
#
# no no RN 0.999297 NEG
# ni ni CC 0.834853 CNEG
# pero pero CC 0.999902 ENDNEG
# sin sin SP 1 TNEG
# se ser VMI000 1 VSE
# , , PNT000 1 COM
# . . PNT000 1 PTO
# observar observar VMIP3S0 0.989241 OBS
# observa observar VMIP3S0 0.989241 OBS
# detectar detectar VMP00SM 1 DET
# detectado detectar VMP00SM 1 DET
# detectaron detectar VMIS3P0 1 DET
# evidenciar evidencia NCFS000 0.614458 EVI
# evidencia evidenciar VMIP3S0 0.373494 EVI
# evidencias evidencia NCFP000 0.989726 EVI
# visualizar visualizar VMIP3S0 0.989236 VIS
# visualizo visualizar VMIP1S0 1 VIS
# identificar identificar VMN0000 1 IDE
# identifican identificar VMIP3P0 1 IDE
# 
#
#	El primer token es lo que buscar el tagger,
#	Y le agrega el ultimo token ...
#	Asi,  en la frase " ... no se han detectado formas ... "
#	queda:
#
#	"... no_NEG se_VSE han detectado_DET formas ... "
#
#  end of comments...
#	




no no RN 0.999297 NEG 

se ser VMI000 1.0 VSE 

ni ni CC 0.834853 CNEG 

sin sin SP 1.0 TNEG 

pero pero CC 0.999902 ENDNEG 

observa observar VMP 0.999909 OBS 
observada observar VMP 0.999909 OBS 
observadas observar VMP 0.999909 OBS 
observadose observar VMP 0.999909 OBS 
observan observar VMP 0.999909 OBS 
observando observar VMP 0.999909 OBS 
observandose observar VMP 0.999909 OBS 
observandosse observar VMP 0.999909 OBS 
observaron observar VMP 0.999909 OBS 
observo observar VMP 0.999909 OBS 

detectado detectar VMP 0.999909 DET 
detectaron detectar VMP 0.999909 DET 
detecto detectar VMP 0.999909 DET 

evidencia evidenciar NCF 0.850123 EVI 
evidencias evidenciar NCF 0.850123 EVI 

visualiza visualizar VMI 0.991255 VIS 
visualizacin visualizar VMI 0.991255 VIS 
visualizacion visualizar VMI 0.991255 VIS 
visualizan visualizar VMI 0.991255 VIS 
visualizando visualizar VMI 0.991255 VIS 
visualizar visualizar VMI 0.991255 VIS 
visualizarse visualizar VMI 0.991255 VIS 
visualizo visualizar VMI 0.991255 VIS 

identifica identificar VMN 0.999999 IDE 
identifican identificar VMN 0.999999 IDE 
identificandose identificar VMN 0.999999 IDE 
identificar identificar VMN 0.999999 IDE 

. . PNT000 1.0 PTO 

, , PNT000 1.0 COM 

; ; PNT000 1.0 PTC 


#endif


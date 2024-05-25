#!/bin/bash

#	copiar.sh
#
#	copia, de los archivos generados en la presente seccion, 
#	los necesarios al directorio de trabajo para la siguiente seccion.
#


echo "copiar.sh"
echo " "

echo "Copio los archivos necesarios para el siguiente paso ... "
echo " " 


# Ultima version del corpus, en formato
# nro reg   T1   aff / neg / pos   T2  palabra ref   T3   frase    \n
#

cp f01                ../21_Tagger1


# Archivo de marcas, con palabras "importantes" para el tagger
#

cp f05_t1.mrk         ../21_Tagger1




echo " "
echo "Archivos para armar el diccionario ... "
echo " "

cp f03_tkn ../15_Diccio1



echo " "
echo "Archivos en directorio para etapa siguiente ... "
echo " "

ls -la ../15_Diccio1
ls -la ../21_Tagger1




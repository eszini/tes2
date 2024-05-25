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

cp f01                ../11_Parser1

echo "Copiando archivos separados en aff neg y pos ... "

cp f01*lin            ../11_Parser1

echo "Copiando archivos con frases, separados en aff neg y pos ... "

cp f01*fra            ../11_Parser1


echo " "
echo "Archivos en directorio para etapa siguiente ... "
echo " "
ls -la ../11_Parser1

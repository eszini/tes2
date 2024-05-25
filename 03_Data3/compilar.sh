#!/bin/bash

#	compilar.sh
#
#	compilar los programas



echo "compilar.sh"
echo " "


echo "Compilo programa para agregar marcas TAB en lineas de corpus  ... tes_agrega01.c ... " 

cc -o tes_agrega01 tes_agrega01.c

echo "Compilo programa para reordenar campos en corpus              ... tes_filtro01.c ... " 

cc -o tes_filtro01 tes_filtro01.c

echo "Compilo programa para reordenar campos en corpus              ... tes_filtro02.c ... " 

cc -o tes_filtro02 tes_filtro02.c


cp tes_agrega01.exe ../bin
cp tes_filtro01.exe ../bin
cp tes_filtro02.exe ../bin





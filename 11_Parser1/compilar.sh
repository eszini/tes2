#!/bin/bash

#	compilar.sh
#
#	compilar los programas



echo "compilar.sh"
echo " "

echo "Compilo programa para armar base de marcas para taggear corpus .. tes_cpar02.c ... " 
echo " "

cc -o tes_cpar02 tes_cpar02.c

echo "Compilo programa para armar tabla de marcas para lexer         .. tes_cpar02.c ... " 
echo " "

cc -o tes_gmarca01 tes_gmarca01.c




cp tes_cpar02.exe   ../bin
cp tes_gmarca01.exe ../bin


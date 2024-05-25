#!/bin/bash

#	verificar1.sh
#
#	Algunas verificaciones basicas al archivo de corpus ...
#


#	
#	Controlamos que invoco correctamente el script
#

if [ $# -ne 3 ]; then
    echo "Uso:  $0  <archivo input (corpus)>  <archivo output-1 (faltan tabs)>  <archivo output-2 (falta \"Affirmed o Negated o Possibility\") "
    echo " 2da linea va "
    exit 1
fi

#
#	Fecha de hoy ...
#

RIGHT_NOW=$(date +"%r %x %Z")

echo $RIGHT_NOW


#
#	Verificamos que todas las lineas tengan alguna de las frases ( Affirmed , Negated , Possibility )
#

cp $1 corpus

cat corpus | grep "Affirmed"    > f01_aff
cat corpus | grep "Negated"     > f01_neg
cat corpus | grep "Possibility" > f01_pos


TRC=$(wc -l corpus)
TRA=$(wc -l f01_aff)
TRN=$(wc -l f01_neg)
TRP=$(wc -l f01_pos)

echo "Total de registros en $1 " 
echo $TRC

echo "Total de registros \"Affirmed\"    en $1 " 
echo $TRA

echo "Total de registros \"Negated\"     en $1 " 
echo $TRN

echo "Total de registros \"Possibility\" en $1 " 
echo $TRP


TRC2=$(TRA+TRN+TRP)
TRC2=$(echo $TRA + $TRN + $TRP | bc)

echo "Cantidades ... "
echo $TRC
echo $TRC2


if [ $TRC -ne $TRC2 ]; then
	echo "Hay registros que no tienen alguna de las marcas \"Affirmed,Negated,Possibility\" ..."
	echo "Lineas en archivo ... $3 "
	cat corpus | egrep -v "Affirmed|Negated|Possibility" > $3
	exit 1
fi




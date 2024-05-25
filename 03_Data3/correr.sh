#!/bin/bash

#	correr.sh
#
#	corre procesos en esta etapa



echo "correr.sh"
echo " "



echo "Agrego marcas de TAB en lineas del corpus "
echo " "


./tes_agrega01   CorpusV3.txt CorpusV4.txt -v  > f01.log


cat CorpusV4.txt | grep TAB1 | wc -l  > t1
cat CorpusV4.txt | grep TAB2 | wc -l >> t1
cat CorpusV4.txt | grep TAB3 | wc -l >> t1

cat t1



echo "Genero archivo con campos de corpus reordenado "
echo " "

./tes_filtro01 CorpusV4.txt f01 -v > f02.log


echo "Separo el corpus en Aff Neg y Pos para futura necesidad .... "

cat f01 | grep " Aff " > f01_aff_lin
cat f01 | grep " Neg " > f01_neg_lin
cat f01 | grep " Pos " > f01_pos_lin

echo "Cantidad de Aff, Neg, Pos ... "

wc -l f01_*lin


echo "Genero archivo conteniendo solo las frases del corpus ... "
echo " "

cat CorpusV4.txt | grep "Affirmed"    > x1
cat CorpusV4.txt | grep "Negated"     > x2
cat CorpusV4.txt | grep "Possibility" > x3


./tes_filtro02 x1  f01_aff_fra  -v > f03a.log
./tes_filtro02 x2  f01_neg_fra  -v > f03b.log
./tes_filtro02 x3  f01_pos_fra  -v > f03c.log


wc -l f01_*fra





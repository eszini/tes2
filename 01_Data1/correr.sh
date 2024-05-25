#!/bin/bash

#	correr.sh
#
#	corre procesos en esta etapa



echo "correr.sh"
echo " "



echo "Verifico que el corpus original cumpla con: "
echo "a) En todas las lineas tiene que estar Affirmed , Negated o Possibility "
echo "b) Todas las lineas tiene que tener 3 tabs para separar campos "
echo " "

cp Anotacion2vm.txt f01


./tes_verifica01  f01  f01_err -v   > f01.log


echo "Lineas sin Affirmed, Negated o Possibility ... "
echo " "

cat f01.log | grep ^SIN 


echo "Lineas con cantidad incorrecta de TAB ... "
echo " "

cat f01.log | grep ^TAB 



tail -6 f01.log









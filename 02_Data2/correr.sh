#!/bin/bash

#	correr.sh
#
#	corre procesos en esta etapa



echo "correr.sh"
echo " "



echo "Emprolijo lineas del corpus "
echo "Quito caracteres excedentes en los fin de linea ... si son tab, blancos o control M (de windows) "
echo " "



./tes_emprolija01   CorpusV2.txt CorpusV3.txt -v  > f01.log



tail -5 f01.log

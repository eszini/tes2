#!/bin/bash

#	correr.sh
#
#	corre procesos en esta etapa



echo "correr.sh"
echo " "

echo "Aplico lexer/gramatica de prueba para ver si pasan bien todas las lineas ... "
echo "Uso de input para prueba: "
echo "Archivo a.txt ... "
echo "- copiar del f01 (todo el corpus ) "
echo "- o editar a mano y colocar las lineas que se quieran probar "
echo " "


./tes_tparser01 a.txt > z1

tail -10 z1


echo " "
echo "Verificamos que no haya errores en la combinacon lexer - gramatica elegida "
echo " "

cat z1 | grep error | wc -l

echo " "
echo "Tiene que dar cero ... "
echo " "



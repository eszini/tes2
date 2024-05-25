






./tes_parser03  -inp=f01_aff -out=f02_aff -v -m -f > zaff
./tes_parser03  -inp=f01_neg -out=f02_neg -v -m -f > zneg


echo "aff con 0 "
grep Clasifi zaff  | grep 0 | wc -l
echo "aff con 1 "
grep Clasifi zaff  | grep 1 | wc -l
echo "neg con 0 "
grep Clasifi zneg  | grep 0 | wc -l
echo "neg con 1"
grep Clasifi zneg  | grep 1 | wc -l


echo " "
echo " "
echo " "
echo " "

echo "Registros con \"Aff\" ... "
tail -6 zaff


echo "Registros con \"Neg\" ... "
tail -6 zneg



echo "Prueba con diccionario ... "
echo " "

./tes_parser03  -inp=f01_neg  -out=f02_neg  -dic=diccio01.txt -v -m -f > z2.log




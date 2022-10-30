# This Script is for easy and fast compile and run c and cpp file
COLOR_GREEN='\033[0;36m'
COLOR_REST='\033[0m';
if [ $# -eq 0 ] ;
then
	read -p "Enter the C/C++ File That You Want To Run : " filename
  cp $filename NeVerUsENaMe.cpp
else
			cp $1 NeVerUsENaMe.cpp
fi
g++ NeVerUsENaMe.cpp -o a
./a
rm a NeVerUsENaMe.cpp
echo   "✔Program is already done!"

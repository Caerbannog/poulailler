LIB=libraries/Sunrise

all:
	g++ -I ${LIB} sunrise.cpp ${LIB}/Sunrise.cpp -o sunrise


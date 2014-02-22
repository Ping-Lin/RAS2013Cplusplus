Test:Test.o
    g++ -o Test Test.o

Test.o:Test.cpp Constants.h
	g++ -c Test.cpp
clean:
    rm *.o

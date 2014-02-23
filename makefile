    test: Test.o
	g++ -o test Test.o
    Test.o: Test.cpp
	g++ -c Test.cpp
    clean: 
	rm *.o test

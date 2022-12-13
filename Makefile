sc.out:	smartCompile.cpp
	clang++ -pedantic -Wall -lboost_regex smartCompile.cpp -o sc.out

pclean:
	rm *.out
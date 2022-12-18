sc.out:	smartCompile.cpp
	clang++ -pedantic -Wall -lboost_regex smartCompile.cpp -o sc.out

install: sc.out
	sudo cp sc.out /bin/sc

pclean:
	rm bin/*.out .build/*
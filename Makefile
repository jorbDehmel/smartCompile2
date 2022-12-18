sc.out:	smartCompile.cpp /usr/include/boost
	clang++ -pedantic -Wall -lboost_regex smartCompile.cpp -o sc.out

install: sc.out
	sudo cp sc.out /bin/sc

/usr/include/boost:
	sudo apt-get install libboost-all-dev -y

pclean:
	rm bin/*.out .build/*
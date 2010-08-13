all: cpposxom

cpposxom: cpposxom.cpp
	g++ -I/usr/include -lboost_filesystem cpposxom.cpp -o cpposxom

clean:
	rm cpposxom


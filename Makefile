all: cpposxom.cgi

cpposxom.cgi: cpposxom.cpp
	g++ -I/usr/include -lboost_filesystem cpposxom.cpp -o cpposxom.cgi

clean:
	rm cpposxom.cgi


program: main.cpp proportional.o integral.o differential.o functions.o
	g++ -o $@ main.cpp proportional.o integral.o differential.o functions.o

proportional.o: proportional.cpp
	g++ -o $@ -c proportional.cpp

integral.o: integral.cpp
	g++ -o $@ -c integral.cpp

differential.o: differential.cpp
	g++ -o $@ -c differential.cpp

functions.o: functions.cpp
	g++ -o $@ -c functions.cpp

clean:
	rm -rf *.o
	rm -rf program

cleandata:
	rm -rf *.csv

cleanall: clean cleandata
open: image
	open image.ppm

image: main
	./main > image.ppm

main: main.cc
	/usr/bin/g++ -std=c++11 -O2 -o main main.cc

clean:
	rm -f core \#* *.o image.ppm main
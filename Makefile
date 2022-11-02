open: image
	open image.ppm

image: main
	./main > image.ppm

main: main.cc
	g++ -std=c++11 -o main main.cc

clean:
	rm main
	rm *.o
	rm image.ppm
LIBS=-lGL -lglfw -lGLEW
HEADERS=constants.h  lodepng.h  myCube.h  myTeapot.h  shaderprogram.h
FILES=lodepng.cpp  main_file.cpp  shaderprogram.cpp

main_file: $(FILES) $(HEADERS)
	g++ -g -o main_file -std=c++11 $(FILES)  $(LIBS) -I.

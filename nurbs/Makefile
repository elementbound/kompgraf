build: 
	g++ -c -std=c++11 -I../ ../frame/window.cpp -o bin/window.o
	g++ -c -std=c++11 -I../ ../frame/resizable_window.cpp -o bin/resizable_window.o
	g++ -c -std=c++11 -I../ nurbs.cpp -o bin/nurbs.o
	g++ bin/*.o -lglfw3 -lgdi32 -lopengl32 -lglew32 -o nurbs
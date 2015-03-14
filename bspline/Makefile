build: 
	g++ -c -std=c++11 -I../ ../frame/window.cpp -o bin/window.o
	g++ -c -std=c++11 -I../ bspline.cpp -o bin/bspline.o
	g++ bin/*.o -lglfw3 -lgdi32 -lopengl32 -lglew32 -o bspline
	copy /Y ..\glew32.dll .\glew32.dll
	
weights: 
	g++ -std=c++11 weights.cpp -o weights
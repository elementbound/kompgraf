build: 
	g++ -c -std=c++11 -I../ ../frame/window.cpp -o bin/window.o
	g++ -c -std=c++11 -I../ ../frame/shader.cpp -o bin/shader.o
	g++ -c -std=c++11 -I../ ../frame/mesh/basic_mesh.cpp -o bin/basic_mesh.o
	g++ -c -std=c++11 -I../ ../frame/mesh/separated_mesh.cpp -o bin/separated_mesh.o
	g++ -c -std=c++11 -I../ meshdev.cpp -o bin/meshdev.o
	g++ bin/*.o -lglfw3 -lgdi32 -lopengl32 -lglew32 -o meshdev
	copy /Y ..\glew32.dll .\glew32.dll
	
debug: 
	g++ -ggdb -c -std=c++11 -I../ ../frame/window.cpp -o bin/window.o
	g++ -ggdb -c -std=c++11 -I../ ../frame/shader.cpp -o bin/shader.o
	g++ -ggdb -c -std=c++11 -I../ ../frame/basic_mesh.cpp -o bin/basic_mesh.o
	g++ -ggdb -c -std=c++11 -I../ ../frame/separated_mesh.cpp -o bin/separated_mesh.o
	g++ -ggdb -c -std=c++11 -I../ meshdev.cpp -o bin/meshdev.o
	g++ -ggdb bin/*.o -lglfw3 -lgdi32 -lopengl32 -lglew32 -o meshdev
	copy /Y ..\glew32.dll .\glew32.dll
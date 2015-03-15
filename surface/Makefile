build:
	g++ -c -std=c++11 -I../ ../frame/util.cpp -o bin/util.o
	g++ -c -std=c++11 -I../ ../frame/window.cpp -o bin/window.o
	g++ -c -std=c++11 -I../ ../frame/shader.cpp -o bin/shader.o
	g++ -c -std=c++11 -I../ ../frame/mesh/basic_mesh.cpp -o bin/basic_mesh.o
	g++ -c -std=c++11 -I../ ../frame/mesh/separated_mesh.cpp -o bin/separate_mesh.o
	g++ -c -std=c++11 -I../ draw_circles.cpp -o bin/draw_circles.o
	g++ -c -std=c++11 -I../ control_surface.cpp -o bin/control_surface.o
	g++ -c -std=c++11 -I../ editable_control_surface.cpp -o bin/editable_control_surface.o
	g++ -c -std=c++11 -I../ spline_surface.cpp -o bin/spline_surface.o
	g++ -c -std=c++11 -I../ surface.cpp -o bin/surface.o
	g++ bin/*.o -lglfw3 -lgdi32 -lopengl32 -lglew32 -o surface
	copy /Y ..\glew32.dll .\glew32.dll
	
debug:
	g++ -ggdb -c -std=c++11 -I../ ../frame/util.cpp -o bin/util.o
	g++ -ggdb -c -std=c++11 -I../ ../frame/window.cpp -o bin/window.o
	g++ -ggdb -c -std=c++11 -I../ ../frame/shader.cpp -o bin/shader.o
	g++ -ggdb -c -std=c++11 -I../ ../frame/mesh/basic_mesh.cpp -o bin/basic_mesh.o
	g++ -ggdb -c -std=c++11 -I../ ../frame/mesh/separated_mesh.cpp -o bin/separate_mesh.o
	g++ -ggdb -c -std=c++11 -I../ draw_circles.cpp -o bin/draw_circles.o
	g++ -ggdb -c -std=c++11 -I../ control_surface.cpp -o bin/control_surface.o
	g++ -ggdb -c -std=c++11 -I../ editable_control_surface.cpp -o bin/editable_control_surface.o
	g++ -ggdb -c -std=c++11 -I../ spline_surface.cpp -o bin/spline_surface.o
	g++ -ggdb -c -std=c++11 -I../ surface.cpp -o bin/surface.o
	g++ -ggdb bin/*.o -lglfw3 -lgdi32 -lopengl32 -lglew32 -o surface
	copy /Y ..\glew32.dll .\glew32.dll
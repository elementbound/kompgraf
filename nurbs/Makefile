build: 
	g++ -c -std=c++11 -I../ ../glwrap/util.cpp -o bin/util.o
	g++ -c -std=c++11 -I../ ../glwrap/window.cpp -o bin/window.o
	g++ -c -std=c++11 -I../ ../glwrap/resizable_window.cpp -o bin/resizable_window.o
	g++ -c -std=c++11 -I../ ../glwrap/mesh/basic_mesh.cpp -o bin/basic_mesh.o
	g++ -c -std=c++11 -I../ ../glwrap/mesh/separated_mesh.cpp -o bin/separated_mesh.o
	g++ -c -std=c++11 -I../ ../glwrap/shader.cpp -o bin/shader.o
	g++ -c -std=c++11 -I../ control_poly.cpp -o bin/control_poly.o
	g++ -c -std=c++11 -I../ editable_control_poly.cpp -o bin/editable_control_poly.o
	g++ -c -std=c++11 -I../ spline_poly.cpp -o bin/spline_poly.o
	g++ -c -std=c++11 -I../ nurbs.cpp -o bin/nurbs.o
	g++ bin/*.o -lglfw3 -lgdi32 -lopengl32 -lglew32 -o nurbs
	copy /Y ..\glew32.dll .\glew32.dll
CXX_FLAGS = -std=c++11 -I../
GLWRAP_BINS = bin/window.o bin/resizable_window.o bin/basic_mesh.o bin/separated_mesh.o bin/shader.o bin/util.o
APP_BINS = bin/polygon.o bin/editable_polygon.o
LINKER_SETTINGS = -lglfw3 -lgdi32 -lopengl32 -lglew32 -lpng -lz

build: glwrap
	g++ ${CXX_FLAGS} -c polygon.cpp -o bin/polygon.o
	g++ ${CXX_FLAGS} -c editable_polygon.cpp -o bin/editable_polygon.o
	g++ ${CXX_FLAGS} -c subdiv_window.cpp -o bin/subdiv_window.o
	g++ ${CXX_FLAGS} -c main.cpp -o bin/main.o
	g++ ${GLWRAP_BINS} ${APP_BINS} bin/subdiv_window.o bin/main.o ${LINKER_SETTINGS} -o subdiv
	copy /Y ..\glew32.dll .\glew32.dll
	
glwrap: 
	g++ ${CXX_FLAGS} -c ../glwrap/util.cpp -o bin/util.o
	g++ ${CXX_FLAGS} -c ../glwrap/window.cpp -o bin/window.o
	g++ ${CXX_FLAGS} -c ../glwrap/resizable_window.cpp -o bin/resizable_window.o
	g++ ${CXX_FLAGS} -c ../glwrap/mesh/basic_mesh.cpp -o bin/basic_mesh.o
	g++ ${CXX_FLAGS} -c ../glwrap/mesh/separated_mesh.cpp -o bin/separated_mesh.o
	g++ ${CXX_FLAGS} -c ../glwrap/shader.cpp -o bin/shader.o
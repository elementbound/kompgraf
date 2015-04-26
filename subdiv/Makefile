CXX_FLAGS = -std=c++11 -I../
GLWRAP_BINS = bin/window.o bin/resizable_window.o bin/basic_mesh.o bin/separated_mesh.o bin/shader.o bin/util.o
APP_BINS = bin/model.o bin/subdiv_operator.o
LINKER_SETTINGS = -lglfw3 -lgdi32 -lopengl32 -lglew32 -lcomdlg32

build: 
	g++ ${CXX_FLAGS} -c model.cpp -o bin/model.o
	g++ ${CXX_FLAGS} -c subdiv_operator.cpp -o bin/subdiv_operator.o

	g++ ${CXX_FLAGS} -c app.cpp -o bin/app.o
	g++ ${CXX_FLAGS} -c main.cpp -o bin/main.o

	g++ ${GLWRAP_BINS} ${APP_BINS} bin/app.o bin/main.o ${LINKER_SETTINGS} -o subdiv
	copy /Y ..\glew32.dll .\glew32.dll

glwrap: 
	g++ ${CXX_FLAGS} -c ../glwrap/util.cpp -o bin/util.o
	g++ ${CXX_FLAGS} -c ../glwrap/window.cpp -o bin/window.o
	g++ ${CXX_FLAGS} -c ../glwrap/resizable_window.cpp -o bin/resizable_window.o
	g++ ${CXX_FLAGS} -c ../glwrap/mesh/basic_mesh.cpp -o bin/basic_mesh.o
	g++ ${CXX_FLAGS} -c ../glwrap/mesh/separated_mesh.cpp -o bin/separated_mesh.o
	g++ ${CXX_FLAGS} -c ../glwrap/shader.cpp -o bin/shader.o
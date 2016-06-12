# #PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
# CXXFILES = ../glsl/shader.cc ../glsl/controls.cc obj_loader.cc main.cc
# CXXFLAGS = -c -Wall -O3
# 
# INCLUDES = -I../glsl -I ../glsl/soil/soil/src/ `pkg-config --cflags assimp`
# LIBS = `pkg-config --libs --cflags glfw3 glew` \
# 	`pkg-config --static --libs assimp` \
#  	../glsl/soil/soil/lib/libSOIL.a \
# 	-framework OpenGL -framework CoreFoundation
# 
# OBJECTS=$(CXXFILES:.cc=.o)
# EXECUTABLE=main
# all: $(CXXFILES) $(EXECUTABLE)
# 
# $(EXECUTABLE): $(OBJECTS)
# 	$(CXX) $(INCLUDES) $(OBJECTS) -o $@ $(LIBS)
# 
# .cc.o:
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@
# 
# clean:
# 	rm -f *.o $(EXECUTABLE)

CXXFLAGS=-Wall
LDFLAGS=`pkg-config --static --libs --cflags glfw3 glew assimp` -I ../glsl/soil/soil/src/ ../glsl/soil/soil/lib/libSOIL.a -I ../glsl/ ../glsl/shader.cc ../glsl/texture.cc ../glsl/controls.cc obj_loader.cc -framework OpenGL -framework CoreFoundation
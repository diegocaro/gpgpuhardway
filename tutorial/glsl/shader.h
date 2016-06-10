#ifndef SHADER_HPP
  #define SHADER_HPP
//GLFW and GLEW libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

  GLuint LoadShaders(const char * vertex_file_path,const char *fragment_file_path);
  #endif
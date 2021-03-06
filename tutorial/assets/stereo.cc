// page 98

#define GLM_FORCE_RADIANS
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "obj_loader.h"
   
using namespace glm;
#include "shader.h"
#include "texture.h"
#include "controls.h"
#include "common.h"

GLFWwindow* window;

const int WINDOWS_WIDTH = 1280;
const int WINDOWS_HEIGHT = 720;
float aspect_ratio = 3.0f/2.0f;
float z_offset = 2.0f;
float rotateY = 0.0f;
float rotateX = 0.0f;

float IOD = 2.0f;
float depthZ = 2.0f;
//Our vertices
bool stereo = true;

/*
//UV map for the vertices
static const GLfloat g_uv_buffer_data[] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};
*/




bool drawPoints = true;
bool drawLines = true;
bool drawTriangles = true;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
        break;
        case GLFW_KEY_SPACE:
            rotateX=0;
            rotateY=0;
        break;
        case GLFW_KEY_Z:
            rotateX+=0.01;
        break;
        case GLFW_KEY_X:
            rotateX-=0.01;
        break;
        case GLFW_KEY_A:
            rotateY+=0.01;
        break;
        case GLFW_KEY_S:
            rotateY-=0.01;
        break;
        default:
        
        break;
    } 
}

int main(int argc, char **argv) {
    //Initialize the GLFW
    if(!glfwInit()){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit(EXIT_FAILURE);
    }
    //enable anti-alising 4x with GLFW
    glfwWindowHint(GLFW_SAMPLES, 4);
    //specify the client API version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    //make the GLFW forward compatible
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //enable the OpenGL core profile for GLFW
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
       
    //create a GLFW windows object
    window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT,
            "Chapter 6 - 3D Models", NULL, NULL);
    if(!window){
        fprintf( stderr, "Failed to open GLFW window. If you " \
            "have an Intel GPU, they are not 3.3 compatible. Try " \
                "the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    /* make the context of the specified window current for
    the calling thread */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Final to Initialize GLEW\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    //keyboard input callback
    glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
    glfwSetKeyCallback(window, key_callback);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                       
    GLuint program_id = LoadShaders( "pointcloud.vert", "pointcloud.frag" );
    
    int width=100;
    int height=100;
   
    ObjLoader *obj_loader = new ObjLoader();
       int result = 0;
       if(argc > 1){
         result = obj_loader->loadAsset(argv[1]);
       }
       else{
         result = obj_loader-> loadAsset("dragon.obj");
       }
       if(result){
         fprintf(stderr, "Final to Load the 3D file\n");
         glfwTerminate();
         exit(EXIT_FAILURE);
    }
    
    GLfloat *g_vertex_buffer_data = (GLfloat*)
      malloc (obj_loader->getNumVertices()*sizeof(GLfloat));
      //load the scene data to the vertex buffer
      obj_loader->loadVertices(g_vertex_buffer_data);
       

    //get the location for our "MVP" uniform variable
    GLuint matrix_id = glGetUniformLocation(program_id, "MVP");

    //get a handler for our "myTextureSampler" uniform
    GLuint texture_sampler_id = glGetUniformLocation(program_id, "textureSampler");

    //attribute ID for the variables
    GLint attribute_vertex, attribute_uv;
    attribute_vertex = glGetAttribLocation(program_id, "vertexPosition_modelspace");
    attribute_uv = glGetAttribLocation(program_id, "vertexUV");
       
    //vertex array
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    //initialize the vertex buffer memory.
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        obj_loader->getNumVertices()*sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);
                
    //initialize the UV buffer memory
//    GLuint uv_buffer;
//    glGenBuffers(1, &uv_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, 
//        GL_STATIC_DRAW);

    glUseProgram(program_id);
    
    //binds our texture in Texture Unit 0
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture_id);
//    glUniform1i(texture_sampler_id, 0);
    
    //1st attribute buffer: vertices for position
    glEnableVertexAttribArray(attribute_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(attribute_vertex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                         
/*    //2nd attribute buffer: UVs mapping
    glEnableVertexAttribArray(attribute_uv);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
*/
    //main loop
    while(!glfwWindowShouldClose(window)){
        //clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


        glfwGetFramebufferSize(window, &width, &height);
       
        if(stereo){
        //draw the LEFT eye, left half of the screen
        glViewport(0, 0, width/2, height);
        //computes the MVP matrix from the IOD and virtual image plane distance computeStereoViewProjectionMatrices(g_window, IOD, depthZ, true); //gets the View and Model Matrix and apply to the rendering glm::mat4 projection_matrix = getProjectionMatrix();
        glm::mat4 projection_matrix = getProjectionMatrix();
        glm::mat4 view_matrix = getViewMatrix();
        glm::mat4 model_matrix = glm::mat4(1.0);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f,
               0.0f, -depthZ));
             model_matrix = glm::rotate(model_matrix, glm::pi<float>() *
               rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
             model_matrix = glm::rotate(model_matrix, glm::pi<float>() *
               rotateX, glm::vec3(1.0f, 0.0f, 0.0f));
             glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
             //sends our transformation to the currently bound shader,
             //in the "MVP" uniform variable
             glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
             //render scene, with different drawing modes
             if(drawTriangles)
             obj_loader->draw(GL_TRIANGLES);
             if(drawPoints)
               obj_loader->draw(GL_POINTS);
             if(drawLines)
               obj_loader->draw(GL_LINES);
             //Draw the RIGHT eye, right half of the screen
             glViewport(width/2, 0, width/2, height);
             computeStereoViewProjectionMatrices(window, IOD, depthZ,
               false);
             projection_matrix = getProjectionMatrix();
             view_matrix = getViewMatrix();
             model_matrix = glm::mat4(1.0);
             model_matrix = glm::translate(model_matrix, glm::vec3(0.0f,
               0.0f, -depthZ));
             model_matrix = glm::rotate(model_matrix, glm::pi<float>() *
               rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
             model_matrix = glm::rotate(model_matrix, glm::pi<float>() *
                    rotateX, glm::vec3(1.0f, 0.0f, 0.0f));
                  mvp = projection_matrix * view_matrix * model_matrix;
                  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
                  if(drawTriangles)
                    obj_loader->draw(GL_TRIANGLES);
                  if(drawPoints)
                    obj_loader->draw(GL_POINTS);
                  if(drawLines)
                    obj_loader->draw(GL_LINES);
                }
                else {
                 //draw the left eye (but full screen)
                    glViewport(0, 0, width, height);
                    //compute the MVP matrix from the IOD and virtual image plane distance
                    //computeStereoViewProjectionMatrices(window, IOD, depthZ, true);
                    computeViewProjectionMatrices(window);
                    //get the View and Model Matrix and apply to the rendering
                    glm::mat4 projection_matrix = getProjectionMatrix();
                    glm::mat4 view_matrix = getViewMatrix();
                    glm::mat4 model_matrix = glm::mat4(1.0);
                    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, -depthZ));
                    model_matrix = glm::rotate(model_matrix, glm::pi<float>()*rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, glm::pi<float>()*rotateX, glm::vec3(1.0f, 0.0f, 0.0f));
                    glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
                    //send our transformation to the currently bound shader,
                    //in the "MVP" uniform variable
                    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
                    /* render scene with different modes that can be enabled separately
                      to get different effects */
                    if (drawTriangles)
                    obj_loader->draw(GL_TRIANGLES);
                    if(drawPoints)
                      obj_loader->draw(GL_POINTS);
                    if(drawLines)
                      obj_loader->draw(GL_LINES);
                }
       

        
        
        //swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
            
    glDisableVertexAttribArray(attribute_vertex);
    //glDisableVertexAttribArray(attribute_uv);
    // Clean up VBO and shader
    glDeleteBuffers(1, &vertex_buffer);
    //glDeleteBuffers(1, &uv_buffer);
    glDeleteProgram(program_id);
    //glDeleteTextures(1, &texture_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    // Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
// page 98

#define GLM_FORCE_RADIANS
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opencv2/opencv.hpp>
   using namespace cv;
   
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
//Our vertices
static const GLfloat g_vertex_buffer_data[] = {
/*    -aspect_ratio,-1.0f,z_offset,
    aspect_ratio,-1.0f,z_offset,
    aspect_ratio,1.0f,z_offset,
    -aspect_ratio,-1.0f,z_offset,
    aspect_ratio,1.0f,z_offset,
    -aspect_ratio,1.0f,z_offset
*/
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f
};
//UV map for the vertices
static const GLfloat g_uv_buffer_data[] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};


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
            "Chapter 4 - Texture Mapping", NULL, NULL);
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
                       
    GLuint program_id = LoadShaders( "transform.vert", "texture_sobel.frag" );
    
   
    char *filepath;
           if(argc<2){
             filepath = (char*)malloc(sizeof(char)*512);
             sprintf(filepath, "video.mov");
           }
           else{
             filepath = argv[1];
           }
           //Handling Video input with OpenCV
           VideoCapture cap(filepath); // open the default camera
           Mat frame;
           if (!cap.isOpened()){ // check if we succeeded
             printf("Cannot open files\n");
             glfwTerminate();
             exit(EXIT_FAILURE);
             }else{

    cap >> frame; // get a new frame from camera
    printf("Got Video, %d x %d\n",frame.size().width,
        frame.size().height);
    }
    cap >> frame; // get a new frame from camera
    GLuint texture_id = initializeTexture(frame.data,
      frame.size().width, frame.size().height, GL_BGR);
    aspect_ratio = (float)frame.size().width/
      (float)frame.size().height;
       
   
    if(!texture_id){
        //if we get 0 with no texture
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
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
        sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
                
    //initialize the UV buffer memory
    GLuint uv_buffer;
    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, 
        GL_STATIC_DRAW);

    glUseProgram(program_id);
    
    //binds our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(texture_sampler_id, 0);
    
    //1st attribute buffer: vertices for position
    glEnableVertexAttribArray(attribute_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(attribute_vertex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                         
    //2nd attribute buffer: UVs mapping
    glEnableVertexAttribArray(attribute_uv);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //time-stamping for performance measurement
    double previous_time = glfwGetTime();
    do{
        //clear the screen
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

        //compute the MVP matrix from keyboard and mouse input
        computeViewProjectionMatrices(window);
        //obtain the View and Model Matrix for rendering
        glm::mat4 projection_matrix = getProjectionMatrix();
        glm::mat4 view_matrix = getViewMatrix();
        glm::mat4 model_matrix = glm::mat4(1.0);
        model_matrix = glm::rotate(model_matrix,
                glm::pi<float>() * rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
        model_matrix = glm::rotate(model_matrix,
                glm::pi<float>() * rotateX, glm::vec3(1.0f, 0.0f,0.0f));
        glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
        //send our transformation to the currently bound shader
        //in the "MVP" uniform variable
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 6); //draw a square
        
        
 /* get the video feed, reset to beginning if it reaches
        the end of the video */
      if(!cap.grab()){
        printf("End of Video, Resetting\n");
        cap.release();
        cap.open(filepath); // open the default camera
      }
      cap >> frame; // get a new frame from camera
      //update the texture with the new frame
      updateTexture(frame.data, frame.size().width,
        frame.size().height, GL_BGR);
        
        
        
        
        //swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while(!glfwWindowShouldClose(window) &&
                          glfwGetKey(window, GLFW_KEY_ESCAPE )!=GLFW_PRESS);
            
    glDisableVertexAttribArray(attribute_vertex);
    glDisableVertexAttribArray(attribute_uv);
    // Clean up VBO and shader
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &uv_buffer);
    glDeleteProgram(program_id);
    glDeleteTextures(1, &texture_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    // Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
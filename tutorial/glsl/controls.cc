#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "common.h"
#include "controls.h"


//initial position of the camera
glm::vec3 g_position = glm::vec3( 0, 0, 2 );
const float speed = 3.0f; // 3 units / second
float g_initial_fov = glm::pi<float>()*0.4f;
//the view matrix and projection matrix
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

glm::mat4 getViewMatrix(){
    return g_view_matrix;
}
glm::mat4 getProjectionMatrix(){
    return g_projection_matrix;
}
              
void computeViewProjectionMatrices(GLFWwindow* window){
    static double last_time = glfwGetTime();
    // Compute time difference between current and last frame
    double current_time = glfwGetTime();
    float delta_time = float(current_time - last_time);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    //direction vector for movement
    glm::vec3 direction(0, 0, -1);
    //up vector
    glm::vec3 up = glm::vec3(0,-1,0);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        g_position += direction * delta_time * speed;
    }
                       
    else if (glfwGetKey(window, GLFW_KEY_DOWN) ==
    GLFW_PRESS){
        g_position -= direction * delta_time * speed;
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) ==
    GLFW_PRESS){
        g_initial_fov -= 0.1 * delta_time * speed;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) ==
    GLFW_PRESS){
        g_initial_fov += 0.1 * delta_time * speed;
    }
    /* update projection matrix: Field of View, aspect ratio,
    display range : 0.1 unit <-> 100 units */
    g_projection_matrix = glm::perspective(g_initial_fov,
    (float)width/(float)height, 0.1f, 100.0f);
    // update the view matrix
    g_view_matrix = glm::lookAt(
        g_position,      // camera position
    g_position+direction, // viewing direction
    up         // up direction
        );
    last_time = current_time;
}



void computeStereoViewProjectionMatrices(GLFWwindow* window,
     float IOD, float depthZ, bool left_eye){
     int width, height;
     glfwGetWindowSize(window, &width, &height);
     //up vector
     glm::vec3 up = glm::vec3(0,-1,0);
     glm::vec3 direction_z(0, 0, -1);
     //mirror the parameters with the right eye
     float left_right_direction = -1.0f;
     if(left_eye)
       left_right_direction = 1.0f;
     float aspect_ratio = (float)width/(float)height;
     float nearZ = 1.0f;
     float farZ = 100.0f;
     double frustumshift = (IOD/2)*nearZ/depthZ;
     float top = tan(g_initial_fov/2)*nearZ;
     float right =
   aspect_ratio*top+frustumshift*left_right_direction;
   //half screen
     float left =
       -aspect_ratio*top+frustumshift*left_right_direction;
     float bottom = -top;
     g_projection_matrix = glm::frustum(left, right, bottom, top,
       nearZ, farZ);
     // update the view matrix
    g_view_matrix =
    glm::lookAt(
      g_position-direction_z+
        glm::vec3(left_right_direction*IOD/2, 0, 0),
        //eye position
      g_position+
        glm::vec3(left_right_direction*IOD/2, 0, 0),
        //centre position
      up //up direction
    );
}
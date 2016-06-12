#ifndef _CONTROLS_H
#define _CONTROLS_H 



void computeViewProjectionMatrices(GLFWwindow* window);
void computeStereoViewProjectionMatrices(GLFWwindow* window,
     float IOD, float depthZ, bool left_eye);
glm::mat4 getProjectionMatrix();
glm::mat4 getViewMatrix();

#endif
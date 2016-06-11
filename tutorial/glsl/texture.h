#ifndef _TEXTURE_H
#define _TEXTURE_H 

GLuint initializeTexture(const unsigned char *image_data,
				         int width, int height, GLenum format);
void updateTexture(const unsigned char *image_data, int width, int height, GLenum format);
GLuint loadImageToTexture(const char * imagepath);

#endif

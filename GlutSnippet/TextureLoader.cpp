#include "TextureLoader.h"
#include "CommonCV.h"
#include <opencv2\opencv.hpp>

TextureLoader* TextureLoader::instance = 0;

void TextureLoader::loadTexture(char * filepath, int id)
{
	if(id < 0 || id >= 8) return; 
	
	if(textureids == 0)
	{
		textureids = new GLuint[8];
		glGenTextures(8,textureids);
	}
	
	// load image and upload it to video memory
	IplImage * textureimg = cvLoadImage(filepath);

	cvCvtColor(textureimg,textureimg,CV_BGR2RGB); // RGB to BGR
	
	glBindTexture(GL_TEXTURE_2D,textureids[id]);
	
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NICEST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NICEST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); // gl_replace will remove light's effects
															// use gl_modulate instead
	glTexImage2D(GL_TEXTURE_2D, 
				0, 
				GL_RGB,
				textureimg->width,
				textureimg->height, 
				0, 
				GL_RGB, 
				GL_UNSIGNED_BYTE,
				textureimg->imageData); 
	
	cvReleaseImage(&textureimg);
}
	

GLuint TextureLoader::getTextureId(int id) 
{
	if(id < 0 || id >= 8) return 0;
	return textureids[id];
}
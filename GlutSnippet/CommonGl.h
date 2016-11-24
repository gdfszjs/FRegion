#pragma once

#include <Windows.h>
#include <gl/gl.h>
#include <gl/freeglut.h>
#include "CommonCV.h"


void writeImage(GLint x,GLint y,
				GLsizei w,GLsizei h,
				GLenum format,GLenum type,
				GLvoid * bits,
				int winx,int winy);

void writeImage(int gx,int gy, int offx, int offy, int w, int h,
				GLenum format, GLenum type,
				IplImage * img,
				int winx, int winy);

void writeImage(int gx, int gy, GLint x,GLint y,
				GLsizei w,GLsizei h,
				GLenum format,GLenum type,
				IplImage * img,
				int winx,int winy,int alpha);

void writeText(void * font,
			   GLint x,GLint y,
			   const char* s,
			   GLfloat r,GLfloat g,GLfloat b,
			   int win_x,int win_y);

void writeImage3C(int gx,int gy, int offx, int offy, int w, int h,
				  GLenum format, GLenum type,
				  IplImage * img,
				  int winx, int winy);

void writeImage(int ox, int oy, IplImage * img, int winx, int winy, bool flag_y_invert = false);

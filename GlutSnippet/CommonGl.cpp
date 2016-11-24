#include "CommonGl.h"
#include <LibIV\libiv.h>

void writeImage(GLint x,GLint y,
				GLsizei w,GLsizei h,
				GLenum format,GLenum type,
				GLvoid * bits,
				int winx,int winy)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0,winx,winy,0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glRasterPos2i(x,y);

	glDrawPixels(w,h,format,type,bits);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// 图片和窗口(gx,gy,w,h)一样大，但是图片相对于窗口有一个偏移量(offx,offy)
// 图片是4-channels的

void writeImage(int gx,int gy, int offx, int offy, int w, int h,
				GLenum format, GLenum type,
				IplImage * img,
				int winx, int winy)
{
	if(!img) return;

	int bw, bh;
	int bxb,bxe,byb,bye;
	int sx,sy;

	int channels = 4;

	if(offx < 0)
	{
		bxb = -offx;
		bxe = w;
		bw = w + offx;
		sx = gx;
	}
	else
	{
		bxb = 0;
		bxe = w - offx;
		bw = w - offx;
		sx = gx + offx;
	}

	if(offy < 0)
	{
		byb = -offy;
		bye = h;
		bh = h + offy;
		sy = gy;
	}
	else
	{
		byb = 0;
		bye = h - offy;
		bh = h - offy;
		sy = gy + offy;
	}

	if(h > 650)
	{
		bye = bh - (h - 650);
		bh = bye - byb;
	}
	
	GLubyte * bits = new GLubyte[bw * bh * channels];

	for(int i = byb, bi = 0; i < bye; i++, bi++)
	{
		for(int j = bxb, bj = 0; j < bxe; j++, bj++)
		{
			uchar * img_ptr = &((uchar*)(img->imageData + img->widthStep * i))[j*channels];
			bits[channels * ((bh-bi-1) * bw + bj)] = img_ptr[2];
			bits[channels * ((bh-bi-1) * bw + bj) + 1] = img_ptr[1];
			bits[channels * ((bh-bi-1) * bw + bj) + 2] = img_ptr[0];
			bits[channels * ((bh-bi-1) * bw + bj) + 3] = img_ptr[3];
		}
	}
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0,winx,winy,0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glRasterPos2i(sx,sy + bh);

	glDrawPixels(bw,bh,format,type,bits);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	delete [] bits;

}

/* 
	write on 2014-12-10
	(ox,oy): the position of <bottom-left> corner of the image on the frame
	
	^
	|
	|
	|
	|(0,0)
	-------------------->
*/
void writeImage(int ox, int oy, IplImage * img, int winx, int winy, bool flag_y_invert)
{
	if(img == 0) return;

	int ixy[4]; // the corners of image
	int wxy[4]; // the corners of frame
	int intersection[4]; // the intersection between the two rect

	ixy[0] = 0;
	ixy[1] = 0;
	ixy[2] = img->width  - 1;
	ixy[3] = img->height - 1;

	wxy[0] = 0		  - ox;
	wxy[1] = 0        - oy;
	wxy[2] = winx - 1 - ox;
	wxy[3] = winy - 1 - oy;
	
	intersection[0] = ixy[0] >= wxy[0] ? ixy[0] : wxy[0];
	intersection[1] = ixy[1] >= wxy[1] ? ixy[1] : wxy[1];
	intersection[2] = ixy[2] <= wxy[2] ? ixy[2] : wxy[2];
	intersection[3] = ixy[3] <= wxy[3] ? ixy[3] : wxy[3];

	if(intersection[0] >= intersection[2] || intersection[1] >= intersection[3]) return;

	int intersection_w = intersection[2] - intersection[0] + 1;
	int intersection_h = intersection[3] - intersection[1] + 1;
	
	GLubyte * bits = new GLubyte[intersection_w * intersection_h * 4];
	
	for(int n = intersection[1]; n <= intersection[3]; n++)
	{
		for(int m = intersection[0]; m <= intersection[2]; m++)
		{
			int nn = n - intersection[1];	
			int mm = m - intersection[0];
			int r,g,b,a;
			if(img->nChannels == 3)
			{
				LibIV::CppHelpers::Global::imgData(r,g,b,img,m,n);
				a = 255;
			}
			else 
				LibIV::CppHelpers::Global::imgData(r,g,b,a,img,m,n);

			if(flag_y_invert)
			{
				bits[4 * (nn * intersection_w + mm)]	 = b;
				bits[4 * (nn * intersection_w + mm) + 1] = g;
				bits[4 * (nn * intersection_w + mm) + 2] = r;
				bits[4 * (nn * intersection_w + mm) + 3] = a;
			}
			else
			{
				bits[4 * ((intersection_h - nn - 1) * intersection_w + mm)]	 = b;
				bits[4 * ((intersection_h - nn - 1) * intersection_w + mm) + 1] = g;
				bits[4 * ((intersection_h - nn - 1) * intersection_w + mm) + 2] = r;
				bits[4 * ((intersection_h - nn - 1) * intersection_w + mm) + 3] = a;
			}
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0,winx,0,winy);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2i(intersection[0] + ox, intersection[1] + oy);

	glDrawPixels(intersection_w,intersection_h,GL_BGRA_EXT,GL_UNSIGNED_BYTE,bits);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	delete [] bits;
}

// 图片和窗口(gx,gy,w,h)一样大，但是图片相对于窗口有一个偏移量(offx,offy)
// 图片是3-channels的

void writeImage3C(int gx,int gy, int offx, int offy, int w, int h,
				GLenum format, GLenum type,
				IplImage * img,
				int winx, int winy)
{
	if(!img) return;

	int bw, bh;
	int bxb,bxe,byb,bye;
	int sx,sy;

	int channels = 4;

	if(offx < 0)
	{
		bxb = -offx;
		bxe = w;
		bw = w + offx;
		sx = gx;
	}
	else
	{
		bxb = 0;
		bxe = w - offx;
		bw = w - offx;
		sx = gx + offx;
	}

	if(offy < 0)
	{
		byb = -offy;
		bye = h;
		bh = h + offy;
		sy = gy;
	}
	else
	{
		byb = 0;
		bye = h - offy;
		bh = h - offy;
		sy = gy + offy;
	}

	GLubyte * bits = new GLubyte[bw * bh * channels];

	for(int i = byb, bi = 0; i < bye; i++, bi++)
	{
		for(int j = bxb, bj = 0; j < bxe; j++, bj++)
		{
			uchar * img_ptr = &((uchar*)(img->imageData + img->widthStep * i))[j*3];
			bits[channels * ((bh-bi-1) * bw + bj)] = img_ptr[2];
			bits[channels * ((bh-bi-1) * bw + bj) + 1] = img_ptr[1];
			bits[channels * ((bh-bi-1) * bw + bj) + 2] = img_ptr[0];
			bits[channels * ((bh-bi-1) * bw + bj) + 3] = 255;
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0,winx,winy,0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glRasterPos2i(sx,sy + bh);

	glDrawPixels(bw,bh,format,type,bits);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	delete [] bits;

}

void writeImage(int gx,int gy,GLint x,GLint y,
				GLsizei w,GLsizei h,
				GLenum format,GLenum type,
				IplImage * img,
				int winx,int winy,int alpha)
{
	if(!img) return;
	
	int channels = 4;

	int bitsWidth = w, bitsHeight = h;
	int xCut = 0,yCut = 0;

	if(x < gx) 
	{
		bitsWidth += 2 * (x-gx);
		xCut = gx - x;
		x = gx;
	}

	if(y < gy) 
	{
		bitsHeight += 2 * (y-gy);
		yCut = gy - y;
		y = gy;
	}

	GLubyte * bits = new GLubyte[bitsWidth * bitsHeight * channels];

	for(int j=yCut;j<h-yCut;j++)
	{
		for(int k = xCut;k<w-xCut;k++)
		{
			uchar * img_ptr = &((uchar*)(img->imageData + img->widthStep * (h-1-j)))[k * 4];
			bits[channels*((j-yCut)*bitsWidth+k-xCut)]   = img_ptr[2];
			bits[channels*((j-yCut)*bitsWidth+k-xCut)+1] = img_ptr[1];
			bits[channels*((j-yCut)*bitsWidth+k-xCut)+2] = img_ptr[0];
			
			// 使用真正的alpha值
			bits[channels*((j-yCut)*bitsWidth+k-xCut)+3] = img_ptr[3];

			//bits[channels*((j-yCut)*bitsWidth+k-xCut)+3] = alpha;
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0,winx,winy,0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glRasterPos2i(x,y + h);

	glDrawPixels(bitsWidth,bitsHeight,format,type,bits);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	delete [] bits;
}


void writeText(void * font,GLint x,GLint y,const char* s,GLfloat r,GLfloat g,GLfloat b,int win_x,int win_y)
{
	int lines = 0;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0,win_x,0,win_y);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(r,g,b);

	glRasterPos2i(x,y);

	int i = 0;
	while(s[i] != '\0')
	{
		if(s[i] == '\n')
		{
			lines++;
			glRasterPos2i(x,y+lines * 24);
		}
		glutBitmapCharacter(font,s[i]);
		i++;
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
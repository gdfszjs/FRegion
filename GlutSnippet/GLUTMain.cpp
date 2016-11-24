#include "CommonGl.h"
#include "GLUTMain.h"
#include "Frame.h"

#include "FrameButton.h"
#include "ButtonClick.h"
#include "Frame2DSnippet.h"
#include "Frame3DSnippet.h"
#include "FrameSlider.h"
#include "SliderClick.h"
#include "FrameChecker.h"

#include "FrameFunctionPanel.h"

GLUTMain * GLUTMain::instance = 0; 

void ReshapeFunc(int w, int h) {
	GLUTMain::Instance()->SetWinX(w);
	GLUTMain::Instance()->SetWinY(h);
	GLUTMain::Instance()->refreshLayout();
}

void DisplayFunc() {
	GLUTMain::Instance()->Draw();
	glutSwapBuffers();
}

void KeyboardFunc(unsigned char key, int x, int y) {
	if(key == 27) exit(0);
	GLUTMain::Instance()->KeyboardFunc(key,x,y);
	glutPostRedisplay();
}

void SpecialKeyboardFunc(int key,int x,int y) {
	GLUTMain::Instance()->SpecialKeyboardFunc(key,x,y);
	glutPostRedisplay();
  }

void MouseWheelFunc(int wheel, int direction, int x, int y) {
	int gx = x;
	int gy = GLUTMain::Instance()->WinY() - 1 - y;
	GLUTMain::Instance()->MouseWheelFunc(wheel,direction,gx,gy);
	glutPostRedisplay();
}

void MouseFunc(int button,int state,int x,int y) {
	int gx = x;
	int gy = GLUTMain::Instance()->WinY() - 1 - y;
	GLUTMain::Instance()->MouseFunc(button,state,gx,gy);
	glutPostRedisplay();
}

void MotionFunc(int x, int y) {
	int gx = x;
	int gy = GLUTMain::Instance()->WinY() - 1 - y;
	GLUTMain::Instance()->MotionFunc(gx,gy);
	glutPostRedisplay();
}

void PassiveMotionFunc(int x, int y) {
	int gx = x;
	int gy = GLUTMain::Instance()->WinY() - 1 - y;
	GLUTMain::Instance()->PassiveMotionFunc(gx,gy);
	glutPostRedisplay();
}

void InitEnvio() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_MULT);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLfloat lightAmbient[] = {0.0f,0.0f,0.0f,1.0f};
	GLfloat lightDiffuse[] = {.8f,.8f,.8f,1.0f};
	GLfloat lightPosition[] = {0.0f,1.0f,0.0f,1.0f};
	GLfloat lightSpec[] = {.5f,.5f,.5f,1.0f};

	GLfloat lightAmbient2[] = {0.5f,0.5f,0.5f,1.0f};
	GLfloat lightDiffuse2[] = {1.0f,1.0f,1.0f,1.0f};
	GLfloat lightPosition2[] = {1.0f,0.0f,0.0f,1.0f};
	GLfloat lightSpec2[] = {1.0f,1.0f,1.0f,1.0f};

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glEnable(GL_NORMALIZE);
	
	glLightfv(GL_LIGHT1,GL_POSITION,lightPosition);
	glLightfv(GL_LIGHT1,GL_AMBIENT,lightAmbient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,lightDiffuse);
	glLightfv(GL_LIGHT1,GL_SPECULAR,lightSpec);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2,GL_POSITION,lightPosition2);
	glLightfv(GL_LIGHT2,GL_AMBIENT,lightAmbient2);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,lightDiffuse2);
	glLightfv(GL_LIGHT2,GL_SPECULAR,lightSpec2);
	glEnable(GL_LIGHT2);
}

void ProcessMenuEvent(int option) { 
}

void OpenGLUTWindow() {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutInitWindowPosition(0,0);
	glutInitWindowSize(GLUTMain::Instance()->WinX(),GLUTMain::Instance()->WinY());
	glutCreateWindow("OpenGL GUI");

	glutReshapeFunc(ReshapeFunc);
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);

	glutSpecialFunc(SpecialKeyboardFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	glutPassiveMotionFunc(PassiveMotionFunc);

	glutMouseWheelFunc(MouseWheelFunc);

	int menuhandler = glutCreateMenu(ProcessMenuEvent);
	
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void main(int argc, char* argv[]) 
{
	glutInit(&argc,argv);
	OpenGLUTWindow();
	InitEnvio();

	glutSwapBuffers();
	glutMainLoop();
}

GLUTMain::~GLUTMain() { 
	if(mainFrame != NULL) delete mainFrame;
}

void GLUTMain::Draw() { 
	if(mainFrame) mainFrame->drawScene();
}

void GLUTMain::MouseWheelFunc(int wheel, int direction, int x, int y) {
	Frame * fm = mainFrame->getFocusFrame(x,y);
	setFocus(fm);

	if(focusFrame) focusFrame->onMouseWheelFunc(wheel,direction,x,y);
}

void GLUTMain::MouseFunc(int button, int state, int gx, int gy) 
{
	if(state == GLUT_DOWN)
	{
		Frame * fm = mainFrame->getFocusFrame(gx,gy);
		setFocus(fm);
	}

	// 2015.05.21	when mouse up and motion, if there is focusFrame, focusFrame processes message
	if(focusFrame) focusFrame->onMouseFunc(button,state,gx,gy);
}

void GLUTMain::PassiveMotionFunc(int gx, int gy) {
	Frame * fm = mainFrame->getFocusFrame(gx,gy);
	setFocus(fm);

	if(fm) fm->onPassiveMotionFunc(gx,gy);
}

void GLUTMain::MotionFunc(int gx, int gy) {
	//Frame * fm = mainFrame->getFocusFrame(gx,gy);
	//setFocus(fm);
	
	// 2015.05.21	when mouse up and motion, if there is focusFrame, focusFrame processes message
	if(focusFrame) focusFrame->onMotionFunc(gx,gy);
}

void GLUTMain::KeyboardFunc(unsigned char key, int x, int y)
{
	if(focusFrame) focusFrame->onKeyboardFunc(key,x,y);
}

void GLUTMain::SpecialKeyboardFunc(int key,int x,int y) {
	if(focusFrame) focusFrame->onSpecialKeyboardFunc(key,x,y);
}

void GLUTMain::refreshLayout() {
	// position and size should be in the form of proportion
	// then this function is valid
	mainFrame->setPosSize(0,0,winx,winy);
}

void GLUTMain::setFocus(Frame * fm) {
	if(focusFrame == fm) return;
	if(focusFrame != NULL) focusFrame->onLoseFocus();
	focusFrame = fm;
}

GLUTMain::GLUTMain() {
	winx = 1270;
	winy = 720;
	focusFrame = 0;

	mainFrame = new Frame(0,0,0,winx,winy,"Main"); 
	mainFrame->setBkColor(96,96,96,0);

	FrameFunctionPanel * function_panel = new FrameFunctionPanel(mainFrame,10,winy - 80,550,70,"FunctionPanel");
	function_panel->setBkColor(41,57,85,0);
	mainFrame->addFrame(function_panel);

	/*Frame2DSnippet * the_frame_2d = new Frame2DSnippet(mainFrame,250,10,1300,940,"2D View");
	the_frame_2d->setBkColor(0,0,0,0);
	mainFrame->addFrame(the_frame_2d);*/

	/*Frame3DSnippet * the_frame_3d = new Frame3DSnippet(mainFrame,520,10,500,500,"3D View");
	the_frame_3d->setBkColor(0,0,0,0);
	mainFrame->addFrame(the_frame_3d);

	FrameSlider<int> * the_slider_example = new FrameSlider<int>(mainFrame, 50,560, 500, 25, "Slider Example", 
		"Value:%d",178,153,153,9,23,12, new SliderClickExample());
	the_slider_example->setBkColor(255,229,204,0);
	mainFrame->addFrame(the_slider_example);

	FrameSlider<double> * the_slider_example_2 = new FrameSlider<double>(mainFrame, 50,520, 500, 25, "Slider Example 2", 
		"Value:%f",178,153,153,9.5,1000.2,12);
	the_slider_example_2->setBkColor(255,229,204,0);
	mainFrame->addFrame(the_slider_example_2);

	FrameChecker * the_checker = new FrameChecker(mainFrame, 600, 520, 200,25, "Checker 1",0,0,0);
	the_checker->setBkColor(255,229,204,0);
	mainFrame->addFrame(the_checker); */
}
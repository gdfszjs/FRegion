#pragma once

class Frame;

class GLUTMain
{
public:
	~GLUTMain();

	int WinX() { return winx; }
	int WinY() { return winy; }
	void SetWinX(int x) {winx = x;}
	void SetWinY(int y) {winy = y;}

	static GLUTMain * Instance(){
		if(instance == 0)
		{
			instance = new GLUTMain();
			static Cleaner clr;
		} 
		return instance;
	}

	// 这个函数负责所有Frame的渲染
	void Draw();
	void MouseFunc(int button, int state, int gx, int gy);
	void MotionFunc(int gx, int gy);
	void KeyboardFunc(unsigned char key, int x, int y);
	void SpecialKeyboardFunc(int key,int x,int y);
	void PassiveMotionFunc(int x, int y);
	void MouseWheelFunc(int wheel, int direction, int x, int y);
	
	void refreshLayout();

	void setFocus(Frame * fm);
	Frame * getFocus() { return focusFrame; }
	Frame * getMainFrame() { return mainFrame; }

private:
	GLUTMain();

	// window size
	int winx,winy;

	Frame * mainFrame;

	Frame * focusFrame;

	// Singleton
	static GLUTMain * instance;
	
	// automatically delete the singleton
	class Cleaner
	{
	public:
		Cleaner() {}
		~Cleaner()
		{
			if(GLUTMain::Instance())
				delete GLUTMain::Instance();
		}
	};
};
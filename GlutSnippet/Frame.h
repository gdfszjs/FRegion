#pragma  once

#include "Common.h"
class GLUTMain;

class Frame {
public:
	Frame();

	Frame(Frame * parent, int lx, int ly, int width, int height,string name);

	virtual ~Frame();
	
	virtual void drawScene();
	virtual void onKeyboardFunc(unsigned char key, int x, int y) {}
	virtual void onSpecialKeyboardFunc(int key,int x,int y) {}
	virtual void onMouseFunc(int button,int state,int x,int y);
	virtual void onMotionFunc(int x,int y);
	virtual void onPassiveMotionFunc(int x, int y);
	virtual void onMouseWheelFunc(int wheel, int direction, int x, int y);


	void addFrame(Frame * fm){
		childFrames.push_back(fm);
	}

	void deleteFrameByName(string name)
	{
		for(std::vector<Frame*>::iterator it = childFrames.begin();it != childFrames.end();)
		{
			if((*it)->name == name)
			{
				delete (*it);
				it = childFrames.erase(it);
			}
			else
			{
				(*it)->deleteFrameByName(name);
				++it;
			}
		}
	}

	Frame* getFrame(size_t n){
		if( n < childFrames.size() )
			return childFrames[n];
		else
			return 0;
	}

	// gx,gy: the global position of point
	bool virtual isPointInFrame(int gx, int gy) {
		int tx, ty;
		_xyInGlobal(tx,ty);
		return (gx >= tx && gx < tx + width && gy >= ty && gy < ty + height);
	}

	void globalPt2Local(int & lx, int & ly, int gx, int gy) {
		int lgx,lgy;
		_xyInGlobal(lgx,lgy);
		lx = gx - lgx;
		ly = gy - lgy;
	}

	void setBkColor(int r, int g, int b, int a){
		br = r;
		bg = g;
		bb = b;
		alpha = a;
	}

	void setPosSize(int x, int y, int width, int height){
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	// 递归地设置Focus Frame
	void setFocusFrame(int gx, int gy);
	// 递归地得到Focus Frame
	Frame * getFocusFrame(int gx, int gy);

	// 在失去焦点时进行的操作
	virtual void onLoseFocus() {}

	string getName() { return name; }

	Frame * getFrameByName(string str)
	{
		if(name == str)
			return this;
		else 
		{	
			Frame * ans = 0;
			for(size_t i = 0;i<childFrames.size();i++)
			{
				ans = childFrames[i]->getFrameByName(str);
				if(ans != 0)
					break;
			}
			return ans;
		}
	}
protected:
	void _xyInGlobal(int & gx, int & gy)
	{
		gx = x;
		gy = y;
		Frame * fm = parentFrame;
		while(fm != 0)
		{
			gx += fm->x;
			gy += fm->y;
			fm = fm->parentFrame;
		}
	}
	
protected:
	// the window linked to
	// GLUTMain * window;

	Frame * parentFrame;
	std::vector<Frame*> childFrames;

	int x, y, width, height;

	// mouse moving
	bool isLeftButtonDown, isRightButtonDown, isMiddleButtonDown; 
	int mouseOx,mouseOy, mouseTranx, mouseTrany, mouseOrgX, mouseOrgY;

	string name;

	// background color
	int br,bg,bb,alpha;
};


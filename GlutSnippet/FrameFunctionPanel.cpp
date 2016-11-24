 #include "FrameFunctionPanel.h"
#include "CommonGl.h"
#include "FrameButton.h"
#include "GLUTMain.h"
#include "ButtonClick.h"

FrameFunctionPanel::FrameFunctionPanel() 
{
}

FrameFunctionPanel::FrameFunctionPanel(Frame * parent, int lx, int ly, int width, int height,string name)
	: Frame(parent,lx,ly,width,height,name) 
{
	FrameButton * fb_FRegion = new FrameButton(this,10,10,120,50,"FRegion",new ButtonFRegionClick());
	this->addFrame(fb_FRegion);

	/*FrameButton * fb_2DSnippet = new FrameButton(this,10,height - 120, width - 20, 50, "2DSnippet", new ButtonTestClick());
	this->addFrame(fb_2DSnippet);*/

	FrameButton * fb_FRStructure = new FrameButton(this,140,10,120, 50, "FRStructure", new ButtonFRStructureClick());
	this->addFrame(fb_FRStructure);

	FrameButton * fb_StructureBlend = new FrameButton(this,270,10,140, 50, "StructureBlend", new ButtonStructureBlendClick());
	this->addFrame(fb_StructureBlend);

	FrameButton * fb_MedialAxis = new FrameButton(this,420,10,120, 50, "MedialAxis", new ButtonMedialAxisClick());
	this->addFrame(fb_MedialAxis);
}

FrameFunctionPanel::~FrameFunctionPanel() 
{
	
}
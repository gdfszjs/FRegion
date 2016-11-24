#include "ButtonClick.h"
#include "CommonGl.h"
#include "Frame.h"
#include "GLUTMain.h"
#include "FrameButton.h"

#include "FrameFRegion.h"
#include "Frame2DSnippet.h"
#include "FrameFRStructure.h"
#include "FrameStructureBlend.h"
#include "FrameMedialAxis.h"
#include "TextureLoader.h"

//#include "Geo2D.h"

void ButtonTestClick::onClick()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	main_frame->deleteFrameByName("FrameView");

	int global_width = GLUTMain::Instance()->WinX();
	int global_height = GLUTMain::Instance()->WinY();
	
	Frame2DSnippet * f_2DSnippet = new Frame2DSnippet(main_frame,250,10,global_width - 260,global_height - 20,"FrameView");
	f_2DSnippet->setBkColor(0,0,0,0);
	main_frame->addFrame(f_2DSnippet);
}

void ButtonFRegionClick::onClick()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	main_frame->deleteFrameByName("FrameView");

	int global_width = GLUTMain::Instance()->WinX();
	int global_height = GLUTMain::Instance()->WinY();
	
	FrameFRegion * f_FRegion = new FrameFRegion(main_frame,10,10,global_width - 20,global_height - 100,"FrameView");
	f_FRegion->setBkColor(0,0,0,0);
	main_frame->addFrame(f_FRegion);

	/*v2d p0 = _v2d_(142,277);
	v2d p1 = _v2d_(61,187);
	v2d p2 = _v2d_(209,238);
	v2d p3 = _v2d_(220,40);

	double arc_len = Geo2D::GetCubicBeizerArcLength(1,p0,p1,p2,p3);
	cout<<arc_len<<endl;*/
}

void ButtonFRStructureClick::onClick()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	main_frame->deleteFrameByName("FrameView");

	int global_width = GLUTMain::Instance()->WinX();
	int global_height = GLUTMain::Instance()->WinY();
	
	FrameFRStructure * f_FRStructure = new FrameFRStructure(main_frame,10,10,global_width - 20,global_height - 100,"FrameView");
	f_FRStructure->setBkColor(0,0,0,0);
	main_frame->addFrame(f_FRStructure);
}

void ButtonFRSSaveFRClick::onClick()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameFRStructure * f_FRStructure = (FrameFRStructure*)(main_frame->getFrameByName("FrameView"));

	f_FRStructure->SaveFRSTree();
}

void ButtonStructureBlendClick::onClick()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	main_frame->deleteFrameByName("FrameView");

	int global_width = GLUTMain::Instance()->WinX();
	int global_height = GLUTMain::Instance()->WinY();
	
	FrameStructureBlend * f_StructureBlend = new FrameStructureBlend(main_frame,10,10,global_width - 20 ,global_height - 100,"FrameView");
	f_StructureBlend->setBkColor(255,255,255,255);
	main_frame->addFrame(f_StructureBlend);

	TextureLoader::Instance()->loadTexture("D:/textures/1.jpg",0);
	TextureLoader::Instance()->loadTexture("D:/textures/2.png",1);
	TextureLoader::Instance()->loadTexture("D:/textures/3.jpg",2);
	TextureLoader::Instance()->loadTexture("D:/textures/4.jpg",3);
	TextureLoader::Instance()->loadTexture("D:/textures/5.jpg",4);
	TextureLoader::Instance()->loadTexture("D:/textures/6.jpg",5);
	TextureLoader::Instance()->loadTexture("D:/textures/7.jpg",6);
	TextureLoader::Instance()->loadTexture("D:/textures/8.jpg",7);
}

void ButtonMedialAxisClick::onClick()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	main_frame->deleteFrameByName("FrameView");

	int global_width = GLUTMain::Instance()->WinX();
	int global_height = GLUTMain::Instance()->WinY();

	FrameMedialAxis * f_MedialAxis = new FrameMedialAxis(main_frame,10,10,global_width - 20, global_height - 100,"FrameView");
	
	f_MedialAxis->setBkColor(0,0,0,255);
	main_frame->addFrame(f_MedialAxis);
}
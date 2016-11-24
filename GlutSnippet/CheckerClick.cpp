#include "CheckerClick.h"
#include "GLUTMain.h"
#include "FrameFRStructure.h"
#include "FrameChecker.h"

void CheckerTestClick::OnClick(bool status)
{
}


void CheckerFRStructureSelectNodeClick::OnClick(bool status)
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameFRStructure * f_FRStructure = (FrameFRStructure*)(main_frame->getFrameByName("FrameView"));
	
	if(status == true)
	{
		/* set other checker to unchecker */
		FrameChecker * frs_checker_add_line = (FrameChecker*)(main_frame->getFrameByName("FRS Add Line"));
		frs_checker_add_line->UnCheck();

		FrameChecker * frs_checker_add_bezier = (FrameChecker*)(main_frame->getFrameByName("FRS Add Bezier"));
		frs_checker_add_bezier->UnCheck();

		FrameChecker * frs_checker_edit_node = (FrameChecker*)(main_frame->getFrameByName("FRS Edit Node"));
		frs_checker_edit_node->UnCheck();

		FrameChecker * frs_sel_ele = (FrameChecker*)(main_frame->getFrameByName("FRS Select Ele"));
		frs_sel_ele->UnCheck();
	}
}

void CheckerFRStructureAddLineClick::OnClick(bool status)
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameFRStructure * f_FRStructure = (FrameFRStructure*)(main_frame->getFrameByName("FrameView"));
	
	if(status == true)
	{
		/* set other checker to unchecker */
		FrameChecker * frs_select_node = (FrameChecker*)(main_frame->getFrameByName("FRS Select Node"));
		frs_select_node->UnCheck();

		FrameChecker * frs_checker_add_bezier = (FrameChecker*)(main_frame->getFrameByName("FRS Add Bezier"));
		frs_checker_add_bezier->UnCheck();

		FrameChecker * frs_checker_edit_node = (FrameChecker*)(main_frame->getFrameByName("FRS Edit Node"));
		frs_checker_edit_node->UnCheck();

		FrameChecker * frs_sel_ele = (FrameChecker*)(main_frame->getFrameByName("FRS Select Ele"));
		frs_sel_ele->UnCheck();
	}
}

void CheckerFRStructureAddBezierClick::OnClick(bool status)
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameFRStructure * f_FRStructure = (FrameFRStructure*)(main_frame->getFrameByName("FrameView"));

	if(status == true)
	{
		FrameChecker * frs_checker_add_line = (FrameChecker*)(main_frame->getFrameByName("FRS Add Line"));
		frs_checker_add_line->UnCheck();
		
		FrameChecker * frs_select_node = (FrameChecker*)(main_frame->getFrameByName("FRS Select Node"));
		frs_select_node->UnCheck();

		FrameChecker * frs_checker_edit_node = (FrameChecker*)(main_frame->getFrameByName("FRS Edit Node"));
		frs_checker_edit_node->UnCheck();

		FrameChecker * frs_sel_ele = (FrameChecker*)(main_frame->getFrameByName("FRS Select Ele"));
		frs_sel_ele->UnCheck();
	}
}

void CheckerFRStructureEditNodeClick::OnClick(bool status)
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameFRStructure * f_FRStructure = (FrameFRStructure*)(main_frame->getFrameByName("FrameView"));

	if(status == true)
	{
		FrameChecker * frs_checker_add_line = (FrameChecker*)(main_frame->getFrameByName("FRS Add Line"));
		frs_checker_add_line->UnCheck();
		
		FrameChecker * frs_select_node = (FrameChecker*)(main_frame->getFrameByName("FRS Select Node"));
		frs_select_node->UnCheck();

		FrameChecker * frs_checker_add_bezier = (FrameChecker*)(main_frame->getFrameByName("FRS Add Bezier"));
		frs_checker_add_bezier->UnCheck();		

		FrameChecker * frs_sel_ele = (FrameChecker*)(main_frame->getFrameByName("FRS Select Ele"));
		frs_sel_ele->UnCheck();
	}
}

void CheckerFRStructureSelectElementClick::OnClick(bool status)
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameFRStructure * f_FRStructure = (FrameFRStructure*)(main_frame->getFrameByName("FrameView"));

	if(status == true)
	{
		FrameChecker * frs_checker_add_line = (FrameChecker*)(main_frame->getFrameByName("FRS Add Line"));
		frs_checker_add_line->UnCheck();
		
		FrameChecker * frs_select_node = (FrameChecker*)(main_frame->getFrameByName("FRS Select Node"));
		frs_select_node->UnCheck();

		FrameChecker * frs_checker_edit_node = (FrameChecker*)(main_frame->getFrameByName("FRS Edit Node"));
		frs_checker_edit_node->UnCheck();

		FrameChecker * frs_checker_add_bezier = (FrameChecker*)(main_frame->getFrameByName("FRS Add Bezier"));
		frs_checker_add_bezier->UnCheck();
	}
}
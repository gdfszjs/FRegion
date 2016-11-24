#include "Common.h"
#include "SliderClick.h"
#include "Frame2DSnippet.h"

void SliderClickTest::OnClick(double v)
{
	cout<<"SliderClickTest.OnClick - "<<v<<endl;
}

void SliderClickExample::OnClick(double v)
{
	cout<<v<<endl;	
}
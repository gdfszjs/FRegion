#ifndef SLIDER_CLICK_H_
#define SLIDER_CLICK_H_

class SliderClick
{
public:
	SliderClick() {}
	virtual ~SliderClick() {}
	
	virtual void OnClick(double v) {}
};

class SliderClickTest : public SliderClick
{
public:
	SliderClickTest() {}
	~SliderClickTest() {}
	
	virtual void OnClick(double v);
};

class SliderClickExample : public SliderClick
{
public:
	SliderClickExample() {}
	~SliderClickExample() {}
	
	virtual void OnClick(double v);
};

#endif // SLIDER_CLICK_H_
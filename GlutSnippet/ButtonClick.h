#pragma  once

#include <LibIV/libiv.h>
#include "Common.h"

class ButtonClick {
public:
	ButtonClick() {}

	virtual ~ButtonClick() {}

	virtual void onClick() {}
};


class ButtonTestClick : public ButtonClick {
public:
	ButtonTestClick() {}

	virtual ~ButtonTestClick() {}

	virtual void onClick();
};

class ButtonFRegionClick : public ButtonClick {
public:
	ButtonFRegionClick() {}

	virtual ~ButtonFRegionClick() {}

	virtual void onClick();
};

class ButtonFRStructureClick : public ButtonClick {
public:
	ButtonFRStructureClick() {}

	virtual ~ButtonFRStructureClick() {}

	virtual void onClick();
};

class ButtonFRSSaveFRClick : public ButtonClick {
public:
	ButtonFRSSaveFRClick() {}

	virtual ~ButtonFRSSaveFRClick() {}

	virtual void onClick();
};

class ButtonStructureBlendClick : public ButtonClick {
public:
	ButtonStructureBlendClick() {}

	virtual ~ButtonStructureBlendClick() {}

	virtual void onClick();
};

class ButtonMedialAxisClick : public ButtonClick {
public:
	ButtonMedialAxisClick() {}

	virtual ~ButtonMedialAxisClick() {}

	virtual void onClick();
};
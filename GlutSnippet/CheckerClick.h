#ifndef CHECKER_CLICK_H_
#define CHECKER_CLICK_H_

class CheckerClick {
public:
	CheckerClick() {}

	virtual ~CheckerClick() {}

	virtual void OnClick(bool status) {}
};

class CheckerTestClick : public CheckerClick
{
public:
	CheckerTestClick() { }
	~CheckerTestClick() { }

	virtual void OnClick(bool status);
};

class CheckerFRStructureSelectNodeClick : public CheckerClick
{
public:
	CheckerFRStructureSelectNodeClick() { }
	~CheckerFRStructureSelectNodeClick() { }

	virtual void OnClick(bool status);
};

class CheckerFRStructureAddLineClick : public CheckerClick
{
public:
	CheckerFRStructureAddLineClick() { }
	~CheckerFRStructureAddLineClick() { }

	virtual void OnClick(bool status);
};

class CheckerFRStructureAddBezierClick : public CheckerClick
{
public:
	CheckerFRStructureAddBezierClick() { }
	~CheckerFRStructureAddBezierClick() { }

	virtual void OnClick(bool status);
};

class CheckerFRStructureEditNodeClick : public CheckerClick
{
public:
	CheckerFRStructureEditNodeClick() { }
	~CheckerFRStructureEditNodeClick() { }

	virtual void OnClick(bool status);
};

class CheckerFRStructureSelectElementClick : public CheckerClick
{
public:
	CheckerFRStructureSelectElementClick() { }
	~CheckerFRStructureSelectElementClick() { }

	virtual void OnClick(bool status);
};

#endif // CHECKER_CLICK_H_
#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ui/UIWidget.h"
#include "SimpleAudioEngine.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

using namespace cocostudio::timeline;
using namespace ui;
using namespace CocosDenshion;

class CButton : public cocos2d::Node
{
private:
	Point pt;
	Rect touchArea;
	Sprite *_show;
	bool _bTouched=false;
	bool _iStatus = false;

public:
	~CButton();
	CButton();
	char *_SpriteName;
	void set(Point,char *);

	void isTouched(Point );
	void isLeave(Point);
	bool isUsed();
	void setUnuse();
	void setUse();

	// implement the "static create()" method manually
	CREATE_FUNC(CButton);
};


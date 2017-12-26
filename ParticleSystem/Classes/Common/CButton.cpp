#include "CButton.h"

USING_NS_CC;

using namespace cocostudio::timeline;
using namespace ui;
using namespace CocosDenshion;

CButton::~CButton() {

}

CButton::CButton(){

}

void CButton::set(Point pt,char *n) {
	_SpriteName = new char[15];
	_SpriteName = n;
	_show = Sprite::createWithSpriteFrameName(n);
	Size sz = _show->getContentSize();
	_show->setPosition(pt);
	touchArea = Rect(pt.x - sz.width / 2, pt.y - sz.height / 2, sz.width, sz.height);
	this->addChild(_show, 0);
}

void CButton::isTouched(Point touchPt){
	if (touchArea.containsPoint(touchPt)) {
		_bTouched = true;
		_show->setScale(1.5);
	}
	
}
void CButton::isLeave(Point touchPt) {
	if (_bTouched) {
		if (touchArea.containsPoint(touchPt) == false) {
			_bTouched = false;
			_show->setScale(1);
		}
	}
}
bool CButton::isUsed() {
	if (_bTouched) {
		_bTouched = false;
		_iStatus = true;
		_show->setColor(Color3B::YELLOW);
		_show->setScale(1);
		return (_iStatus);
	}
	return false;
}

void CButton::setUnuse() {
	_iStatus = false;
	_show->setColor(Color3B::WHITE);
	_show->setScale(1);
	
}

void CButton::setUse() {
	_iStatus = true;
	_show->setColor(Color3B::YELLOW);
	_show->setScale(1);

}


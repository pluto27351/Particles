#include "ParticleSystemScene.h"

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#define btnPos 50

USING_NS_CC;

using namespace cocostudio::timeline;
using namespace ui;
using namespace CocosDenshion;

Scene* ParticleSystemScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = ParticleSystemScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

ParticleSystemScene::~ParticleSystemScene()
{
	this->removeAllChildren();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
}

// on "init" you need to initialize your instance
bool ParticleSystemScene::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    auto rootNode = CSLoader::createNode("MainScene.csb");
    addChild(rootNode);

	// For Emitter
	_EmitterSprite = CDraggableSprite::create();
	_EmitterSprite->setSpriteInfo("emittericon.png", Point(-125.0f + visibleSize.width / 2.0f, visibleSize.height / 2.0f));
	_EmitterSprite->setVisible(false);
	_bEmitterOn = false;
	this->addChild(_EmitterSprite, 5);

	auto emiterpos = (Sprite *)(rootNode->getChildByName("emitterpos"));
	Point loc = emiterpos->getPosition();
	emiterpos->setVisible(false);

	_emitterSwitchBtn = CSwitchButton::create();
	_emitterSwitchBtn->setButtonInfo("emitteroff.png", "emitteron.png", "emittertd.png", loc);
	this->addChild(_emitterSwitchBtn, 2);

	_fireworkSwitchBtn = CSwitchButton::create();
	_fireworkSwitchBtn->setButtonInfo("fireworkoff.png", "fireworkon.png", "fireworktd.png", loc + Vec2(0,100));
	this->addChild(_fireworkSwitchBtn, 2);

	_rollSwitchBtn = CSwitchButton::create();
	_rollSwitchBtn->setButtonInfo("rolloff.png", "rollon.png", "rolltd.png", loc+Vec2(0,200));
	this->addChild(_rollSwitchBtn, 2);

	_waterballSwitchBtn = CSwitchButton::create();
	_waterballSwitchBtn->setButtonInfo("waterball.png", "waterballon.png", "waterballtd.png", loc+ Vec2(0,300));
	this->addChild(_waterballSwitchBtn, 2);

	_bombSwitchBtn = CSwitchButton::create();
	_bombSwitchBtn->setButtonInfo("bomboff.png", "bombon.png", "bombtd.png", loc + Vec2(0,400));
	this->addChild(_bombSwitchBtn, 2);

	// Particle Control System
	// 最好的方式是，以下的 Slider 根據這裡的設定值，顯示出正確的數值與位置
	_ParticleControl.init(*this);
	_ParticleControl._emitterPt = _EmitterSprite->getLoc();
	_ParticleControl._fDir = 90.0;
	_ParticleControl._iNumParticles = 100;
	_ParticleControl._iGenParticles = 0;
	_ParticleControl._fRed = 255;
	_ParticleControl._fGreen = 255;
	_ParticleControl._fBlue = 255;
	_ParticleControl._fSpread = 180.0f;
	_ParticleControl._fVelocity = 2.5;	 // 分子的離開速度
	_ParticleControl._fLifeTime = 3.5f;	 // 分子的存活時間
	_ParticleControl._fSpin = 0;
	_ParticleControl._fGravity = 0;
	_ParticleControl._fOpacity = 255;
	_ParticleControl._fElpasedTime = 0;
	_ParticleControl._cName = "flare.png";
	_ParticleControl.setType(STAY_FOR_TWOSECONDS); // 分子運動的型態，預設為 0
	_ParticleControl._fWindDir = 0; // 本範例沒有實作此項功能
	_ParticleControl._fWindStr = 0;

	// 分子的可調整參數的捲動軸
	// Slider of Gravity
	auto *GravitySlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Gravity"));
	GravitySlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::GravityEvent, this));
	GravitySlider->setMaxPercent(100); 	// 將 0 到 100 對應到 -10 到 10 之間
	_GravityBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("GravityBMFont");

	// Slider of Spread
	auto *SpreadSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Spread"));
	SpreadSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::SpreadEvent, this));
	SpreadSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 180 之間
	_SpreadBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("SpreadBMFont");

	// Slider of Direction
	auto *DirectionSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Direction"));
	DirectionSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::DirectionEvent, this));
	DirectionSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 180 之間
	_DirectionBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("DirectionBMFont");

	// Slider of Spin
	auto *SpinSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Spin"));
	SpinSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::SpinEvent, this));
	SpinSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 360 之間
	_SpinBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("SpinBMFont");

	// Slider of Opacity
	auto *OpacitySlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Opacity"));
	OpacitySlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::OpacityEvent, this));
	OpacitySlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 100 之間
	_OpacityBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("OpacityBMFont");

	// Slider of Particles
	auto *ParticlesSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Particles"));
	ParticlesSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::ParticlesEvent, this));
	ParticlesSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 200 之間
	_ParticlesBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("ParticlesBMFont");

	// Slider of Speed
	auto *SpeedSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Speed"));
	SpeedSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::SpeedEvent, this));
	SpeedSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 40 之間
	_SpeedBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("SpeedBMFont");

	// Slider of Lifetime
	auto *LifetimeSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Lifetime"));
	LifetimeSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::LifetimeEvent, this));
	LifetimeSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 7 之間
	_LifetimeBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("LifetimeBMFont");

	// Slider of Red
	auto *RedSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Red"));
	RedSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::RedEvent, this));
	RedSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 255 之間
	_RedBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("RedBMFont");

	// Slider of Green
	auto *GreenSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Green"));
	GreenSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::GreenEvent, this));
	GreenSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 255 之間
	_GreenBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("GreenBMFont");

	// Slider of Blue
	auto *BlueSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Blue"));
	BlueSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::BlueEvent, this));
	BlueSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 255 之間
	_BlueBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("BlueBMFont");

	// Slider of windDir
	auto *WindDirSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_WindDir"));
	WindDirSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::WindDirEvent, this));
	WindDirSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 -180 到 180 之間
	_WindDirBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("WindDirBMFont");

	// Slider of windStr
	auto *WindStrSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_WindStr"));
	WindStrSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::WindStrEvent, this));
	WindStrSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 10 之間
	_WindStrBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("WindStrBMFont");


	// Slider of Type
	auto *TypeSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Type"));
	TypeSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::TypeEvent, this));
	TypeSlider->setMaxPercent(100); 	// 將 0 到 100 對應到 0 到 360 之間
	_TypeBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("TypeBMFont");

	//btn
	btn = new CButton[7];
	btn[0].set(Vec2(btnPos+80*0, 50), "bubble.png");    addChild(btn+0, 1);
	btn[1].set(Vec2(btnPos+80*1, 50), "circle.png");   addChild(btn+1, 1);
	btn[2].set(Vec2(btnPos+80*2, 50), "cloud.png");    addChild(btn+2, 1);
	btn[3].set(Vec2(btnPos+80*3, 50), "flare.png");    addChild(btn+3, 1);
	btn[4].set(Vec2(btnPos+80*4, 50), "raindrop.png"); addChild(btn+4, 1);
	btn[5].set(Vec2(btnPos+80*5, 50), "comet.png");    addChild(btn+5, 1);
	btn[6].set(Vec2(btnPos+80*6, 50), "spark.png");    addChild(btn+6, 1);
	btn[0].setUse();
	_ParticleControl.setName(btn[0]._SpriteName);

	_listener1 = EventListenerTouchOneByOne::create();	//創建一個一對一的事件聆聽器
	_listener1->onTouchBegan = CC_CALLBACK_2(ParticleSystemScene::onTouchBegan, this);		//加入觸碰開始事件
	_listener1->onTouchMoved = CC_CALLBACK_2(ParticleSystemScene::onTouchMoved, this);		//加入觸碰移動事件
	_listener1->onTouchEnded = CC_CALLBACK_2(ParticleSystemScene::onTouchEnded, this);		//加入觸碰離開事件

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener1, this);	//加入剛創建的事件聆聽器
	this->schedule(CC_SCHEDULE_SELECTOR(ParticleSystemScene::doStep));

    return true;
}

void ParticleSystemScene::doStep(float dt)
{
	_ParticleControl.doStep(dt);
}

bool ParticleSystemScene::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)//觸碰開始事件
{
	Point touchLoc = pTouch->getLocation();
	//顯示 Emitter 時，可拖曳該圖式
	if( _bEmitterOn ) _EmitterSprite->touchesBegan(touchLoc);
	// 沒有顯示 Emitter，而且沒有按在 Emitter 切換按鈕上，才讓 touch 可以點選顯示分子
	if ( !_emitterSwitchBtn->touchesBegan(touchLoc) &&
		!_fireworkSwitchBtn->touchesBegan(touchLoc) && 
		!_rollSwitchBtn->touchesBegan(touchLoc) && 
		!_waterballSwitchBtn->touchesBegan(touchLoc) && 
		!_bombSwitchBtn->touchesBegan(touchLoc) && !_bEmitterOn ) _ParticleControl.onTouchesBegan(touchLoc);

	for (int i = 0; i < 7; i++) {
		btn[i].isTouched(touchLoc);
	}

	return true;
}

void  ParticleSystemScene::onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //觸碰移動事件
{
	Point touchLoc = pTouch->getLocation();	
	if (_bEmitterOn) {
		if (_EmitterSprite->touchesMoved(touchLoc)) // 移動並更新 Emitter 的位置
			_ParticleControl._emitterPt = _EmitterSprite->getLoc();
	}
	// 沒有顯示 Emitter，而且沒有按在 Emitter 切換按鈕上，才讓 touch 可以點選顯示分子
	if ( !_emitterSwitchBtn->touchesMoved(touchLoc) &&
		!_fireworkSwitchBtn->touchesMoved(touchLoc) && 
		!_rollSwitchBtn->touchesMoved(touchLoc) && 
		!_waterballSwitchBtn->touchesMoved(touchLoc) &&
		!_bombSwitchBtn->touchesMoved(touchLoc) && !_bEmitterOn ) _ParticleControl.onTouchesMoved(touchLoc);

	for (int i = 0; i < 7; i++) {
		btn[i].isLeave(touchLoc);
	}
}

void  ParticleSystemScene::onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //觸碰結束事件 
{
	Point touchLoc = pTouch->getLocation();
	if (_bEmitterOn) {
		if (_EmitterSprite->touchesEnded(touchLoc)) {
			//_ParticleControl._emitterPt = _EmitterSprite->getLoc();
			//_ParticleControl.em = _ParticleControl._emitterPt - Vec2(0,450);
		}
	}

	// 點在 Emitter 切換的圖示上，進行必要的狀態改變
	if (_emitterSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _emitterSwitchBtn->getStatus();
		if ( _bEmitterOn ) { // 顯示 Emitter 圖示
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
			}
			_ParticleControl._emitterPt = _EmitterSprite->getLoc();
			_nowbtn = _emitterSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl.em = _ParticleControl._emitterPt;
			_ParticleControl._iType = 99;
		}
		else { // 關閉 Emitter 圖示
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // 更新控制系統中的 Emitter 狀態
	}

	if (_fireworkSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _fireworkSwitchBtn->getStatus();
		if (_bEmitterOn) { // 顯示 Emitter 圖示
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
				
			}
			_nowbtn = _fireworkSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl._iType = 100;
			_ParticleControl.em = _ParticleControl._emitterPt - Vec2(0, 450);
		}
		else { // 關閉 Emitter 圖示
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // 更新控制系統中的 Emitter 狀態
	}

	if (_rollSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _rollSwitchBtn->getStatus();
		if (_bEmitterOn) { // 顯示 Emitter 圖示
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
				
			}
			_nowbtn = _rollSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl.em = _ParticleControl._emitterPt;
			_ParticleControl._iType = 101;
		}
		else { // 關閉 Emitter 圖示
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // 更新控制系統中的 Emitter 狀態
	}

	if (_waterballSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _waterballSwitchBtn->getStatus();
		if (_bEmitterOn) { // 顯示 Emitter 圖示
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
				
			}
			_nowbtn = _waterballSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl.em = _ParticleControl._emitterPt;
			_ParticleControl._iType =102;
		}
		else { // 關閉 Emitter 圖示
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // 更新控制系統中的 Emitter 狀態
	}

	if (_bombSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _bombSwitchBtn->getStatus();
		if (_bEmitterOn) { // 顯示 Emitter 圖示
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
				
			}
			_nowbtn = _bombSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl._iType = 103;
			_ParticleControl.em = _ParticleControl._emitterPt + Vec2(0, 450);
			//_ParticleControl._fSpread = 13.0f;
			_ParticleControl._fElpasedTime = 0;
		}
		else { // 關閉 Emitter 圖示
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // 更新控制系統中的 Emitter 狀態
	}

	if (!_emitterSwitchBtn->touchesMoved(touchLoc) && !_bEmitterOn) _ParticleControl.onTouchesEnded(touchLoc);

	for (int i = 0; i < 7; i++) {
		if (btn[i].isUsed()) {
			if (i != _iUsedBtnNum) {
				btn[_iUsedBtnNum].setUnuse();
				_iUsedBtnNum = i;
				//_ParticleControl.setParticlesName(btn[_iUsedBtnNum]._SpriteName);
				_ParticleControl.setName(btn[_iUsedBtnNum]._SpriteName);
			}
			
		}
	}
}

void ParticleSystemScene::GravityEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		float fGravity = (-50.0f + percent) / 5.0f;
		_GravityBMValue->setString(StringUtils::format("%2.1f", fGravity));
		_ParticleControl.setGravity(fGravity);
	}
}

void ParticleSystemScene::SpreadEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fSpread = percent*1.8f; // 0 到 180 之間
		_SpreadBMValue->setString(StringUtils::format("%2.1f", fSpread));
		_ParticleControl._fSpread = fSpread;
	}
}

void ParticleSystemScene::DirectionEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fDir = percent*3.6f; // 0 到 360 之間
		_DirectionBMValue->setString(StringUtils::format("%2.1f", fDir));
		_ParticleControl._fDir = fDir;
	}
}

void ParticleSystemScene::SpinEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fSpin = percent*0.6f; // 0 到 360 之間
		_SpinBMValue->setString(StringUtils::format("%2.1f", fSpin));
		_ParticleControl._fSpin = fSpin;
	}
}

void ParticleSystemScene::OpacityEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fOpacity = percent*2.55; // 0 到 255 之間
		_OpacityBMValue->setString(StringUtils::format("%2.1f", fOpacity));
		_ParticleControl._fOpacity = fOpacity;
	}
}

void ParticleSystemScene::ParticlesEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fParticles = percent*2.0f; // 0 到 200 之間
		_ParticlesBMValue->setString(StringUtils::format("%2.1f", fParticles));
		_ParticleControl._iNumParticles = fParticles;
	}
}


void ParticleSystemScene::SpeedEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fVel = percent*0.4f; // 0 到 40 之間
		_SpeedBMValue->setString(StringUtils::format("%2.1f", fVel));
		_ParticleControl._fVelocity = fVel;
	}
}

void ParticleSystemScene::LifetimeEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fLifetime = percent*0.07f; // 0 到 7 之間
		_LifetimeBMValue->setString(StringUtils::format("%2.1f", fLifetime));
		_ParticleControl._fLifeTime = fLifetime;
	}
}

void ParticleSystemScene::RedEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fRed = percent*2.55f; // 0 到 255 之間
		_RedBMValue->setString(StringUtils::format("%2.1f", fRed));
		_ParticleControl._fRed = fRed;
	}
}

void ParticleSystemScene::GreenEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fGreen = percent*2.55f; // 0 到 255 之間
		_GreenBMValue->setString(StringUtils::format("%2.1f", fGreen));
		_ParticleControl._fGreen = fGreen;
	}
}

void ParticleSystemScene::BlueEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		float fBlue = percent*2.55f; // 0 到 255 之間
		_BlueBMValue->setString(StringUtils::format("%2.1f", fBlue));
		_ParticleControl._fBlue = fBlue;
	}
}

void ParticleSystemScene::WindDirEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		float fwindDir = (-50.0f + percent) / 5.0f*18;  //-180~180
		_WindDirBMValue->setString(StringUtils::format("%2.1f", fwindDir));
		float t = fwindDir * M_PI / 180.0f;
		_ParticleControl.setWindDir(t);

	}
}

void ParticleSystemScene::WindStrEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		float fStr = percent*0.1f; // 0 到 10 之間
		_WindStrBMValue->setString(StringUtils::format("%2.1f", fStr));
		_ParticleControl.setWindStr(fStr);
	}
}

void ParticleSystemScene::TypeEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(sender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		int iType = ((float)percent/25)*2; // 0 到 8 之間
		_TypeBMValue->setString(StringUtils::format("%2d", iType));
		itype = iType;
		if (iType == THUNDER)_ParticleControl._bThunder = 1;
		else _ParticleControl._bThunder = 0;
		if (!_bEmitterOn)  _ParticleControl.setType(iType);
		
	}
}
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
	// �̦n���覡�O�A�H�U�� Slider �ھڳo�̪��]�w�ȡA��ܥX���T���ƭȻP��m
	_ParticleControl.init(*this);
	_ParticleControl._emitterPt = _EmitterSprite->getLoc();
	_ParticleControl._fDir = 90.0;
	_ParticleControl._iNumParticles = 100;
	_ParticleControl._iGenParticles = 0;
	_ParticleControl._fRed = 255;
	_ParticleControl._fGreen = 255;
	_ParticleControl._fBlue = 255;
	_ParticleControl._fSpread = 180.0f;
	_ParticleControl._fVelocity = 2.5;	 // ���l�����}�t��
	_ParticleControl._fLifeTime = 3.5f;	 // ���l���s���ɶ�
	_ParticleControl._fSpin = 0;
	_ParticleControl._fGravity = 0;
	_ParticleControl._fOpacity = 255;
	_ParticleControl._fElpasedTime = 0;
	_ParticleControl._cName = "flare.png";
	_ParticleControl.setType(STAY_FOR_TWOSECONDS); // ���l�B�ʪ����A�A�w�]�� 0
	_ParticleControl._fWindDir = 0; // ���d�ҨS����@�����\��
	_ParticleControl._fWindStr = 0;

	// ���l���i�վ�Ѽƪ����ʶb
	// Slider of Gravity
	auto *GravitySlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Gravity"));
	GravitySlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::GravityEvent, this));
	GravitySlider->setMaxPercent(100); 	// �N 0 �� 100 ������ -10 �� 10 ����
	_GravityBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("GravityBMFont");

	// Slider of Spread
	auto *SpreadSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Spread"));
	SpreadSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::SpreadEvent, this));
	SpreadSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 180 ����
	_SpreadBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("SpreadBMFont");

	// Slider of Direction
	auto *DirectionSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Direction"));
	DirectionSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::DirectionEvent, this));
	DirectionSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 180 ����
	_DirectionBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("DirectionBMFont");

	// Slider of Spin
	auto *SpinSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Spin"));
	SpinSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::SpinEvent, this));
	SpinSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 360 ����
	_SpinBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("SpinBMFont");

	// Slider of Opacity
	auto *OpacitySlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Opacity"));
	OpacitySlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::OpacityEvent, this));
	OpacitySlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 100 ����
	_OpacityBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("OpacityBMFont");

	// Slider of Particles
	auto *ParticlesSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Particles"));
	ParticlesSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::ParticlesEvent, this));
	ParticlesSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 200 ����
	_ParticlesBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("ParticlesBMFont");

	// Slider of Speed
	auto *SpeedSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Speed"));
	SpeedSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::SpeedEvent, this));
	SpeedSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 40 ����
	_SpeedBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("SpeedBMFont");

	// Slider of Lifetime
	auto *LifetimeSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Lifetime"));
	LifetimeSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::LifetimeEvent, this));
	LifetimeSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 7 ����
	_LifetimeBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("LifetimeBMFont");

	// Slider of Red
	auto *RedSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Red"));
	RedSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::RedEvent, this));
	RedSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 255 ����
	_RedBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("RedBMFont");

	// Slider of Green
	auto *GreenSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Green"));
	GreenSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::GreenEvent, this));
	GreenSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 255 ����
	_GreenBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("GreenBMFont");

	// Slider of Blue
	auto *BlueSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Blue"));
	BlueSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::BlueEvent, this));
	BlueSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 255 ����
	_BlueBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("BlueBMFont");

	// Slider of windDir
	auto *WindDirSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_WindDir"));
	WindDirSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::WindDirEvent, this));
	WindDirSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ -180 �� 180 ����
	_WindDirBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("WindDirBMFont");

	// Slider of windStr
	auto *WindStrSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_WindStr"));
	WindStrSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::WindStrEvent, this));
	WindStrSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 10 ����
	_WindStrBMValue = (cocos2d::ui::TextBMFont *)rootNode->getChildByName("WindStrBMFont");


	// Slider of Type
	auto *TypeSlider = (cocos2d::ui::Slider *)(rootNode->getChildByName("Slider_Type"));
	TypeSlider->addEventListener(CC_CALLBACK_2(ParticleSystemScene::TypeEvent, this));
	TypeSlider->setMaxPercent(100); 	// �N 0 �� 100 ������ 0 �� 360 ����
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

	_listener1 = EventListenerTouchOneByOne::create();	//�Ыؤ@�Ӥ@��@���ƥ��ť��
	_listener1->onTouchBegan = CC_CALLBACK_2(ParticleSystemScene::onTouchBegan, this);		//�[�JĲ�I�}�l�ƥ�
	_listener1->onTouchMoved = CC_CALLBACK_2(ParticleSystemScene::onTouchMoved, this);		//�[�JĲ�I���ʨƥ�
	_listener1->onTouchEnded = CC_CALLBACK_2(ParticleSystemScene::onTouchEnded, this);		//�[�JĲ�I���}�ƥ�

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener1, this);	//�[�J��Ыت��ƥ��ť��
	this->schedule(CC_SCHEDULE_SELECTOR(ParticleSystemScene::doStep));

    return true;
}

void ParticleSystemScene::doStep(float dt)
{
	_ParticleControl.doStep(dt);
}

bool ParticleSystemScene::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)//Ĳ�I�}�l�ƥ�
{
	Point touchLoc = pTouch->getLocation();
	//��� Emitter �ɡA�i�즲�ӹϦ�
	if( _bEmitterOn ) _EmitterSprite->touchesBegan(touchLoc);
	// �S����� Emitter�A�ӥB�S�����b Emitter �������s�W�A�~�� touch �i�H�I����ܤ��l
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

void  ParticleSystemScene::onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //Ĳ�I���ʨƥ�
{
	Point touchLoc = pTouch->getLocation();	
	if (_bEmitterOn) {
		if (_EmitterSprite->touchesMoved(touchLoc)) // ���ʨç�s Emitter ����m
			_ParticleControl._emitterPt = _EmitterSprite->getLoc();
	}
	// �S����� Emitter�A�ӥB�S�����b Emitter �������s�W�A�~�� touch �i�H�I����ܤ��l
	if ( !_emitterSwitchBtn->touchesMoved(touchLoc) &&
		!_fireworkSwitchBtn->touchesMoved(touchLoc) && 
		!_rollSwitchBtn->touchesMoved(touchLoc) && 
		!_waterballSwitchBtn->touchesMoved(touchLoc) &&
		!_bombSwitchBtn->touchesMoved(touchLoc) && !_bEmitterOn ) _ParticleControl.onTouchesMoved(touchLoc);

	for (int i = 0; i < 7; i++) {
		btn[i].isLeave(touchLoc);
	}
}

void  ParticleSystemScene::onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //Ĳ�I�����ƥ� 
{
	Point touchLoc = pTouch->getLocation();
	if (_bEmitterOn) {
		if (_EmitterSprite->touchesEnded(touchLoc)) {
			//_ParticleControl._emitterPt = _EmitterSprite->getLoc();
			//_ParticleControl.em = _ParticleControl._emitterPt - Vec2(0,450);
		}
	}

	// �I�b Emitter �������ϥܤW�A�i�楲�n�����A����
	if (_emitterSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _emitterSwitchBtn->getStatus();
		if ( _bEmitterOn ) { // ��� Emitter �ϥ�
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
			}
			_ParticleControl._emitterPt = _EmitterSprite->getLoc();
			_nowbtn = _emitterSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl.em = _ParticleControl._emitterPt;
			_ParticleControl._iType = 99;
		}
		else { // ���� Emitter �ϥ�
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // ��s����t�Τ��� Emitter ���A
	}

	if (_fireworkSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _fireworkSwitchBtn->getStatus();
		if (_bEmitterOn) { // ��� Emitter �ϥ�
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
				
			}
			_nowbtn = _fireworkSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl._iType = 100;
			_ParticleControl.em = _ParticleControl._emitterPt - Vec2(0, 450);
		}
		else { // ���� Emitter �ϥ�
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // ��s����t�Τ��� Emitter ���A
	}

	if (_rollSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _rollSwitchBtn->getStatus();
		if (_bEmitterOn) { // ��� Emitter �ϥ�
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
				
			}
			_nowbtn = _rollSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl.em = _ParticleControl._emitterPt;
			_ParticleControl._iType = 101;
		}
		else { // ���� Emitter �ϥ�
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // ��s����t�Τ��� Emitter ���A
	}

	if (_waterballSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _waterballSwitchBtn->getStatus();
		if (_bEmitterOn) { // ��� Emitter �ϥ�
			if (_nowbtn) {
				_nowbtn->setStatusfalse();
				
			}
			_nowbtn = _waterballSwitchBtn;
			_EmitterSprite->setVisible(true);
			_ParticleControl.em = _ParticleControl._emitterPt;
			_ParticleControl._iType =102;
		}
		else { // ���� Emitter �ϥ�
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // ��s����t�Τ��� Emitter ���A
	}

	if (_bombSwitchBtn->touchesEnded(touchLoc))
	{
		_bEmitterOn = _bombSwitchBtn->getStatus();
		if (_bEmitterOn) { // ��� Emitter �ϥ�
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
		else { // ���� Emitter �ϥ�
			_EmitterSprite->setVisible(false);
			_ParticleControl._iType = itype;
			_nowbtn = NULL;
		}
		_ParticleControl.setEmitter(_bEmitterOn); // ��s����t�Τ��� Emitter ���A
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
		float fSpread = percent*1.8f; // 0 �� 180 ����
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
		float fDir = percent*3.6f; // 0 �� 360 ����
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
		float fSpin = percent*0.6f; // 0 �� 360 ����
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
		float fOpacity = percent*2.55; // 0 �� 255 ����
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
		float fParticles = percent*2.0f; // 0 �� 200 ����
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
		float fVel = percent*0.4f; // 0 �� 40 ����
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
		float fLifetime = percent*0.07f; // 0 �� 7 ����
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
		float fRed = percent*2.55f; // 0 �� 255 ����
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
		float fGreen = percent*2.55f; // 0 �� 255 ����
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
		float fBlue = percent*2.55f; // 0 �� 255 ����
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
		float fStr = percent*0.1f; // 0 �� 10 ����
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
		int iType = ((float)percent/25)*2; // 0 �� 8 ����
		_TypeBMValue->setString(StringUtils::format("%2d", iType));
		itype = iType;
		if (iType == THUNDER)_ParticleControl._bThunder = 1;
		else _ParticleControl._bThunder = 0;
		if (!_bEmitterOn)  _ParticleControl.setType(iType);
		
	}
}
#include "CParticleSystem.h"

#define NUMBER_PARTICLES 1000 // 預設一次取得 1000 個 Particles

USING_NS_CC;

CParticleSystem::CParticleSystem()
{
	_fGravity = 0;
	_bEmitterOn = false;
}

void CParticleSystem::setEmitter(bool bEm)
{
	_bEmitterOn = bEm;
}

void CParticleSystem::init(cocos2d::Layer &inlayer)
{
	_monster = Sprite::createWithSpriteFrameName("monster.png");
	_monster->setPosition(mouse);
	_monster->setOpacity(180);
	_monster->setVisible(false);
	inlayer.addChild(_monster, 1);

	_iFree = NUMBER_PARTICLES;
	_iInUsed = 0;
	_pParticles = new CParticle[NUMBER_PARTICLES]; // 取得所需要的 particle 空間
	 // 讀入儲存多張圖片的 plist 檔
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("particletexture.plist");
	for (int i = 0; i < NUMBER_PARTICLES; i++) {
		_pParticles[i].setParticle("flare.png", inlayer);
		_FreeList.push_front(&_pParticles[i]);
	}

}

void CParticleSystem::doStep(float dt)
{
	CParticle *get;
	list <CParticle *>::iterator it;
	if (!_bEmitterOn) {
		if (_bThunder == 1) {
			if (_iFree != 0) {
				Vec2 pos;
				pos.x = rand() % 900;
				pos.y = 550 + rand() % 170;
				get = _FreeList.front();
				get->setBehavior(THUNDERCLOUD);
				get->setPosition(pos);
				get->setParticleName("cloud.png");
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else if (_bThunder == 2) {
			if (_iFree >= 60) {
				for (int i = 0; i < 60; i++) {
					Vec2 pos;
					pos.x = rand() % 900;
					pos.y = 550 + rand() % 170;
					get = _FreeList.front();
					get->setBehavior(THUNDERCLOUD);
					get->setPosition(pos);
					get->setParticleName("cloud.png");
					_FreeList.pop_front();
					_InUsedList.push_front(get);
					_iFree--; _iInUsed++;
				}
				_bThunder = 1;
			}
		}
		else if (_bmagic == 1) {
		//	CCLOG("%f", _fElpasedTime);
			// 從 _FreeList 取得一個分子給放到 _InUsed
			if (_iFree > 100) {
				for (int i = 0; i < 10; i++) {
					get = _FreeList.front();
					get->setBehavior(MAGIC);
					get->setPosition(mouse);
					get->setGravity(_fGravity);
					get->setParticleName(name);
					_FreeList.pop_front();
					_InUsedList.push_front(get);
					_iFree--; _iInUsed++;
				}
				Color3B color = Color3B(rand() % 255, rand() % 255, rand() % 255);
				for (int i = 0; i < 50; i++) {
					Vec2 pos;
					float r = (float)i / 50 * M_PI * 2;
					pos.x = mouse.x + 200 * cosf(r);
					pos.y = mouse.y + 200 * sinf(r) / 2;
					get = _FreeList.front();
					get->setBehavior(EMITTER_DEFAULT);
					get->setVelocity(2);
					get->setLifetime(0.5);
					get->setGravity(0);
					get->setPosition(pos);
					get->setColor(color);
					get->setSpin(0);
					get->setOpacity(200);
					get->setSize(0.025f);
					get->setParticleName(name);
					get->setWindDir(0);
					get->setWindStr(0);
					Vec2 vdir(cosf(r) * 2.5, 7);
					get->setDirection(vdir);
					_FreeList.pop_front();
					_InUsedList.push_front(get);
					_iFree--; _iInUsed++;
				}
			}
			if (_fElpasedTime > 12) {
				_monster->setVisible(false);
				_fElpasedTime = 0;
			}
			else if (_fElpasedTime > 10) {
				_monster->setPosition(mouse+Vec2(10,20));
				_monster->setVisible(true);
			}
		}
		else if (_bBalloon == 1) {
			if (_iFree != 0) {
				balloonSize += dt*0.5;
				get = _FreeList.front();
				get->setBehavior(BALLOON);
				get->setPosition(mouse);
				get->setGravity(_fGravity);
				get->setLifetime(0.1f);
				get->setVelocity(0);
				get->setSize(balloonSize);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else if (_bBalloon == 2) {
			if (_iFree > 10) {
				for (int i = 0; i < 10; i++) {
					get = _FreeList.front();
					get->setBehavior(BALLOON);
					get->setPosition(mouse);
					get->setGravity(_fGravity);
					get->setLifetime(10.0f);
					get->setSize(balloonSize);
					get->setVelocity(1);
					get->setWindDir(_fWindDir);
					get->setWindStr(_fWindStr);
					Vec2 vdir(0, 3.0f);
					get->setDirection(vdir);
					_FreeList.pop_front();
					_InUsedList.push_front(get);
					_iFree--; _iInUsed++;
				}
				_bBalloon = 0;
				balloonSize = 0.125f;
			}
		}
	}

	if (_bEmitterOn) { // 根據 Emitter 設定的相關參數，產生相對應的分子
					   // 先計算在累加
		switch (_iType) {
			case EMITTER_DEFAULT:
				if (_iFree != 0) {
					int n = (int)(_fElpasedTime * _iNumParticles); // 到目前為止應該產生的分子個數
					if (n > _iGenParticles) {  // 產生的分子個數不足，產生到 n 個分子
						for (int i = 0; i < n - _iGenParticles; i++) {
							// 根據 Emitter 的相關參數，設定所產生分子的參數
							if (_iFree != 0) {
								get = _FreeList.front();
								get->setBehavior(EMITTER_DEFAULT);
								get->setVelocity(_fVelocity);
								get->setLifetime(_fLifeTime);
								get->setGravity(_fGravity);
								get->setPosition(_emitterPt);
								get->setColor(Color3B(_fRed, _fGreen, _fBlue));
								get->setSpin(_fSpin);
								get->setOpacity(_fOpacity);
								get->setSize(0.125f);
								//get->setParticleName(_cName);
								get->setParticleName(name);
								get->setWindDir(_fWindDir);
								get->setWindStr(_fWindStr);
								// 根據 _fSpread 與 _vDir 產生方向
								float t = (rand() % 1001) / 1000.0f; // 產生介於 0 到 1 間的數
								t = _fSpread - t * _fSpread * 2; //  產生的角度，轉成弧度
								t = (_fDir + t)* M_PI / 180.0f;
								Vec2 vdir(cosf(t), sinf(t));
								get->setDirection(vdir);
								_FreeList.pop_front();
								_InUsedList.push_front(get);
								_iFree--; _iInUsed++;
							}
						}
						_iGenParticles = n; // 目前已經產生 n 個分子

						if (_fElpasedTime >= 1.0f) {
							_fElpasedTime -= 1.0f;
							if (_iGenParticles >= _iNumParticles) _iGenParticles -= _iNumParticles;
							else _iGenParticles = 0;
						}

					}
				}
				break;
			case FIREWORK:
				if (_iFree != 0) {
					if (em.y <= _emitterPt.y) {
						for (int i = 0; i < 55; i++) {
							// 根據 Emitter 的相關參數，設定所產生分子的參數
							if (_iFree != 0) {
								get = _FreeList.front();
								get->setBehavior(EMITTER_DEFAULT);
								get->setVelocity(3.5);
								get->setLifetime(0.5);
								get->setGravity(-1.4);
								get->setPosition(em);
								get->setColor(Color3B(255, 75, 75));
								get->setSpin(0);
								get->setOpacity(255);
								get->setSize(0.125f);
								get->setParticleName("spark.png");
								get->setWindDir(0);
								get->setWindStr(0);
								// 根據 _fSpread 與 _vDir 產生方向
								float t = (rand() % 1001) / 1000.0f; // 產生介於 0 到 1 間的數
								t = 14.5f - t * 14.5f * 2; //  產生的角度，轉成弧度
								t = (270 + t)* M_PI / 180.0f;
								Vec2 vdir(cosf(t), sinf(t));
								get->setDirection(vdir);
								_FreeList.pop_front();
								_InUsedList.push_front(get);
								_iFree--; _iInUsed++;
							}
						}
						em.y += 6;
					}
					else {
						int k = rand() % 3;
						if (k==0 &&_iFree > 100) {
							for (int i = 0; i < 100; i++) {
								get = _FreeList.front();
								get->setBehavior(EXPLOSION);
								get->setPosition(em);
								get->setGravity(_fGravity);
								get->setParticleName("spark.png");
								_FreeList.pop_front();
								_InUsedList.push_front(get);
								_iFree--; _iInUsed++;
							}
							em = _emitterPt - Vec2(0, 450);
						}
					}
				}
				break;
			case ROLL:		
				if (_iFree != 0) {
					float t = _totalTime* M_PI / 180.0f * 750;
					vdir = Vec2(cosf(t), sinf(t));
					em = _emitterPt + 50 * vdir;
						// 根據 Emitter 的相關參數，設定所產生分子的參數
						if (_iFree != 0) {
							get = _FreeList.front();
							get->setBehavior(ROLL);
							get->setVelocity(2.0f);
							get->setLifetime(_fLifeTime);
							get->setGravity(_fGravity);
							get->setPosition(em);
							get->setColor(Color3B(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1));
							get->setSpin(_fSpin);
							get->setOpacity(255);
							get->setSize(0.125f);
							//get->setParticleName(_cName);
							get->setParticleName(name);
							// 根據 _fSpread 與 _vDir 產生方向
							float t = (rand() % 1001) / 1000.0f; // 產生介於 0 到 1 間的數
							get->setDirection(vdir);
							_FreeList.pop_front();
							_InUsedList.push_front(get);
							_iFree--; _iInUsed++;
						}
				}
				break;
			case WATERBALL:
				if (_iFree != 0) {
					float t = _totalTime * M_PI / 180.0f * 1000;
					Vec2 vfdir = Vec2(cosf(t), sinf(t));
					em = _emitterPt + 150 * vfdir;
					for (int i = 0; i < 10; i++) {
						// 根據 Emitter 的相關參數，設定所產生分子的參數
						if (_iFree != 0) {
							get = _FreeList.front();
							get->setBehavior(WATERBALL);
							get->setVelocity(10.0f);
							get->setLifetime(0.7f);
							get->setGravity(_fGravity);
							get->setPosition(em);
							get->setColor(Color3B(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1));
							get->setSpin(255);
							get->setOpacity(255);
							get->setSize(0.05f);
							get->setEm(_emitterPt);
							//get->setParticleName(_cName);
							get->setParticleName(name);
							get->setRDelayTime(0.05f*i);
							// 根據 _fSpread 與 _vDir 產生方向
							get->setDirection(vfdir);
							_FreeList.pop_front();
							_InUsedList.push_front(get);
							_iFree--; _iInUsed++;
						}


					}
				}
				break;
			case BOMB:
				if (_iFree != 0) {
					if (em.y > 100 && _mode == 0) {
						// 根據 Emitter 的相關參數，設定所產生分子的參數
						if (_iFree != 0) {
							get = _FreeList.front();
							get->setBehavior(EMITTER_DEFAULT);
							get->setVelocity(3.5);
							get->setLifetime(0.5);
							get->setGravity(-1.4);
							get->setPosition(em);
							get->setColor(Color3B(255, 75, 75));
							get->setSpin(0);
							get->setOpacity(255);
							get->setSize(0.125f);
							get->setParticleName("spark.png");
							get->setWindDir(0);
							get->setWindStr(0);
							// 根據 _fSpread 與 _vDir 產生方向
							float t = (rand() % 1001) / 1000.0f; // 產生介於 0 到 1 間的數
							t = 13.0f - t * 13.0f * 2; //  產生的角度，轉成弧度
							t = (90 + t)* M_PI / 180.0f;
							Vec2 vdir(cosf(t), sinf(t));
							get->setDirection(vdir);
							_FreeList.pop_front();
							_InUsedList.push_front(get);
							_iFree--; _iInUsed++;
						}
						em.y -= 6;
					}
					else if ((em.y < 500)) {
						_mode = 1;
						for (int i = 0; i <50; i++) {
							// 根據 Emitter 的相關參數，設定所產生分子的參數
							if (_iFree != 0) {
								float dx = cos((em.y - 75) / 425 * M_PI);
								get = _FreeList.front();
								get->setBehavior(EMITTER_DEFAULT);
								get->setLifetime(2);
								get->setPosition(em + Vec2((i-25)*dx,0));
								get->setEm(_emitterPt);
								get->setVelocity(0);
								get->setGravity(0);
								get->setSpin(0);
								get->setOpacity(255);
								get->setRDelayTime(0);
								get->setColor(Color3B(255, 75, 75));
								get->setDelayTime(0);
								get->setWindStr(0);
								if (em.y > 400) { 
									float size = (2+sin((em.y - 400) / 100 * M_PI)*2);
									get->setSize(size);
								}
								else { get->setSize(1.5f); }
								get->setParticleName("cloud.png");
								// 根據 _fSpread 與 _vDir 產生方向
								float k = (i / 5.0f * 4.0f + 30.0f);
								float t = k* M_PI / 180.0f;
								Vec2 vdir(0,1);
								get->setDirection(vdir);
								_FreeList.pop_front();
								_InUsedList.push_front(get);
								_iFree--; _iInUsed++;
							}
						}
						em.y += 20;
					}
					else if(_iInUsed == 0){
						em = _emitterPt+Vec2(0,450);
						_mode = 0;
					}
					
				}
				break;
			case AIRPLANE:
				if (_bAiroplane == 1) {
					if (_iFree >= 5) {
						_fElpasedTime = 0;
						for (int i = 5; i >0; i--) {
							Vec2 pos;
							pos = Vec2(1280, em.y);
							pos.x += 50 * (i / 2);
							pos.y += 50 * (((i % 2) * 2 - 1)*(i/2));  // y+(15*±2)  y+(15*±1)  y+(15*±0)  
							get = _FreeList.front();
							get->setBehavior(AIRPLANE);
							get->setVelocity((float)1400/3.5);
							get->setLifetime(3.5f);
							get->setGravity(0);
							get->setPosition(pos);
							get->setColor(Color3B(255,255,255));
							get->setSpin(0);
							get->setOpacity(255);
							get->setSize(2.0f);
							get->setParticleName("airplane.png");
							get->setRDelayTime(0);
							// 根據 _fSpread 與 _vDir 產生方向
							get->setDirection(Vec2(-1,0));
							_FreeList.pop_front();
							_InUsedList.push_front(get);
							_iFree--; _iInUsed++;
						}
						_bAiroplane = 2;
					}
				}
				else{
					if (_bAiroplane == 2 && _fElpasedTime < 3.5) {
						if (_iFree >= 5) {
							for (int i = 5; i >0; i--) {
								Vec2 pos;
								pos = Vec2(1300, em.y);
								pos.x += 50 * (i / 2) - _fElpasedTime* 1400.0f / 3.5f;
								pos.y += 50 * (((i % 2) * 2 - 1)*(i / 2));  // y+(15*±2)  y+(15*±1)  y+(15*±0)  
								get = _FreeList.front();
								get->setBehavior(AIRPLANE);
								get->setVelocity(0);
								get->setLifetime(1.5f);
								get->setGravity(0);
								get->setPosition(pos);
								if(i/2 == 2)get->setColor(Color3B(255,0,0));       
								else if(i/2 ==1)get->setColor(Color3B(0, 0, 255)); 
								else get->setColor(Color3B(255, 255, 255));        
								get->setSpin(0);
								get->setOpacity(255);
								get->setSize(0.7f);
								get->setParticleName("cloud.png");
								get->setRDelayTime(0);
								// 根據 _fSpread 與 _vDir 產生方向
								get->setDirection(Vec2(0, 0));
								_FreeList.pop_front();
								_InUsedList.push_front(get);
								_iFree--; _iInUsed++;
							}
						}
					}
					else {
						_fElpasedTime = 0;
						_bAiroplane = 1;
						em = _emitterPt;
					}
				}
				break;
		}

		
	}

	_fElpasedTime += dt;
	_totalTime += dt;
	if (_iInUsed != 0) { // 有分子需要更新時
		for (it = _InUsedList.begin(); it != _InUsedList.end(); ) {
			if ((*it)->doStep(dt)) { // 分子生命週期已經到達
									 // 將目前這一個節點的內容放回 _FreeList
				_FreeList.push_front((*it));
				it = _InUsedList.erase(it); // 移除目前這一個, 
				_iFree++; _iInUsed--;
			}
			else it++;
		}
	}
	//CCLOG("%d", _iInUsed);
}


void CParticleSystem::setGravity(float fGravity)
{
	// 設定目前 particle 是在 inused 的 gravity
	_fGravity = fGravity;
	list <CParticle *>::iterator it;
	if (_iInUsed != 0) { // 有分子需要更新時
		for (it = _InUsedList.begin(); it != _InUsedList.end(); it++) {
			(*it)->setGravity(_fGravity);
		}
	}
}
void CParticleSystem::setWindDir(float fwindd)
{
	// 設定目前 particle 是在 inused 的 gravity
	_fWindDir = fwindd;
	list <CParticle *>::iterator it;
	if (_iInUsed != 0) { // 有分子需要更新時
		for (it = _InUsedList.begin(); it != _InUsedList.end(); it++) {
			(*it)->setWindDir(_fWindDir);
		}
	}
}
void CParticleSystem::setWindStr(float fwinds)
{
	// 設定目前 particle 是在 inused 的 gravity
	_fWindStr = fwinds;
	list <CParticle *>::iterator it;
	if (_iInUsed != 0) { // 有分子需要更新時
		for (it = _InUsedList.begin(); it != _InUsedList.end(); it++) {
			(*it)->setWindStr(_fWindStr);
		}
	}
}


void CParticleSystem::setName(char *c)
{
	name = c;
	
}

CParticleSystem::~CParticleSystem()
{
	if (_iInUsed != 0) _InUsedList.clear(); // 移除所有的 NODE
	if (_iFree != 0) _FreeList.clear();
	delete[] _pParticles; // 釋放所有取得資源
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("particletexture.plist");
}

void CParticleSystem::onTouchesBegan(const cocos2d::CCPoint &touchPoint)
{
	CParticle *get;
	switch (_iType)
	{
	case STAY_FOR_TWOSECONDS:
		// 從 _FreeList 取得一個分子給放到 _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(STAY_FOR_TWOSECONDS);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			get->setParticleName(name);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case RANDOMS_FALLING :
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(RANDOMS_FALLING);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			get->setParticleName(name);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case FREE_FLY:
		// 從 _FreeList 取得一個分子給放到 _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(FREE_FLY);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			get->setParticleName(name);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case EXPLOSION:
		// 從 _FreeList 取得一個分子給放到 _InUsed
		if (_iFree > 100) {
			for (int i = 0; i < 100; i++) {
				get = _FreeList.front();
				get->setBehavior(EXPLOSION);
				get->setPosition(touchPoint);
				get->setGravity(_fGravity);
				get->setParticleName(name);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case HEARTSHAPE:
		// 從 _FreeList 取得一個分子給放到 _InUsed
		if (_iFree > 150) {
			for (int i = 0; i < 150; i++) {
				get = _FreeList.front();
				get->setBehavior(HEARTSHAPE);
				get->setPosition(touchPoint);
				get->setGravity(_fGravity);
				get->setParticleName(name);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case BUTTERFLYSHAPE:
		// 從 _FreeList 取得一個分子給放到 _InUsed
		if (_iFree > 150) {
			for (int i = 0; i < 150; i++) {
				get = _FreeList.front();
				get->setBehavior(BUTTERFLYSHAPE);
				get->setPosition(touchPoint);
				get->setGravity(_fGravity);
				get->setParticleName(name);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case THUNDER:
		if (_iFree > 100) {
			_bThunder = 2;
			for (int i = 0; i < 100; i++) {
				get = _FreeList.front();
				get->setBehavior(THUNDER);
				get->setPosition(touchPoint+Vec2(0,450));
				get->setGravity(_fGravity);
				get->setParticleName(name);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		break;
	case MAGIC:
		_bmagic = 1;
		_fElpasedTime = 0;
		mouse = touchPoint;
		break;
	//case MAGIC:
	//	// 從 _FreeList 取得一個分子給放到 _InUsed
	//	if (_iFree > 100) {
	//		for (int i = 0; i < 50; i++) {
	//			get = _FreeList.front();
	//			get->setBehavior(MAGIC);
	//			get->setPosition(touchPoint);
	//			get->setGravity(_fGravity);
	//			get->setParticleName(name);
	//			_FreeList.pop_front();
	//			_InUsedList.push_front(get);
	//			_iFree--; _iInUsed++;
	//		}
	//		Color3B color = Color3B(rand() % 255, rand() % 255, rand() % 255);
	//		for (int i = 0; i < 50; i++) {
	//			Vec2 pos;
	//			float r = (float)i / 50 * M_PI * 2;
	//			pos.x = touchPoint.x + 200 * cosf(r);
	//			pos.y = touchPoint.y + 200 * sinf(r) / 2;
	//			get = _FreeList.front();
	//			get->setBehavior(EMITTER_DEFAULT);
	//			get->setVelocity(2);
	//			get->setLifetime(0.5);
	//			get->setGravity(0);
	//			get->setPosition(pos);
	//			get->setColor(color);
	//			get->setSpin(0);
	//			get->setOpacity(255);
	//			get->setSize(0.025f);
	//			//get->setParticleName(_cName);
	//			get->setParticleName(name);
	//			get->setWindDir(0);
	//			get->setWindStr(0);
	//			Vec2 vdir(cosf(r)*3, 4);
	//			get->setDirection(vdir);
	//			_FreeList.pop_front();
	//			_InUsedList.push_front(get);
	//			_iFree--; _iInUsed++;
	//		}
	//	}
	//	else return;// 沒有分子, 所以就不提供
	//	break;
	case BALLOON:
		balloonSize = 0.5f;
		_bBalloon = 1;
		mouse = touchPoint;
		break;
	}
}

void CParticleSystem::onTouchesMoved(const cocos2d::CCPoint &touchPoint)
{
	CParticle *get;
	switch (_iType)
	{
	case STAY_FOR_TWOSECONDS:
		// 從 _FreeList 取得一個分子給放到 _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(STAY_FOR_TWOSECONDS);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			get->setParticleName(name);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case RANDOMS_FALLING:
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(RANDOMS_FALLING);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			get->setParticleName(name);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// 沒有分子, 所以就不提供
		break;
	case FREE_FLY:
		// 從 _FreeList 取得一個分子給放到 _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(FREE_FLY);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			get->setParticleName(name);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// 沒有分子, 所以就不提供
		break;
	}
	mouse = touchPoint;
}

void CParticleSystem::onTouchesEnded(const cocos2d::CCPoint &touchPoint)
{
	CParticle *get;
	mouse = touchPoint;
	switch (_iType)
	{
		case MAGIC:
			_bmagic = 0;
			_monster->setVisible(false);
			_fElpasedTime = 0;
			break;
		case BALLOON:
			_bBalloon = 2;
			break;

	}
}
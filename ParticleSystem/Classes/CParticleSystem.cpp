#include "CParticleSystem.h"

#define NUMBER_PARTICLES 1000 // �w�]�@�����o 1000 �� Particles

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
	_iFree = NUMBER_PARTICLES;
	_iInUsed = 0;
	_pParticles = new CParticle[NUMBER_PARTICLES]; // ���o�һݭn�� particle �Ŷ�
	 // Ū�J�x�s�h�i�Ϥ��� plist ��
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
	if (_bBalloon == 1) {
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

	if (_bEmitterOn) { // �ھ� Emitter �]�w�������ѼơA���ͬ۹��������l
					   // ���p��b�֥[
		switch (_iType) {
			case EMITTER_DEFAULT:
				if (_iFree != 0) {
					int n = (int)(_fElpasedTime * _iNumParticles); // ��ثe�������Ӳ��ͪ����l�Ӽ�
					if (n > _iGenParticles) {  // ���ͪ����l�ӼƤ����A���ͨ� n �Ӥ��l
						for (int i = 0; i < n - _iGenParticles; i++) {
							// �ھ� Emitter �������ѼơA�]�w�Ҳ��ͤ��l���Ѽ�
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
								get->setParticleName(_cName);
								get->setWindDir(_fWindDir);
								get->setWindStr(_fWindStr);
								// �ھ� _fSpread �P _vDir ���ͤ�V
								float t = (rand() % 1001) / 1000.0f; // ���ͤ��� 0 �� 1 ������
								t = _fSpread - t * _fSpread * 2; //  ���ͪ����סA�ন����
								t = (_fDir + t)* M_PI / 180.0f;
								Vec2 vdir(cosf(t), sinf(t));
								get->setDirection(vdir);
								_FreeList.pop_front();
								_InUsedList.push_front(get);
								_iFree--; _iInUsed++;
							}
						}
						_iGenParticles = n; // �ثe�w�g���� n �Ӥ��l

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
							// �ھ� Emitter �������ѼơA�]�w�Ҳ��ͤ��l���Ѽ�
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
								// �ھ� _fSpread �P _vDir ���ͤ�V
								float t = (rand() % 1001) / 1000.0f; // ���ͤ��� 0 �� 1 ������
								t = 14.5f - t * 14.5f * 2; //  ���ͪ����סA�ন����
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
						if (_iFree > 100) {
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
						// �ھ� Emitter �������ѼơA�]�w�Ҳ��ͤ��l���Ѽ�
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
							get->setParticleName(_cName);
							// �ھ� _fSpread �P _vDir ���ͤ�V
							float t = (rand() % 1001) / 1000.0f; // ���ͤ��� 0 �� 1 ������
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
						// �ھ� Emitter �������ѼơA�]�w�Ҳ��ͤ��l���Ѽ�
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
							get->setParticleName(_cName);
							get->setRDelayTime(0.05f*i);
							// �ھ� _fSpread �P _vDir ���ͤ�V
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
						// �ھ� Emitter �������ѼơA�]�w�Ҳ��ͤ��l���Ѽ�
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
							// �ھ� _fSpread �P _vDir ���ͤ�V
							float t = (rand() % 1001) / 1000.0f; // ���ͤ��� 0 �� 1 ������
							t = 13.0f - t * 13.0f * 2; //  ���ͪ����סA�ন����
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
							// �ھ� Emitter �������ѼơA�]�w�Ҳ��ͤ��l���Ѽ�
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
								if (em.y > 400) { 
									float size = (2+sin((em.y - 400) / 100 * M_PI)*2);
									get->setSize(size);
									CCLOG("%f", size);
								}
								else { get->setSize(1.5f); }
								get->setParticleName("cloud.png");
								// �ھ� _fSpread �P _vDir ���ͤ�V
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
		}

		_fElpasedTime += dt;
		_totalTime += dt;
	}

	if (_iInUsed != 0) { // �����l�ݭn��s��
		for (it = _InUsedList.begin(); it != _InUsedList.end(); ) {
			if ((*it)->doStep(dt)) { // ���l�ͩR�g���w�g��F
									 // �N�ثe�o�@�Ӹ`�I�����e��^ _FreeList
				_FreeList.push_front((*it));
				it = _InUsedList.erase(it); // �����ثe�o�@��, 
				_iFree++; _iInUsed--;
			}
			else it++;
		}
	}
	//CCLOG("%d", _iInUsed);
}


void CParticleSystem::setGravity(float fGravity)
{
	// �]�w�ثe particle �O�b inused �� gravity
	_fGravity = fGravity;
	list <CParticle *>::iterator it;
	if (_iInUsed != 0) { // �����l�ݭn��s��
		for (it = _InUsedList.begin(); it != _InUsedList.end(); it++) {
			(*it)->setGravity(_fGravity);
		}
	}
}
void CParticleSystem::setWindDir(float fwindd)
{
	// �]�w�ثe particle �O�b inused �� gravity
	_fWindDir = fwindd;
	list <CParticle *>::iterator it;
	if (_iInUsed != 0) { // �����l�ݭn��s��
		for (it = _InUsedList.begin(); it != _InUsedList.end(); it++) {
			(*it)->setWindDir(_fWindDir);
		}
	}
}
void CParticleSystem::setWindStr(float fwinds)
{
	// �]�w�ثe particle �O�b inused �� gravity
	_fWindStr = fwinds;
	list <CParticle *>::iterator it;
	if (_iInUsed != 0) { // �����l�ݭn��s��
		for (it = _InUsedList.begin(); it != _InUsedList.end(); it++) {
			(*it)->setWindStr(_fWindStr);
		}
	}
}

void CParticleSystem::setParticlesName(char *c)
{
	_cName = c;
	for (int i = 0; i < NUMBER_PARTICLES; i++) {
		_pParticles[i].setParticleName(_cName);

	}
}

CParticleSystem::~CParticleSystem()
{
	if (_iInUsed != 0) _InUsedList.clear(); // �����Ҧ��� NODE
	if (_iFree != 0) _FreeList.clear();
	delete[] _pParticles; // ����Ҧ����o�귽
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("particletexture.plist");
}

void CParticleSystem::onTouchesBegan(const cocos2d::CCPoint &touchPoint)
{
	CParticle *get;
	switch (_iType)
	{
	case STAY_FOR_TWOSECONDS:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(STAY_FOR_TWOSECONDS);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case RANDOMS_FALLING :
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(RANDOMS_FALLING);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case FREE_FLY:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(FREE_FLY);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case EXPLOSION:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree > 100) {
			for (int i = 0; i < 100; i++) {
				get = _FreeList.front();
				get->setBehavior(EXPLOSION);
				get->setPosition(touchPoint);
				get->setGravity(_fGravity);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case HEARTSHAPE:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree > 150) {
			for (int i = 0; i < 150; i++) {
				get = _FreeList.front();
				get->setBehavior(HEARTSHAPE);
				get->setPosition(touchPoint);
				get->setGravity(_fGravity);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case BUTTERFLYSHAPE:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree > 150) {
			for (int i = 0; i < 150; i++) {
				get = _FreeList.front();
				get->setBehavior(BUTTERFLYSHAPE);
				get->setPosition(touchPoint);
				get->setGravity(_fGravity);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case THUNDER:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree > 60) {
			float r = rand()%51-25;
			r = r*M_PI/180;
			int k = rand() % 2*2-1;
			for (int i = 0; i < 60; i++) {
				Vec2 move = Vec2(0, 450);
				if (i < 20) {
					move.x -= (100 / 20 * i)*k;
					move.y -= (150 / 20 * i );
				}
				else if (i < 40) {
					move.x += (-100 +(200 / 20 * (i-19)))*k;
					move.y -= (150 + (75 / 20 * (i - 19)));
				}
				else {
					move.x -= (-90 + (100 / 20 * (i - 39)))*k;
					move.y -= (213 + (225 / 20 * (i - 39)));
				}
				move.x = cosf(r)*move.x + sinf(r)*move.y;
				move.y = -sinf(r)*move.x + cosf(r)*move.y;
				get = _FreeList.front();
				get->setBehavior(THUNDER);
				get->setPosition(touchPoint + move);
				get->setGravity(_fGravity);
				get->setRDelayTime((float)i/150);
				get->setSize(4-(float)i/16);
				get->setDelayTime(0);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case MAGIC:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree > 100) {
			for (int i = 0; i < 50; i++) {
				get = _FreeList.front();
				get->setBehavior(MAGIC);
				get->setPosition(touchPoint);
				get->setGravity(_fGravity);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
			Color3B color = Color3B(rand() % 255, rand() % 255, rand() % 255);
			for (int i = 0; i < 50; i++) {
				Vec2 pos;
				float r = (float)i / 50 * M_PI * 2;
				pos.x = touchPoint.x + 200 * cosf(r);
				pos.y = touchPoint.y + 200 * sinf(r) / 2;
				get = _FreeList.front();
				get->setBehavior(EMITTER_DEFAULT);
				get->setVelocity(2);
				get->setLifetime(0.5);
				get->setGravity(0);
				get->setPosition(pos);
				get->setColor(color);
				get->setSpin(0);
				get->setOpacity(255);
				get->setSize(0.025f);
				get->setParticleName(_cName);
				get->setWindDir(0);
				get->setWindStr(0);
				Vec2 vdir(cosf(r)*3, 4);
				get->setDirection(vdir);
				_FreeList.pop_front();
				_InUsedList.push_front(get);
				_iFree--; _iInUsed++;
			}
		}
		else return;// �S�����l, �ҥH�N������
		break;
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
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(STAY_FOR_TWOSECONDS);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case RANDOMS_FALLING:
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(RANDOMS_FALLING);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// �S�����l, �ҥH�N������
		break;
	case FREE_FLY:
		// �q _FreeList ���o�@�Ӥ��l����� _InUsed
		if (_iFree != 0) {
			get = _FreeList.front();
			get->setBehavior(FREE_FLY);
			get->setPosition(touchPoint);
			get->setGravity(_fGravity);
			_FreeList.pop_front();
			_InUsedList.push_front(get);
			_iFree--; _iInUsed++;
		}
		else return;// �S�����l, �ҥH�N������
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
		case BALLOON:
			_bBalloon = 2;
			break;

	}
}
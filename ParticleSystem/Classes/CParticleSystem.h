#ifndef __CPARTICLE_SYSTEM_H__
#define __CPARTICLE_SYSTEM_H__

#include "cocos2d.h"
#include "CParticle.h"
#include <list>

using namespace std;


class CParticleSystem
{
private:
	CParticle* _pParticles;
	list<CParticle*> _FreeList;
	list<CParticle*> _InUsedList;
	int _iFree;	// �i�Ϊ�  Particle �Ӽ�
	int _iInUsed;  // ���b�ϥΪ� Particle �Ӽ�
	cocos2d::Point vdir;

	bool _bEmitterOn; // Emitter �O�_���
	int fireworkmode = 1;
	float balloonSize=0.125f;
	int _bBalloon = 0;
	int _mode = 0;
	char *name;
	cocos2d::Point mouse;
	cocos2d::Sprite *_monster;
public:
	int _bThunder = 0;
	int _bAiroplane = 0;
	int _bmagic = 0;
	// Emitter �]�w�������Ѽ�
	cocos2d::Point _emitterPt;// Emitter ����m	
	cocos2d::Point em;
	int _iType=0;  // �ثe������l�B�ʼҦ��O���@�ث��A
	float    _fDir; // Emitter ���Q�g��V�A0 �� 360�סA�f�ɰw��V����
	int		_iNumParticles;	// �C�������ͪ����l�Ӽ�
	int     _iGenParticles; // ��ثe����Ҳ��ͪ����l�Ӽ�(�H�C�����)
	float	_fRed, _fGreen, _fBlue;	 // ���l�C��
	float	_fSpread;	 // 0 �� 180 �סA�w�]�� 180 
	float   _fVelocity;	 // ���l�����}�t��
	float   _fLifeTime;	 // ���l���s���ɶ�
	float	_fSpin;		// ���l������q degree/sec
	float   _fGravity;
	float   _fElpasedTime;	// �g�L�h�֮ɶ��A�H�����A�@�����ͤ��l���p��̾�
	float   _fOpacity;	// ���l�z����
	float   _fWindStr;
	float   _fWindDir;
	char    *_cName;
	float _totalTime=0;

	//cocos2d::Point _windDir;  // ������V�A���d�ҨS����@

	CParticleSystem();
	~CParticleSystem();
	void init(cocos2d::Layer &inlayer);
	void doStep(float dt);
	void setEmitter(bool bEm);
	void setType(int type) { _iType = type; }

 //   void onTouchesEnded(const cocos2d::CCPoint &touchPoint);
    void onTouchesBegan(const cocos2d::CCPoint &touchPoint);
    void onTouchesMoved(const cocos2d::CCPoint &touchPoint);
	void onTouchesEnded(const cocos2d::CCPoint &touchPoint);

	void setGravity(float fGravity);
	void setWindDir(float fwindd);
	void setWindStr(float fwinds);
	void setName(char *);
};

#endif
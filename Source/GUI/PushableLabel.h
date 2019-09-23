#pragma once
#include"Label.h"
#include<string>


namespace GUI{
class Manager;

//�������̌��m
class PushableLabel : public Label {
public:
	PushableLabel(
		int x, int y,
		int sizeX, int sizeY,
		unsigned int color = 0xffffffff,
		unsigned int fontColor = 0xff000000,
		float u0=0.f, float v0=0.f,
		float u1=1.f, float v1=1.f );
	virtual ~PushableLabel();

	bool isPushed()const{
		return mIsPushed;
	}
	//�}�E�X�����g�̗̈��ɂ��邩
	bool isHover()const{
		return mIsHover;
	}
	bool isPushing()const{
		return mIsPushing;
	}

	//false�ɂ����isPusshing()��isPushed()�����false�ɕԂ��悤�ɂ���
	void setPushable(bool enable);

	virtual void update();
	virtual void draw( Manager* mngr );

private:
	//disallow copy
	PushableLabel( const PushableLabel& );	
	//disallow assin
	void operator=( const PushableLabel& );
private:	
	bool		mIsPushed;//�{�^����ŉ����ė������ꍇtrue
	bool		mIsPushing;//�{�^����ŉ������ꍇtrue
	bool		mIsHover;//�{�^����Ƀ}�E�X�J�\�[��������ꍇtrue
	bool		mPushable;
};

}//namespace GUI

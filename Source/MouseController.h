#ifndef __MOUSECONTROLLER_H__
#define __MOUSECONTROLLER_H__
#include<GameLib/Input/Mouse.h>
using namespace GameLib::Input;

class CMouseController{
public:
	static float velocityY()
	{
		Mouse m;
		return static_cast<float>(m.velocityY());
	}
	static float velocityX()
	{
		Mouse m;
		return static_cast<float>(m.velocityX());
	}

	static bool isOnLeftClick(){
		Mouse m;
		m.isOn(Mouse::BUTTON_LEFT);
		return !mIsOnLeftDoubleClicked && m.isOn(Mouse::BUTTON_LEFT);
	}

	static bool isTriggeredLeftClick(){
		Mouse m;
		m.isOn(Mouse::BUTTON_LEFT);
		return !mIsOnLeftDoubleClicked && m.isTriggered(Mouse::BUTTON_LEFT);
	}


	static bool isOnLeftDoubleClick(){
		return mIsOnLeftDoubleClicked;
	}

	static bool isTriggeredLeftDoubleClick(){
		return mIsTriggeredLeftDoubleClicked;
	}

	static void update(){
		Mouse m;
		//�_�u���N���b�N���
		//�{�^���������Ɖ����Ă�ꍇ
		if( mIsOnLeftDoubleClicked ){
			if( m.isOn(Mouse::BUTTON_LEFT) ){
				//�p��
				mIsOnLeftDoubleClicked=true;
				mIsTriggeredLeftDoubleClicked=false;
				mLeftDoubleClickAbleTime=0;
			}
			else{
				//�ʏ��Ԃɖ߂�
				mIsOnLeftDoubleClicked=false;
				mIsTriggeredLeftDoubleClicked=false;
				mLeftDoubleClickAbleTime=0;
			}
		}
		//�_�u���N���b�N�\���
		else if(mLeftDoubleClickAbleTime > 0){
			--mLeftDoubleClickAbleTime;
			//�_�u���N���b�N�����{�^��������
			if( m.isTriggered(Mouse::BUTTON_LEFT) ){
				//�_�u���N���b�N���ɏ�ԑJ��
				mIsOnLeftDoubleClicked=true;
				mIsTriggeredLeftDoubleClicked=true;
				mLeftDoubleClickAbleTime=0;
			}
		}
		//�ʏ���
		else{
			if( m.isTriggered(Mouse::BUTTON_LEFT) ){
				//�_�u���N���b�N�\��ԂɑJ��
				mLeftDoubleClickAbleTime=DOUBLE_CLICK_ABLE_TIME;
			}
		}
	}
private:
	CMouseController();
	~CMouseController();
	static int mLeftDoubleClickAbleTime;
	static bool mIsOnLeftDoubleClicked;
	static bool mIsTriggeredLeftDoubleClicked;

	static int const DOUBLE_CLICK_ABLE_TIME;
};



#endif//__MOUSECONTROLLER_H__
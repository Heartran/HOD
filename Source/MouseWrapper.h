#ifndef __MOUSE_WRAPPER_H__
#define __MOUSE_WRAPPER_H__

#include<GameLib/Input/Mouse.h>
//#include<GameLib/Math/Vector2.h>

using namespace GameLib::Input;
//using namespace GameLib::Math;

class CMouseWrapper{
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


	//�_�u���N���b�N�̍ŏ��̃N���b�N�Ԃ�true�ɂȂ�G
	static bool isOnClick( Mouse::Button b ){
		Mouse m;
		return !sIsOnDoubleClicked[b] && m.isOn(b);
	}

	static bool isTriggeredClick( Mouse::Button b ){
		Mouse m;
		return !sIsOnDoubleClicked[b] && m.isTriggered(b);
	}

	static bool isOnDoubleClick( Mouse::Button b ){
		return sIsOnDoubleClicked[b];
	}

	static bool isTriggeredDoubleClick(Mouse::Button b){
		Mouse m;		
		return sIsOnDoubleClicked[b] && m.isTriggered(b);
	}
	
	//�}�E�X�J�[�\�����E�[�ɂ��邩
	static bool isCursolOnRight(){
		return sIsOnRight;
	}

		//�}�E�X�J�[�\�������[�ɂ��邩
	static bool isCursolOnLeft(){
		return sIsOnLeft;
	}


	static void update(){
		Mouse m;
		//�{�^���̃N���b�N�����֘A
		for(int i=0; i<2 ; ++i ) {
			Mouse::Button b = static_cast<Mouse::Button>(i);
			//�_�u���N���b�N��Ԃ�
			//�{�^���������Ɖ����Ă�ꍇ
			if( sIsOnDoubleClicked[b] ){
				if( m.isOn( b ) ){
					//�p��
					sIsOnDoubleClicked[b]=true;
					//sIsTriggeredDoubleClicked[b]=false;
					sDoubleClickAbleTime[b]=0;
				}
				else{
					//�ʏ��Ԃɖ߂�
					sIsOnDoubleClicked[b]=false;
					//sIsTriggeredDoubleClicked[b]=false;
					sDoubleClickAbleTime[b]=0;
				}
			}
			//�_�u���N���b�N�\���
			else if(sDoubleClickAbleTime[b] > 0){
				--sDoubleClickAbleTime[b];
				//�_�u���N���b�N�����{�^��������
				if( m.isTriggered( b ) ){
					//�_�u���N���b�N���ɏ�ԑJ��
					sIsOnDoubleClicked[b]=true;
					//sIsTriggeredDoubleClicked[b]=true;
					sDoubleClickAbleTime[b]=0;
				}
			}
			//�ʏ���
			else{
				if( m.isTriggered( b ) ){
					//�_�u���N���b�N�\��ԂɑJ��
					sDoubleClickAbleTime[b]=DOUBLE_CLICK_ABLE_TIME;
				}
			}
		}

		//�}�E�X�J�[�\�����E�[�����[�ɂ��邩
		if( m.x() < skLeft ) {
			sIsOnLeft=true;
			sIsOnRight=false;
		} else if( m.x() > skRight ) {
			sIsOnLeft=false;
			sIsOnRight=true;
		} else {
			sIsOnLeft=false;
			sIsOnRight=false;
		} 
	}
private:
	CMouseWrapper();
	~CMouseWrapper();
	static int sDoubleClickAbleTime[2];
	static bool sIsOnDoubleClicked[2];
	
	//static bool sIsTriggeredDoubleClicked[2];����Ȃ��^�f�R�����g�A�E�g

	static bool sIsOnRight;
	static bool sIsOnLeft;

	//const �ɂ��Ă��邪�@�Q�[�����ς�������ʂ����邩��
	static const int skRight;
	static const int skLeft;

	static int const DOUBLE_CLICK_ABLE_TIME;
};

#endif
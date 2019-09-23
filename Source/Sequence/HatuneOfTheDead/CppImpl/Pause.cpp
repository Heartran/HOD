#include <GameLib/GameLib.h>
#include <GameLib/Framework.h>
#include <GameLib/Input/Keyboard.h>
#include <GameLib/DebugScreen.h>
using namespace GameLib;

#include "helper/Image.h"
#include "Sequence/HatuneOfTheDead/CppImpl/Pause.h"
#include "Sequence/HatuneOfTheDead/CppImpl/Parent.h"
#include "Sequence/HatuneOfTheDead/CppImpl/Play.h"
#include "Sequence/Title.h"

namespace Sequence{
namespace HatuneOfTheDead{
namespace CppImpl{

Pause::Pause() : mImage( 0 ),mCursorPosistion( 0 ){
	mImage = new Image( "Data/GUI/image/dummy.dds" ); 
}

Pause::~Pause(){
	SAFE_DELETE( mImage );
}

Base* Pause::update( Parent* parent ){
	Base* next = this;
	if(!isReady()){
		return next;
	}
	//0:���Ȃ���
	//1:�ʃZ����
	//2:�^�C�g����ʂ�
	//3:�߂�
	Input::Keyboard k;
	if ( k.isTriggered( k.KEY_DOWN ) ){
		--mCursorPosistion;
		if ( mCursorPosistion < 0 ){ //�}�C�i�X�͍ő�l�Ƀ��[�v
 			mCursorPosistion = 1;
		}
	}else if ( k.isTriggered( k.KEY_UP ) ){
		++mCursorPosistion;
		if ( mCursorPosistion > 1 ){ //1���z������0�Ƀ��[�v
			mCursorPosistion = 0;
		}
	}else if ( k.isTriggered( k.KEY_RETURN ) ){
		if ( mCursorPosistion == 0 ){ //�Q�[���ɖ߂�
			next = new Play;
		}else if ( mCursorPosistion == 1 ){ //�^�C�g���֖߂�
			next = new Title;
		}
	}
	//�`��
	//�܂��Q�[�����
	parent->drawState();
	//��ɏd�˂�
	mImage->draw();
	//����`��
	DebugScreen sc;
	sc.draw( 0, 0, "[�߰��]" );
	sc.draw( 1, 2, "�ްѦ��޹�" );
	sc.draw( 1, 3, "���������" );
	//�J�[�\��������
	sc.draw( 0, mCursorPosistion + 2, ">" );

	return next;
}

bool Pause::isReady(){
	return mImage->isReady();
}

}}} //namespace Sequence::HatuneOfTheDead::CppImpl

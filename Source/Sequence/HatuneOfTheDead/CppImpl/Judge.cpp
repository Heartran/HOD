#include <GameLib/GameLib.h>
#include <GameLib/Framework.h>
#include <GameLib/DebugScreen.h>
#include <GameLib/Input/Keyboard.h>

#include "helper/Image.h"
#include "Sequence/HatuneOfTheDead/CppImpl/Judge.h"
#include "Sequence/HatuneOfTheDead/CppImpl/Parent.h"
#include "Sequence/HatuneOfTheDead/CppImpl/Ready.h"
#include "Sequence/Title.h"

#include <assert.h>

namespace Sequence{
namespace HatuneOfTheDead{
namespace CppImpl{

Judge::Judge() : mImage( 0 ), mCursorPosistion( 0 ){
	mImage = new Image( "Data/GUI/image/dummy.dds" ); 
}

Judge::~Judge(){
	SAFE_DELETE( mImage );
}

Base* Judge::update( Parent* parent ){
	Base* next = this;
	if(!isReady()){
		return next;
	}
	
	GameLib::Input::Keyboard k;

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
		if ( mCursorPosistion == 0 ){
			//�����ď���
			assert(false);//TODO
			//�{����State�����Z�b�g���đ��肽�����Ƃ肠����
			next = new Ready(nullptr);
		}else if ( mCursorPosistion == 1 ){
			//�^�C�g����
			next = new Title;
		}
	}
	//�`��
	//�܂��Q�[�����
	parent->drawState();
	//��ɏd�˂�
	mImage->draw();

	//�܂��ǂ������������̂��\��
	GameLib::DebugScreen sc;
	sc.draw( 0, 0, "[���ʲ��ò]" );

	//Parent::PlayerID winner = parent->winner();
	//if ( winner == Parent::PLAYER_1 ){
	//	sc.draw( 0, 1, "1P� ��!" );
	//}else if ( winner == Parent::PLAYER_2 ){
	//	sc.draw( 0, 1, "2P� ��!" );
	//}else{
	//	sc.draw( 0, 1, "�����ż! ˷ܹ!" );
	//}
	//�߂ʁ[
	sc.draw( 1, 3, "��� �ۼ��" );
	sc.draw( 1, 4, "��� ���� ���� �" );
	//�J�[�\��������
	sc.draw( 0, mCursorPosistion + 3, ">" );

	return next;
}

bool Judge::isReady(){
	return mImage->isReady();
}

}}} //namespace Sequence::HatuneOfTheDead::CppImpl

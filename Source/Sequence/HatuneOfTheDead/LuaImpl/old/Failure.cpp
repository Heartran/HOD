#include <GameLib/GameLib.h>
#include <GameLib/Framework.h>
#include <GameLib/DebugScreen.h>

#include "Sequence/HatuneOfTheDead/LuaImpl/Failure.h"
#include "Sequence/HatuneOfTheDead/LuaImpl/Parent.h"
#include "Sequence/HatuneOfTheDead/LuaImpl/Ready.h"
#include "Sequence/GameOver.h"
#include "helper/Image.h"

namespace Sequence{
namespace HatuneOfTheDead{
namespace LuaImpl{

Failure::Failure() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "Data/GUI/image/dummy.dds" ); 
}

Failure::~Failure(){
	SAFE_DELETE( mImage );
}

Base* Failure::update( Parent* parent ){
	Base* next = this;
	if(!isReady()){
		return next;
	}
	
	if ( mCount == 60 ){ //1�b�҂� 
		next = new GameOver;
	}
	//�`��
	//�܂��Q�[����ʂ�`��
	parent->drawState();
	//��ɏj���̃��b�Z�[�W��`��
	mImage->draw();
	//�N���A�[�Ǝ���`��
	GameLib::DebugScreen sc;
	sc.draw( 0, 0, "���!" );

	++mCount;

	return next;
}

bool Failure::isReady(){
	return mImage->isReady();
}

}}} //namespace Sequence::HatuneOfTheDead::LuaImpl

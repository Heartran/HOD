#include <GameLib/GameLib.h>
#include <GameLib/Framework.h>
#include <GameLib/DebugScreen.h>

using namespace GameLib;

#include "Sequence/GameOver.h"
#include "Sequence/Parent.h"
#include "Sequence/Title.h"
#include "helper/Image.h"

namespace Sequence{

GameOver::GameOver() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "Data/GUI/image/dummy.dds" ); 
}

GameOver::~GameOver(){
	SAFE_DELETE( mImage );
}

Base* GameOver::update( Parent* ){
	Base* next = this;
	if(!isReady()){
		return next;
	}

	if ( mCount == 60 ){ //1�b�҂� 
		next = new Title;
	}
	//�`��
	mImage->draw();
	//�����o��
	DebugScreen sc;
	sc.setPosition( 0, 0 );
	sc << "�ްѵ��ް ������޼��\n" ;

	++mCount;

	return next;
}
bool GameOver::isReady(){
	return mImage->isReady();
}

} //namespace Sequence
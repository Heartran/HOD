#include <GameLib/GameLib.h>
#include <GameLib/Framework.h>
#include <GameLib/DebugScreen.h>

using namespace GameLib;

#include "Sequence/Ending.h"
#include "Sequence/Parent.h"
#include "Sequence/Title.h"

#include "helper/Image.h"

namespace Sequence{

Ending::Ending() : mImage( 0 ), mCount( 0 ){
	mImage = new Image( "Data/GUI/image/dummy.dds" ); 
}

Ending::~Ending(){
	SAFE_DELETE( mImage );
}

Base* Ending::update( Parent* ){
	Base* next = this;
	if(!isReady()){
		return next;
	}

	if ( mCount == 120 ){ //2秒待つ 
		next = new Title;
	}
	//描画
	mImage->draw();
	//字を出す
	DebugScreen sc;
	sc.setPosition( 0, 0 );
	sc << "ｵﾒﾃﾞﾄｳ! ｱﾅﾀﾊ ｺﾉｹﾞｰﾑｦ ｾｲﾊｼﾏｼﾀ\nﾂｸｯﾀﾋﾄ ﾋﾗﾔﾏﾀｶｼ\n       kgme\n";

	++mCount;

	return next;
}

bool Ending::isReady(){
	return mImage->isReady();
}

} //namespace Sequence
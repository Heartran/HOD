#include <GameLib/GameLib.h>
#include"Parent.h"
#include"Child.h"
#include"SimpleWalker/Parent.h"
#include"Sword2/Parent.h"
#include"Nyoibou/Parent.h"
#include"TameSword/Parent.h"

#include"GameObject/Player.h"

namespace GameObject{ namespace PlayerUtils { namespace Updater {

Parent::Parent( Player *pl)
	:mChild( nullptr )
{
	//mChild = new SimpleWalker::Parent(pl);
	mChild = new Sword2::Parent(pl);
	//mChild = new Nyoibou::Parent(pl);
	//mChild = new TameSword::Parent(pl);
}

Parent::~Parent(){
	SAFE_DELETE( mChild );
}

void Parent::update( float dt ){
	Base* nextChild = mChild->update( this, dt );
	if ( nextChild != mChild ){
		//�J�ڔ���
		Child* casted = dynamic_cast< Child* >( nextChild );
		ASSERT( casted ); //���s�͂��肦�Ȃ��B
		SAFE_DELETE( mChild );

		mChild = casted;
	}
	nextChild = 0; //�O�̂���
}

} } } //GameObject::PlayerUtils::Updater

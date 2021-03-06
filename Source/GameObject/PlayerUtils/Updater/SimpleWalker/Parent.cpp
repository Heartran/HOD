#include <GameLib/GameLib.h>
#include"Parent.h"
#include"Child.h"
#include"Stand.h"
#include"GameObject/Player.h"
#include<BulletDynamics/Dynamics/btRigidBody.h>
#include<assert.h>

namespace GameObject{ namespace PlayerUtils { namespace Updater { namespace SimpleWalker {

Parent::Parent( Player *pl)
	:mChild( 0 ),
	mRigidBody(0),
	mStatus( pl->getStatus() ),
	mModel( pl->getModel() ),
	mMouse( pl->getMouse() ),
	mPlayer(pl)
{
	mRigidBody = btRigidBody::upcast(pl->getBtCollisionObject());
	mChild = new Stand(this);
}

Parent::~Parent(){
	//残っていれば抹殺
	SAFE_DELETE( mChild );
}

Root::Base* Parent::update( GrandParent* ,float dt ){
	Base* next = this;
		
	Base* nextChild = mChild->update( this, dt );
	if ( nextChild != mChild ){
		//遷移判定
		SimpleWalker::Child* casted = dynamic_cast< SimpleWalker::Child* >( nextChild );
		if( casted ){
			SAFE_DELETE( mChild );
			mChild = casted;
		} else {
			assert(false && "設計上よろしくない気がする" );
			//ここで処理できないので上に任せる
			next = nextChild;
		}
		casted = nullptr;
	}

	return next;
}

} } } } //GameObject::PlayerUtils::Updater::SimpleWalker

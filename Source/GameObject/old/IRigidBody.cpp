#include"IRigidBody.h"
#include"BulletPhysics/Manager.h"
namespace GameObject {

RTTI_IMPL(IRigidBody, ICollider );

IRigidBody::IRigidBody
()
	:mIsInWorld(false),
	mFilterGroup(0),
	mFilterMask(0),
	mMass(0.f)
{
}
IRigidBody::~IRigidBody(){
}

bool IRigidBody::isInWorld()const{
	return mIsInWorld;
}

btCollisionObject* IRigidBody::getBtCollisionObject(){
	return mRigidBody.get();
}
	
void IRigidBody::defalut_removeFromWorld(){
	if(mIsInWorld){
		BulletPhysics::Manager::instance()->removeFromWorld( mRigidBody.get() );
		mIsInWorld=false;
	}
}
	
void IRigidBody::defalut_addToWorld(){
	if(!mIsInWorld){
		BulletPhysics::Manager::instance()->addToWorld(
			mRigidBody.get(),
			mFilterGroup,
			mFilterMask );
		mIsInWorld=true;
	}
}


void IRigidBody::hitback( const btVector3 &dir, float hitbackValue ){
	if (hitbackValue == 0.f) {
		return;
	}

	
	mRigidBody->clearForces();
	mRigidBody->setLinearVelocity( btVector3( 0.f, 0.f, 0.f ) );

	//force=ma
	//mRigidBody->setLinearVelocity( normalDir*hitbackValue );
	//���Ԋ֌W�Ȃ��������̂�impulse
	//��񓖂������Ƃ��������ɑ΂��ē���̋�������ނ�����
	mRigidBody->applyCentralImpulse( dir*mMass*hitbackValue );
}
	
void IRigidBody::onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj)
{
	ICollider* col = static_cast<ICollider*>( obj->getUserPointer() );

	btVector3 dir =  me->getWorldTransform().getOrigin() - col->getWorldTransform().getOrigin();
	float length = dir.length();
	if(length==0.f){
		btVector3 forward = mRigidBody->getWorldTransform().getBasis()[2];
		forward.setX(-forward.x());
		forward.normalize ();

		dir = -forward;
	} else {
		dir /= length;
	}

	//���[��A�R���g���[�����₷������ɂ͐�����ԗʂ͑��葤�Ɉˑ�����̂ł͂Ȃ��������Ɉˑ�������ׂ��Ȃ̂ł�
	hitback( dir, col->getWeaponStatus()->hitbackValue );

	col->attack( mStatus.get() );
}

} //end namespace gameobject
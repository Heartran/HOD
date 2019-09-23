#include"CharacterControllerRigidBody.h"

#include<GameLib/GameLib.h>
#include<GameLib/DebugScreen.h>
#include<btBulletDynamicsCommon.h>

#include"BulletPhysics/Manager.h"
#include"helper/MyOutputDebugString.h"
#include"helper/PrintBtVector3.h"
#include<tchar.h>


namespace GameObject { namespace OldPlayer {

static float calcVel(
	const float &resist,
	const float &vel0,//���݂̑��x
	const float &maxVel,
	const float &accel )
{
	float vel = vel0 + accel - resist*vel0;
	if( vel > maxVel ){
		vel=maxVel;
	}
	if( vel < resist ){
		vel=0.f;
	}
	return vel;
}

CharacterControllerRigidBodySharedPtr CharacterControllerRigidBody::create(
	BtRigidBodySharedPtr rb,	
	float velocity,
	float angularSpeed,
	float mass )
{
	CharacterControllerRigidBodySharedPtr sp(
		NEW CharacterControllerRigidBody(
			rb,
			velocity,
			angularSpeed,
			mass ) 
	);

	//�e�ɍ폜�̐ӔC��C�����ق������R��
	//���̑���e�̓R���X�g���N�^����CharacterController�����Ȃ�
	//bug��܂�
	//enable_shared_from_this���g���Ă���̂ŏo���܂���
	//�ꉞ�����̂��߁A�R�����g�c���Ă���
	//CharacterControllerRigidBodySharedPtr sp(
	//	parent->getIColliderSharedPtr(),
	//	NEW CharacterControllerRigidBody(
	//		parent,
	//		startTransform,
	//		characterHeight,
	//		characterWidth,
	//		velocity,
	//		angularSpeed ) 
	//);
	return sp;
}

CharacterControllerRigidBody::CharacterControllerRigidBody(
	BtRigidBodySharedPtr rb,	
	float maxSpeed,
	float maxAngularSpeed,
	float mass )
	:
	mRigidBody(rb),
	mShape(0),
	mMaxSpeed(maxSpeed),
	mMaxAngularSpeed(maxAngularSpeed),
	mIsInWorld(false),
	mHasMoved(true),
	mCanMove(false),
	mHasTurned(true),
	mFilterGroup(btBroadphaseProxy::DefaultFilter),
	mFilterMask( ~BulletPhysics::Manager::MMDObjectFilter ),
	//mFilterMask(btBroadphaseProxy::StaticFilter ),
	mMass(mass),
	mIsOnGround(false)
{
	mShape = dynamic_cast<btCylinderShape*>( mRigidBody->getCollisionShape() );
	assert( mShape);
	//jump�̐ݒ�
	float g = abs(BulletPhysics::Manager::instance()->getWorldPtr()->getGravity().y());
	float h = mShape->getHalfExtentsWithoutMargin().y();
	float t = sqrt(2.f*h/g);
	float v0 = g*t;
	mJumpSpeed=v0*sqrt(2.f);

	stop();

	addToWorld();	
}

CharacterControllerRigidBody::~CharacterControllerRigidBody(){
	removeFromWorld();	
}


void CharacterControllerRigidBody::draw() const {
	BulletPhysics::Manager::drawObject( mRigidBody.get(), 0xff000000 );
}

void CharacterControllerRigidBody::warp( const btVector3 &pos ) {
	mRigidBody->getWorldTransform().setOrigin(pos);
}

btVector3 CharacterControllerRigidBody::getVelocity() const {
	return mRigidBody->getLinearVelocity();
}

bool CharacterControllerRigidBody::onGround() const{
	return mIsOnGround;
}


void CharacterControllerRigidBody::removeFromWorld(){
	if(mIsInWorld) {
		BulletPhysics::Manager::instance()->removeFromWorld( mRigidBody.get() );
		mIsInWorld = false;
	}
}

void CharacterControllerRigidBody::addToWorld(){
	if(!mIsInWorld) {
		BulletPhysics::Manager::instance()->addToWorld(
			mRigidBody.get(),
			mFilterGroup,
			mFilterMask
		);

		mTarget = mRigidBody->getWorldTransform().getOrigin();
		mIsInWorld = true;
	}
}

void CharacterControllerRigidBody::update( float elapsedTime )
{
	const btTransform &xform = mRigidBody->getWorldTransform ();
	
	//checkOnGround()
	btVector3 from=xform.getOrigin(), to=xform.getOrigin();
	to[1] -= mShape->getHalfExtentsWithMargin().y()+mShape->getMargin();

	class ClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
	public:
		ClosestNotMeRayResultCallback (btCollisionObject* me)
			: btCollisionWorld::ClosestRayResultCallback(
				btVector3(0.0, 0.0, 0.0),
				btVector3(0.0, 0.0, 0.0) )
		{
			m_me = me;
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
		{
			if (rayResult.m_collisionObject == m_me)
				return 1.0;

			return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
		}
	protected:
		btCollisionObject* m_me;
	};
	ClosestNotMeRayResultCallback callback(mRigidBody.get());
	callback.m_collisionFilterGroup = mFilterGroup;
	callback.m_collisionFilterMask = mFilterMask;
	BulletPhysics::Manager::instance()->getWorldPtr()->rayTest( from, to, callback );

	if( callback.hasHit() ){
		//btVector3 groundPos;
		//groundPos.setInterpolate3( from, to, callback.m_closestHitFraction );
		mIsOnGround=true;
	} else {
		mIsOnGround=false;
	}


	//�������߂�
	//xform*z0
	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.setX(-forwardDir.x());
	//mmd��������W�Ƃ��č���Ă���̂ŋt�������Ă���̂Ŕ��Ε�����O�Ƃ���B
	forwardDir = -forwardDir;
	forwardDir.normalize ();


	//printf("forwardDir=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
	btVector3 upDir = xform.getBasis()[1];
	btVector3 strafeDir = xform.getBasis()[0];
	upDir.normalize ();
	strafeDir.normalize();

	
	if( !mHasMoved || !mHasTurned ){

		//bool isBrake = false;
		
		//�����ɗ��Ƃ���targetDir��forwardDir�̊Ԃ̊p�x
		float angleBetweenForwardAndTarget=0.f;

		//target�̕����Ɍ���
		if( !mHasTurned && mCanTurn )
		{
			//��]����p�x
			float theata = 0.f;
			
			//mTarget���I�u�W�F�N�g"�㕽�ʏ�"�ɗ��Ƃ��B
			btVector3 targetDir = mTarget-xform.getOrigin();
			targetDir -= targetDir.dot( upDir ) * upDir;
			//0���΍�
			if( targetDir.length2() == 0.f ){
				theata = 0.f;
			} else {
				targetDir.normalize();
				float dot;
				dot =  forwardDir.dot(targetDir);
				if( dot > 1.f ){
					dot=1.f;
				}
				theata = acosf( forwardDir.dot(targetDir) );
			}
			//�񐔂̏ꍇ�A(���������̏ꍇ)
			if( theata!=theata ){
				mHasTurned = true;				
				theata = 0.f;
			}
			//120�x�ȏ�̏ꍇ �u���[�L������
			//if(theata>3.14159265f*2.f/3.f){
			//	isBrake=true;
			//}
			if(theata!=0.f){
				//��]�������
				btScalar angularSpeed = mMaxAngularSpeed * elapsedTime;
				//assert( theata>=0.f );
				angleBetweenForwardAndTarget = theata;
				if( theata > angularSpeed ){
					theata=angularSpeed;
				}
				//��]���E�������Bcos��+-�������Ă�̂ł��ׂā{�̉�]�ɂȂ��Ă���̂�
				btVector3 cross=forwardDir.cross(targetDir);
				if( cross.y() < 0.f){
					theata=-theata;
				}
				btMatrix3x3 orn = xform.getBasis();
				orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), theata));
				mRigidBody->getWorldTransform ().setBasis(orn);
			} else {
				mHasTurned = true;
			}
		}

		//�ړ�
		if( !mHasMoved && mCanMove )
		{
			btVector3 walkDir = (mTarget-xform.getOrigin());
			assert( walkDir==walkDir);
			walkDir.setY(0.f);		
			const float distance = walkDir.length();
		
			if(distance < 0.001){
				stop();
				mSpeed=0.f;
				mHasMoved=true;
				mHasTurned=true;
			}
			else
			{
				walkDir.normalize();
			
				//when theata=0;
				//	cos( theata*0.5f )=1.f;
				//when theata=PI/2;
				//	cos( theata*0.5f )=1.f/squrt(2.f);
				//when theata=PI;
				//	cos( theata*0.5f )=0.f;

				//�������^���ʂ���Ȃ��Ƃ��̒x�����邽�߂̂���
				float rate = cosf( 0.5f*angleBetweenForwardAndTarget );
				//float rate =1.f;

				mSpeed = mMaxSpeed * rate;
		
				//if( distance < mSpeed ){
				//	mSpeed = distance;
				//} 
				//TRACE0(_T("pos : ") );
				//PrintBtVector3::toOutput( xform.getOrigin() );
				//TRACE0(_T("pos+vel : ") );
				//PrintBtVector3::toOutput(xform.getOrigin()+walkDir*mSpeed);
				////mCharacter->setWalkDirection( walkDir*mSpeed );
				
				mRigidBody->applyCentralImpulse( walkDir*mSpeed*mMass );
				
				btVector3 vel = mRigidBody->getLinearVelocity();
				float speed=vel.length();
				if( mMaxSpeed<speed ){
					mRigidBody->setLinearVelocity(vel/speed*mMaxSpeed);
					speed=mMaxSpeed;
				}

				if( distance < speed*elapsedTime ){
					stop();
				}
			}
		} 
	} else {
		mSpeed=0.f;
		mHasTurned = true;
	}

	
}



void CharacterControllerRigidBody::stop(){
	mHasMoved = true;
	mHasTurned = true;
	mCanMove = false;
	mCanTurn=false;
	mSpeed=0.f;
	mRigidBody->clearForces();
	mRigidBody->setLinearVelocity( btVector3( 0.f, 0.f, 0.f ) );
}

void CharacterControllerRigidBody::look( const btVector3 &pos ){
	mTarget = pos;
	mCanMove = false;
	mCanTurn = true;
	mHasMoved = false;
	mHasTurned = false;	
}

void CharacterControllerRigidBody::cut(){
	mCanMove = false;
	mCanTurn = false;
	mHasMoved = false;
	mHasTurned = false;
}

void CharacterControllerRigidBody::move( const btVector3 &pos ){
	mTarget = pos;
	mCanMove = true;
	mCanTurn = true;
	mHasMoved = false;
	mHasTurned = false;
}

void CharacterControllerRigidBody::strafe( const btVector3 &pos ){
	mTarget = pos;
	mCanMove = true;
	mCanTurn = false;
	mHasMoved = false;
	mHasTurned = false;
}

bool CharacterControllerRigidBody::isMoving() const{
	return !mHasMoved && mCanMove;
}

bool CharacterControllerRigidBody::isTurning() const{
	return !mHasTurned && mCanTurn; 
}

bool CharacterControllerRigidBody::isStopping() const{
	return !isMoving() && !isTurning();
}

void CharacterControllerRigidBody::jump(){
	if(mIsOnGround){
		mRigidBody->applyCentralImpulse( btVector3(0.f, mJumpSpeed*mMass, 0.f ) );
	}
}

void CharacterControllerRigidBody::jumpHi(){
	if(mIsOnGround){
		mRigidBody->applyCentralImpulse( btVector3(0.f, mJumpSpeed*mMass*2.f, 0.f ) );
	}
}

void CharacterControllerRigidBody::jumpLow(){
	if(mIsOnGround){
		mRigidBody->applyCentralImpulse( btVector3(0.f, mJumpSpeed*mMass*0.5f, 0.f ) );
	}
}

void CharacterControllerRigidBody::forceJump(){
	mRigidBody->applyCentralImpulse( btVector3(0.f, mJumpSpeed*mMass, 0.f ) );
}

void CharacterControllerRigidBody::jump( const float &impulseY ){
	if(mIsOnGround){
		mRigidBody->applyCentralImpulse( btVector3(0.f, impulseY, 0.f ) );
	}
}

void CharacterControllerRigidBody::forceJump( const float &impulseY ){
	mRigidBody->applyCentralImpulse( btVector3(0.f, impulseY, 0.f ) );
}

const btTransform & CharacterControllerRigidBody::getWorldTransform() const{
	return mRigidBody->getWorldTransform();
}

btTransform & CharacterControllerRigidBody::getWorldTransform(){
	return mRigidBody->getWorldTransform();
}

btCollisionObject* CharacterControllerRigidBody::getBtCollisionObject(){
	return mRigidBody.get();
}

} }//end namespace GameObject::OldPlayer

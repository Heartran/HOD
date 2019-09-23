#include"CharacterControllerGhostObject.h"

#include<GameLib/GameLib.h>
#include<GameLib/DebugScreen.h>
#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionDispatch/btGhostObject.h>
//#include<BulletDynamics/Character/btKinematicCharacterController.h>
#include"BulletPhysics/CharacterController/kztrCharacterController2.h"

#include"BulletPhysics/Manager.h"
#include"helper/MyOutputDebugString.h"
#include"helper/PrintBtVector3.h"
#include<tchar.h>

namespace GameObject {

RTTI_IMPL( CharacterControllerGhostObject , ICollider );

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

CharacterControllerGhostObjectSharedPtr CharacterControllerGhostObject::create(
		ICollider* parent,
		const btTransform &startTransform,
		float characterHeight,
		float characterWidth,
		float velocity,
		float angularSpeed )
{
	CharacterControllerGhostObjectSharedPtr sp(
		NEW CharacterControllerGhostObject(
			parent,
			startTransform,
			characterHeight,
			characterWidth,
			velocity,
			angularSpeed ) 
	);

	//�e�ɍ폜�̐ӔC��C�����ق������R��
	//���̑���e�̓R���X�g���N�^����CharacterController�����Ȃ�
	//bug��܂�
	//enable_shared_from_this���g���Ă���̂ŏo���܂���
	//�ꉞ�����̂��߁A�R�����g�c���Ă���
	//CharacterControllerGhostObjectSharedPtr sp(
	//	parent->getIColliderSharedPtr(),
	//	NEW CharacterControllerGhostObject(
	//		parent,
	//		startTransform,
	//		characterHeight,
	//		characterWidth,
	//		velocity,
	//		angularSpeed ) 
	//);
	return sp;
}

CharacterControllerGhostObject::CharacterControllerGhostObject(
	ICollider* parent,
	const btTransform &startTransform, 
	float characterHeight,
	float characterWidth,
	float maxSpeed,
	float maxAngularSpeed )
	:mParent(parent),
	mCharacter(NULL),
	mGhostObject(NULL),
	mCapsule(NULL),
	mMaxSpeed(maxSpeed),
	mMaxAngularSpeed(maxAngularSpeed),
	mIsDash(false), mIsJump(false),
	mIsInWorld(false),
	mHasMoved(true),
	mCanMove(false),
	mHasTurned(true),
	mFilterGroup(btBroadphaseProxy::DefaultFilter),
	mFilterMask( ~BulletPhysics::Manager::MMDObjectFilter )
	//mFilterMask(btBroadphaseProxy::StaticFilter ),
	//mMass(30.f)
{
	btDiscreteDynamicsWorld *pWorld = BulletPhysics::Manager::instance()->getWorldPtr();
	btAxisSweep3* sweepBP = dynamic_cast<btAxisSweep3*>(pWorld->getBroadphase());
	assert( sweepBP && "" );
	mGhostObject = new btPairCachingGhostObject();
	mGhostObject->setWorldTransform(startTransform);
	mGhostObject->setUserPointer( this );
	
	mCapsule = new btCapsuleShape(characterWidth, characterHeight);
	mGhostObject->setCollisionShape (mCapsule);
	//mGhostObject->setCollisionFlags (  btCollisionObject::CF_CHARACTER_OBJECT );//mGhostObject->getCollisionFlags() |

	btScalar stepHeight = btScalar(0.35);
	//mCharacter = new btKinematicCharacterController (mGhostObject,capsule,stepHeight);
	mCharacter = new kztr::CharacterController2( mGhostObject, mCapsule, stepHeight);
	stop();

	//jump�̐ݒ�
	float g = mCharacter->getGravity();
	float h = mCapsule->getHalfHeight();
	float t = sqrt(2.f*h/g);
	float v0 = g*t;
	mJumpLowSpeed=v0;
	mJumpSpeed=v0*sqrt(2.f);
	mJumpHiSpeed=v0*2.f;

	addToWorld();
	
}

CharacterControllerGhostObject::~CharacterControllerGhostObject(){
	removeFromWorld();
	
	//cleanup in the reverse order of creation/initialization
	
	//btCollisionShape *shape=mGhostObject->getCollisionShape();
	SAFE_DELETE(mCapsule);
	SAFE_DELETE(mCharacter);
	SAFE_DELETE(mGhostObject);
}


//const btTransform & CharacterControllerGhostObject::getWorldTransform() const {
//	return mGhostObject->getWorldTransform();
//}
//btTransform & CharacterControllerGhostObject::getWorldTransgorm(){
//	return mGhostObject->getWorldTransform();
//}


void CharacterControllerGhostObject::draw() const {
	BulletPhysics::Manager::drawObject( mGhostObject, 0xff000000 );
}

void CharacterControllerGhostObject::warp( const btVector3 &pos ) {
	mCharacter->warp( pos );
}

//void CharacterControllerGhostObject::setUserPtr( void *ptr ){
//	mGhostObject->setUserPointer( ptr );
//}

btVector3 CharacterControllerGhostObject::getVelocity() const {
	return mCharacter->getVelocity();
}

bool CharacterControllerGhostObject::onGround() const{
	return mCharacter->onGround();
}

btCollisionObject* CharacterControllerGhostObject::getBtCollisionObject(){
	return mGhostObject;
}

void CharacterControllerGhostObject::removeFromWorld(){
	if(mIsInWorld) {
		btDiscreteDynamicsWorld *pWorld = BulletPhysics::Manager::instance()->getWorldPtr();
		//BulletPhysics::Manager* mngr = BulletPhysics::Manager::instance();
		if (mCharacter)
		{
			pWorld->removeCollisionObject( mGhostObject );
			pWorld->removeAction( mCharacter );
		}
		mIsInWorld = false;
	}
}

void CharacterControllerGhostObject::addToWorld(){
	if(!mIsInWorld) {
		btDiscreteDynamicsWorld *pWorld = BulletPhysics::Manager::instance()->getWorldPtr();
		pWorld->addCollisionObject(
			mGhostObject,
			mFilterGroup,
			mFilterMask
		);

		pWorld->addAction(mCharacter);
		mTarget = mGhostObject->getWorldTransform().getOrigin();
		mIsInWorld = true;
	}
}

void CharacterControllerGhostObject::update( float elapsedTime )
{
	///set walkDirection for our character
	const btTransform &xform = mGhostObject->getWorldTransform ();
	
	//example
	//btVector3 z=xform.getBasis()[2];
	//btVector3 z0(0.f, 0.f, 1.f);
	//float rad = acosf ( z.dot(z0) );
	//float s = sinf(-rad);
	//float c = cosf(-rad);
	//TRACE2("sin: %f, cos: %f \n", s, c);
	//TRACE2("x  : %f, z  : %f \n", z.x(), z.z() );
	
	//���W�n?�A�E�˂��A���˂�?
	//���͉�]���t�ƂȂ胂�f���̌����Ɛi�ތ�������v���Ȃ����ƁB
	//���R�������ł��Ȃ��̂��C���������B
	//���̐�����z�����ǂꂾ���ˉe����Ă��邩�̒l���Ɖ��͎v���Ă��邯�ǈႤ�̂��H
	//��������B���ꂪ�������B
	//assert( xform.getBasis()[2] == xform*btVector3(0,0,1) );
	//�͈�v���Ȃ�
	//bulletphysics�̃f������肭�����Ă�悤�Ɍ�����͍̂��E�Ώۂ̃I�u�W�F�N�g���g���Ă��邩��

	
	//�ȉ��̌v�Z�͑O���ǂ����ɂ��邩�𒲂ׂ�B
	//�p�͉�2�̌v�Z���Ɠ���
	//example1 �t��]����
	//btVector3 z0(0.f, 0.f, 1.f);
	//btVector3 z=xform.getBasis()[2];
	//rotMat( axis(0,1,0), -acos(z�Ez0) )�Ez0

	//exsample2�@�������߂�
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
			btVector3 targetDir = mTarget-mGhostObject->getWorldTransform().getOrigin();
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
				btMatrix3x3 orn = mGhostObject->getWorldTransform().getBasis();
				orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), theata));
				mGhostObject->getWorldTransform ().setBasis(orn);
			} else {
				mHasTurned = true;
			}
		}

		//�ړ�
		if( !mHasMoved && mCanMove )
		{
			btVector3 walkDir = (mTarget-xform.getOrigin());
			walkDir.setY(0.f);		
			const float distance = walkDir.length();
		
			if(distance < 000001.f){
				mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
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

				float rate = cosf( 0.5f*angleBetweenForwardAndTarget );

				//setWalkVelocity���g���ꍇ
				//mSpeed = mMaxSpeed * 1.f/60.f * rate;
				mSpeed = mMaxSpeed * rate;
		
				//if( distance < mSpeed ){
				//	mSpeed = distance;
				//} 
				//TRACE0(_T("pos : ") );
				//PrintBtVector3::toOutput( xform.getOrigin() );
				//TRACE0(_T("pos+vel : ") );
				//PrintBtVector3::toOutput(xform.getOrigin()+walkDir*mSpeed);
				////mCharacter->setWalkDirection( walkDir*mSpeed );
				mCharacter->setVelocity( walkDir*mSpeed );
			}
		} 
	} else {
		mSpeed=0.f;
		mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
		mHasTurned = true;
	}

	//controller�ł�jump
	if( mIsJump ){
		mCharacter->setJumpSpeed( 30.f );
		mCharacter->jump();
	}
	
	//�ǏR��̏���
	if(false)
	if( mIsJump ) {
		//������rayCast�o���āA�Ԃ���΃W�����v�ł���
		btSphereShape sphere(0.4f);
		btTransform from,to;
		from.setIdentity();
		const btTransform &characterWorldTrans = getWorldTransform();
		from.setOrigin( characterWorldTrans.getOrigin() );
		const btCapsuleShape *shape = dynamic_cast<const btCapsuleShape*>( mGhostObject->getCollisionShape() );
		assert(shape);
		//�������烌�C���o��
		const float &offsetY = shape->getHalfHeight();
		from.getOrigin().setY( from.getOrigin().y() - offsetY );
		to.setIdentity();
		//�ǂ܂ł̋�����3�ȉ��Ȃ�A�ǏR�肷��
		static const float kDistanceToAllowJump=3.f;
		to.setOrigin( from.getOrigin()+forwardDir*kDistanceToAllowJump );
	
		btCollisionWorld::ClosestConvexResultCallback cb( from.getOrigin(), to.getOrigin() );
		//�ÓI���̂̂ݓ����蔻�肠���mask
		cb.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
		
		BulletPhysics::Manager::instance()->getWorldPtr()->convexSweepTest( &sphere, from, to, cb );
		
		//�ǏR��ł����
		if (cb.hasHit())
		{
			//btScalar minFraction  = cb.m_closestHitFraction;//btMax(btScalar(0.3),cb.m_closestHitFraction);
			//�ǏR��̏���	
		}

	}
}

void CharacterControllerGhostObject::checkArrivalInNexTime() {
	if( !mHasMoved ){
		btVector3 walkDir = (mTarget-mGhostObject->getWorldTransform().getOrigin());
		walkDir.setY(0.f);		
		const float distance = walkDir.length();
		
		if(distance < 000001.f){
			mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
			mHasMoved=true;
			mHasTurned=true;
			return;
		}
		
		if( distance < mSpeed ){
			walkDir /= distance;
			mSpeed = distance;
			mCharacter->setWalkDirection( mSpeed*walkDir );				
		}

	}
}

void CharacterControllerGhostObject::checkArrivalInNexTime( float dt ) {
	if( !mHasMoved ){
		assert( dt==1.0f/60.0f );
		btVector3 walkDir = (mTarget-mGhostObject->getWorldTransform().getOrigin());
		walkDir.setY(0.f);		
		const float distance = walkDir.length();
		
		if(distance < 000001.f){
			mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
			mHasMoved=true;
			mHasTurned=true;
			return;

		}
		
		if( distance < mSpeed * dt ){
			walkDir /= distance;
			mSpeed = distance;
			mCharacter->setWalkDirection( mSpeed*walkDir );				
		}

	}
}

void CharacterControllerGhostObject::stop(){
	mHasMoved = true;
	mHasTurned = true;
	mCanMove = false;
	mCanTurn=false;
	mSpeed=0.f;
	mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
}

void CharacterControllerGhostObject::look( const btVector3 &pos ){
	mTarget = pos;
	mCanMove = false;
	mCanTurn = true;
	mHasMoved = false;
	mHasTurned = false;
	
	mCharacter->setVelocity( mCharacter->getVelocity() );
}

void CharacterControllerGhostObject::cut(){
	mCanMove = false;
	mCanTurn = false;
	mHasMoved = false;
	mHasTurned = false;
	
	mCharacter->setVelocity( mCharacter->getVelocity() );
}

void CharacterControllerGhostObject::move( const btVector3 &pos ){
	mTarget = pos;
	mCanMove = true;
	mCanTurn = true;
	mHasMoved = false;
	mHasTurned = false;
}

void CharacterControllerGhostObject::strafe( const btVector3 &pos ){
	mTarget = pos;
	mCanMove = true;
	mCanTurn = false;
	mHasMoved = false;
	mHasTurned = false;
}

bool CharacterControllerGhostObject::isMoving() const{
	return !mHasMoved && mCanMove;
}

bool CharacterControllerGhostObject::isTurning() const{
	return !mHasTurned && mCanTurn; 
}

bool CharacterControllerGhostObject::isStopping() const{
	return !isMoving() && !isTurning();
}

//void CharacterControllerGhostObject::hitback( const btVector3 & impulse ){
//	mCanMove = true;
//	mCanTurn = false;
//	mHasArrived = false;
//	mHasTurned = false;
//	btVector3 vel=impulse/mMass;
//	mTarget = mGhostObject->getWorldTransform().getOrigin()+vel;
//}

void CharacterControllerGhostObject::jump(){
	mCharacter->setJumpSpeed( mJumpSpeed );
	mCharacter->jump();
}

void CharacterControllerGhostObject::jumpHi(){
	mCharacter->setJumpSpeed( mJumpHiSpeed );
	mCharacter->jump();
}

void CharacterControllerGhostObject::jumpLow(){
	mCharacter->setJumpSpeed( mJumpLowSpeed );
	mCharacter->jump();
}

void CharacterControllerGhostObject::forceJump(){
	mCharacter->setJumpSpeed( mJumpLowSpeed );
	mCharacter->forceJump();
}

void CharacterControllerGhostObject::jump( const float &speed ){
	mCharacter->setJumpSpeed( speed );
	mCharacter->jump();
}

void CharacterControllerGhostObject::forceJump( const float &speed ){
	mCharacter->setJumpSpeed( speed );
	mCharacter->jump();
}
}//end namespace GameObject

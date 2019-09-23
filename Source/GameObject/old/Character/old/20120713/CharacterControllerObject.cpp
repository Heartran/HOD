#include"CharacterControllerObject.h"

#include<GameLib/GameLib.h>
#include<GameLib/DebugScreen.h>
#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionDispatch/btGhostObject.h>
#include<BulletDynamics/Character/btKinematicCharacterController.h>
#include"BulletPhysics/CharacterController/kztrCharacterController2.h"

#include"BulletPhysics/BulletPhysics_Manager.h"
#include"helper/MyOutputDebugString.h"
#include"helper/PrintBtVector3.h"
#include<tchar.h>

//�ǂ̂悤�ɓ������̃e�X�g�̂��߂�include�B�}�E�X�Ƃ��̃R���g���[���͂����ɒu���Ȃ�
#include"MouseWrapper.h"
#include<GameLib/Input/Keyboard.h>

namespace GameObject {

RTTI_IMPL( CharacterControllerObject , IGameObject );

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




CharacterControllerObject::CharacterControllerObject(
	const btTransform &startTransform, 
	float characterHeight,
	float characterWidth,
	float maxSpeed,
	float maxAngularSpeed )
	:mId( getGameObjectsNum() ),
	mCharacter(NULL),
	mGhostObject(NULL),
	mMaxSpeed(maxSpeed),
	mMaxAngularSpeed(maxAngularSpeed),
	mIsDash(false), mIsJump(false),
	mIsInWorld(false),
	mIsArrived(true),
	mFilterGroup(btBroadphaseProxy::DefaultFilter),
	mFilterMask(btBroadphaseProxy::AllFilter & ~BulletPhysics::Manager::MMDObjectFilter )
{
	btDiscreteDynamicsWorld *pWorld = BulletPhysics::Manager::instance()->getWorldPtr();
	btAxisSweep3* sweepBP = dynamic_cast<btAxisSweep3*>(pWorld->getBroadphase());
	assert( sweepBP && "" );
	mGhostObject = new btPairCachingGhostObject();
	mGhostObject->setWorldTransform(startTransform);
	mGhostObject->setUserPointer( this );
	
	btConvexShape* capsule = new btCapsuleShape(characterWidth, characterHeight);
	mGhostObject->setCollisionShape (capsule);
	//mGhostObject->setCollisionFlags (  btCollisionObject::CF_CHARACTER_OBJECT );//mGhostObject->getCollisionFlags() |

	btScalar stepHeight = btScalar(0.35);
	//mCharacter = new btKinematicCharacterController (mGhostObject,capsule,stepHeight);
	mCharacter = new kztr::CharacterController2( mGhostObject, capsule, stepHeight);
	
	addToWorld();
	

}

CharacterControllerObject::~CharacterControllerObject(){
	removeFromWorld();
	
	//cleanup in the reverse order of creation/initialization
	
	btCollisionShape *shape=mGhostObject->getCollisionShape();
	SAFE_DELETE(shape);
	SAFE_DELETE(mCharacter);
	SAFE_DELETE(mGhostObject);
}

void CharacterControllerObject::update( float elapsedTime )
{

	/* Character stuff &*/
	if (mCharacter)
	{
		
	}
	
	///set walkDirection for our character
	btTransform xform;
	xform = mGhostObject->getWorldTransform ();
	
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

	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	mSpeed = mMaxSpeed * elapsedTime;

	btScalar angularSpeed = mMaxAngularSpeed * elapsedTime;
	
	bool isBrake = false;

	//target�̕����Ɍ���
	{
		//mTarget���I�u�W�F�N�g"�㕽�ʏ�"�ɗ��Ƃ��B
		btVector3 targetDir = mTarget-mGhostObject->getWorldTransform().getOrigin();
		targetDir -= targetDir.dot( upDir ) * upDir;
		float theata;
		//0���΍�
		if( targetDir.length2() == 0.f ){
			theata = 0.f;
		} else {
			targetDir.normalize();
			theata = acosf( forwardDir.dot(targetDir) );
		}
		//0���΍�etc �܂���ł���Ă���̂ł���Ȃ���
		//�񐔂̏ꍇ�A(���������̏ꍇ)
		if( theata!=theata ){
			//bool a=theata<0.001f;
			//bool b=theata>0.001f;
			return;
		}
		//120�x�ȏ�̏ꍇ �u���[�L������
		if(theata>3.14159265f*2.f/3.f){
			isBrake=true;
		}
		//0-1�̊Ԃɕς���B90�x�̂Ƃ���ԉ�]����
		float s=sinf(theata);
		s*=s;//�������������邽�ߒ���

		//TRACE1("targetPos	:%f\n", targetPos.y() );
		//TRACE1("theata		:%f\n", theata);
		//TRACE1("sin			:%f\n", s);
		
		//��]�������
		theata=angularSpeed*s;
		
		//��]���E�������Bcos��+-�������Ă�̂ł��ׂā{�̉�]�ɂȂ��Ă���̂�
		btVector3 cross=forwardDir.cross(targetDir);
		if( cross.y() < 0.f){
			theata=-theata;
		}

		if(theata!=0.f){
			btMatrix3x3 orn = mGhostObject->getWorldTransform().getBasis();
			orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), theata));
			mGhostObject->getWorldTransform ().setBasis(orn);
		}
	}


	if(mIsDash){
		mCharacter->setVelocity( forwardDir*mCharacter->getMinSpeed() );
		//mCharacter->applyCentralForce( forwardDir*100.f );
		mCharacter->addAccelVelocityForTimeInterval( forwardDir*mSpeed, 6.f/30.f );
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
	mMode = MODE_ACCEL;

}

const btTransform & CharacterControllerObject::getWorldTransform() const {
	return mGhostObject->getWorldTransform();
}
btTransform & CharacterControllerObject::getWorldTransgorm(){
	return mGhostObject->getWorldTransform();
}


void CharacterControllerObject::draw() const {
	BulletPhysics::Manager::drawObject( mGhostObject, 0xff000000 );
}

void CharacterControllerObject::warp( const btVector3 &pos ) {
	mCharacter->warp( pos );
}

void CharacterControllerObject::setUserPtr( void *ptr ){
	mGhostObject->setUserPointer( ptr );
}
const btVector3& CharacterControllerObject::getVelocity() const {
	return mCharacter->getVelocity();
}

bool CharacterControllerObject::onGround() const{
	return mCharacter->onGround();
}
bool CharacterControllerObject::onGroundPerhaps() const{
	return mCharacter->onGroundPerhaps();
}

btCollisionObject* CharacterControllerObject::getBtCollisionObject(){
	return mGhostObject;
}

void CharacterControllerObject::removeFromWorld(){
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

void CharacterControllerObject::addToWorld(){
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

void CharacterControllerObject::walkControleUpdate( float elapsedTime ){
	///set walkDirection for our character
	btTransform xform;
	xform = mGhostObject->getWorldTransform ();
	
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
	forwardDir.normalize();

	//printf("forwardDir=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
	btVector3 upDir = xform.getBasis()[1];
	btVector3 strafeDir = xform.getBasis()[0];
	upDir.normalize ();
	strafeDir.normalize ();

	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	mSpeed = mMaxSpeed * elapsedTime;

	btScalar angularSpeed = mMaxAngularSpeed * elapsedTime;
	if (mIsLeft)
	{
		btMatrix3x3 orn = mGhostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), angularSpeed));
		mGhostObject->getWorldTransform ().setBasis(orn);
	}

	if (mIsRight)
	{
		btMatrix3x3 orn = mGhostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), -angularSpeed));
		mGhostObject->getWorldTransform ().setBasis(orn);
	}

	if (mIsUp)
	{
		walkDirection += forwardDir;
	}
	if (mIsDown)
	{
		walkDirection -= forwardDir;	
	}

	mCharacter->setWalkDirection(walkDirection*mSpeed);
	mMode = MODE_WALK;
	
}

void CharacterControllerObject::update2( float elapsedTime )
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

	
	if(!mIsArrived){

		//bool isBrake = false;

		//target�̕����Ɍ���
		{
			//mTarget���I�u�W�F�N�g"�㕽�ʏ�"�ɗ��Ƃ��B
			btVector3 targetDir = mTarget-mGhostObject->getWorldTransform().getOrigin();
			targetDir -= targetDir.dot( upDir ) * upDir;
			float theata;
			//0���΍�
			if( targetDir.length2() == 0.f ){
				theata = 0.f;
			} else {
				targetDir.normalize();
				theata = acosf( forwardDir.dot(targetDir) );
			}
			//0���΍�etc �܂���ł���Ă���̂ł���Ȃ���
			//�񐔂̏ꍇ�A(���������̏ꍇ)
			if( theata!=theata ){
				//bool a=theata<0.001f;
				//bool b=theata>0.001f;
				return;
			}
			//120�x�ȏ�̏ꍇ �u���[�L������
			//if(theata>3.14159265f*2.f/3.f){
			//	isBrake=true;
			//}
		
			//��]�������
			btScalar angularSpeed = mMaxAngularSpeed * elapsedTime;
			assert( theata>=0.f );
			if( theata > angularSpeed ){
				theata=angularSpeed;
			}
			//��]���E�������Bcos��+-�������Ă�̂ł��ׂā{�̉�]�ɂȂ��Ă���̂�
			btVector3 cross=forwardDir.cross(targetDir);
			if( cross.y() < 0.f){
				theata=-theata;
			}

			if(theata!=0.f){
				btMatrix3x3 orn = mGhostObject->getWorldTransform().getBasis();
				orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), theata));
				mGhostObject->getWorldTransform ().setBasis(orn);
			}

			btVector3 walkDir = (mTarget-xform.getOrigin());
			walkDir.setY(0.f);		
			const float distance = walkDir.length();
		
			if(distance < 000001.f){
				mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
				mIsArrived=true;
			}
			else
			{
				walkDir.normalize();
			
				//mSpeed = mMaxSpeed * elapsedTime;
				mSpeed = mMaxSpeed * 1.f/60.f;
		
				if( distance < mSpeed ){
					mSpeed = distance;
				} 
				//TRACE0(_T("pos : ") );
				//PrintBtVector3::toOutput( xform.getOrigin() );
				//TRACE0(_T("pos+vel : ") );
				//PrintBtVector3::toOutput(xform.getOrigin()+walkDir*mSpeed);
				mCharacter->setWalkDirection( walkDir*mSpeed );
			}
		}
	} else {
		mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
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
	mMode = MODE_CLICK;
}

void CharacterControllerObject::checkArrivalInNexTime() {
	if( !mIsArrived && mMode==MODE_CLICK){
		btVector3 walkDir = (mTarget-mGhostObject->getWorldTransform().getOrigin());
		walkDir.setY(0.f);		
		const float distance = walkDir.length();
		
		if(distance < 000001.f){
			mCharacter->setWalkDirection( btVector3( 0.f, 0.f, 0.f ) );
			mIsArrived=true;			
			return;

		}
		walkDir.normalize();
		
		if( distance < mSpeed ){
			mSpeed = distance;
			mCharacter->setWalkDirection( mSpeed*walkDir );				
		}

	}
}

}//end namespace GameObject

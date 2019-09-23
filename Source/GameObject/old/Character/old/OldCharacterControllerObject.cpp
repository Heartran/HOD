#include"OldCharacterControllerObject.h"

#include<GameLib/GameLib.h>
#include<GameLib/DebugScreen.h>
#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionDispatch/btGhostObject.h>
#include<BulletDynamics/Character/btKinematicCharacterController.h>
#include"BulletPhysics/CharacterController/kztrCharacterController2.h"

#include"BulletPhysics/BulletPhysics_Manager.h"
#include"helper/MyOutputDebugString.h"
#include"helper/PrintBtVector3.h"


//�ǂ̂悤�ɓ������̃e�X�g�̂��߂�include�B�}�E�X�Ƃ��̃R���g���[���͂����ɒu���Ȃ�
#include"MouseWrapper.h"
#include<GameLib/Input/Keyboard.h>

namespace Old {

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
	float maxVelocity,
	float maxAngularVelocity )
	:mVelocity(0.f),
	mAngularVelocity(0.f),
	mMaxVelocity(maxVelocity),
	mMaxAngularVelocity(maxAngularVelocity),
	mDashVel(0.f),
	mDashMaxVel(5.f),
	mDashAccel(1.f),
	mDashDeltaTime(10.f),
	mDashEnableTime(0.f),
	mDashMaxEnableTime( (1.f/30.f)*60.f ),//2sec
	mDashAddTime( (1.f/30.f)*6.f ),//0.2sec
	mAirResistance(0.1f),//��C��R�W��
	mCoefficientOfFriction(0.5f),//���C�W��
	mJumpVel(0.f),
	mJumpMaxVel(10.f),
	mJumpAccel(0.5f),
	mJumpDeltaTime(10.f),
	mJumpEnableTime(0.f),
	mJumpMaxEnableTime( (1.f/30.f)*12.f ),//0.4sec
	mJumpAddTime( (1.f/30.f)*12.f )//0.4sec
{
	btDiscreteDynamicsWorld *pWorld = BulletPhysics::Manager::instance()->getWorldPtr();
	btAxisSweep3* sweepBP = dynamic_cast<btAxisSweep3*>(pWorld->getBroadphase());
	assert( sweepBP && "" );
	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);
	
	
	btConvexShape* capsule = new btCapsuleShape(characterWidth, characterHeight);
	m_ghostObject->setCollisionShape (capsule);
	m_ghostObject->setCollisionFlags (  btCollisionObject::CF_CHARACTER_OBJECT);//m_ghostObject->getCollisionFlags() |

	btScalar stepHeight = btScalar(0.35);
	//m_character = new btKinematicCharacterController (m_ghostObject,capsule,stepHeight);
	m_character = new kztr::CharacterController2(m_ghostObject,capsule,stepHeight);
	
	//const int mmd_filter =  0x7fc0;

	//pWorld->addCollisionObject(
	//	m_ghostObject,
	//	btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter,
	//	btBroadphaseProxy::AllFilter & ~mmd_filter
	//);
	pWorld->addCollisionObject(
		m_ghostObject,
		btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter,
		btBroadphaseProxy::AllFilter
	);

	pWorld->addAction(m_character);

}

CharacterControllerObject::~CharacterControllerObject(){
	
	btDiscreteDynamicsWorld *pWorld = BulletPhysics::Manager::instance()->getWorldPtr();
	//BulletPhysics::Manager* mngr = BulletPhysics::Manager::instance();
	//cleanup in the reverse order of creation/initialization
	if (m_character)
	{
		pWorld->removeCollisionObject( m_ghostObject );
		pWorld->removeAction( m_character );
	}
	
	btCollisionShape *shape=m_ghostObject->getCollisionShape();
	SAFE_DELETE(shape);
	SAFE_DELETE(m_character);
	SAFE_DELETE(m_ghostObject);
}
void CharacterControllerObject::update(
	float elapsedTime,
	bool isForward,
	bool isBackward,
	bool isLeft,
	bool isRight 
){
	//float dt = getDeltaTimeMicroseconds() * 0.000001f;

	/* Character stuff &*/
	if (m_character)
	{
		
	}

	//during idle mode, just run 1 simulation step maximum
	//int maxSimSubSteps = m_idle ? 1 : 2;
	//if (m_idle) {dt = 1.0/420.f };


	///set walkDirection for our character
	btTransform xform;
	xform = m_ghostObject->getWorldTransform ();
	
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
	//bulletphysics�̃f������肭�����Ă�悤�Ɍ�����͍̂��E�Ώ̂̃I�u�W�F�N�g���g���Ă��邩��

	
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
	strafeDir.normalize ();

	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	btScalar walkVelocity = mMaxVelocity;
	btScalar walkSpeed = walkVelocity * elapsedTime;

	btScalar angularSpeed = mMaxAngularVelocity * elapsedTime;
	//rotate view
	if (isLeft)
	{
		btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), angularSpeed));
		m_ghostObject->getWorldTransform ().setBasis(orn);
	}

	if (isRight)
	{
		btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), -angularSpeed));
		m_ghostObject->getWorldTransform ().setBasis(orn);
	}

	if (isForward)
	{
		walkDirection += forwardDir;
	}
	if (isBackward)
	{
		walkDirection -= forwardDir;	
	}

	m_character->setWalkDirection(walkDirection*walkSpeed);
}

void CharacterControllerObject::update(
		float elapsedTime,
		float deltaPitch,
		float deltaYaw,
		bool isDash,
		bool isJump,
		bool isAprochTarget,
		const btVector3 &targetPos )
{
	//PrintBtVector3::toDebugScreen( m_character->getVelocity(), 0, 1 );

	/* Character stuff &*/
	if (m_character)
	{
		
	}
	
	///set walkDirection for our character
	btTransform xform;
	xform = m_ghostObject->getWorldTransform ();
	
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
	strafeDir.normalize ();

	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	btScalar walkVelocity = mMaxVelocity;
	btScalar walkSpeed = walkVelocity * elapsedTime;

	btScalar angularSpeed = mMaxAngularVelocity * elapsedTime;
	
	bool isBrake = false;

	//�}�E�X�ʒu�̕����Ɍ���
	{
		//mTarget���I�u�W�F�N�g"�㕽�ʏ�"�ɗ��Ƃ��B
		btVector3 targetDir = targetPos-m_ghostObject->getWorldTransform().getOrigin();
		//targetDir.setValue( 0,0,0 );
		targetDir -= targetDir.dot( upDir ) * upDir;
		float theata;
		//0���΍�
		if( targetDir.length2() == 0.f ){
			theata = 0.f;
		} else {
			targetDir.normalize();
			theata = acosf( forwardDir.dot(targetDir) );
		}
		//0���΍�etc
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
			btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
			orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), theata));
			m_ghostObject->getWorldTransform ().setBasis(orn);
		}


	}

	float const aprochLength=3.f;

	if(isAprochTarget){
		btVector3 pos = xform.getOrigin();
		btVector3 dir=targetPos-pos;
		if(dir.length() > aprochLength){
			walkDirection +=dir.normalize()*aprochLength;
		}
	}

	if(isDash){
		m_character->setVelocity( forwardDir*m_character->getMinSpeed() );
		//m_character->applyCentralForce( forwardDir*100.f );
		m_character->addAccelVelocityForTimeInterval( forwardDir*100.f, 6.f/30.f );
	}
	//���O�ł̎���
	if(false)
	if(isDash)
	{
		mDashEnableTime += mDashAddTime;
		mDashDeltaTime = 0.f;
		if(mDashEnableTime>mDashMaxEnableTime){
			mDashEnableTime=mDashMaxEnableTime;
		}
	}

	//calcVec()
	//if(mDashEnableTime>0.f)
	{
		float resist=0;
		if(isBrake){
			resist += 0.4f;
		}

		if( m_character->onGround() ){
			resist+=mCoefficientOfFriction;
			float accel = mDashAccel/(1.f+mDashDeltaTime);
			mDashVel = calcVel( resist, mDashVel, mDashMaxVel, accel );
		} else {
			resist+=mAirResistance;
			mDashVel = calcVel( 0.f, mDashVel, mDashMaxVel, 0.f );
		}
		walkDirection += forwardDir*mDashVel;
		mDashDeltaTime += elapsedTime;
		mDashEnableTime -= elapsedTime;
	}

	//controller�ł�jump
	if( isJump ){
		m_character->setJumpSpeed( 30.f );
		m_character->jump();
	}
	//���O�ł�jump����
	if(false)//����
	if( isJump && m_character->onGround() )
	{
		mJumpEnableTime += mJumpAddTime;
		mJumpDeltaTime = 0.f;
		if(mJumpEnableTime>mJumpMaxEnableTime){
			mJumpEnableTime=mJumpMaxEnableTime;
		}
	}
	//�ǏR��̏���
	else if( isJump ) {
		//������rayCast�o���āA�Ԃ���΃W�����v�ł���
		btSphereShape sphere(0.4f);
		btTransform from,to;
		from.setIdentity();
		const btTransform &characterWorldTrans = getWorldTransform();
		from.setOrigin( characterWorldTrans.getOrigin() );
		const btCapsuleShape *shape = dynamic_cast<const btCapsuleShape*>( m_ghostObject->getCollisionShape() );
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
			
			mJumpEnableTime += mJumpAddTime;
			mJumpDeltaTime = 0.f;
			if(mJumpEnableTime>mJumpMaxEnableTime){
				mJumpEnableTime=mJumpMaxEnableTime;
			}
		}

	}

	//��C��R�A���C�Ƃ�
	{
		float resist=mAirResistance;
		if(mJumpEnableTime>0.f) {
			mJumpVel = calcVel( resist, mJumpVel, mJumpMaxVel, mJumpAccel );
			mJumpDeltaTime += elapsedTime;
			mJumpEnableTime -= elapsedTime;
		} else {
			mJumpVel = calcVel( resist, mJumpVel, mJumpMaxVel, 0.f );
			mJumpDeltaTime = 0.f;
			mJumpEnableTime = 0.f;
		}
	}

	GameLib::Input::Keyboard kb;
	bool isKnockBack= kb.isOn('k');
	if(isKnockBack){
		m_character->setVelocity( btVector3( 0.f, 0.f, 0.f ) );
		m_character->applyCentralForce( -forwardDir*1000 );
	}
	
	walkDirection += upDir*mJumpVel;

	//m_character->setWalkDirection( walkDirection );
}

const btTransform & CharacterControllerObject::getWorldTransform() const {
	return m_ghostObject->getWorldTransform();
	
}

void CharacterControllerObject::draw() const {
	BulletPhysics::Manager::drawObject( m_ghostObject, 0xff000000 );
}

void CharacterControllerObject::warp( const btVector3 &pos ) {
	m_character->warp( pos );
}



void CharacterControllerObject::update2(
		float elapsedTime,
		float deltaPitch,
		float deltaYaw,
		bool isDash,
		bool isJump,
		bool isAprochTarget,
		const btVector3 &targetPos )
{

	/* Character stuff &*/
	if (m_character)
	{
		
	}

	///set walkDirection for our character
	btTransform xform;
	xform = m_ghostObject->getWorldTransform ();
	
	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.setX(-forwardDir.x());
	//mmd��������W�Ƃ��č���Ă���̂ŋt�������Ă���̂Ŕ��Ε�����O�Ƃ���B
	forwardDir = -forwardDir;
	forwardDir.normalize ();


	//printf("forwardDir=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
	btVector3 upDir = xform.getBasis()[1];
	btVector3 strafeDir = xform.getBasis()[0];
	upDir.normalize ();
	strafeDir.normalize ();

	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	btScalar walkVelocity = mMaxVelocity;
	btScalar walkSpeed = walkVelocity * elapsedTime;

	btScalar angularSpeed = mMaxAngularVelocity * elapsedTime;
	
	bool isBrake = false;

	if( CMouseWrapper::isCursolOnLeft() ){
		btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), angularSpeed));
		m_ghostObject->getWorldTransform ().setBasis(orn);
	} else if( CMouseWrapper::isCursolOnRight() ){
		btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), -angularSpeed));
		m_ghostObject->getWorldTransform ().setBasis(orn);
	}

	float const aprochLength=3.f;

	if(isAprochTarget){
		btVector3 pos = xform.getOrigin();
		btVector3 dir=targetPos-pos;
		if(dir.length() > aprochLength){
			walkDirection +=dir.normalize()*aprochLength;
		}
	}
	
	if(isDash)
	{
		mDashEnableTime += mDashAddTime;
		mDashDeltaTime = 0.f;
		if(mDashEnableTime>mDashMaxEnableTime){
			mDashEnableTime=mDashMaxEnableTime;
		}
	}

	//calcVec()
	//if(mDashEnableTime>0.f)
	{
		float resist=0;
		if(isBrake){
			resist += 0.4f;
		}

		if( m_character->onGround() ){
			resist+=mCoefficientOfFriction;
			float accel = mDashAccel/(1.f+mDashDeltaTime);
			mDashVel = calcVel( resist, mDashVel, mDashMaxVel, accel );
		} else {
			resist+=mAirResistance;
			mDashVel = calcVel( 0.f, mDashVel, mDashMaxVel, 0.f );
		}
		walkDirection += forwardDir*mDashVel;
		mDashDeltaTime += elapsedTime;
		mDashEnableTime -= elapsedTime;
	}

	



	if( isJump && m_character->onGround() )
	{
		mJumpEnableTime += mJumpAddTime;
		mJumpDeltaTime = 0.f;
		if(mJumpEnableTime>mJumpMaxEnableTime){
			mJumpEnableTime=mJumpMaxEnableTime;
		}
	}

	{
		float resist=mAirResistance;
		if(mJumpEnableTime>0.f) {
			mJumpVel = calcVel( resist, mJumpVel, mJumpMaxVel, mJumpAccel );
			mJumpDeltaTime += elapsedTime;
			mJumpEnableTime -= elapsedTime;
		} else {
			mJumpVel = calcVel( resist, mJumpVel, mJumpMaxVel, 0.f );
			mJumpDeltaTime = 0.f;
			mJumpEnableTime = 0.f;
		}
	}
	
	walkDirection += upDir*mJumpVel;

	m_character->setWalkDirection( walkDirection );
}

void CharacterControllerObject::setUserPtr( void *ptr ){
	m_ghostObject->setUserPointer( ptr );
}
const btVector3& CharacterControllerObject::getVelocity() const {
	return m_character->getVelocity();
}

bool CharacterControllerObject::onGround() const{
	return m_character->onGround();
}
bool CharacterControllerObject::onGroundPerhaps() const{
	return m_character->onGroundPerhaps();
}

btCollisionObject* CharacterControllerObject::getBtCollisionObject(){
	return m_ghostObject;
}
}//end namespace Old

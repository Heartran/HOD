#include"CarMover.h"
#include<btBulletDynamicsCommon.h>
//#include<btBulletCollisionCommon.h>
//#include<BulletDynamics/Dynamics/btRigidBody.h>
#include"GameObjectData/Status.h"
#include"BulletPhysics/HelperCallbacks.h"
#include"BulletPhysics/Manager.h"

namespace GameObject { namespace PlayerUtils{ namespace Updater { 
CarMover::CarMover(btRigidBody* rb, GameObjectData::Status *s)
	:mRigidBody(rb),
	mStatus(s)
{}
CarMover::~CarMover(){
	mRigidBody=0;
	mStatus=0;
}

void CarMover::applyImpulseTowardTarget(const btVector3& targetPos, float elapsedTime ){
	
	const float &accel = mStatus->accel;
	const float &maxSpeed = mStatus->speed;
	const float &angleSpeed = mStatus->angleSpeed;
	const float &mass = mStatus->mass;

	const float rateBeforeSpeed=0.1f;

	btTransform xform = mRigidBody->getWorldTransform();
	//�������߂�
	//xform*z0
	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.setX(-forwardDir.x());
	//mmd��������W�Ƃ��č���Ă���̂ŋt�������Ă���̂Ŕ��Ε�����O�Ƃ���B
	forwardDir = -forwardDir;
	forwardDir.normalize ();

	btVector3 upDir = xform.getBasis()[1];
	btVector3 strafeDir = xform.getBasis()[0];
	upDir.normalize ();
	strafeDir.normalize();


	//�����ɗ��Ƃ���targetDir��forwardDir�̊Ԃ̊p�x
	float angleBetweenForwardAndTarget=0.f;

	//target�̕����Ɍ���
	{
		//��]����p�x
		float theata = 0.f;

		//targetPos���I�u�W�F�N�g"�㕽�ʏ�"�ɗ��Ƃ��B
		btVector3 targetDir = targetPos-xform.getOrigin();
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
			theata = 0.f;
		}

		if(theata!=0.f){
			//��]�������(��b��45�x��])
			btScalar angularSpeed = angleSpeed * elapsedTime;
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
			btVector3 vel=mRigidBody->getLinearVelocity();
			float speed=vel.length();
			vel.normalize();
			targetDir;

			//�ړ������̂̃R���g���[��
			mRigidBody->setLinearVelocity( ( (1-rateBeforeSpeed)*vel+targetDir*rateBeforeSpeed ) * speed );
			//�V���v���ɂ��������H
			//mRigidBody->setLinearVelocity( ( (1-rateBeforeSpeed)*vel ) * speed );

		}
	}

	//�ړ�
	{
		btVector3 walkDir = targetPos-xform.getOrigin();
		walkDir.setY(0.f);		
		const float distance = walkDir.length();

		if(distance < 0.001){

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

			mRigidBody->applyCentralImpulse( walkDir*accel*mass*rate );
			limitSpeed( maxSpeed );			
		}
	}
}

void CarMover::limitSpeed( float maxSpeed ){
	const btVector3 &vel = mRigidBody->getLinearVelocity();
	float speed2=vel.length2();
	if( maxSpeed*maxSpeed < speed2 ){
		float speed = sqrt( speed2 );
		mRigidBody->setLinearVelocity(vel/speed*maxSpeed);
	}
}
void CarMover::applyImpulseTowardTargetIf(const btVector3& target, float elapsedTime ){
	if( mStatus->canRun( elapsedTime ) ){
		applyImpulseTowardTarget( target, elapsedTime );
	}
}

const btCollisionObject* CarMover::rayTest( const btVector3 &start, const btVector3 &end ){

	//�n�_�ƏI�_
	
	
	BulletPhysics::ClosestRayResultCallbackToIgnoreObject cb( mRigidBody );
	cb.m_collisionFilterMask = BulletPhysics::AllFilterExceptMMDObject;
		
	BulletPhysics::Manager::instance()->getWorldPtr()->rayTest( start, end, cb );

	if (cb.hasHit())
	{
		return cb.m_collisionObject;
	}

	
	return 0;
}

} } } //GameObject::PlayerUtils::Updater
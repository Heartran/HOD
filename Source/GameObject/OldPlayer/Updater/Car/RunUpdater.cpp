#include"RunUpdater.h"
#include"GameObject/OldPlayer/Player.h"
#include<GameLib/Input/Mouse.h>
#include"Input/PlayerMouse.h"
#include<GameLib/GameLib.h>
#include"Camera/PlayerCamera.h"
#include"GameObjectData/Status.h"
//#include<LinearMath/btVector3.h>

#include"BulletPhysics/Manager.h"

#include"helper/PrimitiveDrawer.h"
#include"SegaTypeHelper.h"

#include<GameLib/DebugScreen.h>
namespace{
	const float elapsedTime= 1.f/30.f;

	void applyImpulseToTarget( btRigidBody* rb, const btVector3&target ){
		//tofix
		const float speed=2.f;
		const float maxSpeed=90.f;
		const float mass=30.f;
		const float angleSpeed = 3.14*0.25;
		const float rateBeforeSpeed=0.1;
		btTransform xform = rb->getWorldTransform();
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

			//mouse���I�u�W�F�N�g"�㕽�ʏ�"�ɗ��Ƃ��B
			btVector3 targetDir = target-xform.getOrigin();
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
				rb->getWorldTransform ().setBasis(orn);
				btVector3 vel=rb->getLinearVelocity();
				float speed=vel.length();
				vel.normalize();
				targetDir;

				rb->setLinearVelocity( ( (1-rateBeforeSpeed)*vel+targetDir*rateBeforeSpeed ) * speed );
			}
		}

		//�ړ�
		{
			btVector3 walkDir = target-xform.getOrigin();
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

				rb->applyCentralImpulse( walkDir*speed*mass*rate );

				btVector3 vel = rb->getLinearVelocity();
				float speed=vel.length();
				if( maxSpeed<speed ){
					rb->setLinearVelocity(vel/speed*maxSpeed);
				}

				if( distance < speed*elapsedTime ){
					//mPlayer->stop();
				}
			}
		}
	}
//��ԋ߂�btCollisionObject��Ԃ�
btCollisionObject* rayTest( const btVector3 &start, const btVector3 &end, const btCollisionObject& obj ){
	//�n�_�ƏI�_
	btTransform bttr( btMatrix3x3::getIdentity(), start );
	btTransform bttr1( btMatrix3x3::getIdentity(), end );
	
	struct ClosestConvexResultCallbackToIgnoreObject : public btCollisionWorld::ClosestConvexResultCallback
	{
		btCollisionObject const *m_ignoredObj;

		ClosestConvexResultCallbackToIgnoreObject(
			const btCollisionObject *ignoredObj,
			const btVector3& convexFromWorld,
			const btVector3& convexToWorld )
			:ClosestConvexResultCallback( convexFromWorld, convexToWorld ),
			m_ignoredObj(ignoredObj)
		{}
		virtual ~ClosestConvexResultCallbackToIgnoreObject(){}

		virtual btScalar addSingleResult( btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
		{
			//�w��I�u�W�F�N�g���Ԃ����ĂȂ��ꍇ
			if( convexResult.m_hitCollisionObject == m_ignoredObj ){
				return 1.0;
			}
			return ClosestConvexResultCallback::addSingleResult( convexResult, normalInWorldSpace );
		}

	};
	const btConvexShape* shape = dynamic_cast<const btConvexShape*>( obj.getCollisionShape() );
	assert(shape);
	ClosestConvexResultCallbackToIgnoreObject cb( &obj, start,  end );
	cb.m_collisionFilterMask = btBroadphaseProxy::AllFilter & ~BulletPhysics::Manager::MMDObjectFilter;
		
	BulletPhysics::Manager::instance()->getWorldPtr()->convexSweepTest( shape, bttr, bttr1, cb );

	if (cb.hasHit())
	{
		return cb.m_hitCollisionObject;
	}
	return 0;
}

//��ԋ߂�btCollisionObject��Ԃ�
btCollisionObject* rayTestSphere( const btVector3 &start, const btVector3 &end, const btCollisionObject* ignoreobj, float radius ){
	//�n�_�ƏI�_
	btTransform bttr( btMatrix3x3::getIdentity(), start );
	btTransform bttr1( btMatrix3x3::getIdentity(), end );
	
	struct ClosestConvexResultCallbackToIgnoreObject : public btCollisionWorld::ClosestConvexResultCallback
	{
		btCollisionObject const *m_ignoredObj;

		ClosestConvexResultCallbackToIgnoreObject(
			const btCollisionObject *ignoredObj,
			const btVector3& convexFromWorld,
			const btVector3& convexToWorld )
			:ClosestConvexResultCallback( convexFromWorld, convexToWorld ),
			m_ignoredObj(ignoredObj)
		{}
		virtual ~ClosestConvexResultCallbackToIgnoreObject(){}

		virtual btScalar addSingleResult( btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
		{
			//�w��I�u�W�F�N�g���Ԃ����ĂȂ��ꍇ
			if( convexResult.m_hitCollisionObject == m_ignoredObj ){
				return 1.0;
			}
			return ClosestConvexResultCallback::addSingleResult( convexResult, normalInWorldSpace );
		}

	};
	btSphereShape sphere(radius);
	//const btConvexShape* shape = ;
	ClosestConvexResultCallbackToIgnoreObject cb( ignoreobj, start,  end );
	cb.m_collisionFilterMask = btBroadphaseProxy::AllFilter & ~BulletPhysics::Manager::MMDObjectFilter;
		
	BulletPhysics::Manager::instance()->getWorldPtr()->convexSweepTest( &sphere, bttr, bttr1, cb );

	if (cb.hasHit())
	{
		return cb.m_hitCollisionObject;
	}
	return 0;
}

}
namespace GameObject{ namespace OldPlayer{ namespace Updater{ namespace Car{

RunUpdater::RunUpdater( Player* player )
	:IUpdater(player)
{
	mPlayer->changeAnimation( ANIM_RUN );
}
RunUpdater::~RunUpdater(){}
void RunUpdater::update( float elapsedTime ){
	GameLib::Input::Mouse m;
	btRigidBody* rb= btRigidBody::upcast(mPlayer->getBtCollisionObject());
	//rb->getCollisionShape()->getBoundingSphere();
	const btVector3& start = rb->getWorldTransform().getOrigin();
	const btVector3& end = start+rb->getLinearVelocity()*elapsedTime;
	btCollisionObject *hit = rayTestSphere( start, end, rb, 3.f );
	if(hit){
		mPlayer->stop();
		mPlayer->changeUpdater( CAR_STAND );
		return;
	}

	if( m.isOn(m.BUTTON_LEFT) ){
		applyImpulseToTarget( rb, mPlayer->getMousePos() );	
	}

	if( m.isOn(m.BUTTON_RIGHT) ){
		mPlayer->stop();
		mPlayer->changeUpdater( CAR_STAND );
		return;
	}
	GameLib::DebugScreen ds;
	ds << "Car::RunUpdate" <<GameLib::endl;
}

void RunUpdater::onCollisionStay( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj ){
	mPlayer->stop();
	mPlayer->changeUpdater( CAR_STAND );
}




} } } }



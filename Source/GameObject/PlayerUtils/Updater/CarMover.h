#pragma once

class btRigidBody;
class btCollisionObject;
class btVector3;
namespace GameObjectData{
	class Status;
}

namespace GameObject { namespace PlayerUtils{ namespace Updater { 
class CarMover{
public:
	CarMover(btRigidBody* rb, GameObjectData::Status *s);
	virtual ~CarMover();
	
	//target�����ɗ͂𑫂������ł����āA���̏ꏊ�܂ł͍s���Ȃ�
	//�Ȃ̂ŁA���̖��O�͂��������Ǝv��
	//void moveTo(const btVector3 &targetPos, float elapsedTime );
	
	//applyImpulseTorwardTarget
	void applyImpulseTowardTarget(const btVector3 &targetPos, float elapsedTime );
	//apply impulse if has enough stamina.	
	void applyImpulseTowardTargetIf(const btVector3 &targetPos, float elapsedTime );
	
	void limitSpeed(float maxSpeed);

	//return closest object from start.
	const btCollisionObject* rayTest( const btVector3 &start, const btVector3 &end);
private:
	btRigidBody *mRigidBody;
	GameObjectData::Status *mStatus;
};

} } } //GameObject::PlayerUtils::Updater
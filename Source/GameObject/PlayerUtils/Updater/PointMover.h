#pragma once
#include<LinearMath/btVector3.h>

class btRigidBody;
class btVector3;
namespace GameObjectData{
	class Status;
}

namespace GameObject { namespace PlayerUtils{ namespace Updater { 
class PointMover{
public:
	PointMover(btRigidBody* rb, GameObjectData::Status *s);
	virtual ~PointMover();

	//void update( float elapsedTime );

	void updateStrafe( float elapsedTime );
	void updateLook( float elapsedTime );

	void move( const btVector3 &pos );
	void moveForward( const float &moveValue );
	
	void jump();
	// jump If have enough stamina.
	bool jumpIf();
	
	//void forceJump();
	//void jump(const float &speed);

	//jumpAnywhere
	//void boost();


	//���̏�ɒ�~
	void stop();
	//���C�Ŏ����ɂƂ܂�(��������)
	//void cut();

	void look( const btVector3 & eyePos );
	//��]�Ȃ��̈ړ�
	void strafe( const btVector3 & targetPos );

	bool isStopped() const;
	bool isTurning() const;
	bool isMoving() const;
	bool onGround() const;

	//�X�^�~�i�����̂ݏ�Ԉڍs
	bool useStaminaIfCanRunHigh();	
	bool useStaminaIfCanRun();
	bool useStaminaIfCanWalk();

	void setMaxSpeed( float speed );

	void setMaxSpeedToLow();
	void setMaxSpeedToNormal();
	void setMaxSpeedToHigh();

	void healStamina();


protected:
	btRigidBody *mRigidBody;
	GameObjectData::Status *mStatus;

	unsigned short mFilterGroup;
	unsigned short mFilterMask;

	btVector3 mTargetPos;
	btVector3 mEyePos;
	
	const float &mMass;
	const float &mAccel;

	const float	&mMaxSpeed;
	const float	&mMaxAngularSpeed;

	btVector3 mOldPos;//��t���[���O�̈ʒu
	float mAngle;

	bool mIsLookingFinished;
};

} } } //GameObject::PlayerUtils::Updater
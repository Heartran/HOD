#pragma once

#include<btBulletCollisionCommon.h>
#include"GameObject/ICollider.h"
#include"RTTI/RTTI.h"
#include<memory>


class btCapsuleShape;
class btCylinderShape;
typedef std::shared_ptr<btCylinderShape> BtCylinderShapeSharedPtr;
typedef std::shared_ptr<btRigidBody> BtRigidBodySharedPtr;
class btCharacterControllerInterface;
class btKinematicCharacterController;
class btPairCachingGhostObject;
class btCollisionShape;
class btGhostPairCallback;
class btTransform;
class btVector3;
namespace GameObject { namespace OldPlayer {

class CharacterControllerRigidBody;
typedef std::shared_ptr<CharacterControllerRigidBody> CharacterControllerRigidBodySharedPtr;
//namespace Controller�ɂł�����ׂ���������Ȃ�
//GameObject�Ƃ������btRigitBody�̃��b�p�[�ɋ߂�
//�Փˌ��o���x������̂�CharacterControllerRigidBody�����


//�J�v�Z���^�̃L�����N�^�[�𑀍삷��I�u�W�F�N�g
class CharacterControllerRigidBody {
public:

	//float maxSpeed,�@:�@�ō����x
	//float maxAngularSpeed:�ō��p���x
	//float mass �d��
	static CharacterControllerRigidBodySharedPtr create(
		BtRigidBodySharedPtr rb,
		float maxSpeed,
		float maxAngularSpeed,
		float mass
	);
	virtual ~CharacterControllerRigidBody();
	
	btCollisionObject* getBtCollisionObject();

	const btTransform & getWorldTransform() const;

	btTransform & getWorldTransform();
	
	void removeFromWorld();
	void addToWorld();
	
	void draw()const;

	void warp( const btVector3 &pos );
	
	btVector3 getVelocity() const;
	//void setSpeed( const float &v );
	
	//float getAngularSpeed();
	//void setAngularSpeed( const float &v );

	//void setUserPtr( void *ptr );

	bool onGround() const;

	void stop();
	//�����ɖ��C�łƂ܂点��
	void cut();
	
	void look( const btVector3 &pos );

	void move( const btVector3 &pos );
	
	//�����Ă�H
	//canMove��false�̏ꍇfalse
	bool isMoving() const;
	//��]���Ă�H
	//canTurn��false�̏ꍇfalse
	bool isTurning() const;
	//�~�܂��Ă���H
	//�~�܂��Ă��Ă������œ����Ă�ꍇ������
	bool isStopping() const;

	//���s�ړ�����B�i��]���Ȃ��ňړ��j
	void strafe( const btVector3 & pos );

	//�g����2�{
	void jumpHi();
	//�g���̔���
	void jumpLow();
	//�g����
	void jump();
	//�ڒn���Ă��Ȃ��Ă��W�����v
	void forceJump();
	//���x�Z�b�g
	void jump(const float &impulseY );
	void forceJump( const float &impulseY );

	//void clearForce();

	//void hitback( const btVector3 & impulse );


	void update(float elapsedTime);

	void setMaxSpeed( float maxSpeed ){
		mMaxSpeed = maxSpeed;
	}

private:
	//disallow raw pointer. use create().
	//�J�v�Z���`��̓����蔻������
	CharacterControllerRigidBody(
		BtRigidBodySharedPtr rb,
		float maxSpeed,
		float maxAngularSpeed,
		float mass );
	//disallow assign
	CharacterControllerRigidBody( const CharacterControllerRigidBody & );
	//disallow copy
	void operator=( const CharacterControllerRigidBody & );

	ICollider *mParent;

	BtRigidBodySharedPtr			mRigidBody;
	btCylinderShape					*mShape;

	float							mMaxSpeed;
	float							mMaxAngularSpeed;

	btVector3 mTarget;

	bool mHasMoved;//�ړI�n�ɒ������H
	bool mCanMove;//�ړ��ł���Hlook����������̂Ɏg�p
	bool mCanTurn;//��]�ł���H
	bool mHasTurned;//��]�͏I�������H

	unsigned short mFilterGroup;
	unsigned short mFilterMask;

	bool mIsInWorld;

	float mSpeed;
	float mMass;

	float mJumpSpeed;

	bool mIsOnGround;
};

} }//end namespace GameObject::OldPlayer


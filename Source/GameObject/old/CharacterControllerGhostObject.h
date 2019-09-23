#ifndef __GAMEOBJECT_CHARACTERCONTOROLERGHOSTOBJECT_H__
#define __GAMEOBJECT_CHARACTERCONTOROLERGHOSTOBJECT_H__

#include<btBulletCollisionCommon.h>
#include"GameObject/ICollider.h"
#include"RTTI/RTTI.h"
#include<memory>

namespace kztr{
	class CharacterController2;
}
class btCapsuleShape;
class btCharacterControllerInterface;
class btKinematicCharacterController;
class btPairCachingGhostObject;
class btCollisionShape;
class btGhostPairCallback;
class btTransform;
class btVector3;
namespace GameObject {

class CharacterControllerGhostObject;
typedef std::shared_ptr<CharacterControllerGhostObject> CharacterControllerGhostObjectSharedPtr;

//�Փˌ��o���x������̂�CharacterControllerRigidBody�����
//�J�v�Z���^�̃L�����N�^�[�𑀍삷��I�u�W�F�N�g
class CharacterControllerGhostObject : public ICollider {
public:
	static CharacterControllerGhostObjectSharedPtr create(
		ICollider* parent,
		const btTransform &startTransform,
		float characterHeight,
		float characterWidth,
		float velocity,
		float angularSpeed
	);
	virtual ~CharacterControllerGhostObject();

	//btTransform const & getWorldTransform() const;
	//btTransform & getWorldTransgorm();
	
	RTTI_DECL;
	
	//�Ȃ����NULL�Ԃ�
	virtual btCollisionObject* getBtCollisionObject();
	
	virtual void removeFromWorld();
	virtual void addToWorld();
	
	//btCollisionObject���ڐG���Ă���ԁA�Ă΂ꑱ����
	virtual void onCollisionStay( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj){
		mParent->onCollisionStay( manifold, me, obj );
	}

	//btCollisionObject���Փ˂����Ƃ��Ă΂��
	virtual void onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj){
		mParent->onCollisionEnter( manifold, me, obj );	
	}

	//btCollisionObject�����ꂽ�Ƃ��Ă΂��
	virtual void onCollisionExit( btCollisionObject *me, btCollisionObject *obj ){
		mParent->onCollisionExit( me, obj );
	}

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
	void jump(const float &speed );
	void forceJump( const float &speed );

	//void clearForce();

	//void hitback( const btVector3 & impulse );


	void update(float elapsedTime);

	void setMaxSpeed( float maxSpeed ){
		mMaxSpeed = maxSpeed;
	}


	//�����������m�F
	//����ɓ�������Ȃ�X�s�[�h�𒲐�
	//�������Ă���΁A�X�s�[�h��0��
	//setWalkDirection �o�[�W����
	void checkArrivalInNexTime();
	//setVelocity �o�[�W����
	void checkArrivalInNexTime( float dt );
private:
	//disallow raw pointer. use create().
	//�J�v�Z���`��̓����蔻������
	CharacterControllerGhostObject(
		ICollider* parent,
		const btTransform &startTransform,
		float characterHeight,
		float characterWidth,
		float velocity,
		float angularSpeed );
	//disallow assign
	CharacterControllerGhostObject( const CharacterControllerGhostObject & );
	//disallow copy
	void operator=( const CharacterControllerGhostObject & );

	ICollider *mParent;

	kztr::CharacterController2		*mCharacter;//btKinematicCharacterController	*m_character;
	btPairCachingGhostObject		*mGhostObject;
	btCapsuleShape					*mCapsule;

	float							mMaxSpeed;
	float							mMaxAngularSpeed;

	bool	mIsDash;
	bool	mIsJump;
	btVector3 mTarget;

	bool mHasMoved;//�ړI�n�ɒ������H
	bool mCanMove;//�ړ��ł���Hlook����������̂Ɏg�p
	bool mCanTurn;//��]�ł���H
	bool mHasTurned;//��]�͏I�������H

	unsigned short mFilterGroup;
	unsigned short mFilterMask;

	bool mIsInWorld;

	float mSpeed;
	//float mMass;

	float mJumpSpeed;
	float mJumpHiSpeed;
	float mJumpLowSpeed;
};

}//end namespace GameObject

#endif //__GAMEOBJECT_CHARACTERCONTOROLERGHOSTOBJECT_H__
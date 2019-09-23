#ifndef __CHARACTERCONTOROLEROBJECT_H__
#define __CHARACTERCONTOROLEROBJECT_H__

#include<btBulletCollisionCommon.h>
#include"GameObject/IGameObject.h"
#include"RTTI/RTTI.h"

namespace kztr{
	class CharacterController2;
}

class btCharacterControllerInterface;
class btKinematicCharacterController;
class btPairCachingGhostObject;
class btCollisionShape;
class btGhostPairCallback;
class btTransform;
class btVector3;
namespace GameObject {
//�J�v�Z���^�̃L�����N�^�[�𑀍삷��I�u�W�F�N�g
class CharacterControllerObject : public IGameObject {
public:
	enum Mode {
		MODE_WALK,
		MODE_ACCEL,
		MODE_CLICK
	};

	//�J�v�Z���`��̓����蔻������
	CharacterControllerObject(
		const btTransform &startTransform,
		float characterHeight,
		float characterWidth,
		float velocity,
		float angularSpeed );

	virtual ~CharacterControllerObject();

	virtual btTransform const & getWorldTransform() const;
	virtual btTransform & getWorldTransgorm();

	virtual GameObjectType getGameObjectType() const {
		return CHARACTER_CONTROLLER;
	}

	virtual GameObjectId getID() const {
		return mId;
	}
	
	RTTI_DECL;

	virtual bool isTerrain() const{
		return false;
	}

	virtual bool isMMDObject() const{
		return false;
	}

		
	virtual bool enableCollidedMMDObject() const{
		return false;
	}
	
	//�Ȃ����NULL�Ԃ�
	virtual btCollisionObject* getBtCollisionObject();
	
	//BoneCollisionObject�̎����̃p���`�������ɓ�����Ȃ��悤�ɂ��邽�ߕK�v
	//�����蔻���2�ȏ㎝�ꍇ���ꍇ�i�n�`�p�A���̑��j�K�v�B
	//����Ȃ��ꍇ��getEmptyBtCollisionObjects()���g���Ď����B
	//virtual const std::vector< const btCollisionObject* > & getNotCollideBtCollisionObjects() const
	//{
	//	return getEmptyBtCollisionObjects();
	//}

	//virtual void applyForce( const btVector3 &force ){};

	virtual void removeFromWorld();
	virtual void addToWorld();
	

	void draw()const;

	void warp( const btVector3 &pos );
	
	const btVector3& getVelocity() const;
	//void setSpeed( const float &v );
	
	//float getAngularSpeed();
	//void setAngularSpeed( const float &v );

	void setUserPtr( void *ptr );

	bool onGround() const;
	bool onGroundPerhaps() const;

	void setControleFlags( 
		bool isDash,
		bool isJump,
		const btVector3 &target )
	{
		mIsDash = isDash;
		mIsJump = isJump;
		mTarget = target;
		mIsArrived = false;
	}

	//
	void setWalkControleFlags(
		bool up,
		bool down,
		bool left,
		bool right 	){
			mIsUp		= up;
			mIsDown	= down;
			mIsLeft	= left;
			mIsRight	= right;
	}

	//update3���1�t���[���Ԃɂǂꂩ������Ăяo������

	//�E���ŉ�]�㉺�őO��i
	void walkControleUpdate( float elapsedTime );
	
	//�^�[�Q�b�g�Ɍ������Đi�ރA�N�Z���^
	void update(float elapsedTime);

	//�^�[�Q�b�g�Ɍ������Đi�ގ����^
	void update2( float elapsedTime );

	//�����������m�F
	//����ɓ�������Ȃ�X�s�[�h�𒲐�
	//�������Ă���΁A�X�s�[�h��0��
	void checkArrivalInNexTime();
private:
	const GameObjectId	mId;

	kztr::CharacterController2		*mCharacter;//btKinematicCharacterController	*m_character;
	btPairCachingGhostObject		*mGhostObject;
	
	float							mMaxSpeed;
	float							mMaxAngularSpeed;

	bool	mIsDash;
	bool	mIsJump;
	btVector3 mTarget;

	bool mIsArrived;//�ړI�n�ɓ����������H

	bool mIsUp;
	bool mIsDown;
	bool mIsLeft;
	bool mIsRight;

	unsigned short mFilterGroup;
	unsigned short mFilterMask;


	bool mIsInWorld;

	Mode mMode;

	float mSpeed;
};

}//end namespace GameObject

#endif //__CHARACTERCONTOROLEROBJECT_H__
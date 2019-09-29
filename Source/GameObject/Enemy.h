#pragma once
#ifndef __GAMEOBJECT_ENEMY_H__
#define __GAMEOBJECT_ENEMY_H__
#include<memory>
#include"BaseObject.h"

class btRigidBody;
class btSphereShape;
class btBoxShape;
class btTransform;

typedef std::shared_ptr<btBoxShape> BtBoxShapeSharedPtr;


namespace GameObjectData{
	class Status;
	class WeaponStatus;
}

namespace GUI{
	class Label;
}

namespace MMD{
	class HumanModel;
}

namespace MME{ namespace Object {
	class ModelKai;
	typedef std::shared_ptr<ModelKai> ModelKaiSharedPtr;
} }

namespace Component {
	class RigidBodyComponent;
	typedef std::shared_ptr<RigidBodyComponent> RigidBodyComponentSharedPtr;
}

namespace GameObject {

class Enemy;
typedef std::shared_ptr<Enemy> EnemySharedPtr;

class Enemy : public  BaseObject {
public:
	static EnemySharedPtr create(
		float mass,
		MME::Object::ModelKaiSharedPtr model,
		BtBoxShapeSharedPtr shape,
		BtMotionStateSharedPtr motionState,
		BtRigidBodySharedPtr rigidbody,
		GameObjectData::StatusSharedPtr status );

	virtual ~Enemy();
	
	RTTI_DECL;

	void postdraw() const;

	void update(float elapsedTime);

	virtual void removeFromWorld();
	virtual void addToWorld();

	virtual void onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj );
	
	bool isDied()const;

	//void setPlayer(IColliderSharedPtr player){
	//	mPlayer=player;
	//}

	void updateWhenDied( float );

	btCollisionObject* getBtCollisionObject();

	const btCollisionObject* getConstBtCollisionObject()const;

	const btTransform & getWorldTransform() const;

	btTransform & getWorldTransform();

	bool isInWorld()const;

	//�����Ń_���[�W�v�Z���s���A�q�b�g�o�b�N�������͂܂��ʂ̏��ōs��
	virtual void takeDamage(
		BaseObject* DamageCauser
	);

	virtual int calculateDamage()override;

	//�_���[�W��^����I�u�W�F�N�g���ǂ����H
	virtual bool canDamage()override;

	//�_���[�W�����߂�̂Ɏg�p
	//�_���[�W��^����I�u�W�F�N�g�łȂ����null��Ԃ����Ƃ�����
	virtual const GameObjectData::WeaponStatus* getWeaponStatus()override;

	int getChain() const {
		return mChain;
	}

private:
	//�������Ɏ����̂̃X�}�[�g�|�C���^�g�������ꍇ�����肻���Ȃ̂ł����Ń����o�[�̏������͂��Ȃ�
	//�v�͒x����������������
	Enemy();
	void init(
		float mass,
		MME::Object::ModelKaiSharedPtr model,
		BtBoxShapeSharedPtr shape,
		BtMotionStateSharedPtr motionState,
		BtRigidBodySharedPtr rigidbody,
		GameObjectData::StatusSharedPtr status
	);

	
	//disallow copy
	void operator=( const Enemy & )=delete;
	//disallow assign
	Enemy(const Enemy & )=delete;
	
	void initBullet();

	float mMass;

	Component::RigidBodyComponentSharedPtr mRigidBodyComponent;

	MME::Object::ModelKaiSharedPtr mModel;

	GameObjectData::StatusSharedPtr mStatus;

	float			mElapsedTime;
	float			mElapsedTimeSinceDeath;

	float mElapsedTimeAfterLunch;
	
	GUI::Label *mHpBar;
	GUI::Label *mHpBackGroundBar;

	unsigned short	mFilterGroup;
	unsigned short	mFilterMask;

	int mDamageValueForChain;
	int mChain;


};
}//end namespace GameObject
#endif
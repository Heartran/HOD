#ifndef __GAMEOBJECT_BONE_COLLISIONOBJECT_H__
#define __GAMEOBJECT_BONE_COLLISIONOBJECT_H__
#include"GameObject/ICollider.h"
#include"RTTI/RTTI.h"
#include<vector>
#include<memory>
class btPairCachingGhostObject;
class btGhostObject;
class btCollisionShape;
class btVector3;
class btCollisionObject;
class CBone;
namespace MMD{
	namespace AnimationState {
		class BaseAnimationState;
	}
}

namespace GameLib {
	namespace Scene {
		class Model;
		class Batch;
		class Container;
	}
}

namespace GameObjectData{
	class Status;
	class WeaponStatus;
}

namespace GameObject {
class Enemy;

//�p���`�Ƃ��L�b�N�̃{�[�����K�v�ȓ����蔻��
//kinematic collision object with bone
//�����ɖ󂷂� �{�[���ɒǏ]����L�l�}�e�B�b�N�����蔻��
//kinematic object �Ƃ͉������Ƃ͏o���邯�ǁA�����͉�����Ȃ��I�u�W�F�N�g
//mRigidBody->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE );
//mRigidBody->setActivationState( DISABLE_DEACTIVATION );
// ��̐ݒ�Ō��݂͉����Ȃ��Ȃ��Ă���
class BoneCollisionObject : public ICollider {
public:
	friend class MMD::AnimationState::BaseAnimationState;
	//shape�ɂȂ�shared_ptr���g���Ă��邩�H
	//delete�̐ӔC�������܂��ɂȂ邩��
	//�����̃R�[�f�B���O���[���́Anew�����N���X��delete���邾���A
	//�����ł��̐݌v�ɂ����shape�������Ȃ��ėǂ��N���X�Ŏ����ƂɂȂ邵�A
	//delete�̏��݂������܂��ɂȂ�B
	//shape�͂�����delete�������B
	//shared_ptr�ƃi�}�|�������邱�ƂɂȂ邪�����͌�����
	BoneCollisionObject(
		float mass,
		float start, float end,
		std::shared_ptr<btCollisionShape> shape,
		btCollisionObject *owner,
		CBone *bone,
		GameObjectData::WeaponStatus *weaponStatus );
	virtual ~BoneCollisionObject();
	
	virtual void addToWorld();
	virtual void removeFromWorld();
	
	void draw() const;

	RTTI_DECL;
	
	//�Ȃ����NULL�Ԃ�
	virtual btCollisionObject* getBtCollisionObject();
	
	//virtual void onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj );
	virtual void onCollisionStay( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj );
	//virtual void onCollisionExit(  btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj );

	//float getMass() const ;

	//void loadAccessory( const char *fileName );
	//bool isReady();

	void createModel( unsigned int numBatchs, float scale );
	void setBatch( unsigned int index, GameLib::Scene::Batch b );

	//void calcDamage( GameObjectData::Status *st ){
	//	mWeaponStatus->attack(st);
	//}
	//
	//void attack( Enemy* e ){
	//	attack( e->getStatus() );
	//	e->hitback();
	//}
	//void attack( Enemy* e, GameObjectData::Status *enemySt, btRigidBody *enemyBody );
	//void attack( Player* p, GameObjectData::Status *playerSt );


	void attack( Enemy *enemy, GameObjectData::Status *enemyStatus );

	//void setWeaponStatus( GameObjectData::WeaponStatus *weaponStatus );


private:
	//float �͂����Ă��̏ꍇ�@elapsedTime=1.f/30.f sec
	//�����ABoneCollisionObject�̏ꍇ�̂� currentFrame
	//�����t���[���ƏI���t���[���œ����蔻����o�����ߌ��݂̃t���[�����~����
	//private�Ȃ̂�frend�ŃA�N�Z�X�R���g���[�����Ă邽��
	virtual void update( float currentFrame );
	
	// Kinematic�p���[�V�����X�e�[�g
	struct KinematicMotionState;
	
	//disallow assingn
	BoneCollisionObject( const BoneCollisionObject& );
	//disallow copy
	void operator=( const BoneCollisionObject& );

	void calcDamage( GameObjectData::Status *st );


	//btPairCachingGhostObject	*mGhost;
	btRigidBody							*mRigidBody;
	std::shared_ptr<btCollisionShape> mShape;
	btCollisionObject					*mBtObjOfOwner;
	CBone								*mBone;
	bool								mIsInWorld;

	float mStart;
	float mEnd;

	
	struct Model;
	Model *mModel;
	//GameLib::Scene::Container *mContainer;

	GameObjectData::WeaponStatus *mWeaponStatus;
};
}//end namespace GameObject
#endif//__GAMEOBJECT_BONE_COLLISIONOBJECT_H__

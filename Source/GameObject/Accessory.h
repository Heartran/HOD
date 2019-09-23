#pragma once
#ifndef __GAMEOBJECT_ACCESSORY_H__
#define __GAMEOBJECT_ACCESSORY_H__
#include"GameObject/BaseObject.h"
#include"RTTI/RTTI.h"
#include<memory>
namespace MMD{
class Bone;
}// namespace MMD

namespace MME {
	enum MMDPass;
}

class btCollisionObject;
class btCollisionShape;
class btTransform;
class btVector3;
struct btDefaultMotionState;
typedef std::shared_ptr<btDefaultMotionState> BtDefaultMotionStateSharedPtr;

typedef std::shared_ptr<btCollisionShape> BtCollisionShapeSharedPtr;
class btTriangleMesh;
typedef std::shared_ptr<btTriangleMesh> BtTriangleMeshSharedPtr;
//if btCollisionShape don't use btTriangleMesh, pair.second returns NULL. 
//���̌^��btTiangleMesh�̍폜�̐ӔC��GameObject::Accessory�ɈϏ����邽�߂ɕK�v
typedef std::pair<BtCollisionShapeSharedPtr, BtTriangleMeshSharedPtr> BtCollisionShapeAndBtTriangleMesh; 

class btRigidBody;
typedef std::shared_ptr<btRigidBody> BtRigidBodyShardPtr;


namespace GameLib {
	namespace Math {
		class Matrix34;
	}
}

namespace Component {
	class RigidBodyComponent;
	typedef std::shared_ptr<RigidBodyComponent> RigidBodyComponentSharedPtr;
	class CanCollideExceptIgnoredObjectsCallback;
}

namespace GameObjectData {
	class WeaponStatus;
	typedef std::shared_ptr<WeaponStatus> WeaponStatusSharedPtr;
}

namespace GameObject {

namespace AccessoryUtils {
	namespace MME{
		class Model;
	}
}

class Accessory;
typedef std::shared_ptr<Accessory> AccessorySharedPtr;


//�\���Ɠ����蔻����Ǘ�����N���X
//�\�������A�����蔻�肾���Ŏg�����Ƃ��\
//AccessoryHook�Ƒg�ݍ��킹�Ďg��
class Accessory : public BaseObject {
public:
	//bone�Ǐ]�̃L�l�}�e�B�b�N���̃I�u�W�F�N�g
	//Accessory( 
	//	std::shared_ptr<btCollisionShape> shape,
	//	std::shared_ptr<AccessoryUtils::MME::Model> model,
	//	float mass,
	//	btCollisionObject *owner,
	//	Bone *bone=NULL
	//);

	//�ʏ�̍��̃I�u�W�F�N�g
	Accessory( 
		BtCollisionShapeAndBtTriangleMesh shapeAndMesh,
		std::shared_ptr<AccessoryUtils::MME::Model> model,
		float mass,
		btCollisionObject *owner,
		const btTransform &startTrans
	);

	virtual ~Accessory();

	RTTI_DECL;

	btCollisionObject* getBtCollisionObject();

	void removeFromWorld();
	void addToWorld();

	void draw() const;

	const std::string& getModelFileName()const;

	void debug_draw() const;
	
	void preDrawMMD();


	//�߂荞�݂��������H
	//�ђʒe�Ƃ�
	//����U��Ƃ��q�b�g�o�b�N���������ė~�����Ȃ��ꍇ,false
	//onCollisionEnter, onCollisionStay, onCollisionExit �͔�������
	void enableContactResponse( bool enable );

	void attachBone(MMD::Bone* bone=NULL);
	void separeteFromBone();
	void setVelocity( const btVector3 &velocity );

	bool isInWorld()const{
		return mIsInWorld;
	}

	bool canDraw()const;

	void setCanDraw( bool canDraw );

	void setCanDraw( MME::MMDPass pass, bool canDraw );
	
	virtual void onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj );
	//btCollisionObject���ڐG���Ă���ԁA�Ă΂ꑱ����
	virtual void onCollisionStay(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj) 
	{}
	//btCollisionObject�����ꂽ�Ƃ��Ă΂��
	virtual void onCollisionExit(btCollisionObject *me, btCollisionObject *obj)
	{}


	bool isReady();
	const MMD::Bone* getBone()const{
		return mBone;
	}

	btCollisionObject* getOwner(){
		return mOwner;
	}

	float getScale()const;
	void setScale(float scale);

	void setWeaponStatus(GameObjectData::WeaponStatusSharedPtr weapon);


	//���݂̂̂����蔻��
	bool canCollideOnlyOnce()const;

	virtual int calculateDamage();

	virtual bool canDamage();

	virtual const GameObjectData::WeaponStatus* getWeaponStatus();


private:
	Accessory(const Accessory &) = delete;
	void operator=(const Accessory &) = delete;
	
	// Kinematic�p���[�V�����X�e�[�g
	struct KinematicMotionState;
	typedef std::shared_ptr<KinematicMotionState> KinematicMotionStateSharedPtr;
	//void setIsKinematic( bool isKinematic );


	BtRigidBodyShardPtr mRigidBody;
	BtCollisionShapeSharedPtr mShape;
	BtTriangleMeshSharedPtr mMesh;
	std::shared_ptr<AccessoryUtils::MME::Model> mModel;
	KinematicMotionStateSharedPtr mKinematicMotion;
	BtDefaultMotionStateSharedPtr mDefaultMotion;
	MMD::Bone		*mBone;

	bool mIsInWorld;

	btCollisionObject *mOwner;

	Component::RigidBodyComponentSharedPtr mRigidBodyComponent;
	Component::CanCollideExceptIgnoredObjectsCallback *mCanCollideCallback;


	GameObjectData::WeaponStatusSharedPtr mWeapon;
};
}//end namespace GameObject

#endif

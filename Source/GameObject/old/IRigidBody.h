#pragma once
#include"ICollider.h"
#include<memory>
class btRigidBody;
//typedef std::shared_ptr<btRigidBody> BtRigidBodySharedPtr;
typedef std::unique_ptr<btRigidBody> BtRigidBodyUniquePtr;

namespace GameObject {
class IRigidBody : public ICollider {
public:
	IRigidBody();
	virtual ~IRigidBody();

	RTTI_DECL;

	bool isInWorld()const;

	virtual btCollisionObject* getBtCollisionObject();
	
	//mIsInWorld��true�Ȃ�A�������E����폜����mIsInWorld��true�ɃZ�b�g����
	void defalut_removeFromWorld();

	//mIsInWorld��false�Ȃ�A
	//�������E�ɒǉ�����(mFilterGroup, mFilterMask���l������)�A
	//mIsInWorld��false�ɃZ�b�g����
	void defalut_addToWorld();

	void hitback( const btVector3 &dir, float hitbackValue );

	virtual void onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj);
	
	float getMass() const {
		return mMass;
	}

protected:
	bool mIsInWorld;
	BtRigidBodyUniquePtr mRigidBody;
	float mMass;

	unsigned short	mFilterGroup;
	unsigned short	mFilterMask;
};

}//end namespace GameObject
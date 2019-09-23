#pragma once
#ifndef __PMDRIGIDBODY_H__
#define __PMDRIGIDBODY_H__

#include "Pmd.h"
#include "Bone.h"
#include "BulletPhysics/Manager.h"
#include "BulletPhysics/BulletPhysicsHelper.h"
#include "MMDMacro.h"
#include <btBulletDynamicsCommon.h>
#include "GameObject/BaseObject.h"
#include<memory>


namespace Component {
	class RigidBodyComponent;
}

namespace MMD {

	class PMDRigidBody : public GameObject::BaseObject
	{
	public:
		static std::shared_ptr<PMDRigidBody> create() {
			return std::make_shared<PMDRigidBody>();
		}
		PMDRigidBody();
		virtual ~PMDRigidBody();

		RTTI_DECL;

		bool initialize(const PMD_RigidBody *pPMDRigidBody, Bone *pBone);

		//rigidbody�ɑΉ������{�[�����X�V
		//�{�[����skiningMatrix_��rigidbody�ɍ��킹��B
		void updateBoneTransform(void);

		//�{�[���ɍ��킹��
		void fitBone();

		void release(void);

		void debugDraw();
		Bone* getBonePtr() { return mBone; }

		void addToWorld();
		bool removeFromWorld();


		unsigned short getFilterGroup() const {
			return mCollisionFilterGroup;
		}
		unsigned short getFilterMask() const {
			return mCollisionFilterMask;
		}

		void setVelocityZero();

		btRigidBody* getRigidBody();

		virtual int calculateDamage()override;

		virtual bool canDamage()override;

		virtual const GameObjectData::WeaponStatus* getWeaponStatus()override;


	private:
		DISALLOW_COPY_AND_ASSIGN(PMDRigidBody);
		
		Component::RigidBodyComponent *mRigidBodyComponent;


		btTransform			mBoneOffsetTransform;
		btTransform			mInvStartTransform;

		Bone				*mBone;
		int					mBoneType;
		bool				m_bNoCopyToBone;

		//mmd�̂��߂̓Ǝ��t�B���^�[�Ɏg�p
		//bulletPhysics��id, filterMask�Ƃ͕ʕ�
		unsigned short mCollisionFilterGroup;
		unsigned short mCollisionFilterMask;

		class CanCollideCallback;
		class btKinematicMotionState;

		CanCollideCallback *mCanCollideCallback;
	};
}
#endif	// __PMDRIGIDBODY_H__

#include"BaseCollisonObject.h"
#include"GameObjectData/WeaponStatus.h"
#include<btBulletDynamicsCommon.h>
#include"BulletPhysics/Manager.h"

namespace CollisionObject {

	BaseCollisionObject::BaseCollisionObject()
		:mIsInWorld(false),
		mFilterGroup(0),
		mFilterMask(0),
		mMass(0.f)
	{}

	BaseCollisionObject::~BaseCollisionObject() {}


	void BaseCollisionObject::attack(GameObjectData::Status* st) const {
		if (mWeaponStatus && st) {
			mWeaponStatus->attack(st);
		}
	}

	//�����o�ϐ��ŊǗ�����ׂ���
	bool BaseCollisionObject::canCollideOnlyOnce() {
		return mWeaponStatus && mWeaponStatus->canCollideOnlyOnce;
	}

	void BaseCollisionObject::copyWeaponStatus(const GameObjectData::WeaponStatusSharedPtr &src) {
		if (!mWeaponStatus) {
			mWeaponStatus = std::make_shared<GameObjectData::WeaponStatus>();
		}
		*mWeaponStatus = *src;
	}

	void BaseCollisionObject::copyWeaponStatus(const GameObjectData::WeaponStatus &src) {
		if (!mWeaponStatus) {
			mWeaponStatus = std::make_shared<GameObjectData::WeaponStatus>();
		}
		*mWeaponStatus = src;
	}


	bool BaseCollisionObject::isInWorld()const {
		return mIsInWorld;
	}

	btCollisionObject* BaseCollisionObject::getBtCollisionObject() {
		return mRigidBody.get();
	}

	//mIsInWorld��true�Ȃ�A�������E����폜����mIsInWorld��true�ɃZ�b�g����
	void BaseCollisionObject::defalut_removeFromWorld() {
		if (mIsInWorld) {
			BulletPhysics::Manager::instance()->removeFromWorld(mRigidBody.get());
			mIsInWorld = false;
		}
	}

	//mIsInWorld��false�Ȃ�A
	//�������E�ɒǉ�����(mFilterGroup, mFilterMask���l������)�A
	//mIsInWorld��false�ɃZ�b�g����
	void BaseCollisionObject::defalut_addToWorld() {
		if (!mIsInWorld) {
			BulletPhysics::Manager::instance()->addToWorld(
				mRigidBody.get(),
				mFilterGroup,
				mFilterMask);
			mIsInWorld = true;
		}
	}

	void BaseCollisionObject::hitback(const btVector3 &dir, float hitbackValue) {
		if (hitbackValue == 0.f) {
			return;
		}
		
		mRigidBody->clearForces();
		mRigidBody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));

		//force=ma
		//mRigidBody->setLinearVelocity( normalDir*hitbackValue );
		//���Ԋ֌W�Ȃ��������̂�impulse
		//��񓖂������Ƃ��������ɑ΂��ē���̋�������ނ�����
		mRigidBody->applyCentralImpulse(dir*mMass*hitbackValue);

	}

	void BaseCollisionObject::onCollisionEnter(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj) {
		ICollisionObject* col = static_cast<ICollisionObject*>(obj->getUserPointer());
		const BaseCollisionObject* other = dynamic_cast<BaseCollisionObject*>(col);

		//���葤�͓������Ȃ����v�Z���Ȃ�

		//if the other object has status.
		//calculatte status and hitback.
		if (other) {
			btVector3 dir = me->getWorldTransform().getOrigin() - obj->getWorldTransform().getOrigin();
			float length = dir.length();
			if (length == 0.f) {
				btVector3 forward = mRigidBody->getWorldTransform().getBasis()[2];
				forward.setX(-forward.x());
				forward.normalize();

				dir = -forward;
			}
			else {
				dir /= length;
			}


			//���[��A�R���g���[�����₷������ɂ͐�����ԗʂ͑��葤�Ɉˑ�����̂ł͂Ȃ��������Ɉˑ�������ׂ��Ȃ̂ł�
			hitback(dir, other->getWeaponStatus()->hitbackValue);


			other->attack(mStatus.get());
		}
	}




}//end namespace GameObject

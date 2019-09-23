#pragma once
#include"BaseComponent.h"
#include<memory>
#include<vector>
#include"helper/Delegate.h"


class btRigidBody;
class btCollisionObject;
class btCollisionShape;
class btTransform;
class btVector3;
class btMotionState;
class btPersistentManifold;


class btRigidBody;
typedef std::shared_ptr<btRigidBody> BtRigidBodySharedPtr;
class btCollisionShape;
typedef std::shared_ptr<btCollisionShape> BtCollisionShapeSharedPtr;
typedef std::shared_ptr<btMotionState> BtMotionStateSharedPtr;

namespace MMD {
	class PMDRigidBody;
}


namespace GameObject {
	class BaseObject;
	typedef std::weak_ptr<BaseObject> BaseObjectObjectWeakPtr;
}


namespace Component {
	class RigidBodyComponent;
	typedef std::shared_ptr<RigidBodyComponent> RigidBodyComponentSharedPtr;


	//namespace�����ł����̂��H
	class ICanCollideCallback {
	public:
		ICanCollideCallback() = default;
		virtual ~ICanCollideCallback() = default;

		virtual bool needCollision(const btCollisionObject*) const = 0;
	};

	//
	class CanCollideExceptIgnoredObjectsCallback : public ICanCollideCallback {
	public:
		CanCollideExceptIgnoredObjectsCallback()= default;
		virtual ~CanCollideExceptIgnoredObjectsCallback() = default;

		virtual bool needCollision(const btCollisionObject *obj) const  {
			for (const btCollisionObject *ignoreObj : mIgnoredObjects) {
				if (ignoreObj == obj) {
					return false;
				}
			}
			return true;	
		}

		//add ignored list.
		void addIgnoredObject(const btCollisionObject *obj) {
			mIgnoredObjects.push_back(obj);
		}

		void clearIgnoredObjects() {
			mIgnoredObjects.clear();
		}

		//sorted_vector�ɂ����ق����T������̑������낤���ǁA
		//�����܂Ő����������Ȃ��̂Ő��`�T���ł������낤
		std::vector<const btCollisionObject*> mIgnoredObjects;

	};

	//����3�̃R�[���o�b�N
	//�e�N���X��shared_ptr�������ďz�Q�Ƃɂ��Ȃ��悤��

	class IOnCollisionEnterCallback {
	public:
		IOnCollisionEnterCallback() {}
		virtual ~IOnCollisionEnterCallback() {}
		virtual void operator()(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj)=0;
	};

	class IOnCollisionStayCallback {
	public:
		IOnCollisionStayCallback() {}
		virtual ~IOnCollisionStayCallback() {}
		virtual void operator()(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj) = 0;
	};

	class IOnCollisionExitCallback {
	public:
		IOnCollisionExitCallback() {}
		virtual ~IOnCollisionExitCallback() {}
		virtual void operator()(btCollisionObject *me, btCollisionObject *obj) = 0;
	};

	class RigidBodyComponent : public BaseComponent {
	public:
		static RigidBodyComponentSharedPtr create(
			GameObject::BaseObject* owner,
			BtCollisionShapeSharedPtr shape,
			BtMotionStateSharedPtr motionState,
			BtRigidBodySharedPtr rigidbody);

		RigidBodyComponent(
			GameObject::BaseObject* owner,
			BtCollisionShapeSharedPtr shape,
			BtMotionStateSharedPtr motionState,
			BtRigidBodySharedPtr rigidbody
		);

		virtual ~RigidBodyComponent();

		//mCanCollideCallback����������ĂȂ��ꍇtrue��Ԃ�
		virtual bool canCollide(const btCollisionObject *obj) const
		{
			if (mCanCollideCallback) {
				return mCanCollideCallback->needCollision(obj);
			}
			else {
				return true;
			}
		}

		btCollisionObject* getBtCollisionObject();

		const btCollisionObject* getConstBtCollisionObject()const;

		const btTransform & getWorldTransform() const;

		btTransform & getWorldTransform();

		BtMotionStateSharedPtr getMotionState() {
			return mMotionState;
		}

		void setMotionState(const BtMotionStateSharedPtr motionstate);

		BtRigidBodySharedPtr getRigidBody() {
			return mRigidBody;
		}

		BtCollisionShapeSharedPtr getShape() {
			return mShape;
		}

		bool isInWorld()const;

		//�ǉ��ł����ꍇtrue��Ԃ�
		bool addToWorld(unsigned short filterGroup, unsigned short filterMask);
		//��菜�����ꍇtrue��Ԃ�
		virtual bool removeFromWorld();

		void setCanCollideCallback(ICanCollideCallback *callback) {
			mCanCollideCallback = callback;
		}

		void setIsKinematic(bool isKinematic);

		//btCollisionObject���ڐG���Ă���ԁA�Ă΂ꑱ����
		virtual void onCollisionStay(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj);

		//btCollisionObject���Փ˂����Ƃ��Ă΂��
		virtual void onCollisionEnter(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj);

		//btCollisionObject�����ꂽ�Ƃ��Ă΂��
		virtual void onCollisionExit(btCollisionObject *me, btCollisionObject *obj);

		//void setOnCollisonEnterCallback(IOnCollisionEnterCallback* callback);
		//void setOnCollisonStayCallback(IOnCollisionStayCallback* callback);
		//void setOnCollisonExitCallback(IOnCollisionExitCallback* callback);

		DECLARE_MULTICAST_DELEGATE_PARAM_3(TCollisionEnterCallback, void, btPersistentManifold*, btCollisionObject*, btCollisionObject*);
		DECLARE_MULTICAST_DELEGATE_PARAM_3(TCollisionStayCallback, void, btPersistentManifold*, btCollisionObject*, btCollisionObject*);
		DECLARE_MULTICAST_DELEGATE_PARAM_2(TCollisionExitCallback, void, btCollisionObject*, btCollisionObject*);

		TCollisionEnterCallback& getOnEnterCallback() {
			return mOnEnterCallback;
		}
		TCollisionStayCallback& getOnStayCallback() {
			return mOnStayCallback;
		}
		TCollisionExitCallback& getOnExitCallback() {
			return mOnExitCallback;
		}




	protected:
		BtCollisionShapeSharedPtr mShape;
		BtMotionStateSharedPtr mMotionState;
		BtRigidBodySharedPtr mRigidBody;

		bool mIsInWorld;

		ICanCollideCallback* mCanCollideCallback;

		TCollisionEnterCallback mOnEnterCallback;
		TCollisionStayCallback mOnStayCallback;
		TCollisionExitCallback mOnExitCallback;

	};
}


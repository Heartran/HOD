#pragma once
class btPersistentManifold;
class btCollisionObject;

namespace CollisionObject {

	class ICollisionObject {
	public:
		ICollisionObject() {}
		virtual ~ICollisionObject() {}

		virtual bool isMMDObject() const = 0;
		virtual bool enableCollidedMMDObject() const = 0;

		//btCollisionObject���ڐG���Ă���ԁA�Ă΂ꑱ����
		virtual void onCollisionStay(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj) = 0;

		//btCollisionObject���Փ˂����Ƃ��Ă΂��
		virtual void onCollisionEnter(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj) = 0;

		//btCollisionObject�����ꂽ�Ƃ��Ă΂��
		virtual void onCollisionExit(btCollisionObject *me, btCollisionObject *obj) = 0;

	private:
		ICollisionObject(const ICollisionObject &) = delete;
		void operator=(const ICollisionObject &) = delete;
	};

}//


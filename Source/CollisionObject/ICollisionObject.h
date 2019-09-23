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

		//btCollisionObjectが接触している間、呼ばれ続ける
		virtual void onCollisionStay(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj) = 0;

		//btCollisionObjectが衝突したとき呼ばれる
		virtual void onCollisionEnter(btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj) = 0;

		//btCollisionObjectが離れたとき呼ばれる
		virtual void onCollisionExit(btCollisionObject *me, btCollisionObject *obj) = 0;

	private:
		ICollisionObject(const ICollisionObject &) = delete;
		void operator=(const ICollisionObject &) = delete;
	};

}//


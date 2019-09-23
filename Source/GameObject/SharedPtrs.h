#ifndef __GAMEOBJECT_SHAREDPTRS_H__
#define __GAMEOBJECT_SHAREDPTRS_H__
#include<memory>

//effective C++�W�����C�u�����̑O���錾�͂�����include������ď����Ă��������A
//template�̎����̑O���錾�����Ԉ��Ȃ���΁A�O���錾�����ق����ǂ��̂��H
//namespace std{
//	namespace tr{
//		template<class T> class shared_ptr;
//	}
//	using namespace tr;
//}

namespace GameObject {
	class Player;
	typedef std::shared_ptr<Player> PlayerSharedPtr;
	

	class Enemy;
	typedef std::shared_ptr<Enemy> EnemySharedPtr;
	
	class Stage;
	typedef std::shared_ptr<Stage> StageSharedPtr;

	class Bullet;
	typedef std::shared_ptr<Bullet> BulletSharedPtr;

	class ICollider;
	typedef std::shared_ptr<ICollider> IColliderSharedPtr;
	typedef std::weak_ptr<ICollider> IColliderWeakPtr;

	class Accessory;
	typedef std::shared_ptr<Accessory> AccessorySharedPtr;

	class CharacterControllerGhostObject;
	typedef std::shared_ptr<CharacterControllerGhostObject> CharacterControllerGhostObjectSharedPtr;

}

namespace GameObjectData{
	class Status;
	typedef std::shared_ptr<Status> StatusSharedPtr;
}

#endif

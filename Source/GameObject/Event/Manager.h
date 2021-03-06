#ifndef __GAMEOBJECT_EVENT_MANAGER_H__
#define __GAMEOBJECT_EVENT_MANAGER_H__
#include"Event/MyEvent.h"
#include<memory>
#include"GameObject/SharedPtrs.h"
#include<list>
#include"GameObject/BaseObject.h"
#include"GameObject/Event/EventData/DefeatedAllEnemy.h"
namespace GameObject{
	namespace Event {

class Manager : public EventManager
{
public:
	static Manager* instance(){
		UptrSingleton & s = getThis();
		if( !s )
		{
			s.reset( new Manager );
		}
		return s.get();
	}

	static void destroy(){
		getThis().reset(nullptr);
	}


	
	//eventManagerではなくて
	//GameObjectManagerかEnemyUtil::Managerに
	//移行するべき
	//ここでオブジェクトの管理までするといろいろ混ざる気がする
	//現状ではHOD::EnemyManagerあるからそこだね
	bool add(const GameObject::BaseObjectSharedPtr &obj){
		assert(false && "使うな。使っても副作用はないけど設計がカス、わざわざリファクタリングする時間が取れないのでとりあえずこのまま");
		if(obj->getRTTI().isExactly( Rttis::Enemy() )){
			mEnemiesInWorld.push_back( obj );
			return true;
		}else {
			return false;
		}
	}
	bool remove(const  GameObject::BaseObjectSharedPtr &obj){
		if(obj->getRTTI().isExactly( Rttis::Enemy() )){
			mEnemiesInWorld.remove( obj );
			if( mEnemiesInWorld.empty() ){
				//defeated all Enemy event;
				IEventDataPtr ev = EventData::DefeatedAllEnemies::create();
				queueEvent( ev );
			}
			return true;

		}else {
			return false;
		}
	}

private:
	Manager()
		:EventManager("GameObject::Event::Manager")
	{}
	virtual ~Manager(){}

	//disallow assign
	Manager( const Manager & )=delete;
	//disallow copy
	void operator=( const Manager & )=delete;
	struct Deleter
	{
		void operator()( Manager const* const p ) const
		{
			delete p;
		}
	};

	typedef std::unique_ptr< Manager, Deleter > UptrSingleton;
	static UptrSingleton & getThis(){
		static UptrSingleton sThis;
		return sThis;
	}

	std::list< GameObject::BaseObjectSharedPtr> mObjectsInWorld;

	std::list< GameObject::BaseObjectSharedPtr> mEnemiesInWorld;
	std::list< GameObject::BaseObjectSharedPtr> mEnemiesOutsideWorld;
};

	}//end	namespace Event
}//end namespace GameObject
#endif
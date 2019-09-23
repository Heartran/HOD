#ifndef __GAMEOBJECT_BASEGAMEOBJECT_H__
#define __GAMEOBJECT_BASEGAMEOBJECT_H__
#include "IGameObject.h"
#include <vector>

namespace GameObject{

extern static GameObjectId numGameObjects;

class BaseGameObject {
public:
	virtual ~BaseGameObject(){}
	static const std::vector< const btCollisionObject* > & getEmptyBtCollisionObjects(){
		return sEmptyCollisionObject;
	}
private:
	//getNotCollideBtCollisionObjects()で何もいらない場合、空のvectorを返したいので
	static const std::vector< const btCollisionObject* > sEmptyCollisionObject;
};
}
}
#endif
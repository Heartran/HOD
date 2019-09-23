#ifndef __PHYSICS_EVENT_DATA_H__
#define __PHYSICS_EVENT_DATA_H__
#include "Event/MyEvent.h"
#include "RTTI/RTTI.h"

class btPersistentManifold;
class btCollisionObject;

// enterevent��stayEvent�͓����ɋN����B
// enterEvent�Ƃ����������邱�Ƃ́A��΋N����Ȃ�
//�ڐG�����u�Ԃ��~�����ꍇ�̂݁AenterEvent���g��

class OnCollisionStayEventData : public IEventData{
public:
	OnCollisionStayEventData( btPersistentManifold *manifold, btCollisionObject *obj0, btCollisionObject *obj1 )
		:mManifold(manifold),
		mObj0(obj0),
		mObj1(obj1)
	{}

	RTTI_DECL;
	btPersistentManifold *mManifold;
	btCollisionObject *mObj0;
	btCollisionObject *mObj1;
};

class OnCollisionEnterEventData : public IEventData{
public:
	OnCollisionEnterEventData( btPersistentManifold *manifold, btCollisionObject *obj0, btCollisionObject *obj1 )
		:mManifold(manifold),
		mObj0(obj0),
		mObj1(obj1)
	{}

	RTTI_DECL;
	btPersistentManifold *mManifold;
	btCollisionObject *mObj0;
	btCollisionObject *mObj1;

};

class OnCollisionExitEventData : public IEventData{
public:
	OnCollisionExitEventData( btCollisionObject *obj0, btCollisionObject *obj1 )
		:mObj0(obj0),
		mObj1(obj1)
	{}

	RTTI_DECL;
	btCollisionObject *mObj0;
	btCollisionObject *mObj1;

};
#endif
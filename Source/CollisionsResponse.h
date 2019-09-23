#ifndef __COLLISION_RESPONSE_FUNC_H__
#define __COLLISION_RESPONSE_FUNC_H__
#include<assert.h>
#include<btBulletDynamicsCommon.h>
#include<BulletPhysics/BulletPhysicsManager.h>
#include<BulletCollision/CollisionDispatch/btGhostObject.h>


#ifndef MMD_FILTER
#define MMD_FILTER 0x7fc0
#endif

class CBone;
class ICollisionObject{};

//�����蔻��݂̂̃N���X
class CollisionObject{
public:
	CollisionObject( int startFrame, int endFrame, ??? shapeType, ??? sizeParam){
		mGhost = new btPairCachingGhostObject();
		mShape = new btSphereShape( radius );
		mAction = new btActionInterface( mGhost, mShape );
		btDiscreteDynamicsWorld *world;
		world->addAction(mAction);
	}
	virtual ~CollisionObject();
	virtual void update( int currentFrame ){
		if(mEnable){
			//�J�n���Ԃ��O���I��������肠�ƂȂ�A�����蔻�����菜��
			if( currentFrame < mStartFrame || currentFrame > mEndFrame ){
				mEnable=false;
				CBulletPhysicsManager::instance()->removeFromWorld( mGhost );
			}
		}
		else{
			if( currentFrame >= mStartFrame && currentFrame <= mEndFrame ){
				mEnable=true;
				CBulletPhysicsManager::instance()->addToWorld( mGhost, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter & ~MMD_FILTER );
				mGhost->getWorldTransform().setOrigin( toBtTransform( mBone->getSkinningMatrix() ).getOrigin() );
			}
		}
		//���������Ƃ��̏������ǂ��������H
		//	btActionInterFace
		//	�����ɏ������H
		{
			int num = mGhost->getNumOverlappingObjects();
			for( int i=0; i<num; ++i ){
				btCollisionObject* overlappingObj = mGhost->getOverlappingObject( i );
				if( getOwner()==overlappingObj ){
					continue;
				}
				static_cast<IGameEntity*>(overlappingObj->getUserPointer());
				
			}
	
		}
	}
	const btCollisionObject* getOwner() const {
		return pOwner;
	}
private:
	btCollisionObject* pOwner;

	bool mEnable;
	int mGroupId;
	int mFilterMask;

	CBone* parent;//�U����������;
	btTransform world;
	int	mStartFrame;//�����t���[��
	int mEndFrame;//�I���t���[��
	btPairCachingGhostObject* mGhost;
	btCollisionShape* mShape;
	btActionInterface* mAction;
};

class CollisionDetection{}

class CollisionResponseFunc{
public:
	CollisionResponseFunc(){}
	~CollisionResponseFunc(){}
	enum CollisonType{
		PLAYER,;
		ITEM,
		ENEMY,
		BULLET,
	};
	template<int, int>
	func( void *p1, void *p2){}
}
#endif
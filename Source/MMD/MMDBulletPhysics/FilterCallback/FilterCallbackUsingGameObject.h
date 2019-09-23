#ifndef __FILTER_CALLBACK_USING_GAMEOBJECT_H__
#define __FILTER_CALLBACK_USING_GAMEOBJECT_H__
#include<btBulletCollisionCommon.h>
#include<BulletCollision/BroadphaseCollision/btOverlappingPairCallback.h>
#include"GameObject/IGameObject.h"
#include"MMD/PMDRigidBody.h"

namespace BulletPhysics {

class FilterCallbackUsingGameObject : public btOverlapFilterCallback
{
public:
	// return true when pairs need collision
	virtual bool	needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const
	{
		bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
		collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
		
		if(collides){
			btCollisionObject *co0 = static_cast<btCollisionObject*>(proxy0->m_clientObject);
			btCollisionObject *co1 = static_cast<btCollisionObject*>(proxy1->m_clientObject);

			GameObject::IGameObject *userPtr0 = static_cast<GameObject::IGameObject*>( co0->getUserPointer() );
			GameObject::IGameObject *userPtr1 = static_cast<GameObject::IGameObject*>( co1->getUserPointer() );
			//MMD�Ŏg���z��
			//���z�֐��g���Ă邩��x���B���̒��ł͏d������������肽���Ȃ�
			//�܂��A�C�ɂ��Ă����傤���Ȃ����x���ł͂���
			//�X�e�[�W�Ƃ��������񂠂���̂�NULL�ɂ��Ƃ�
			if( userPtr0 && userPtr1 ){
				//�����Ƃ�mmd�I�u�W�F�N�g�Ȃ�A�Ǝ��̃t�B���^�����O
				if( userPtr0->isMMDObject() && userPtr1->isMMDObject() ){
					CPMDRigidBody* mmd0 = static_cast<CPMDRigidBody*>(userPtr0);
					CPMDRigidBody* mmd1 = static_cast<CPMDRigidBody*>(userPtr1);
					if( mmd0->getParentBtCollisionObject() == mmd1->getParentBtCollisionObject() ){
						const short & mmd0group = mmd0->getFilterGroup();
						const short & mmd1group = mmd1->getFilterGroup();
						const short & mmd0mask = mmd0->getFilterMask();
						const short & mmd1mask = mmd1->getFilterMask();
						return (mmd0group & mmd1mask) && (mmd1group & mmd0mask);
					} else{
						return true;
					}
				}
				//�ǂ��炩��mmd�I�u�W�F�N�g�̏ꍇ
				else if( userPtr0->isMMDObject() ) {
					if( !userPtr1->enableCollidedMMDObject() ){
						return false;
					}
				}
				else if( userPtr1->isMMDObject() ) {
					if( !userPtr0->enableCollidedMMDObject() ){
						return false;
					}
				}
				else {
				}

				if( !userPtr0->canCollide(co1) ){
					return false;
				}

				if( !userPtr1->canCollide(co0) ){
					return false;
				}				
			}
		}
		
		//add some additional logic here that modified 'collides'
		return collides;
	}
};
}
#endif//__FILTER_CALLBACK_USING_GAMEOBJECT_H__
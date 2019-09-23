#pragma once
#ifndef __INPUT_PLAYERMOUSE_H__
#define __INPUT_PLAYERMOUSE_H__

#include<LinearMath/btVector3.h>
#include<memory>

namespace GameObject {
	class BaseObject;
	typedef std::weak_ptr<BaseObject> BaseObjectObjectWeakPtr;
}

namespace GameLib{
	namespace Scene{
		class Model;
	}
}

class btCollisionObject;


namespace Input{
/*
	�������@�Ƃ���
		btGhostPairCacheObject* ���g�����@
			����ꏊ�Ɠ����Ă������X�V���āAbulletphisicsManager��onEnter,onExit�������Ȏ������@�ɂ���̂�����
			����Pair�łȂ��Ă��ǂ��ˁB
		
			typedef std::pair< btCollisionObject*, btCollisionObject* > CollisionPair;
			typedef std::set< CollisionPair > CollisionPairs;
			CollisionPairs							previousCollisionPairs_;
			CollisionPairs							clickedCollisionPairs_;

		���C���΂����@

		contactPairTest

		player��contactPairTest���͂₻��

		���͕����̃I�u�W�F�N�g���ڐG���Ă���ꍇ���ǂ����邩�H
			���C�����₷��

		���C�ł��

//player�Əz�Q�ƂɂȂ肻���ŕ|��
//IColliderSharedPtr mOnObject��player�̂Ƃ��폜�ł��܂����H
//�Ƃ������Ƃ�
//IColliderWeakPtr�ɂ����B

*/
class PlayerMouse {
public:
	PlayerMouse( btCollisionObject* player, float radius );
	~PlayerMouse();
	
	void update(const float elapsedTime);

	// velocity = currentPos - oldPos	
	bool isFlickedPlayer( int *velocityX, int *velocityY )const;
	bool isFlicked( int *velocityX, int *velocityY )const;

	bool onPlayer()const;
	//Q �͈͓���gameObject������ꍇ�̑I�����Ă���I�u�W�F�N�g�̗D��x���ǂ�����B
	//A �J��������̋����ɋ߂��I�u�W�F�N�g�i�v���C���[����߂��I�u�W�F�N�g�����肩�ȂƎv�����j
	bool onObject()const;
	
	bool isTriggeredPlayer()const;
	bool isTriggeredObject()const;
	bool isClickingPlayer()const;	
	bool isClickingObject()const;	
	bool isReleasedPlayer()const;
	bool isReleasedObject()const;

	GameObject::BaseObjectObjectWeakPtr  getOnObject();
	GameObject::BaseObjectObjectWeakPtr getDragingObject();

	//GameObject::IColliderSharedPtr getOnObject()const;
	//GameObject::IColliderSharedPtr getDragingObject()const;

	btVector3 getPos()const;

	bool isModeChangeButtonPushed() const;

	void drawMousePos( int color=0xffffffff ) const;
private:
	PlayerMouse( const PlayerMouse & );
	void operator=( const PlayerMouse & );
	
	int mClickedPos[2];
	int mClickedFrame;;
	
	bool mIsOnPlayer;

	//player���N���b�N������
	bool mHasClickedOnPlayer;

	//���������邩
	bool mIsInit;

	btVector3 mMousePos;
	btVector3 mClickedMousePos;
	float mMouseRadius;
	
	btCollisionObject *mPlayerBtObject;

	GameObject::BaseObjectObjectWeakPtr mOnObject;
	GameObject::BaseObjectObjectWeakPtr mDragingObject;

	GameLib::Scene::Model *mArrowModel;

};

}//end namespace Input
#endif

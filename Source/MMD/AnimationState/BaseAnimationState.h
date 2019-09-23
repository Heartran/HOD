#ifndef __BASE_ANIMATION_STATE_H__
#define __BASE_ANIMATION_STATE_H__

#include"IAnimationState.h"
#include"Hook/IHook.h"
#include<vector>

//Effective C++ 34�� �C���^�[�t�F�[�X�̌p���Ǝ����̌p���̋�ʂ����悤

struct D3DXMATRIX;

//namespace GameObject{
//	class BoneCollisionObject;
//}

namespace MMD {
	class CModel;
	class Animation;

namespace AnimationState {
//default��������̂������N���X
//mNextScene��NULL���AisWeightBlend()�AisRepeat�̎����ɂ���ĕω�����B
class BaseAnimationState : public IAnimationState
{
public:
	BaseAnimationState( const std::shared_ptr<Animation> &anim, const char* vmdName );
	virtual ~BaseAnimationState();
	
	virtual std::shared_ptr<Animation> getAnimation() const{
		return mAnimation;
	}
	virtual const char* getVmdName() const {
		return mVmdName;
	};

	//virtual void addCollisionObject( GameObject::BoneCollisionObject *obj );

	virtual void addHook( Hook::IHookSharedPtr hook );
	void shrinkToFitAllHookContainers();
	void setPlaySpeed( const float & playSpeed ){
		mPlaySpeed=playSpeed;
	}
protected:
	//�����ω����Ȃ��ꍇthis��Ԃ�
	//isBlendWeight()��isRepeat()�̎���, mNextScene��NULL���ɂ���ď������ω�
	//restartFrame��isOver()��isRepeat()��true�̏ꍇ�A�ĊJ�n����t���[���ʒu
	IAnimationState* defalut_stepTime( float elapsedTime, CModel *model, float restartFrame=0.f );

	//isBlendWeight()��isRepeat()�̎����ɂ���ď������ω�
	//�����ł́AweightBlend�̌�n���Ǝ��̃��[�V�����������ꂢ�ɓ����������B
	//�ڍs�ł��Ȃ��Ȃ�this��Ԃ��B
	IAnimationState* defalut_changeState( IAnimationState *nextState, CModel *model  );

	bool default_isOver( CModel *model );
	
	//void removeFromWorldAllCollisionObjects();

	//default�̎������g��Ȃ��Ȃ玩���ŌĂяo������
	//	default_stepTime���g��Ȃ��Ȃ�AinformUpdate��informAnimOver�����������Ăяo������
	//	default_changeState���g��Ȃ��Ȃ�AinformChangeState��informStart�𐳂����Ăяo������
	//
	//informAnimationOver�̈ʒu�ɋC�����邱��
	//	stepTime���ň��Ăяo���̂�������
	//	�Ăяo���̂̓t���[�������ɂ��邱�ƁB
	//	isOver()�̒���true�ɂȂ邽�ьĂяo���Ȃ�����
	virtual void informUpdate( float currentFrame );
	virtual void informStart();
	virtual void informChangeState();
	virtual void informAnimationOver();


	
	char *mVmdName;
	std::shared_ptr<Animation> mAnimation;
	float mPlaySpeed;
	//std::vector<GameObject::BoneCollisionObject*> mCollisionObjects;
	IAnimationState *mNextState;

	std::vector<Hook::IHookSharedPtr> mHooks;

};
}
}
#endif//__BASE_ANIMATION_STATE_H__
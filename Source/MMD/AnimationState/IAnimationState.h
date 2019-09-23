#ifndef __IANIMATION_STATE_H__
#define __IANIMATION_STATE_H__
struct D3DXMATRIX;
class BaseObject;

#include<memory>

namespace MMD {
	class CModel;
	class Animation;
namespace AnimationState {
namespace Hook{
	class IHook;
	typedef std::shared_ptr<IHook> IHookSharedPtr;
}//MMD::AnimationState::Hook

class IAnimationState{
public:
	virtual ~IAnimationState(){}
	//�ς��Ȃ��Ȃ�this��Ԃ��B
	virtual IAnimationState* stepTime( float elapsedTime, CModel *model )=0;
	
	//virtual bool load()=0;

	//�z���C�g���X�g�I�Ɏ������邱��
	//�������̂ق����o�O�����Ȃ�
	//�z���C�g���X�g�I�Ƃ����̂́A�m���ɐM���ł���l�̂�true��Ԃ�����
	//�J�ڂł�����̂�true��Ԃ��A���̑��S����false��Ԃ�����
	//�V������Ԃ�ǉ������ꍇ�A�����I��false��Ԃ��悤��
	//�ł��A���̎����߂�ǂ��ȁB
	//������Ԃ���J�ڂł���̂́A�u���b�N���X�g�I�ɏ������H
	//�Ƃ肠�����A�o���邾���Afalse��Ԃ�������
	virtual bool isChangeAble( IAnimationState *pS, CModel *model ) const = 0;
	//������Ԃ̃A�j�����H
	virtual bool isStand() const = 0;

	virtual bool isOver( CModel *model ) =0;

	//�ڒn���Ă��邩�H
	//������ik�g���������̒����̂��
	//virtual bool isGround();
	
	//���̊֐����ĂԑO�ɁAisChangeAble�֐����Ăь��؂��邱��
	virtual IAnimationState* changeState( IAnimationState* nextState, CModel *model ) = 0;
	
	//virtual int getType() const = 0;
	virtual bool isWeightBlend() const = 0;
	virtual bool isRepeat() const = 0;


	//�ȉ���BaseAnimationState�Ŏ�������܂�

	virtual std::shared_ptr<Animation> getAnimation() const = 0;
	virtual const char* getVmdName() const = 0;
	
	virtual void addHook( Hook::IHookSharedPtr ) = 0;
	
	//virtual void addCollisionObject( BaseObject *obj )=0;
	
	virtual void informUpdate( float currentFrame )=0;
	virtual void informStart()=0;
	virtual void informChangeState()=0;
	virtual void informAnimationOver()=0;
};
}
}
#endif//__IANIMATION_STATE_H__
#ifndef __ANIMATIONSTATE_STARTEND_H__
#define __ANIMATIONSTATE_STARTEND_H__
#include"BaseAnimationState.h"
#include"IAnimationState.h"
#include<assert.h>
#include"../AnimationModel.h"

class CModel;
struct D3DXMATRIX;

namespace MMD {
namespace AnimationState {

//�A�j���[�V�������j�]��h�����߂��ߏI���ƊJ�n�̃t���[����ݒ�o����A�j���[�V����
class StartEnd : public BaseAnimationState {
public:
	//
	explicit StartEnd( AnimationModel *model, const char *vmdName, float restartFrame, float endFrame, bool isWeightBlend=true, bool isRepeat=true, bool isStand=true, IAnimationState *nextState=NULL )
		:BaseAnimationState( model->loadVMD(vmdName), vmdName ),
		mIsRepeat(isRepeat),
		mRestartFrame( restartFrame ),
		mEndFrame( endFrame ),
		mIsWeightBlend(isWeightBlend),
		mIsStand(isStand)
	{
		assert( !(isRepeat && nextState) && "�J��Ԃ�����Ǝ��̃A�j���[�V�����͗������܂���" );

		mNextState=nextState;
	}
	virtual ~StartEnd(){}

	//�ς��Ȃ��Ȃ�this��Ԃ��B
	virtual IAnimationState* stepTime( float elapsedTime, CModel *model ){
		return defalut_stepTime( elapsedTime, model, mRestartFrame );
	}
	
	//�z���C�g���X�g�I�Ɏ������邱��
	//�������̂ق����o�O�����Ȃ�
	//�z���C�g���X�g�I�Ƃ����̂́A�m���ɐM���ł���l�̂�true��Ԃ�����
	//�J�ڂł�����̂�true��Ԃ��A���̑��S����false��Ԃ�����
	//�V������Ԃ�ǉ������ꍇ�A�����I��false��Ԃ��悤��
	//�ł��A���̎����߂�ǂ��ȁB
	//������Ԃ���J�ڂł���̂́A�u���b�N���X�g�I�ɏ������H
	//�Ƃ肠�����A�o���邾���Afalse��Ԃ�������
	virtual bool isChangeAble( IAnimationState *nextState, CModel *model ) const {
		if(this==nextState)
		{
			return false;
		}

		if( nextState->isStand() ){
			return true;
		} else {
			return false;
		}
	}
	
	//������Ԃ̃A�j�����H
	virtual bool isStand() const {
		return mIsStand;
	}

	//�ڍs�ł��Ȃ��ꍇ��this
	virtual IAnimationState* changeState( IAnimationState* nextState, CModel *model ){
		IAnimationState* next=this;
		if( isChangeAble( nextState, model ) ){
			next=defalut_changeState( nextState, model );
		}
		return next;
	}

	virtual bool isOver( CModel *model );

	virtual bool isWeightBlend() const {
		return mIsWeightBlend;
	}
	
	virtual bool isRepeat() const {
		return mIsRepeat;
	}
private:
	bool mIsWeightBlend;
	bool mIsRepeat;
	bool mIsStand;
	float mRestartFrame;
	float mEndFrame;
};

}
}
#endif //__ANIMATIONSTATE_STARTEND_H__
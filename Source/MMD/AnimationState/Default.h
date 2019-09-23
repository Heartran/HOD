#ifndef __ANIMATIONSTATE_DEFAULT_H__
#define __ANIMATIONSTATE_DEFAULT_H__
#include"BaseAnimationState.h"
#include"IAnimationState.h"
#include<assert.h>
#include"../AnimationModel.h"
class CModel;
struct D3DXMATRIX;

namespace MMD {
namespace AnimationState {

//���̏�Ԃ�������ԂȂ�΁A�ڍs����B�ڍs���������Ȃ�ʂ邢�̂ŁA�A�j���[�V�����ɔj�]������������
class Default : public BaseAnimationState {
public:
	
	//�������̂ق����킩��₷�����H
	//Default* animationState = model->loadVmd( vmdFileName, isWeightBlend, isRepeat, isStand );
	Default( AnimationModel *model, const char *vmdFileName, bool isWeightBlend, bool isRepeat, bool isStand, IAnimationState *nextState=NULL )
		:BaseAnimationState( model->loadVMD( vmdFileName ), vmdFileName ),
		mIsRepeat(isRepeat),
		mIsWeightBlend(isWeightBlend),
		mIsStand(isStand)
	{
		assert( !(isRepeat && nextState) && "�J��Ԃ�����Ǝ��̃A�j���[�V�����͗������܂���" );

		mNextState=nextState;
	}


	virtual ~Default(){}

	//�ς��Ȃ��Ȃ�this��Ԃ��B
	virtual IAnimationState* stepTime( float elapsedTime, CModel *model ){
		return defalut_stepTime( elapsedTime, model );
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

	virtual bool isOver( CModel *model ){
		return default_isOver(model);
	}

	virtual bool isWeightBlend() const {
		return mIsWeightBlend;
	}
	
	virtual bool isRepeat() const {
		return mIsRepeat;
	}

	bool mIsWeightBlend;
	bool mIsRepeat;
	bool mIsStand;
};

}
}
#endif //__ANIMATIONSTATE_DEFAULT_H__
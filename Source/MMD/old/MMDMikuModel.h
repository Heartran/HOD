#ifndef __MMDMIKUMODEL_H__
#define __MMDMIKUMODEL_H__

#include"Model.h"
#include<assert.h>

class CMMD_MikuModel{
public:
	enum AnimationState{
		ANIM_STAND=0,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_IJIME_PUNCH,
		ANIM_DOUBLE_LARIAT,
		ANIM_DAMAGED,
		ANIM_DOWN,
		ANIM_WAKE,


		ANIM_NUM
	};
	CMMD_MikuModel();
	~CMMD_MikuModel();

	//�����ʒu���Z�b�g
	void init( const D3DXMATRIX &world );
	void draw();
	
	
	void update( const D3DXMATRIX &world, float elapsedTime );
	//IKBone���w��̍����ɂ���
	//example
	//if( mObj.onGround() ){
	//	mModle.update( world, elapsedTime );
	//} else {
	//	//���ݒn���牺�Ɍ�������rayCast��n�ʂɓ��Ă�groundHeight���߂�
	//	//mObj.getPos().y()-obj.height*2.f //�P���ɂ���ł�������
	//	//mModle.update( world, elapsedTime, groundHeight );
	//}
	void update( const D3DXMATRIX &world, float elapsedTime, float groundHeight);

	bool isAnimOver();

	AnimationState getCurrentAnimation();
	void changeAnimation(AnimationState state);
	float getFrame(){
		return mModel.getFrame( mState );
	}

private:
	//�L�[�{�[�h��zxcv�ő���
	void debugControll( const D3DXMATRIX &w, float elapsedTime );
	AnimationState mState;
	AnimationState mOldState;
	float mWeight;

	CModel mModel;
	class IAnimationStrategy;
	class CBaseAnimationStrategy;
	class CStandAnimation;
	class CWalkAnimation;
	class CRunAnimation;
	class CIjimePunchAnimation;
	class CDoubleLariatAnimation;
	class CDamagedAnimation;
	class CDownAnimation;
	class CWakeAnimation;

	IAnimationStrategy *m_pStrategy;
//�ȉ��͖{����MMDMikuModelAnimationStrategy.cpp�ɒ�`��������vc�̃o�O�ŃG���[���o�ăE�U�C�̂�
//�d���Ȃ������ɒu��

//CMMD_MikuModel�������Ƃ���Strategy�p�^�[�����ۂ���
//����new, delete����̂��_���C�B�g���܂킵�����B
//�N���X�����o�ϐ��Ȃ��̎����ɂ����B
//�ǂ݂ɂ����B���߂āAmState,mOldState,mWeight�̓����o�ϐ��Ƃ��Ď����Ă��悩�������H
//�������ACMMD_MikuModel���|�C���^�œn���Ă��āA�����ł����̕ϐ������ʂ��Ď����Ă���΁A�����񂾂����̎����͂Ȃ񂩖��ʂ�����B
class IAnimationStrategy{
public:
	virtual ~IAnimationStrategy(){}
	virtual void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )=0;
	
	//virtual bool load()=0;

	//�z���C�g���X�g�I�Ɏ������邱��
	//�������̂ق����o�O�����Ȃ�
	//�z���C�g���X�g�I�Ƃ����̂́A�m���ɐM���ł���l�̂�true��Ԃ�����
	//�J�ڂł�����̂�true��Ԃ��A���̑��S����false��Ԃ�����
	//�V������Ԃ�ǉ������ꍇ�A�����I��false��Ԃ��悤��
	//�ł��A���̎����߂�ǂ��ȁB
	//������Ԃ���J�ڂł���̂́A�u���b�N���X�g�I�ɏ������H
	//�Ƃ肠�����A�o���邾���Afalse��Ԃ�������
	virtual bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku ) = 0;
	//������Ԃ̃A�j�����H
	virtual bool isStand() = 0;

	//�ڒn���Ă��邩�H
	//������ik�g���������̒����̂��
	//virtual bool isGround();
	
	//���̊֐����ĂԑO�ɁAisChangeAble�֐����Ăь��؂��邱��
	virtual IAnimationStrategy* changeState( IAnimationStrategy* pS, CMMD_MikuModel *pMiku ) = 0;
	virtual AnimationState getType() = 0;
protected:
	virtual bool isOver( CMMD_MikuModel *pMiku ) =0;
};

#define INCREASED_WEIGHT_VALUE 0.05f;

//default�������
//weightBelnd����A���s�[�g����B
class CBaseAnimationStrategy : public IAnimationStrategy {
public:
	CBaseAnimationStrategy(){}
	virtual ~CBaseAnimationStrategy(){}
	
protected:
	//�Đ����I�������J��Ԃ�0����Đ�
	void default_update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		pMiku->mModel.update( world, true );
		float elapsedFrame = elapsedTime*30;

		if( pMiku->mWeight >= 1.f ) {
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
		} else {
			pMiku->mModel.setWeight( pMiku->mState, pMiku->mWeight );
			pMiku->mModel.setWeight( pMiku->mOldState, 1.f-pMiku->mWeight );
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
			pMiku->mWeight += INCREASED_WEIGHT_VALUE;
			if( pMiku->mWeight >= 1.f ) {
				pMiku->mModel.setWeight( pMiku->mOldState, 0.f );
				pMiku->mModel.setWeight( pMiku->mState, 1.f );
			}
		}

		if( isOver( pMiku ) ){
				pMiku->mModel.setFrame( pMiku->mState, 0.f );
		}
	}
	//�����ł́AweightBlend�̌�n���Ƃ��Ƃ̃��[�V�����������ꂢ�ɓ����������B
	//���̊K�w�œK�؂Ȓlpoiter��Ԃ����ƁB
	void default_changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		assert( pS->getType()<ANIM_NUM );
		assert( isChangeAble( pS, pMiku ) && "���̊֐����ĂԑO�ɁAisChangeAble�֐����Ăь��؂��邱��" );
		
		//��n��
		//���݂̏d����1��
		pMiku->mModel.setWeight( pMiku->mState, 1.0f );
		//�Â����̂����Z�b�g
		pMiku->mModel.setFrame( pMiku->mOldState, 0.f );
		pMiku->mModel.setWeight( pMiku->mOldState, 0.f );
		
		pMiku->mOldState=pMiku->mState;
		pMiku->mState=pS->getType();
		
		//���̏���//����������ĂȂ��Ƃ����Ȃ��̂ŔO�̂��߃��Z�b�g
		pMiku->mWeight=0.f;
		pMiku->mModel.setWeight( pMiku->mState, 0.f );
		pMiku->mModel.setFrame( pMiku->mState, 0.f );
	}

	bool default_isOver( CMMD_MikuModel *pMiku )
	{
		return pMiku->mModel.isFrameOver( pMiku->mState );
	}

private:
};

class CStandAnimation : public CBaseAnimationStrategy {
public:
	CStandAnimation(){}
	virtual ~CStandAnimation(){}
	
	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		default_update( world, elapsedTime, pMiku );
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		default_changeState( pS, pMiku );
		return pS;
	}

	bool isChangeAble( IAnimationStrategy *pS , CMMD_MikuModel *pMiku )
	{
		AnimationState state=pS->getType();

		if( state==ANIM_WAKE ||
			state==ANIM_STAND ){
			return false;
		}
		if( pS->isStand() ) {
			return true;
		}
		return false;
	}

	AnimationState getType(){
		return ANIM_STAND;
	}

	bool isStand(){
		return true;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return default_isOver( pMiku );
	}
};

class CWalkAnimation : public CBaseAnimationStrategy {
public:
	CWalkAnimation(){}
	virtual ~CWalkAnimation(){};

	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		default_update( world, elapsedTime, pMiku );
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		default_changeState( pS, pMiku );
		return pS; 
	}
	AnimationState getType()
	{
		return ANIM_WALK;
	}
	bool isStand() {
		return true;
	}
	bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		AnimationState s=pS->getType();
		if(s==ANIM_WALK){
			return false;
		}
		if( pS->isStand() )
		{
			return true;
		}
		return false;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return pMiku->mModel.getFrame( pMiku->mState ) > 32;
	}
};

class CRunAnimation : public CBaseAnimationStrategy {
	public:
	CRunAnimation(){}
	virtual ~CRunAnimation(){};

	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		default_update( world, elapsedTime, pMiku );
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		default_changeState( pS, pMiku );
		return pS; 
	}
	AnimationState getType()
	{
		return ANIM_RUN;
	}
	bool isStand() {
		return true;
	}
	bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		AnimationState s=pS->getType();
		if(s==ANIM_RUN){
			return false;
		}
		if( pS->isStand() )
		{
			return true;
		}
		return false;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return pMiku->mModel.getFrame( pMiku->mState ) > 20;
	}
};

class CIjimePunchAnimation : public CBaseAnimationStrategy {
public:
	CIjimePunchAnimation(){}
	virtual ~CIjimePunchAnimation(){};

	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		default_update( world, elapsedTime, pMiku );
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		default_changeState( pS, pMiku );
		return pS; 
	}
	AnimationState getType()
	{
		return ANIM_IJIME_PUNCH;
	}
	bool isStand() {
		return true;
	}
	bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		AnimationState s=pS->getType();
		if(s==ANIM_IJIME_PUNCH){
			return false;
		}
		if( pS->isStand() )
		{
			return true;
		}
		return false;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return default_isOver(pMiku);
	}
};

class CDamagedAnimation : public CBaseAnimationStrategy {
public:
	CDamagedAnimation(){}
	virtual ~CDamagedAnimation(){};
	//repeat�Ȃ� stand�ɑJ��
	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
			pMiku->mModel.update( world, true );
		float elapsedFrame = elapsedTime*30;

		if( pMiku->mWeight >= 1.f ) {
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
		} else {
			pMiku->mModel.setWeight( pMiku->mState, pMiku->mWeight );
			pMiku->mModel.setWeight( pMiku->mOldState, 1.f-pMiku->mWeight );
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
			pMiku->mWeight += INCREASED_WEIGHT_VALUE;
			if( pMiku->mWeight >= 1.f ) {
				pMiku->mModel.setWeight( pMiku->mOldState, 0.f );
				pMiku->mModel.setWeight( pMiku->mState, 1.f );
			}
		}

		if( isOver( pMiku ) ){
			pMiku->m_pStrategy=changeState( &(pMiku->mStand), pMiku );
		}
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		default_changeState( pS, pMiku );
		return pS; 
	}
	AnimationState getType()
	{
		return ANIM_DAMAGED;
	}
	bool isStand() {
		return true;
	}
	bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		AnimationState s=pS->getType();
		if(s==ANIM_DAMAGED){
			return false;
		}
		if( pS->isStand() )
		{
			return true;
		}
		return false;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return default_isOver(pMiku);
	}
};

class CDoubleLariatAnimation : public CBaseAnimationStrategy {
public:
	CDoubleLariatAnimation(){}
	virtual ~CDoubleLariatAnimation(){};
	//���ꃋ�[�v�@���s�[�g�͓r������
	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		pMiku->mModel.update( world, true );
		float elapsedFrame = elapsedTime*30;

		if( pMiku->mWeight >= 1.f ) {
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
		} else {
			pMiku->mModel.setWeight( pMiku->mState, pMiku->mWeight );
			pMiku->mModel.setWeight( pMiku->mOldState, 1.f-pMiku->mWeight );
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
			pMiku->mWeight += INCREASED_WEIGHT_VALUE;
			if( pMiku->mWeight >= 1.f ) {
				pMiku->mModel.setWeight( pMiku->mOldState, 0.f );
				pMiku->mModel.setWeight( pMiku->mState, 1.f );
			}
		}

		if( isOver( pMiku ) ){
				pMiku->mModel.setFrame( pMiku->mState, 30.f );
		}
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		default_changeState( pS, pMiku );
		return pS; 
	}
	AnimationState getType()
	{
		return ANIM_DOUBLE_LARIAT;
	}
	bool isStand() {
		return true;
	}
	bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		AnimationState s=pS->getType();
		if(s==ANIM_DOUBLE_LARIAT){
			return false;
		}
		if( pS->isStand() )
		{
			return true;
		}
		return false;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return default_isOver(pMiku);
	}
};

class CDownAnimation : public CBaseAnimationStrategy {
public:
	CDownAnimation(){}
	virtual ~CDownAnimation(){};
	//weight blend ����B���s�[�g�Ȃ�
	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		pMiku->mModel.update( world, true );
		float elapsedFrame = elapsedTime*30;
		
		if( pMiku->mWeight >= 1.f ) {
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
		} else {
			pMiku->mModel.setWeight( pMiku->mState, pMiku->mWeight );
			pMiku->mModel.setWeight( pMiku->mOldState, 1.f-pMiku->mWeight );
			pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );
			pMiku->mWeight += INCREASED_WEIGHT_VALUE;
			if( pMiku->mWeight >= 1.f ) {
				pMiku->mModel.setWeight( pMiku->mOldState, 0.f );
				pMiku->mModel.setWeight( pMiku->mState, 1.f );
			}
		}

		if( isOver( pMiku ) ){
			pMiku->m_pStrategy=changeState( &(pMiku->mWake), pMiku );
		}
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		assert( isChangeAble( pS, pMiku ) && "���̊֐����ĂԑO�ɁAisChangeAble�֐����Ăь��؂��邱��" );
		
		pMiku->mModel.setFrame( pMiku->mState, 0.f );
		pMiku->mModel.setWeight( pMiku->mState, 0.f );
		pMiku->mModel.setFrame( pMiku->mOldState, 0.f );
		pMiku->mModel.setWeight( pMiku->mOldState, 0.f );
		pMiku->mOldState=pMiku->mState;
		pMiku->mState=pS->getType();
		pMiku->mWeight = 1.f;
		pMiku->mModel.setWeight( pMiku->mState, 1.f );
		pMiku->mModel.setFrame( pMiku->mState, 0.f );
		return pS;
	}
	AnimationState getType()
	{
		return ANIM_DOWN;
	}
	bool isStand() {
		return true;
	}
	bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{	
		if( isOver(pMiku) ){
			AnimationState s=pS->getType();
			if(s==ANIM_WAKE){
				return true;
			}
		}
		return false;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return default_isOver(pMiku);
	}
};


class CWakeAnimation : public CBaseAnimationStrategy {
public:
	CWakeAnimation(){}
	virtual ~CWakeAnimation(){};
	//blend�Ȃ��A���s�[�g�Ȃ�
	void update( const D3DXMATRIX &world, float elapsedTime, CMMD_MikuModel *pMiku )
	{
		
		pMiku->mModel.update( world, true );
		float elapsedFrame = elapsedTime*30;

		assert( pMiku->mModel.getWeight(pMiku->mState)==1.0f );
		//pMiku->mModel.setWeight( pMiku->mState, 1.f );
		pMiku->mModel.stepFrame( pMiku->mState, elapsedFrame );

		if( isOver( pMiku ) ){
			pMiku->m_pStrategy=changeState( &(pMiku->mStand), pMiku );
		}
	}
	IAnimationStrategy* changeState( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		assert( isChangeAble( pS, pMiku ) && "���̊֐����ĂԑO�ɁAisChangeAble�֐����Ăь��؂��邱��" );
		
		pMiku->mModel.setFrame( pMiku->mState, 0.f );
		pMiku->mModel.setWeight( pMiku->mState, 0.f );
		pMiku->mModel.setFrame( pMiku->mOldState, 0.f );
		pMiku->mModel.setWeight( pMiku->mOldState, 0.f );
		pMiku->mOldState=pMiku->mState;
		pMiku->mState=pS->getType();
		pMiku->mWeight = 1.f;
		pMiku->mModel.setWeight( pMiku->mState, 1.f );
		pMiku->mModel.setFrame( pMiku->mState, 0.f );
		return pS;
	}
	AnimationState getType()
	{
		return ANIM_WAKE;
	}
	bool isStand() {
		return false;
	}
	bool isChangeAble( IAnimationStrategy *pS, CMMD_MikuModel *pMiku )
	{
		if( isOver(pMiku) ){
			AnimationState s=pS->getType();
			if(s==ANIM_STAND){
				return true;
			}
		}
		return false;
	}
protected:
	bool isOver( CMMD_MikuModel *pMiku ){
		return default_isOver(pMiku);
	}
};

	CStandAnimation mStand;
	CWalkAnimation mWalk;
	CRunAnimation mRun;
	CIjimePunchAnimation mIjimePunch;
	CDoubleLariatAnimation mDoubleLariat;
	CDamagedAnimation mDamaged;
	CDownAnimation mDown;
	CWakeAnimation mWake;

}; //class CMMD_MikuModel
#endif//__MMDMIKUMODEL_H__
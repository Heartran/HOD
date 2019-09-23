#include"MMDMikuModel.h"
#include"Animation.h"
#include<assert.h>
#include<GameLib/Input/Keyboard.h>
#include<GameLib/Math/Random.h>
#include<ctime>
#include"BulletPhysics/Manager.h"
CMMD_MikuModel::CMMD_MikuModel()
	:mState(ANIM_STAND),
	mOldState(ANIM_STAND),
	mWeight(1.f),
	m_pStrategy(NULL)

{
	bool b;
	b=mModel.loadPMD("Data/MMDData/miku2.pmd", true);
	assert(b);
	//b=mModel.setRightFootIKBone();
	//assert(b);
	b=mModel.setRightToeIKBone();
	assert(b);
	//b=mModel.setLeftFootIKBone();
	//assert(b);
	b=mModel.setLeftToeIKBone();
	assert(b);
	//���ԑ厖
	b=mModel.loadVMD("Data/MMDData/vmd/��{_�\��_�L�b�N�{�N�V���O_000.vmd");
	//b=mModel.loadVMD("Data/MMDData/vmd/GOMYWAY.vmd");
	//b=mModel.loadVMD("Data/MMDData/vmd/init.vmd");
	assert(b);
	b=mModel.loadVMD("Data/MMDData/vmd/A16_SO_���X�L�b�v_p32.vmd");
	assert(b);
	b=mModel.loadVMD("Data/MMDData/vmd/H17_SO_�����~�N_p20.vmd");
	assert(b);
	b=mModel.loadVMD("Data/MMDData/vmd/�����߂����q�̋t�PA.vmd");
	assert(b);
	b=mModel.loadVMD("Data/MMDData/vmd/�n�K�[(��]��30-37).vmd");
	assert(b);
	b=mModel.loadVMD("Data/MMDData/vmd/��_��낯_��O_��_000.vmd");
	assert(b);
	b=mModel.loadVMD("Data/MMDData/vmd/��_�_�E��_�㍶_�]�k�_000.vmd");
	assert(b);
	b=mModel.loadVMD("Data/MMDData/vmd/awake2.vmd");
	assert(b);
	//b=mModel.loadVMD("Data/MMDData/vmd/");
	//assert(b);
	mModel.setWeight( mState, 1.f );

	m_pStrategy=&mStand;
}

CMMD_MikuModel::~CMMD_MikuModel(){

}

void CMMD_MikuModel::init( const D3DXMATRIX &world ){
	mModel.fitRigidBodeisToBone(world);
	mModel.update(world, true);
	//BulletPhysics::Manager::instance()->update(0.f);
}

void CMMD_MikuModel::draw() {
	mModel.draw();
}

void CMMD_MikuModel::update( const D3DXMATRIX &world, float elapsedTime ){
	m_pStrategy->update(world, elapsedTime, this);
	
	//debugControll( world, elapsedTime );
	//m_pStrategy->update( world, elapsedTime );
} 



CMMD_MikuModel::AnimationState CMMD_MikuModel::getCurrentAnimation() {
	return mState;
}

void CMMD_MikuModel::changeAnimation(AnimationState state) {
	//changeAnimation�����Ȃ����Ԃ��~��������
	IAnimationStrategy *pS=NULL;
	assert( state<ANIM_NUM );
	switch(state){
		case ANIM_STAND:
			pS=&mStand;
			break;
		case ANIM_WALK:
			pS=&mWalk;
			break;
		case ANIM_RUN:
			pS=&mRun;
			break;
		case ANIM_IJIME_PUNCH:
			pS=&mIjimePunch;
			break;
		case ANIM_DOUBLE_LARIAT:
			pS=&mDoubleLariat;
			break;
		case ANIM_DAMAGED:
			pS=&mDamaged;
			break;
		case ANIM_DOWN:
			pS=&mDown;
			break;
		case ANIM_WAKE:
			pS=&mWake;
			break;
		default:
			assert(false && "�V������Ԃ�ǉ�������case����ǉ����Ă��������B");
			break;
	}

	if( m_pStrategy->isChangeAble( pS, this ) )
	{
		m_pStrategy=m_pStrategy->changeState( pS, this );
	}
}

void CMMD_MikuModel::debugControll( const D3DXMATRIX &w, float elapsedTime ){
	GameLib::Input::Keyboard k;
	if( k.isTriggered('z') ){
	}

	if( k.isTriggered('x') ){
		m_pStrategy->update(w, -elapsedTime, this);
	}
		
	//if( k.isTriggered('c') ){
	//if( k.isOn('c') ){
		m_pStrategy->update(w, elapsedTime, this);
	//}

	if( k.isTriggered('v') ){
		//int c = getCurrentAnimation();
		int c = GameLib::Math::Random::create( static_cast<int>(std::time(NULL)) ).getInt(0, 8);
		if(c>7){
			c=0;
		}
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(c) );
	}
	
	if( k.isTriggered('0') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(0) );
	}
	if( k.isTriggered('1') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(1) );
	}
	if( k.isTriggered('2') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(2) );
	}
	if( k.isTriggered('3') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(3) );
	}
	if( k.isTriggered('4') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(4) );
	}
	if( k.isTriggered('5') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(5) );
	}
	if( k.isTriggered('6') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(6) );
	}
	if( k.isTriggered('7') ){
		changeAnimation( static_cast< CMMD_MikuModel::AnimationState >(7) );
	}
}

void CMMD_MikuModel::update( const D3DXMATRIX &world, float elapsedTime, float groundHeight){
	m_pStrategy->update(world, elapsedTime, this);	
	mModel.update(world, groundHeight, true );
}


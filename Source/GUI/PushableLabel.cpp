#include"PushableLabel.h"
#include<GameLib/Input/Mouse.h>

namespace GUI {


PushableLabel::PushableLabel(
	int x, int y,
	int sizeX, int sizeY,
	unsigned int color,
	unsigned int fontColor,
	float u0, float v0,
	float u1, float v1 )
	:Label( 
		x, y,
		sizeX, sizeY,
		color,
		fontColor,
		u0, v0,
		u0, v1 ),
	mIsPushed(false),
	mIsPushing(false),
	mIsHover(false),
	mPushable(true)
{
}

PushableLabel::~PushableLabel(){
}
		
void PushableLabel::update(){
	Label::update();

	GameLib::Input::Mouse m;
	const int &x = m.x(); 
	const int &y = m.y(); 
	mIsPushed = false;

	//�}�E�X�J�\�[�����{�^����ɂ��邩�H
	mIsHover = BaseComponent::isHover(x, y);

	if( mPushable ){
		//�{�^����ŃN���b�N�����Ƃ�
		if( m.isTriggered(m.BUTTON_LEFT) && mIsHover ){
			mIsPushing = true;
		}


		//�{�^����ŃN���b�N���Ă���A�������ꍇ
		if( mIsPushing  && m.isReleased( m.BUTTON_LEFT ) ) {
			//�{�^����ŗ������ꍇ
			if( mIsHover )  {
				mIsPushed = true;
			}
			//�������t���O����
			mIsPushing = false;
		}
	}
}

void PushableLabel::draw( Manager* mngr ){
	Label::draw(mngr);
}

void PushableLabel::setPushable( bool enable ){
	mPushable = enable;
	
	if(!mPushable){
		mIsPushed=false;
		mIsPushing=false;
	}
}

}//namespace GUI
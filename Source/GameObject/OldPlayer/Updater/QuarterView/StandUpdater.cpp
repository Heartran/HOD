#include"StandUpdater.h"

#include"GameObject/OldPlayer/Player.h"
#include<GameLib/Input/Mouse.h>
#include<GameLib/Input/Keyboard.h>
#include"Input/PlayerMouse.h"
#include<GameLib/GameLib.h>
#include"Camera/PlayerCamera.h"
#include"GameObjectData/Status.h"
namespace GameObject{
	namespace OldPlayer{
		class Player;
		namespace Updater{
			namespace QuarterView{

StandUpdater::StandUpdater(Player* player)
	:IUpdater(player),
	mCoolTime(2.f)
{
	mPlayer->getCamera()->changeQuarterView();
	//���������������ꍇ�����邾�낤����
	//mPlayer->stop();
	mPlayer->changeAnimation( ANIM_STAND );
	mPlayer->getCamera()->lockOff();
	mPlayer->setMaxSpeedHigh();

}
StandUpdater::~StandUpdater(){
}
void StandUpdater::update( float elapsedTime ){

	GameLib::Input::Mouse m;
	 GameLib::Input::Keyboard k;
	if( m.isTriggered(m.BUTTON_LEFT) ){
		//�N���b�N�����ʒu�ɃI�u�W�F�N�g���������ꍇ
		if( mPlayer->getMouse()->onObject() ){
			IColliderSharedPtr obj = mPlayer->getMouse()->getOnObject();
			const RTTI &rtti = obj->getRTTI();
			if( rtti.isExactly( Rttis::Enemy() ) ){
				//mPlayer->getCamera()->lockOn( obj );
				mPlayer->changeUpdater( QV_SWING );
				
				//IUpdater* attack = new AttackUpdater();
				//mPlayer->changeUpdater( attack );
			}
			//else if( rtti.isExactly( Rttis::Item() ){
			//}
		}
		//�N���b�N�����ʒu�ɉ����Ȃ��ꍇ
		else {
			mPlayer->changeUpdater( QV_RUN );
			return;
		}
		return;
	}else if( m.isReleased( m.BUTTON_RIGHT ) ){
		if( mPlayer->onGround() ){
			mPlayer->changeUpdater( QV_SWING );
			return;
		}
	} else if( k.isTriggered('c') ){
		if( mPlayer->onGround() ){
			//mPlayer->changeUpdater( TPS_STAND );
			mPlayer->changeUpdater( CAR_STAND );
			return;
		}
	}

	if( mPlayer->onGround() ){
		if(mCoolTime < 0.f){
			mPlayer->healStamina();			
		} else {
			mCoolTime -= mPlayer->getElapsedTime();
		}
	}
}


}//end namespaces
}
}
}

#include"StandUpdater.h"
#include"GameObject/OldPlayer/Updater/QuarterView/StandUpdater.h"

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
			namespace TPS{

StandUpdater::StandUpdater(Player* player)
	:IUpdater(player),
	mCoolTime(1.f)

{
	//���������������ꍇ�����邾�낤����
	//mPlayer->stop();
	mPlayer->changeAnimation( ANIM_STAND );
	mPlayer->getCamera()->changeTPS();
	mPlayer->setMaxSpeedLow();
}

StandUpdater::~StandUpdater(){
}
void StandUpdater::update( float elapsedTime ){
	GameLib::Input::Mouse m;
	GameLib::Input::Keyboard k;
	int x,y;

	if( mPlayer->getMouse()->isFlickedPlayer( &x, &y ) && mPlayer->onGround() ){
		//if( mPlayer->mStatus->canRunHighSpeed() ){
			mPlayer->changeUpdater( TPS_STEP );
		//}
		return;

	} else if( m.isTriggered(m.BUTTON_LEFT) ){
		//�N���b�N�����ʒu�ɃI�u�W�F�N�g���������ꍇ
		if( mPlayer->getMouse()->onObject() ){
			IColliderSharedPtr obj = mPlayer->getMouse()->getOnObject();
			const RTTI &rtti = obj->getRTTI();
			if( rtti.isExactly( Rttis::Enemy() ) ){
				//IUpdater* attack = new AttackUpdater();
				//mPlayer->changeUpdater( attack );
				mPlayer->getCamera()->lockOn( obj );
			}
			//else if( rtti.isExactly( Rttis::Item() ){
			//}
		}
		//�N���b�N�����ʒu�ɉ����Ȃ��ꍇ
		else {
			mPlayer->changeUpdater( TPS_RUN );
			return;
		}
		return;
	}else if( m.isTriggered( m.BUTTON_RIGHT ) ){
		mPlayer->changeUpdater( TPS_SWING );
		
		//Weapontype wp= mPlayer->hasWeaponType();
		//if( wp==SWORD ){
		//} else if( wp==GUN ){
		//} else {}
		//
		//IUpdaterSharedPtr attack( NEW attack( mPlayer ) );
		//mPlayer->changeUpdater( attack );
		return;
	} else if( k.isTriggered('c') ){
		if( mPlayer->onGround() ){
			mPlayer->changeUpdater( QV_STAND );
			return;
		}
	} else {
		if(mPlayer->onGround() && mPlayer->isStopping() ){
			if(mCoolTime < 0.f){
				mPlayer->healStamina( );			
			} else {
				mCoolTime -= mPlayer->getElapsedTime();
			}
		}
	}
}


}//end namespaces
}
}
}

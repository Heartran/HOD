#include"SoundManager.h"
#include<GameLib/GameLib.h>
#include<GameLib/Sound/Wave.h>
#include<GameLib/Sound/Player.h>
//#include<GameLib/Sound/Manager.h>
#include <sstream>
using namespace GameLib;

SoundManager::UptrManager SoundManager::mInstance = 0;

SoundManager* SoundManager::instance(){
	if(mInstance == 0){
		mInstance.reset( new SoundManager() );
	}
	return mInstance.get();
}

void SoundManager::create(){
	//ASSERT( !mInstance );
	if(mInstance == 0){
		mInstance.reset( new SoundManager() );
	}
}

void SoundManager::destroy(){
	//ASSERT( mInstance );
	mInstance.reset();
}

SoundManager::SoundManager() :
mSePlayerPos( 0 ){
	//�t�@�C�������X�g�����[�h�J�n�B
	//�����ƍ��Ȃ�ɂ̓t�@�C�������X�g�͕ʃt�@�C���ɂ�����A
	//�e�L�X�g�t�@�C���ɂ��ă��[�h�����肷�邱�ƂɂȂ�B

	//�Ȃ��A����͎���Ȃ̂őS�������t�@�C��...
	const char* bgmFiles[] = {
		"title.wav", //BGM_TITLE
		"charara.wav", //BGM_GAME
	};
	const char* seFiles[] = {
		"dokaan.wav", //SE_EXPLOSION
		"dokaan.wav", //SE_DEATH
		"dokaan.wav", //SE_CURSOR_MOVE
		"dokaan.wav", //SE_SELECTION
		"dokaan.wav", //SE_SET_BOMB
	};
	ostringstream oss; //�����񍇐��p������X�g���[��
	for ( int i = 0; i < BGM_MAX; ++i ){
		oss.str( "" ); //������
		oss << "data/sound/bgm/" << bgmFiles[ i ];
		mBgmWaves[ i ] = Sound::Wave::create( oss.str().c_str() );
	}
	for ( int i = 0; i < SE_MAX; ++i ){
		oss.str( "" ); //������
		oss << "data/sound/se/" << seFiles[ i ];
		mSeWaves[ i ] = Sound::Wave::create( oss.str().c_str() );
	}
}

SoundManager::~SoundManager(){
	//GameLib::Sound::Manager���J������O�ɊJ�����邱��
	//SoundBuffer���J�����邽�߂�DirectSound���K�v�Ȃ̂�
}

bool SoundManager::isReady(){
	//�S����Wave��true��Ԃ������ׂ�
	//&&������Ă����ƈ�ł�false�Ȃ�false�ɂȂ�킯���B
	bool ret = true;
	for ( int i = 0; i < BGM_MAX; ++i ){
		ret = ret && mBgmWaves[ i ].isReady();
	}
	for ( int i = 0; i < SE_MAX; ++i ){
		ret = ret && mSeWaves[ i ].isReady();
	}
	return ret;
}

void SoundManager::playBgm( Bgm bgm ){
	mBgmPlayer = Sound::Player::create( mBgmWaves[ bgm ] );
	mBgmPlayer.play( true ); //���[�v�Đ�
}

void SoundManager::stopBgm(){
	if ( mBgmPlayer ){
		mBgmPlayer.stop();
	}
}

void SoundManager::playSe( Se se ){
	mSePlayers[ mSePlayerPos ] = Sound::Player::create( mSeWaves[ se ] );
	mSePlayers[ mSePlayerPos ].play(); //�Đ�
	++mSePlayerPos;
	//�����߂�
	if ( mSePlayerPos == SE_PLAYER_MAX ){
		mSePlayerPos = 0;
	}
}
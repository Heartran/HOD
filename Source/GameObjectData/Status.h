#pragma once
#ifndef __GAMEOBJECTDATA_STATUS_H__
#define __GAMEOBJECTDATA_STATUS_H__
#include<assert.h>
#include<algorithm>
#include"fwd.h"
#include"helper/RangeNumber.h"

namespace boost{
	namespace serialization{
		class access;
	}
}

namespace GameLib{
	namespace FileIO {
		class InFile;
	}
}

namespace GameObjectData{
	
class Status {
public:
	static StatusSharedPtr create(){
		StatusSharedPtr st( new Status() );
		return st;
	}
	Status();
	virtual ~Status(){}

	StatusSharedPtr clone()const;

	//���N���X��serialize�֐��𒼐ڌĂяo���Ȃ����ƁI�I
	friend class boost::serialization::access;
	

	RangeNumber<float> hp;
	RangeNumber<float> mp;
	RangeNumber<float> lp;
	RangeNumber<float> stamina;
	
	RangeNumber<float> str;
	RangeNumber<float> inte;
	RangeNumber<float> will;
	RangeNumber<float> dex;
	RangeNumber<float> qui;
	RangeNumber<float> vit;
	RangeNumber<float> psy;
	RangeNumber<float> luck;

	float mass;

	//3��ނ̃X�s�[�h�����悤�ɂ��Ă���
	//�g�����͎����ɂ��
	//player�̏ꍇ
	//��퓬���Ɛ퓬���̕����̃X�s�[�h�Ɏg��
	float speedLow;
	float speed;
	float speedHigh;

	float jumpSpeed;

	float angleSpeed;//�p���x

	float accel;

	//�X�^�~�i�̎g�p��	
	float usageStaminaPerSecSpeedLow;	// �P�� stamina/seccond
	float usageStaminaPerSecSpeed;		// �P�� stamina/seccond
	float usageStaminaPerSecSpeedHigh;	// �P�� stamina/seccond
	float usageStaminaPerJump;			// �P�� stamina/jump
	float healingStaminaPerSec;//�񕜗�
	
	//�����h��
	unsigned int elementalDefs;


	unsigned short downValue;
	unsigned short staggerValue;
	float elapsedTimeAfterDamaged;
	
	unsigned char group;

	unsigned char damageCorrectedRate;

	//����
	bool canStagger;//superArmor
	
	//�S��Ԃ���Lying�Ɉڍs�ł���悤��

	//�d��
	//�q�b�g�X�g�b�v
	//�K�[�h
	//���

	float unContrableTime;
	float mutekiTime;//invincibleTime;
	
	//���K�E�Z�Q�[�W
	//�o��
	//�I�[��
	unsigned char powerGauge;
	
	bool loadBinary( const char *fileName );
	bool loadBinaryFromArchive( const GameLib::FileIO::InFile &infileInArchive );
	bool saveBinary( const char *fileName );
	
	bool loadText( const char *fileName );
	bool loadTextFromArchive( const GameLib::FileIO::InFile &infileInArchive );
	bool saveText( const char *fileName );
	
	bool loadXML( const char *fileName );
	bool loadXMLFromArchive( const GameLib::FileIO::InFile &infileInArchive );
	bool saveXML( const char *fileName );

	void reset();

	void update( float dt );
	
	//it return true, if it is died.
	bool isDied()const{
		return hp <= 0;
	}
	//��{�́@���̎��Ԃ��BelemDef(0-254) * vit * (100 + vitRate) / 100;
	//�����A�����h��l��max�̏ꍇ�@unsigned int max(-1)��Ԃ�;
	//unsigned int calcDef( unsigned char damageTypes ) const {
	//	char num=0;
	//	unsigned char elementalDef = 0;

	//	for( char i=0; i<16 ; ++i ){
	//		if(  1 & (damageTypes>>i) ){
	//			num+=1;
	//			elementalDef += ( ( 0xf << (4*i) ) & elementalDefs ) >> (4*i) ;
	//		}
	//	}
	//	//0-15 �� 0-255
	//	unsigned char averageDef = num==0 ? 0 : elementalDef*17/num;
	//	unsigned int def = averageDef==255 ? -1 : averageDef * vit  / 100;
	//	return def;
	//}

	bool emptyStamina()const;
	bool overStamina( const float &usageStamina )const{
		return stamina < usageStamina;
	}
	bool overOrEqualStamina( const float & usageStamina )const{
		return stamina <= usageStamina;	
	}

	bool canJump()const{
		return stamina >= usageStaminaPerJump;
	}

	bool canRunHighSpeed( const float & elapsedTime )const{
		return stamina >= usageStaminaPerSecSpeedHigh * elapsedTime;
	}

	bool canRun( const float & elapsedTime )const{
		return stamina >= usageStaminaPerSecSpeedLow * elapsedTime;
	}
	bool canWalk( const float & elapsedTime )const{
		return stamina >= usageStaminaPerSecSpeedLow * elapsedTime;
	}

	void jump(){
		assert( canJump() );
		stamina.add( -usageStaminaPerJump );
	}

	void runHighSpeed( const float & elapsedTime ){
		assert( canRunHighSpeed( elapsedTime ) );
		stamina.add(-usageStaminaPerSecSpeedHigh * elapsedTime );
	}

	void run( const float & elapsedTime ){
		assert( canRun(elapsedTime) );
		stamina.add(-usageStaminaPerSecSpeed * elapsedTime);
	}
	void walk( const float & elapsedTime ){
		assert( canWalk(elapsedTime) );
		stamina.add(-usageStaminaPerSecSpeedLow * elapsedTime);
	}

	void healStamina( const float & elapsedTime ){
		stamina.add(healingStaminaPerSec * elapsedTime);
	}

	float getSpeed() const {
		return speed;
	}

	float getSpeedHigh() const {
		return speedHigh;
	}
	float getSpeedLow() const {
		return speedLow;
	}

	float getAngleSpeed()const {
		return angleSpeed;
	}

private:
	//���N���X��serialize�֐��𒼐ڌĂяo���Ȃ����ƁI�I
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int ver);
	template<class Archive>
	void load(Archive& ar, const unsigned int version);
	template<class Archive>
	void save(Archive& ar, const unsigned int version)const;


	//clone�֐������̂��߃N���X���ł͋�����
	Status(const Status &) = default;
	//�֎~
	void operator=(const Status &) = delete;
};

}//end GameObjectData
#endif //__GAMEOBJECTDATA_STATUS_H__
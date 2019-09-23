#ifndef __GAMEOBJECTDATA_WEAPONSTATUS_H__
#define __GAMEOBJECTDATA_WEAPONSTATUS_H__

#include<vector>
#include<map>
#include<string>
#include"fwd.h"
#include"Status.h"

namespace boost{
	namespace serialization{
		class access;
	}
}

namespace GameLib{
	namespace FileIO {
		class InFile;
	}
	namespace PseudoXml {
		class ConstElement;
	}
}


namespace GameObjectData {


class AdditionalEffect{
public:
	virtual ~AdditionalEffect(){}
	virtual void exec( Status *st )=0;
};
//static std::map< std::string, AdditionalEffect* > gsAdditionalEffectMap;

//exvs�ƃ{�[�_�[�u���C�N, saga�Q�l
//saga


//DamageData�Ƃ��ł�������
class WeaponStatus{
public:
	friend boost::serialization::access;
	
	WeaponStatus( StatusSharedPtr status );
	WeaponStatus();
	virtual ~WeaponStatus(){}


	unsigned char damageTypes;

	int atkHp;
	int atkMp;
	int atkLp;
	int atkStamina;
	
	unsigned char damageCorrectedRate;//�_���[�W�␳��
	unsigned char downValue;//�_�E���l
	unsigned char staggerValue;//��낯�l
	float unControlableTime;//����s�\���ԁA�d������

	//float knockBackNormalVel[3];
	float upDownwardOffset;//�ł��グ�A���������U���̂��߂̒l
	//float knockBackVelSpeed[3];//�Ԃ���є{�� //���邩�H
	
	//�q�b�g�o�b�N����Ƃ��ɗ^����l
	//����Ȋ����Ŏg�p
	//mRigidBody->applyCentralImpulse( dir*mMass*hitbackValue );
	float hitbackValue;

	//������΂��Ƃ��ɗ^����l
	//����Ȋ����Ŏg�p
	//mRigidBody->applyCentralImpulse( dir*mMass*hitbackValue );
	//float blowValue;
	
	unsigned short staminaPerAttack;//���킶��Ȃ��ăA�j���[�V�����s���ɂ���������������?

	bool canCollideOnlyOnce;

	StatusWeakPtr me;
	//std::vector<AdditionalEffect*> additionalEffects;
	//AdditionalEffect *additionalEffects;
	//unsigned char numAdditionalEffects

	bool loadBinary( const char *fileName );
	bool loadBinaryFromArchive( const GameLib::FileIO::InFile &infileInArchive );
	bool saveBinary( const char *fileName );
	
	bool loadText( const char *fileName );
	bool loadTextFromArchive( const GameLib::FileIO::InFile &infileInArchive );
	bool saveText( const char *fileName );
	
	bool loadXML( const char *fileName );
	bool loadXMLFromArchive( const GameLib::FileIO::InFile &infileInArchive );
	bool saveXML( const char *fileName );
	
	void print() const ;

	virtual int caluculateDamage()const;
	//me(Status) �ȊO���R�s�[
	//void copy( const WeaponStatus & );

private:
	//disallow assign
	//WeaponStatus( const WeaponStatus & );
	//disallow copy
	//void operator=( const WeaponStatus & );

	//���N���X��serialize�֐��𒼐ڌĂяo���Ȃ����ƁI�I
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int ver);
};

}//end namespace GameObjectData

#endif //__GAME_OBJECTDATA_WEAPONDATA_H__
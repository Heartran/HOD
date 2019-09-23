#pragma once
#include<sstream>
#include<assert.h>
#include"fwd.h"

#include<boost/serialization/assume_abstract.hpp>

namespace boost{
	namespace serialization{
		class access;
	}
}

namespace GameObjectData {

//class AdditionalEffect{
//public:
//	virtual ~AdditionalEffect(){}
//	virtual void exec( Status *st )=0;
//};
//static std::map< std::string, AdditionalEffect* > gsAdditionalEffectMap;

//exvs�ƃ{�[�_�[�u���C�N, saga�Q�l
//saga



class IWeaponStatus{
public:
	//IWeaponStatus( StatusSharedPtr status );
	IWeaponStatus()
		:hitbackValue_(0.f), 
		canCollideOnlyOnce_(false)
	{};
	virtual ~IWeaponStatus(){}

	virtual void attack( Status* st )=0;

	virtual void print( std::ostringstream &ss )const{
		assert(false);
	}

	bool canCollideOnlyOnce()const{
		return canCollideOnlyOnce_;
	}

	//�q�b�g�o�b�N����Ƃ��ɗ^����l
	//����Ȋ����Ŏg�p
	//mRigidBody->applyCentralImpulse( dir*mMass*hitbackValue );
	float hitbackValue_;
	bool canCollideOnlyOnce_;


private:
	//disallow assign
	IWeaponStatus( const IWeaponStatus & );
	//disallow copy
	void operator=( const IWeaponStatus & );

	friend boost::serialization::access;
	//���N���X��serialize�֐��𒼐ڌĂяo���Ȃ����ƁI�I
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int ver) {
		using boost::serialization::make_nvp;
		ar & make_nvp("hitbackValue",			hitbackValue_ );
		ar & make_nvp("canCollideOnlyOnce",		canCollideOnlyOnce_ );
	}
};
BOOST_SERIALIZATION_ASSUME_ABSTRACT(IWeaponStatus)

}//end namespace GameObjectData
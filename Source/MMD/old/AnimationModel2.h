#pragma once

#include<memory>
#include<map>
#include<vector>
#include"GameObjectData/Status.h"
#include"AnimationModel.h"
#include"MME/Object/IObject.h"
#include<boost/unordered_map.hpp>

class btCollisionObject;
class btTransform;
class CBone;

namespace MMD{
	namespace AnimationState{
		class IAnimationState;
		typedef std::shared_ptr<IAnimationState> IAnimationStateSharedPtr;
		namespace Hook{
			class IHook;
			typedef std::shared_ptr<IHook> IHookSharedPtr;
		}
	}
}


namespace GameLib {
	namespace PseudoXml {
		class ConstElement;
	}
}

namespace GameObject{
	class Accessory;
}
namespace MMD{
//animationModel�̉��ǃo�[�W����
class AnimationModel2 : public AnimationModel {
public:
	AnimationModel2();
	virtual ~AnimationModel2();
	
	bool isReady();

	//gameLib���g����draw
	void drawUsingGameLib() const;
	
	//mme���g���ꍇ�Adraw����O�ɌĂ�
	//�A�N�Z�T���̈ʒu��Bone�̈ʒu�ɍX�V����
	void preDrawMME()const;

	void update( const btTransform &world, float elapsedTime );

	void loadXML(
		const char *filename,
		btCollisionObject* ignoredObject,//accessory���e�𖳎����邽�߂ɕK�v
		GameObjectData::StatusSharedPtr status//���킪�U���͂����߂邽�߂ɕK�v
	);

	void changeAnimation( const std::string& animName );

	std::shared_ptr<GameObject::Accessory> getAccessory( const std::string &name );

	void addHookToAnim( const std::string &toAnimName, MMD::AnimationState::Hook::IHookSharedPtr hook );


protected:
	//�B�������̂�protected
	//using AnimationModel::update;
	//using AnimationModel::isAnimOver;
	//using AnimationModel::getFrame;
	//using AnimationModel::getBone;
	//using AnimationModel::loadPMDFromDB;
	//using AnimationModel::getCurrentAnimationIndex;

private:
	//disallow assingn
	AnimationModel2( const AnimationModel2& );
	//disallow copy
	void operator=( const AnimationModel2& );

	MMD::AnimationState::IAnimationStateSharedPtr createAnimationState(
		GameObjectData::StatusSharedPtr status,
		const GameLib::PseudoXml::ConstElement &e );
	typedef boost::unordered_map<std::string, MMD::AnimationState::IAnimationStateSharedPtr> AnimationMap;
	AnimationMap mAnimationMap;
	typedef boost::unordered_map< const std::string, std::shared_ptr<GameObject::Accessory> > AccessoryMap;
	AccessoryMap mAccessoryMap;
};

//MMEObject�Ή��o�[�W����
class AnimationModel3 : private AnimationModel2, public MME::Object::IObject {
public:
	~AnimationModel3(){
		//���ꂢ���setCanRemove(true);
	}
	
	//�����ʒu���Z�b�g(RigidBody�̂���).
	//warp�̕������O����?setInitWorldTransform();reset
	void init( const btTransform &world );
	float mHeightOffset;
};

}//end namespace MMD

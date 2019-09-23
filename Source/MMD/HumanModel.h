#pragma once

#include<memory>
#include<map>
#include<vector>
#include"GameObjectData/Status.h"

class btCollisionObject;
class btTransform;

namespace MMD{
	class Bone;

	namespace AnimationState{
		class IAnimationState;
		typedef std::shared_ptr<IAnimationState> IAnimationStateSharedPtr;
		namespace Hook{
			class IHook;
			typedef std::shared_ptr<IHook> IHookSharedPtr;
		}
	}
}

namespace MME{
	namespace Object {
		class Model;
		typedef std::shared_ptr<Model> ModelSharedPtr;
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
//�A�j���[�V�����̌Œ艻(changeStandAnim ��)��xml���[�h�̂���
//changeState��enum�ϐ����g���Ď�������Ă����������킴�킴enum���ׂ�̂��邢
//�A�N�Z�T���������Ɏ�������̂��x�^�[�ȋC������
//�A�N�Z�T�������̂����f�������A�A�j���[�V�����ɐ[���֌W���Ă���̂�
//���O�ύX�@
//class FixedAnimationModelWithAccessories;����HumanModel��

class HumanModel;
typedef std::shared_ptr<HumanModel> HumanModelSharedPtr;

class HumanModel {
public:
	HumanModel();
	HumanModel(
		MME::Object::ModelSharedPtr model,
		float heightOffset,
		MMD::AnimationState::IAnimationStateSharedPtr standAnim,
		MMD::AnimationState::IAnimationStateSharedPtr punchAnim,
		MMD::AnimationState::IAnimationStateSharedPtr runAnim,
		MMD::AnimationState::IAnimationStateSharedPtr stumbleAnim,
		MMD::AnimationState::IAnimationStateSharedPtr lieAnim,
		MMD::AnimationState::IAnimationStateSharedPtr swingAnim0,
		MMD::AnimationState::IAnimationStateSharedPtr swingAnim1,
		MMD::AnimationState::IAnimationStateSharedPtr swingAnim2,
		MMD::AnimationState::IAnimationStateSharedPtr swingAnim3
	);

	virtual ~HumanModel();
	
	bool isReady();

	//�����ʒu���Z�b�g(RigidBody�̂���).
	//warp�̕������O����?setInitWorldTransform();reset
	void init( const btTransform &world );
	
	//gameLib���g����draw
	void draw() const;
	//mme���g���ꍇ�Adraw����O�ɌĂ�
	//�A�N�Z�T���̈ʒu��Bone�̈ʒu�ɍX�V����
	void preDrawMME()const;

	void update( const btTransform &world, float elapsedTime );
	bool isAnimOver() const;
	
	float getFrame() const;

	Bone* getBone( const char* boneName );

	void loadXML(
		const char *filename,
		btCollisionObject* ignoredObject,//accessory���e�𖳎����邽�߂ɕK�v
		GameObjectData::StatusSharedPtr status//���킪�U���͂����߂邽�߂ɕK�v
	);


	// enum AnimationType{
	//	Run=0,	
	//	Punch,......
	//};
	// void changeAnim( AnimationType );
	// void changeAnim( const std::string & );
	void changeStandAnim();
	void changeRunAnim();
	void changePunchAnim();
	void changeStumbleAnim();
	void changeLieAnim();
	void changeSwingAnim();

	std::shared_ptr<GameObject::Accessory> getAccessory( const char* name );

	int getNumSwingAnims()const{
		return mSwingAnims.size();
	}

	void addHookToStandAnim( MMD::AnimationState::Hook::IHookSharedPtr hook );
	void addHookToRunAnim( MMD::AnimationState::Hook::IHookSharedPtr hook );
	void addHookToPunchAnim( MMD::AnimationState::Hook::IHookSharedPtr hook );
	void addHookToStumbleAnim( MMD::AnimationState::Hook::IHookSharedPtr hook );
	void addHookToLieAnim( MMD::AnimationState::Hook::IHookSharedPtr hook );
	void addHookToSwingAnim( int index, MMD::AnimationState::Hook::IHookSharedPtr hook );

private:
	//disallow assingn
	HumanModel( const HumanModel& );
	//disallow copy
	void operator=( const HumanModel& );

	MMD::AnimationState::IAnimationStateSharedPtr createAnimationState(
		GameObjectData::StatusSharedPtr status,
		const GameLib::PseudoXml::ConstElement &e );

	MME::Object::ModelSharedPtr mModel;

	MMD::AnimationState::IAnimationStateSharedPtr mStandAnim;
	MMD::AnimationState::IAnimationStateSharedPtr mPunchAnim;
	MMD::AnimationState::IAnimationStateSharedPtr mRunAnim;
	MMD::AnimationState::IAnimationStateSharedPtr mStumbleAnim;
	MMD::AnimationState::IAnimationStateSharedPtr mLieAnim;
	std::vector< MMD::AnimationState::IAnimationStateSharedPtr > mSwingAnims;

	//std::map< std::string, MMD::AnimationState::IAnimationState* >

	typedef std::map< const std::string, std::shared_ptr<GameObject::Accessory> > AccessoryMap;
	AccessoryMap mAccessoryMap;

	float mHeightOffset;

};

}//end namespace MMD

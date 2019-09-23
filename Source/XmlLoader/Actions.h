#pragma once
#include<string>
#include<memory>
namespace boost{ namespace serialization{
	class access;
} } //boost::serialization

namespace GameLib{ namespace FileIO {
		class InFile;
} } //GameLib::FileIO

class btCollisionObject;
class btTransform;

namespace GameObjectData {
	class Status;
	typedef std::shared_ptr<Status> StatusSharedPtr;
}

namespace MME { namespace Object { 
	class ModelKai;
} }

namespace XmlLoader{ 
namespace MMD { namespace AnimationState {
	class IAnimationState;
	namespace Hook {
		class AccessoryHook;
	}
}} //XmlLoader::MMD::AnimationState

namespace GameObject{
	class Accessory;
}//XmlLoader::GameObject

class Actions{
public:
	Actions();
	~Actions();
	bool isReady();

	bool loadStart( const std::string &xmlFileName );
	void instance( 
		::MME::Object::ModelKai *model,
		btCollisionObject* owner,//accessory���e�̓�����𖳎����邽�߂ɕK�v
		::GameObjectData::StatusSharedPtr status//���킪�U���͂����߂邽�߂ɕK�v
	);
private:
	bool areAnimationsReady();
	bool areAccessoriesReady();

	friend boost::serialization::access;	
	
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
	template<class Archive>
	void load(Archive& ar, const unsigned int version);
	template<class Archive>
	void save(Archive& ar, const unsigned int version)const;

	std::string mName;
	bool mIsXmlFileFinished;
	GameLib::FileIO::InFile *mFile;


	int mNumAccessories;
	GameObject::Accessory *mAccessories;

	int mNumAnimations;
	MMD::AnimationState::IAnimationState *mAnimations;

	int mNumAccessoryHooks;
	MMD::AnimationState::Hook::AccessoryHook *mAccessoryHooks;

};
}//end namespace XmlLoader
#pragma once
#include<memory>
#include<vector>
#include<unordered_map>

#include"Sequence/HatuneOfTheDead/LuaImpl/Child.h"
#include"XmlLoader/GameObject/Player.h"

class btTransform;


namespace HOD {
	class EnemyManager;
	class  EnemySpawner;
}

namespace MMD {
	class CameraAnimation;
}

namespace XmlLoader {
	namespace GameObject {
		class Player;
		typedef std::unique_ptr<Player> PlayerUniquePtr;
		class Enemy;
		typedef std::shared_ptr<Enemy> EnemySharedPtr;
	}
}

namespace Camera {
	class ICamera;
	typedef std::shared_ptr<ICamera> ICameraSharedPtr;
}

namespace GameObject {
	class Player;
	typedef std::shared_ptr<Player> PlayerSharedPtr;
	class Enemy;
	typedef std::shared_ptr<Enemy> EnemySharedPtr;
	class Stage;
	typedef std::shared_ptr<Stage> StageSharedPtr;
}



namespace Sequence { namespace HatuneOfTheDead { namespace LuaImpl {

class Resource {
public:
	static Resource* instance();
	void release();

	bool isReady();

	void loadEnemy(const std::string &filename);
	void loadPlayer(const std::string &filename);
	void loadCameraAnimation(const std::string &filename);
	void loadStage(const std::string &filename);


	//get�n�̊֐��͏��L�����ړ�������ׂ������������ʂɏ��L�����c���Ă����Ă����قǍ���Ȃ��Ǝv������
	//�ă��[�h�̂Ƃ���������g�������ȋC�������̂ł��̂܂܂ɂ���


	GameObject::PlayerSharedPtr instancePlayer()const;
	
	GameObject::StageSharedPtr getStage()const;

	const std::unordered_map<std::string, XmlLoader::GameObject::EnemySharedPtr> &
		getEnemies();

	const Camera::ICameraSharedPtr& getCameraAnimation();
private:
	Resource();
	virtual ~Resource();

	struct Deleter
	{
		void operator()(Resource const* const p) const
		{
			delete p;
		}
	};
	typedef std::unique_ptr<Resource, Deleter > UptrState;
	static UptrState mThis;


	Resource(const Resource &) = delete;
	void operator=(const Resource &) = delete;


	XmlLoader::GameObject::PlayerUniquePtr mTemplatePlayer;
	std::string mTemplatePlayerFileName;

	GameObject::StageSharedPtr	mStage;
	std::string mStageFileName;

	//�J�����؂�ւ������Ȃ肻���A�Ƃ肠�����̎�����
	Camera::ICameraSharedPtr mCameraAnimation;
	std::string mCameraAnimationFileName;

	std::unordered_map<std::string, XmlLoader::GameObject::EnemySharedPtr> mTemplateEnemies;

	bool mEnableForceLoad;

};

}}} //namespace Sequence::HatuneOfTheDead::LuaImpl

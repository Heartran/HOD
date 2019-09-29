#include"Resource.h"

#include<GameLib/DebugScreen.h>
#include<GameLib/GameLib.h>

#include"Camera/HodCamera.h"

#include"GameObject/Stage.h"
#include"GameObject/Player.h"
#include"GameObject/Enemy.h"

#include"GUI/Manager.h"

#include"MME/Manager.h"
#include"MME/Drawer/GameDrawer.h"
#include"MME/Object/PointLight.h"
#include"MME/PostEffect.h"

#include"MMD/CameraAnimation.h"

#include"Lua/LuaHelper.h"

#include"XmlLoader/GameObject/Player.h"
#include"XmlLoader/GameObject/Enemy.h"



namespace Sequence { namespace HatuneOfTheDead { namespace LuaImpl {


Resource::UptrState Resource::mThis = nullptr;

Resource* Resource::instance() {

	if (!mThis) {
		mThis.reset(NEW Resource);
	}

	return mThis.get();
}

// ���
void Resource::release()
{
	mThis.reset();
}

Resource::Resource()
	:mEnableForceLoad(false)
{
}
Resource::~Resource() {
	while (mTemplatePlayer && !mTemplatePlayer->isReady()) {}
	while (mStage && !mStage->isReady()) {}
	auto hodcamera = std::dynamic_pointer_cast< Camera::HodCamera, Camera::ICamera>(mCameraAnimation);

	while (hodcamera && !hodcamera->isReady()) {}
	mTemplatePlayer.reset();
	mStage.reset();
	mCameraAnimation.reset();

}

bool Resource::isReady()
{
	
	assert(mTemplatePlayer &&
		mStage &&
		mCameraAnimation &&
		"3�Ƃ����[�h�����Ȃ��Ɖ��̏������������Ȃ�B�Ⴄ�@�\�K�v�Ȃ�l������");
	bool result = mTemplatePlayer->isReady();
	
	for (auto& te : mTemplateEnemies) {
		result = result && te.second->isReady();
	}
	result = result && mStage->isReady();
	auto hodcamera =  std::dynamic_pointer_cast< Camera::HodCamera, Camera::ICamera>(mCameraAnimation);
	assert(hodcamera);
	result = result && hodcamera && hodcamera->isReady();

	return result;
}

void Resource::loadEnemy(const std::string &filename) {
	
	auto it = mTemplateEnemies.find(filename);
	if (it == mTemplateEnemies.end() ) {
		auto enemy = XmlLoader::GameObject::Enemy::createSP();
		enemy->loadXml(filename);

		mTemplateEnemies.insert( std::make_pair(filename, enemy) );
	}
	else if (mEnableForceLoad) {
		auto enemy = XmlLoader::GameObject::Enemy::createSP();
		enemy->loadXml(filename);
		it->second = enemy;
	}
	
}
void Resource::loadPlayer(const std::string &filename) {
	
	
	if ( !mTemplatePlayer || //�������[�h���ĂȂ��ꍇ
		(mTemplatePlayer && (mTemplatePlayerFileName !=filename)) ||//���[�h���Ă��邪���O���قȂ�ꍇ
		mEnableForceLoad )//�������[�h��true
	{
		mTemplatePlayerFileName = filename;
		mTemplatePlayer.reset(NEW XmlLoader::GameObject::Player());
		mTemplatePlayer->loadXml(filename);
	}
}
void Resource::loadCameraAnimation(const std::string &filename) {
	if (!mCameraAnimation || //�������[�h���ĂȂ��ꍇ
		(mCameraAnimation && (mCameraAnimationFileName != filename)) ||//���[�h���Ă��邪���O���قȂ�ꍇ
		mEnableForceLoad)//�������[�h��true
	{
		mCameraAnimationFileName = filename;
		auto camera = std::make_shared<MMD::CameraAnimation>();
		camera->load(filename);
		mCameraAnimation.reset( NEW Camera::HodCamera(camera) );
	}

}
void Resource::loadStage(const std::string &filename)
{
	if (!mStage || //�������[�h���ĂȂ��ꍇ
		(mStage && (mStageFileName != filename)) ||//���[�h���Ă��邪���O���قȂ�ꍇ
		mEnableForceLoad)//�������[�h��true
	{
		mStageFileName = filename;
		mStage = GameObject::Stage::create(filename.c_str());
	}
}


GameObject::PlayerSharedPtr Resource::instancePlayer() const
{
	return mTemplatePlayer->instance();
}

GameObject::StageSharedPtr Resource::getStage() const
{
	return mStage;
}

const std::unordered_map<std::string, XmlLoader::GameObject::EnemySharedPtr>& 
Resource::getEnemies()
{
	return mTemplateEnemies;
}


const Camera::ICameraSharedPtr& Resource::getCameraAnimation()
{
	return mCameraAnimation;
}
} } } //namespace Sequence::HatuneOfTheDead::LuaImpl

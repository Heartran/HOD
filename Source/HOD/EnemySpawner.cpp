#include"EnemySpawner.h"

#include<algorithm>
#include<assert.h>
#include<btBulletCollisionCommon.h>

#include"GameObject/Enemy.h"
#include"helper/MyOutputDebugString.h"
#include"XmlLoader/GameObject/Enemy.h"


namespace HOD {
	
namespace {

	std::vector<XmlLoader::GameObject::EnemySharedPtr>::iterator 
		find(
			std::vector<XmlLoader::GameObject::EnemySharedPtr> &refSource,
			const std::string &findName)
	{


		auto isName = [findName](const XmlLoader::GameObject::EnemySharedPtr &enemy) -> bool
		{
			return enemy->getName() == findName;

		};

		auto itTemplateEnemy = std::find_if(refSource.begin(), refSource.end(), isName);
		return itTemplateEnemy;
	}
}//end nonamespace



struct EnemySpawner::Request {
	float mSpawnTime;
	std::string mName;
	btTransform mTrans;
	GameObject::EnemySharedPtr mEnemy;

	bool operator<(const Request & right)const {
		return mSpawnTime < right.mSpawnTime;
	}

	bool operator<(const float & time)const {
		return mSpawnTime < time;
	}
};

EnemySpawner::EnemySpawner()
	:mTime(0.f)
{
}

EnemySpawner::~EnemySpawner() {}

void EnemySpawner::update(float elapsedTime) {

	//�܂��킴�킴���`�T�����Ȃ��Ă���ԍŏ��̔z��̗v�f�̎��Ԃƌ��ݎ���+elapsedTime���ׂď�������ΓG���o�������č폜����ł����񂾂���
	//�����߂��Ƃ�������C�Ȃ����킴�킴2���T��������K�v���������Ȃ����A�����܂ŕ��ׂłȂ��̂ł�����
	//�r��������s���ăQ�[���f�U�C���C���������Ƃ����̂����邩������
	auto it = std::lower_bound(mRequests.begin(), mRequests.end(), mTime);
	auto itStart = it;
	while (it != mRequests.end() && it->mSpawnTime < mTime + elapsedTime) {
		auto e = it->mEnemy;
		
		if (e->isInWorld()) {
			//�����[�Əo�������������Ȃ�
			assert(false);
		}
		else {
			e->getWorldTransform() = it->mTrans;
			e->addToWorld();
		}
		++it;

	}
	//�{���͏��Ԃ��t�ɂ��Č�납��폜����悤�ɂ���΍폜������l�߂��Ƃ�����Ȃ��̂Ō����I�����������Ă��܂��������̂܂܂�
	//�ꉞ�����R�[�h��sorted_reverese_vector.cpp�Ƃ��ď�����
	//
	//�������r������n�܂�Ƃ��Ȃ��Ǝv��(�f�o�b�O���A�r���X�^�[�g�͏\�����蓾�邩)����itStart��mRequests.begin()�ł����Ǝv������
	//
	//�����Ďg��Ȃ��ł���A�킴�킴�o�����������ݎ������Â��̂��g����ʂ��v�����Ȃ����ǁc
	//�܂��f�o�b�O�Ŏg���������炢��
	mRequests.erase(itStart, it);

	mTime += elapsedTime;
}

GameObject::EnemySharedPtr EnemySpawner::reserve(const std::string& enemyName, float spawnTime, const btTransform& trans) {
	
	GameObject::EnemySharedPtr outEnemy;
	
	auto itTemplateEnemy = find(mTempateEnemies, enemyName);
	
	if (itTemplateEnemy == mTempateEnemies.end()) {
		assert(false && "���O��������܂���");
	}
	else {
		outEnemy = itTemplateEnemy->get()->instance();
		Request req{
			spawnTime,
			enemyName,
			trans,
			outEnemy
		};

		std::vector<Request>::iterator it = std::lower_bound(mRequests.begin(), mRequests.end(), req  );
		mRequests.insert(it, std::move(req));

	}
	
	return outEnemy;
}

void HOD::EnemySpawner::registerEnemy(XmlLoader::GameObject::EnemySharedPtr enemy)
{
	//���[�h�������̂�o�^��������
	if (!enemy->isReady()) {
		TRACE("enemy is not ready! it will force to be ready at this frame.");
		while (!enemy->isReady()) {}
	}

	auto it = find(mTempateEnemies, enemy->getName() );
	if (it == mTempateEnemies.end())
	{
		mTempateEnemies.push_back(enemy);
	}
	else {
		assert(false && "same name is registered!");
	}
}

void HOD::EnemySpawner::loadEnemy(const std::string & filename)
{
	auto enemy = XmlLoader::GameObject::Enemy::createSP();
	enemy->loadXml(filename);
	
	mLoadingTempateEnemies.push_back(enemy);
}


bool HOD::EnemySpawner::isReady()
{
	if (mLoadingTempateEnemies.empty()) {
		return true;
	}

	//std::vector< XmlLoader::GameObject::EnemySharedPtr > finished;
	auto isReady = [this](const XmlLoader::GameObject::EnemySharedPtr &enemy)->bool
	{
		bool result = enemy->isReady();
		
		//Loading�����犮�������Ɉڂ��A�Q�ƃJ�E���g�𑝂₷
		//�폜�̓��[�v���I�������ł��
		//�Ƃ��������K�v�Ȃ��Ȃ��������̃R���e�i�l�߂��Ƃ�remove_if�ɂ���Ă��炤
		if (result) {
			mTempateEnemies.push_back(enemy);
			//finished.push_back(enemy);
			//�����o�ϐ��̃L���v�`�����Ȃ񂩂��܂������Ȃ��̂łƂ肠���������ǂ��܂���������
			//�R�����g�A�E�g���Ă����Ǝv���܂����Ƃ��Ă����܂���
		}
		return result;
	};


	auto itNewEnd = std::remove_if(
		mLoadingTempateEnemies.begin(),
		mLoadingTempateEnemies.end(),
		isReady );

	mLoadingTempateEnemies.erase(
		itNewEnd,
		mLoadingTempateEnemies.end());

	

	return false;
}

}

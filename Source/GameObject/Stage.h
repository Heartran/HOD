#ifndef __GAMEOBJECT_STAGE_STAGE_H__
#define __GAMEOBJECT_STAGE_STAGE_H__

#include<vector>
#include<GameLib/Math/Vector3.h>
#include"GameObject/SharedPtrs.h"
#include"GameObject/BaseObject.h"
#include<memory>


namespace MME {
	namespace Object{
		class Accessory;
		typedef std::shared_ptr<Accessory> AccessorySharedPtr;

	}
}

namespace GameLib{
	namespace Math{
		class Vector3;
		class Matrix44;
	}
}
class btRigidBody;

namespace GameObject {
class Stage;
typedef std::shared_ptr<Stage> StageSharedPtr;

//class Terrain�ɕ��������ق����������H
//���O��sequence�Ƃ��Ă�stage�Ȃ̂�
//gameobject�Ƃ��Ă�stage�Ȃ̂��킩��ɂ���
//�N���A�֘A�C�x���g�������Ɏ������Ă��邯�ǁA�����ŊǗ�������̂Ȃ̂�
//�Q�[�����[���N���X�Ƃ��ق����̂�
//
//��̃R�����g���邯��
//��������͉����X�N���v�g��GameObject����肽���̂�
//C++��GameObject����肽�����ŕς���Ă���
//
//C++��GameObject����肽���ꍇ�͂܂����̂܂܂ł����Ȃ����낤
//�X�N���v�g���ō�肽���Ǝv���Ȃ班�Ȃ��Ƃ���L�̂��̂�Lua�Ŏ����ł���悤��
//���Ȃ���΂Ȃ�Ȃ����낤
//
//Lua�̓��x���G�f�B�^�ŃQ�[���S�̂̐U�镑�������C�͂Ȃ��Ƃ����̂����݂̍l���ŕς��Ȃ��̂Ȃ�
//���f����RigidBody�����ł���


class Stage : public GameObject::BaseObject {
public:
	static StageSharedPtr create(const char * xmlInitStageFile );
	virtual ~Stage();
	void drawUsingGameLib();

	void createBvhTriangleMesh();
	
	RTTI_DECL;
	
	virtual btCollisionObject* getBtCollisionObject(){
		return NULL;
	}

	virtual void removeFromWorld();
	virtual void addToWorld();
	
	bool isClear()const {
		return mIsClear;
	}
	void handleEventThatDefeatedAllEnemies(){
		mIsClear = true;
	}

	bool isReady();

	virtual int calculateDamage()override;

	virtual bool canDamage()override;

	virtual const GameObjectData::WeaponStatus* getWeaponStatus()override;


private:
	//disallow raw pointer. use create()
	Stage( const char * xmlInitStageFile );
	//disallow assign
	Stage( const Stage & )=delete;
	//disallow copy
	void operator=( const Stage & )=delete;

	//XFile::Model				*mModel;
	MME::Object::AccessorySharedPtr		mModel;
	std::vector<btRigidBody*>	mRigidBodies;

	std::vector<EnemySharedPtr> mEnemies;

	bool mIsClear;
	bool mIsReady;
};


}//end namespace GameObject
#endif
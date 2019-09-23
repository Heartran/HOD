#pragma once
#include<memory>
#include"RTTI/RTTI.h"
#include<LinearMath/btTransform.h>
#include"GameObjectData/WeaponStatus.h"



namespace GameObject {
typedef unsigned int GameObjectId;

class IGameObject : public std::enable_shared_from_this<IGameObject>
{
public:
	//shared_ptr��Ԃ� create()����邱��
	//friend PlayerSharedPtr createPlayer();��錾���Ă�����
	//createPlayer()�ɂ���ׂ����H�O���[�o���֐��^
	//Player::create()�ɂ���ׂ����H
	//
	//��̂ق����t�@�C���ˑ������Ȃ�����ォ�ȁBPlayer��m��Ȃ��Ă�����
	//���̓N���X���ɓ����Ă��邩��APlayer��m��Ȃ��Ă͂Ȃ�Ȃ��B
	
	//���ϖ߂�l��Ԃ��ÓI���z�֐������悤�ɋ������������A
	//�X�}�[�g�|�C���^���g���Ă�����薳��
	//static  virtual IColliderSharedPtr create( IArgs* args )=0;

	virtual ~IGameObject() {}

	static IGameObject* cast(void *vp){
		return static_cast<IGameObject*>( vp );
	}

	GameObjectId getID() const{
		return mId;
	}
	
	RTTI_DECL;

	virtual const btTransform & getWorldTransform() const= 0;

	virtual btTransform & getWorldTransform()=0;
	void setWeaponStatus( GameObjectData::WeaponStatusSharedPtr weaponStatus )
	{
		mWeaponStatus = weaponStatus;
	}

	GameObjectData::WeaponStatusSharedPtr getWeaponStatus()const{
		return mWeaponStatus;
	}

	GameObjectData::StatusSharedPtr	getStatus()const{
		return mStatus;
	}

	void copyWeaponStatus( const GameObjectData::WeaponStatusSharedPtr &src );
	void copyWeaponStatus( const GameObjectData::WeaponStatus &src );

	//��񂾂������Փ˂��Ȃ��ꍇ
	bool canCollideOnlyOnce();
	
	void attack( GameObjectData::Status* );

protected:
	//shared_from_this���g������protected
	//�K���Ashared_ptr���g���Ashared_ptr�ŊǗ����Ȃ���΂Ȃ�Ȃ�
	//�I�u�W�F�N�g���쐬����̂�create();
	IGameObject();

	static unsigned int getGameObjectsNum(){ return sNum; }
	
	GameObjectData::StatusSharedPtr		mStatus;	
	GameObjectData::WeaponStatusSharedPtr	mWeaponStatus;

private:
	//disallow
	IGameObject( const IGameObject& )=delete;
	void operator=( const IGameObject& )=delete;

	static unsigned int sNum;
	const GameObjectId mId;
};
}//end namespace
#pragma once
class btPersistentManifold;
class btCollisionObject;
#include"RTTI/RTTI.h"
#include"Component/RigidBodyComponent.h"
#include"GameObjectData/WeaponStatus.h"
#include<memory>



namespace GameObjectData {
	class WeaponStatus;
}

namespace GameObject {
	class BaseObject;
	typedef std::weak_ptr<BaseObject> BaseObjectWeakPtr;
	typedef std::shared_ptr<BaseObject> BaseObjectSharedPtr;
	

	//BaseActor
	//BaseGameObject
	//
	class BaseObject : public std::enable_shared_from_this<BaseObject> {
	public:
		BaseObject()=default;
		virtual ~BaseObject()=default;
	
		virtual const RTTI & getRTTI() const = 0
		{
			return rtti;
		}
		static const RTTI rtti;


		//�_���[�W�֘A�͂ǂ��������悤���B�������B
		//�������Ԃ������Ă��邱�ƁA�V�X�e�����V���v���ɂ���������
		//�p���ŃC���^�[�t�F�C�X�����K�w�I�ȍ\���ŃV���v���Ɏ������邱�Ƃɂ���
		//�ق��̈ĂƂ��Ă�
		//�R���|�[�l���g�ɂ���
		//�f���Q�[�^�[���g��
		//�C�x���g�쓮�ɂ���Ƃ�����ue�I�Ȑ݌v���v���O���~���O���Ă݂����������Ƃ肠����
		//�_���[�W���v�Z���Ȃ��I�u�W�F�N�g�����邵�Q�[���G���W���I�ɍl����Ε����������ق����������̂��낤��
		//HOD�����g���C�Ȃ������̃G���W���͂��낢�돑�����������A�Đ݌v���Ȃ��������Ƃ��������B
		//HOD��p�̃Q�[���I�u�W�F�N�g���Ǝv���Ζ��Ȃ��ł��傤
		//�ӎv������R���s���[�^�ɔC����̂ł͂Ȃ��O�����čs���B
		
		//�����Ń_���[�W�v�Z���s���A�q�b�g�o�b�N�������͂܂��ʂ̏��ōs��
		//�o�[�`�����ɂ���K�v�Ȃ�
		//����BaseObject�̏�ԂŌĂԋC�Ȃ�
		//����BaseObject�ɂ��Ă��邯��WeaponStatus�����
		//virtual void takeDamage(
		//	BaseObject* DamageCauser
		//)=0;

		virtual int calculateDamage()=0;

		//�_���[�W��^����I�u�W�F�N�g���ǂ����H
		virtual bool canDamage() = 0;

		//�_���[�W�����߂�̂Ɏg�p
		//�_���[�W��^����I�u�W�F�N�g�łȂ����null��Ԃ����Ƃ�����
		virtual const GameObjectData::WeaponStatus* getWeaponStatus()=0;

		unsigned short mDamageGroup;
		unsigned short mDamageMask;		
	};


}

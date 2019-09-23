#pragma once
#include<vector>
#include<memory>
#include"MME/Object/IObject.h"
#include<boost/shared_array.hpp>

namespace MME{
class BaseEffect;

namespace Object{
	class IObject;
	typedef std::shared_ptr<IObject> IObjectSharedPtr;
	typedef std::weak_ptr<IObject> IObjectWeakPtr;
}// Object

namespace Batch {
class OffScreenBatch;


//DrawObjects;
//DrawObjectList;
//DrawableObjects;
//DrawContainer
//���L���X�g�ɂ����̂�zbuffer�Ń\�[�g�����Ƃ��Ƀ��X�g��(effectbatch��)�Ƀ\�[�g����̂�����������������
//�������o�����߂�z�\�[�g���K�v�̂Ƃ��̂���
//�R���e�i�̎�ނ�vector, list�������Ǝv��
//�}���폜���l�����list�����A
//��ł܂Ƃ߂č폜�������������Ă���{���ɓ����Ă���̂��|�C���^�Ȃ̂�
//�\�[�g����Ƃ�
typedef std::vector<Object::IObjectWeakPtr> DrawObjects;
typedef std::shared_ptr< DrawObjects > DrawObjectsSharedPtr;

class BaseBatch {
public:
	BaseBatch(BaseEffect*);
	virtual ~BaseBatch();
	void add(const Object::IObjectSharedPtr &obj);
	void setObjectsPtr(const DrawObjectsSharedPtr &);
	void drawOffScreens();
	const std::string & getName()const;
protected:
	BaseEffect* mBaseEffect;
	DrawObjectsSharedPtr mObjects;
	std::vector<OffScreenBatch*> mOffScreens;
};
} } //MME::Batch

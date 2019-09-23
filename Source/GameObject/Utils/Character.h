#pragma once
#include<string>
#include<memory>


class btCollisionShape;
typedef std::shared_ptr<btCollisionShape> BtCollisionShapeSharedPtr;

namespace MME {
	namespace Object {
		class ModelKai;
		typedef std::shared_ptr<ModelKai> ModelKaiSharedPtr;
	}
}

namespace GameObject {namespace Utils {
	//�e�L�����ɑΉ������l�X�Ȑݒ��ێ�����N���X
	//���f���Ƃ����蔻��n�̏����܂Ƃ߂�������
	//�l�ԂłȂ��ꍇ�͂Ƃ��A�ق��ɒǉ��p�����[�^���������ꍇ�����肻��
	//ModelTimeLineController��Player(Enemy�AGameObject�j�̋��ʕ�����ێ����邽�߂̃N���X
	//GameObject���p�����ĂȂ�����Utils�l�[���X�y�[�X�ɓ��ꂽ����

	//��ԏ��߂̍l���ł̓��f���Ƃ����蔻��̓y�A�Ŏ��ׂ��Ǝv���Ă�������
	//�ʁX�̂ق�������������
	//�~�N�Ƃ����L�����N�^�[�͓��������蔻��������d����`���ꏏ�݂����ȍl���������������炾��
	//�C�x���g���l�����ăv���O�����̍ė��p�����₷���悤�ɍl���Ă��Ǝv��
	//�R���|�[�l���g�Ƃ��čl���n�߂��ꍇ�����͕ʁX�ɂ���GameObject�̒��őg�ݍ��킹��ׂ��ł͂Ȃ����Ɗ����Ă���

	class Character {
	public:
		Character(
			const std::string &name,
			float height,
			float mass,
			BtCollisionShapeSharedPtr shape,
			MME::Object::ModelKaiSharedPtr model
		);
		~Character();
		
		const std::string& getName()const {
			return mName;
		}

		float getHeight() const {
			return mHeight;
		}


		BtCollisionShapeSharedPtr getShape() const {
			return mShape;
		}

		MME::Object::ModelKaiSharedPtr getModel() const {
			return mModel;
		}
		
		float getMass() const {
			return mMass;
		}

	private:
		Character(const Character &) = delete;
		void operator=(const Character &) = delete;

		std::string mName;
		float mHeight;
		float mMass;
		BtCollisionShapeSharedPtr mShape;
		MME::Object::ModelKaiSharedPtr mModel;
	
	};

} }//end GameObject::Utils
#pragma once
#include<memory>
#include<string>

namespace MME {
	namespace Object {
		class ModelKai;
		typedef std::shared_ptr<ModelKai> ModelKaiSharedPtr;
	}
}

namespace MMD {
	namespace AnimationState {
	}
}


namespace XmlLoader {
	namespace MMD {
		namespace AnimationState {

			class IAnimationState;
			typedef std::shared_ptr<IAnimationState> IAnimationStateSharedPtr;
		}
	}
}

namespace Lua {

//�����蔻��Ȃ����f��
//���A�Ŏp���𐧌䂵�\�����������Ƃ��Ɏg��
//�ꏊ�Ɖ�](�p��)���A�j���[�V�������������ꍇ�g�����Ƃ�z��
//�ړ��Ȃ��A�j���[�V�������ړ�������Ƃ��Ɏg�p
//�Q�[���ł͈ړ��ʂ��v���O����������R���g���[�����������߈ړ��𔺂�Ȃ��A�j���[�V�������g�����Ƃ�����
//������J�b�g�V�[���ōė��p�A���邽�߂̃N���X
//���O���
//ModelRootController, TimeLine RootTransformController
//self.miku = Lua.ModelRootTransformTimeLineControllerModel(
//ModelTimeLineController�Ɍ���
//���R �R���g���[�����������̂�rootTransform�ƃA�j���[�V����������
//
//�Ƃ����������ꏊ(���O���)GameObject::Utils������̂ق��������̂ł�


class ModelTimeLineController {
public:
	ModelTimeLineController(const MME::Object::ModelKaiSharedPtr &model);
	~ModelTimeLineController();

	void insert(
		float time,
		float x, float y, float z,
		float degreeX, float degreeY, float degreeZ,
		const std::string &animName );

	void update(float elapsedTime);

	//�����ʒu���Z�b�g(RigidBody�̂���).
	void init();

	bool isAnimOver() const;

	MME::Object::ModelKaiSharedPtr getModel();

private:
	class Impl;
	Impl *mImpl;
};

}//end namespaceLua
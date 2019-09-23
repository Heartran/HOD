#pragma once
#include<memory>


namespace GameObject {
	class BaseObject;
}

namespace Component {
	class BaseComponent;
	typedef std::shared_ptr<BaseComponent> BaseComponentSharedPtr;
	typedef std::weak_ptr<BaseComponent> BaseComponentWeakPtr;

	//class BaseComponent : public std::enable_shared_from_this<BaseComponent> {
	class BaseComponent {
	public:
		virtual ~BaseComponent() = default;


		//getRTTI();//����Ȃ�����
		//���̊֐���GameObject����

		//BaseComponent* nextSibling();
		//����Ȃ��ł��傱���ŕ����R���|�[�l���g��g�ݍ��킹�ė��p�̂�GameObject����
		//���邮��񂷂̂������Ŏ�������K�v�͂Ȃ�

		GameObject::BaseObject* getOwner() {
			return mOwner;
		}
	//protected:
		BaseComponent()
			:mOwner(nullptr)
		{}
		GameObject::BaseObject* mOwner;


	};
}
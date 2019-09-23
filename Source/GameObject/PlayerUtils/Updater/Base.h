#pragma once

namespace MME{ namespace Object{
	class ModelKai;
} }

namespace Input{
class PlayerMouse;
}

namespace GameObjectData{
	class Status;
}

namespace GameObject{ 
	class Player;
namespace PlayerUtils { namespace Updater {

//���̏�ԑJ�ڂ̐݌v��
//1��Ԃɂ�1�̃A�j���[^�V�����ɂ����ق���
//�R�[�h��������₷��

class Base{
public:
	virtual ~Base(){} //�Ȃɂ����Ȃ��̂ł���ł���
	virtual Base* update( Base*, float dt ) = 0;
};

} } } //GameObject::PlayerUtils::Updater


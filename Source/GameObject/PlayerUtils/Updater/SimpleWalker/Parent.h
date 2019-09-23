#pragma once
#include<memory>
#include"MME/Object/ModelKai.h"
#include"Input/PlayerMouse.h"
#include"../Child.h"
class btVector3;
class btRigidBody;

/*
move, run, walk
point
target

move ���Ɩ��@�������邩
straf�Ƃ����֐��ɑ΂���walk�̂ق����}�b�`���Ă���C������
mover->move( pos );
mover->walk( pos );

namespace Car{
	class Mover;
}
namespace Point{
	class Mover;
}

namespace Target{
	class Mover;
}

�E�N���b�N�̓���ƍ��N���b�N�̓����
����ύX�ǂ�����


class CarMover;
class PoinMover;

if(isAnimOver()){
	changeAnim();
	next=new Stand()
}


class Shooter : public CarMover{};

���[��
Shooter::Parent
Shooter::Stand

Jumper::Stand
Jumper::Jump
Jumper::Walk

namespace ���했 ��stand, run, swing
updater�łȂɂ������́H

if( mouse.on(left) ){
	walkIf( target );
}

*/
namespace GameObjectData{
	class Status;
}

namespace GameObject{
class Player;

namespace PlayerUtils {namespace Updater { namespace SimpleWalker {

namespace Root = GameObject::PlayerUtils::Updater;
class Child;
typedef Root::Parent GrandParent;



class Parent : public Root::Child {
public:
	Parent(Player*);
	~Parent();

	virtual Root::Base* update( GrandParent*, float dt);
//private:
	btRigidBody *mRigidBody;
	GameObjectData::Status *mStatus;
	MME::Object::ModelKai *mModel;
	Input::PlayerMouse *mMouse;
	Player* mPlayer;
	Root::SimpleWalker::Child* mChild;	
	//Camera::PlayerCamera *mCamera;shared_ptr
	//Input::PlayerMouse *mMouse;shared_ptr
};

} } } } //GameObject::PlayerUtils::Updater::SimpleWalker



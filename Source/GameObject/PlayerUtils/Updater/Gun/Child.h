#pragma once
#include"../Base.h"

namespace GameObject{ namespace PlayerUtils { namespace Updater { namespace Gun {
class Parent;

class Child : public Base{
public:
	virtual ~Child();
	Base* update( Base*, float dt ); //これは実体を作ってやる
	virtual Base* update( Parent* , float dt ) = 0;
};

} } } } //GameObject::PlayerUtils::Updater::Gun


#pragma once
#include"Child.h"

namespace GameObject { namespace PlayerUtils{ namespace Updater { namespace Gun {

class Stand : public Child  {
public:
	Stand( Parent* );
	~Stand();

	Base* update( Parent*, float elapsedTime );
private:
	MME::Object::ModelKai *mModel;
	Input::PlayerMouse *mMouse;
};

} } } }//GameObject::PlayerUtils::Updater::Gun
#pragma once

namespace GameObject{ namespace EnemyUtils { namespace Updater {


class Base{
public:
	virtual ~Base(){} //�Ȃɂ����Ȃ��̂ł���ł���
	virtual Base* update( Base*, float dt ) = 0;
};

} } } //GameObject::EnemyUtils::Updater


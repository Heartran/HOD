#include <GameLib/FileIO/Manager.h>
#include <GameLib/GameLib.h>

#include "Sequence/HatuneOfTheDead/LuaImpl/GameLoop.h"
#include "Sequence/HatuneOfTheDead/LuaImpl/Parent.h"
#include "Sequence/HatuneOfTheDead/LuaImpl/Resource.h"
#include "Sequence/HatuneOfTheDead/LuaImpl/State.h"
#include "Sequence/Parent.h"

#include "helper/Image.h"

#include "MMD/MMDDataBase.h"
#include "MME/Manager.h"
#include "Lua/LuaHelper.h"


namespace Sequence{
namespace HatuneOfTheDead{
namespace LuaImpl{

Parent::Parent()
	:mChild( 0 ),
	mState(0) {
	TRACE("Sequence::HatuneOfTheDead::LuaImpl::Parent()\n");
	//���̃N���X��HatuneOfTheDead��Lua�̐����Ǘ�������
	LuaCreate("HatuneOfTheDead");

	mState =  State::instance();
	Resource::instance();

	mChild = new Sequence::HatuneOfTheDead::LuaImpl::GameLoop();

}

Parent::~Parent(){
	TRACE("Sequence::HatuneOfTheDead::LuaImpl::~Parent()\n");

	LuaDestroy();
	mState->release();
	Resource::instance()->release();

	SAFE_DELETE( mChild );
}

Base* Parent::update( GrandParent* ){
	Base* next = this;
	Base* nextChild = mChild->update( this );
	//�J�ڔ���
	if ( nextChild != mChild ){
		HatuneOfTheDead::LuaImpl::Child* casted = dynamic_cast< HatuneOfTheDead::LuaImpl::Child* >( nextChild );
		if ( casted ){
			SAFE_DELETE( mChild );
			mChild = casted;
		}else{
			next = nextChild;
		}
		casted = 0;
	}
	nextChild = 0; //�O�̂���

	return next;
}

void Parent::updateState() {
	mState->update();
}
void Parent::drawState() {
	mState->draw();
}

bool Parent::isReady() {
	static int c = 0;
	++c;
	if (c < 300) {
		return false;
	}
	return mState->isReady();
}

bool Parent::isClear() const {
	return mState->isClear();
}

void Parent::reloadLua() {
	ReloadLuaFiles("HatuneOfTheDead/all");
}


}}} //namespace Sequence::HatuneOfTheDead::LuaImpl

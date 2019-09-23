#include <assert.h>
#include <GameLib/GameLib.h>
#include <GameLib/Input/Manager.h>
#include <GameLib/Input/Keyboard.h>
#include <GameLib/DebugScreen.h>
#include <GameLib/Framework.h>

#include "GUI/Manager.h"
#include "Sequence/HatuneOfTheDead/LuaImpl/GameLoop.h"
#include "Sequence/HatuneOfTheDead/LuaImpl/Parent.h"
#include "Lua/LuaHelper.h"
#include "State.h"
#include "Resource.h"
#include "helper/MyOutputDebugString.h"


namespace Sequence{
namespace HatuneOfTheDead{
namespace LuaImpl{

GameLoop::GameLoop(){
}

GameLoop::~GameLoop(){
}

//�Q�[���{��
Base* GameLoop::update( Parent* parent ){
	auto state = State::instance();
	state->setNextSequence(this);

	using namespace Lua;
	LuaFuncParam result;
	while (!CallLuaFunc("framefunc", &result, 1)) {
		WaitInputOnError(GetLuaError(), "Reload OK?");
		//assert(false && "���󂱂��Ƃ���C�Ȃ�");//����Ƃ��邱�Ƃ���Ǝv������f
		//�S������
		state->resetBulletPhysics();
		State::instance()->release();
		GUI::Manager::instance()->destory();
		Resource::instance()->release();
		state = State::instance();//�o�O�΍�A�J�������z���̂܂܎g���Ă��܂�����B�V�����̂�����ēo�^
		state->setNextSequence(this);
		parent->reloadLua();
		//HatneOfDead/gameinit.lua�Ŏw�肳�ꂽ��Ԃɂɖ߂�
	}

	if ( state->needDrawCall()) {
		state->draw();
	}


	GameLib::Input::Keyboard k;
	if (k.isTriggered('1')) {
		//���\�[�X�ȊO�͑S�J��
		//���[�h���鎞�ԒZ�����邽��
		TRACE("reload lua in GameLoop.cpp\n");
		GUI::Manager::instance()->destory();

		state->resetBulletPhysics();
		//resource�͊J�����Ȃ�
		state->release();
		parent->reloadLua();
		//���r�[�ɖ߂�
		while (!CallLuaFunc("framefunc", &result, 1)) {
			WaitInputOnError(GetLuaError(), "Reload OK?");
			//assert(false && "���󂱂��Ƃ���C�Ȃ�");//lua�ł̃R���p�C���G���[���x�ł�����ł���
			state = State::instance();
			state->resetBulletPhysics();
			state->release();
			parent->reloadLua();
			//���r�[�ɖ߂�
		}
		state = State::instance();//�o�O�΍�A�J�������z���̂܂܎g���Ă��܂�����B�V�����̂������ēo�^
		state->setNextSequence(this);
	}

	return state->getNextSequence();
}


}}} //namespace Sequence::HatuneOfTheDead::LuaImpl

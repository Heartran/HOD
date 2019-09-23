#include "Fonts.h"
#include <memory>


namespace{
std::unique_ptr< GameLib::Scene::Font > gGothic;
}
namespace GUI{ namespace Fonts{

GameLib::Scene::Font getGothic(){
	if( !gGothic ){
		gGothic.reset( new  GameLib::Scene::Font() );
		*gGothic = GameLib::Scene::Font::create( "�l�r �S�V�b�N", 24, false, false, false, true ); //MS�͑S�p�A���̃X�y�[�X�͔��p�B�C�����悤�B
	}
	return *gGothic;
}

}}// end GUI::Fonts

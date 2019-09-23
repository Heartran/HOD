#ifndef __KZTR_GUI_H__
//GUI�N���X���ĕ��ʂɂ��Ԃ肻��������A�ړ������Ă���
#define __KZTR_GUI_H__

#include<list>

class Image;
namespace GameLib{
	namespace Scene{
		class StringRenderer;
		class Font;
		class PrimitiveRenderer;
	}
	namespace Graphics{
		class Texture;
	}
}

using GameLib::Scene::StringRenderer;
using GameLib::Scene::Font;
using GameLib::Scene::PrimitiveRenderer;
using GameLib::Graphics::Texture;


namespace GUI {

/*���
	�`�揇���ǂ����邩�H
		�D��x���R���e�i�H
		���l���Ȃ�

	window�Ƃ��e�֌W���ǂ����邩�H
		
	�V���O���g���ł͂Ȃ���������K�v���Ƃ���B�ł́A�Ăяo���Ƃ��ǂ�����H
		���V���O���g���ł���
		�V���O���g���ł����񂶂�Ȃ��H���O�������C�����Ă܂��B
		GUIManager�̖��O��������V���O���g���ł��I�b�P�H
		HUD gameHud = HUD::findInstance("game");
		HUD::create( const char*, ���낢��
		static std::map< const char*, HUD* > mThese;
		static void releaseAll();
		static void release( const char * );

	isEnable()�L�����ǂ����B�{�^������ɏ㏑������Ă�����,�����Ȃ�����

	list��add���āA�킴�킴remove���Ȃ��悤�ɂ������B���̂��߂ɂ̓f�X�g���N�^��list��m�肽���B
		���V���O���g���ɂ��Ă�̂ŁA�Ăяo����
		

	Texture defalutTextur����������ꍇ�A�Q�ƃJ�E���g������ɂȂ�Ȃ����H

*/





//�����Ă�ԁA������true��Ԃ��{�^��
//class RepeatButton{};
	
//�I���W�i���摜�̃{�^��
//�摜��܂Ƃ߃o�[�W�������摜2�o�[�W�������H
//class ImageButton{
//	ImageButton(
//		Texture tex,
//		Texture pushedTex,
//		int x, int y,
//		int sizeX, int sizeY,
//		float u0, float v0, 
//		float u1, float v1 );
	
//	
//class HorizotialSlider{};
//class VerticalScrollBar{};
//
//
////�w�i�����Label
//class Box{};
//
////��s�̕�����
//class TextField{};
//class TextArea{};
//	
////�`�F�b�N�{�b�N�X
//class Toggle{};
//
//class Toolbar{};
//class SelectionGrid{};
//

}//namespace GUI

#endif
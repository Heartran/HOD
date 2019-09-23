#ifndef __KZTR_GUI_MANAGER_H__
#define __KZTR_GUI_MANAGER_H__
#include<list>
#include"IModalDialog.h"

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


namespace GUI{
class IComponent;
class IModalDialog;
/*���_
TODO
���̂��낢��Ɩ�肶��Ȃ����H
���t�@�N�^�����O��
	unique_ptr������ŕЂÂ���
	�R���X�g���N�^�Ń��[�h��҂�
	���ׂă��\�[�X��ǂݍ��܂���
	�҂����Ă������܂Œx���Ȃ��ł���

*/
/*�g����
GUI::Manager::instance();
��������
GUI::Manager::instance()->isReady();
�Ńe�N�X�`�����[�h�I��������Ƃ��m�F���邱��

�g���I�������A�K��
	GUI::Manager::instance()->destory();
���ĂԂ��ƁB

void constructor_or_init(){
	mLabel = new GUI::Label( 50, 50, 200, 200 );
	mLabel->setText( "red�������������������������ǂ�����" );
	GUI::Manager::instance()->add( mLabel );
	Texture t = Texture::create( "Data/alphablend_gradation_white_256x1.png" );
	//Texture t = Texture::create( "Data/alphablend_gadation_black_256x1.png" );
	//Texture t = Texture::create( "Data/gradation_black_white.png" );
	while(!t.isReady()){}
	mLabel->setTexture(t);
	mLabel->setColor( 0xffff0000 );

	mButton = new GUI::Button( 50, 100, 200, 50 );
	mButton->setText( "push start" );
	mButton->setColor( 0xffffff00 );
	
	//�s����
	mButton->setBlendMode( GameLib::Graphics::BLEND_OPAQUE);
	mButton->setIsEnableDepthTest(false);
	mButton->setIsEnableDepthWrite(false);
	
	GUI::Manager::instance()->add( mButton );
	

	mImageButton = new GUI::ImageButton( 300, 0, 100, 100 );
	//�����Z�b�g���Ȃ��Ɠ���
	//mImageButton->setTexture(0);
	mImageButton->setText( "nya---" );
	mImageButton->setFontColor( 0xff0000ff );
	GUI::Manager::instance()->add( mImageButton );
}

void gameLoop(){
	if( mButton->isPushed() ){
		//next Game Scene
	}
	
	if( mImageButton->isPushing() ){
		if( mImageButton->isHover() ){
			mImageButton->setText( "pushing. hover" );			
		} else {
			mImageButton->setText( "pushing" );
		}
	} else if( mImageButton->isPushed() ){
		std::ostringstream oss;
		oss << "����������: " << GameLib::Framework::instance().time();
		mImageButton->setText( oss.str().c_str() );
	}
		
	//�`��
	GUI::Manager::instance()->draw();
}

void destory(){
	SAFE_DELETE( mImageButton );
	SAFE_DELETE( mLabel );
	SAFE_DELETE( mButton );
	GUI::Manager::instance()->destory();
}
*/

//GUIMangaer
class Manager {
public:
	friend class IComponent;
	
	friend void IModalDialog::show();
	friend void IModalDialog::hide();
	
	/*font�֘A�͂ЂƂ܂��u���Ƃ��B
	enum FONT_STYLE
	{
		GOTHIC,
	
	};
	//bold�Ƃ�italic�Ƃ��ǂ�����́H
	void changeFont( FONT_STYLE ){}
	*/

	static Manager* instance();
	static void destory();
	void draw();
	void add( IComponent* component );
	void remove( IComponent* component );
	bool isReady();

	//PrimitiveRenderer renderer();
	//StringRenderer stringRederer();
	Texture texture();
	//Font font();

private:
	Manager();
	~Manager();
	static bool isInstance();//GUI::Icomponent�̂���
private:
	static Manager			*mThis;
	static const char		*mkTextureName;

	Texture					*mTexture;
	//PrimitiveRenderer		*mRender;//���񒸓_�f�[�^�\�z����̂���B���ʖ��ʂ�����
	//StringRenderer			*mStringRender;
	//std::vector<Font*>		mFonts;
	//Font					*mFont;
	
	std::list<GUI::IComponent*> mComponents;
	IModalDialog *mModalDialog;
};
}//namespace GUI
#endif//__KZTR_GUI_MANAGER_H__
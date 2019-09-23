#ifndef __KZTR_GUI_IMAGEBUTTON_H__
#define __KZTR_GUI_IMAGEBUTTON_H__
#include"IComponent.h"

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


namespace GUI{
	//using GameLib::Scene::StringRenderer;
	//using GameLib::Scene::Font;
	//using GameLib::Scene::PrimitiveRenderer;
	//using GameLib::Graphics::Texture;

class Manager;
class ImageButton : public ComponentWithRender {
public:
	ImageButton( 
		int x, int y,
		int sizeX, int sizeY,
		unsigned int color = 0xffffffff,
		unsigned int fontColor = 0xff000000,
		float u0=0.f, float v0=0.f,
		float u1=1.f, float v1=1.f );
		
	virtual ~ImageButton();

	bool isPushed() const;
	//�}�E�X�����g�̗̈��ɂ��邩
	bool isHover() const;
	bool isPushing() const;

	virtual void update();
	virtual void draw( Manager* mngr );

	void setText( const char *text );
	void setTexture(GameLib::Graphics::Texture t );
	void setUV( 
		float u0, float v0,
		float u1, float v1 );

private:
	//disallow copy
	ImageButton( const ImageButton& );	
	//disallow assign
	void operator=( const ImageButton& );
protected:
#define GUI_IMAGE_BUTTON_MAX_CHAR 32
	char			mText[GUI_IMAGE_BUTTON_MAX_CHAR];
	
	bool		mIsPushed;//�{�^����ŉ����ė������ꍇtrue
	bool		mIsPushing;//�{�^����ŉ������ꍇtrue
	bool		mIsHover;//�{�^����Ƀ}�E�X�J�\�[��������ꍇtrue

	static const unsigned int skMaxChar;

	GameLib::Graphics::Texture *mTexture;
	float	mU0, mV0;
	float	mU1, mV1;

	float mBigSizeRate;
};
}//namespace GUI
#endif//__KZTR_GUI_IMAGEBUTTON_H__
#ifndef __GUI_TEXTWINDOW_H__
#define __GUI_TEXTWINDOW_H__
#include<vector>
#include<string>
#include<map>
#include"IComponent.h"
namespace GameLib {
	namespace Graphics {
		class Texture;
	}
	namespace PseudoXml{
		class Document;
	}
}

namespace GUI{

class Manager;

class TextWindow : public IComponent {
public:
	TextWindow(
		//int x, int y,
		//int sizeX, int sizeY,
		//unsigned int color,
		//unsigned int fontColor 
	);
	~TextWindow();

	bool load( const char* fileName );

	bool isReady();

	virtual void update();
	virtual void draw( Manager* );

	bool hasRead() const ;

	virtual bool isEnabled() const {
		return mIsEnable;
	}
	virtual void setIsEnabled( bool b ){
		mIsEnable = b;
	}

	virtual bool isDrawAble() const {
		return mIsDrawAble;
	}
	virtual void setIsDrawAble( bool b ){
		mIsDrawAble = b;
	}

	void setPos( int x, int y ){
		mX=x;
		mY=y;
	}

	void getPos( int *x, int *y) const {
		*x=mX;
		*y=mY;
	}
	void setFontColor( unsigned int fc ){
		mFontColor=fc;
	}

	void setColor( unsigned int c ){
		mColor=c;
	}

	void setIsEnableDepthTest( bool isdt ){
		mIsEnableDepthTest=isdt;
	}
	void setIsEnableDepthWrite(bool isdw ){
		mIsEnableDepthWrite=isdw;
	}
	void setBlendMode( GameLib::Graphics::BlendMode bm ){
		mBlendMode=bm;
	}
	void setCullMode( GameLib::Graphics::CullMode cm ){
		mCullMode=cm;
	}

private:
	TextWindow( const TextWindow& );
	void operator=( const TextWindow& );

	void calcMaxCharLengthInDisplay();

	bool	mIsEnable;
	bool	mIsDrawAble;
	int		mSizeX, mSizeY;
	int		mX, mY;

	int		mIconSizeX, mIconSizeY;
	int		mIconX, mIconY;

	int const mFontOffsetX;
	int const mFontOffsetY;


	unsigned int	mColor; 
	unsigned int	mFontColor;

	bool							mIsEnableDepthTest;
	bool							mIsEnableDepthWrite;
	GameLib::Graphics::BlendMode	mBlendMode;
	GameLib::Graphics::CullMode		mCullMode;

	GameLib::Scene::PrimitiveRenderer		*mRender;
	GameLib::Scene::StringRenderer			*mStringRender;

	int mCurrentLine;
	int mCurrentCharPos;
	int mCurrentMessage;

	enum{
		DEFALUT_MAX_LENGTH_OF_LINE=40,
		DEFALUT_MAX_DISPLAY_LINES=3,
	};
	const int mkMaxLengthOfLine;
	const int mkMaxDisplayLines;

	typedef std::vector<std::string> Lines;
	typedef std::pair< std::string, GameLib::Graphics::Texture > Icon;// charName
	typedef std::pair< Icon, Lines > Message;
	
	std::vector<Message>	mMessages;

	GameLib::Graphics::Texture		*mWindowTexture;

	GameLib::PseudoXml::Document *mDocument;

	int mMaxCharLengthInDisplay;
	/*
<icon
  name = "�����~�N"
  file = "Data/GUI/Icon/dummy.bmp"
/>

<message
  name = "�����~�N"
  text = "\n����a��\n
���͂悤123456789
���������Ȃ͂܂���a
�@�ؗ��i�����߁j���i����́j�����Γ��i���₫�ǂ��j�A\n���ɂ͂킴�ƐԊ~��p�Ђ�����i����łӁj���̒��Δ��ɑ΂ЂĘb���G�i�������j���Ȃ��B��l�A\n�����͗҂������ɍ��苏��O�\�O��̏��A\n�j�̂₤�ɗ��h�Ȕ��������i���j�|�Ђ�������鍭�̐ƁA\n�������o�ނׂ��J��̎R�̐F���ƁU�߂Đ��i�݂ǂ�j�̂Ј�g���ُ������A�@�؂�ƒʂ��K�L���R�Ə��A��Д�������^�_�ƍ��i�ނ��j���ہi�܂�j�߂Ĉ��􎆂�������ЂɈ�{�ρi���ۂ񂴂��j�ł����Ɨ��߂��h�����F�C���̗l�͂���ǁA"
/>
	*/


};


}

#endif

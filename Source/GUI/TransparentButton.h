#pragma once
#include"IComponent.h"

namespace GUI{
class Manager;
class TransparentButton : public ComponentWithRender {
public:
		
	TransparentButton( 
		int x, int y,
		int sizeX, int sizeY,
		unsigned int fontColor = 0xff000000 );
		
	virtual ~TransparentButton();

	bool isPushed() const;
	void setIsPushed( bool b ){
		mIsPushed = b;
	}
	//�}�E�X�����g�̗̈��ɂ��邩
	bool isHover() const;

	virtual void update();
	virtual void draw( Manager* mngr );

	void setText( const char *text );

private:
	//disallow copy
	TransparentButton( const TransparentButton& );	
	//disallow assin
	void operator=( const TransparentButton& );
	using ComponentWithRender::setColor;
private:
	enum{
		kMaxChar=32,
	};
	char			mText[kMaxChar];
	
	
	bool		mIsPushed;//�{�^����ŉ����ė������ꍇtrue
	bool		mIsPushing;//�{�^����ŉ������ꍇtrue
	bool		mIsHover;//�{�^����Ƀ}�E�X�J�\�[��������ꍇtrue
	
	static const float	skU0, skV0;
	static const float	skU1, skV1;
};
}//namespace GUI

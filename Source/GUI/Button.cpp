#include"Button.h"
#include"Manager.h"
#include<string.h>
#include<assert.h>
#include<GameLib/Input/Mouse.h>
#include<GameLib/Scene/PrimitiveRenderer.h>
#include<GameLib/Scene/StringRenderer.h>
#include<GameLib/Math/Vector2.h>
#include<GameLib/Graphics/Texture.h>


namespace GUI{
	using GameLib::Math::Vector2;
	const float Button::skU0=0.f;
	const float Button::skV0=0.f;
	const float Button::skU1=128.f/512.f;
	const float Button::skV1=64.f/512;
						 
	const float Button::skUPushed0=0.f/512.f;
	const float Button::skVPushed0=64.f/512;
	const float Button::skUPushed1=128.f/512.f;
	const float Button::skVPushed1=128.f/512;

	const int	Button::skMaxChar = GUI_BUTTON_MAX_CHAR;

	Button::Button( 
		int x, int y,
		int sizeX, int sizeY,
		unsigned int color,
		unsigned int fontColor )
		:ComponentWithRender(
			x, y,
			sizeX, sizeY,
			color,
			fontColor,
			false, false,
			GameLib::Graphics::BLEND_LINEAR, GameLib::Graphics::CULL_NONE
		),
		mIsPushed(false),
		mIsPushing(false),
		mIsHover(false)
	{
		memset( mText, 0x00, sizeof( mText ) );
	}
		
	Button::~Button(){}
	void Button::setText( const char *text ){
		assert( strlen( text ) < skMaxChar );
		if( strlen( text ) > skMaxChar )
		{
			return;
		}

		strncpy( mText, text, skMaxChar );

	}

	bool Button::isPushed() const{
		return mIsPushed;
	}

	bool Button::isHover() const {
		return mIsHover;
	}

	void Button::update(){
		GameLib::Input::Mouse m;
		const int &x = m.x(); 
		const int &y = m.y(); 
		mIsPushed = false;

		//�}�E�X�J�\�[�����{�^����ɂ��邩�H
		mIsHover = BaseComponent::isHover(x, y);
		
		//�{�^����ŃN���b�N�����Ƃ�
		if( m.isTriggered(m.BUTTON_LEFT) && mIsHover ){
			mIsPushing = true;
		}

		//�{�^����ŃN���b�N���Ă���A�������ꍇ
		if( mIsPushing  && m.isReleased(m.BUTTON_LEFT) ) {
			//�{�^����ŗ������ꍇ
			if( mIsHover )  {
				mIsPushed = true;
			}
			mIsPushing = false;
		}
	}
	void Button::draw( Manager* mngr ){
		namespace gs=GameLib::Scene;
		mRender->enableDepthTest( mIsEnableDepthTest );
		mRender->enableDepthWrite( mIsEnableDepthWrite );
		mRender->setBlendMode( mBlendMode );
		mRender->setCullMode( mCullMode );
		mRender->setTexture( mngr->texture() );
		
		//�}�E�X�J�[�\�����{�^����ɂ���Ȃ�
		if( mIsHover ){
			//�������t���O�������Ă���Ȃ�΁A�ւ��񂾃{�^��������
			if(mIsPushing ){
				mRender->addRectangle( 
					Vector2( static_cast<float>(mX), static_cast<float>(mY) ),
					Vector2( static_cast<float>(mX+mSizeX), static_cast<float>(mY+mSizeY) ),
					Vector2( skUPushed0, skVPushed0 ),
					Vector2( skUPushed1, skVPushed1 ),
					mColor );
			}
			else {
				GameLib::Input::Mouse m;
				//�}�E�X�J�[�\�����{�^����ɂ����āA���̃{�^���������Ă��Ȃ��Ȃ�΁A������Ƒ傫������
				if( !m.isOn(m.BUTTON_LEFT) ){
					mRender->addRectangle( 
						Vector2( static_cast<float>(mX)*0.99f, static_cast<float>(mY)*0.99f ),
						Vector2( static_cast<float>(mX+mSizeX)*1.01f, static_cast<float>(mY+mSizeY)*1.01f ),
						Vector2( skU0, skV0 ),
						Vector2( skU1, skV1 ),
						mColor );
				}
				//�ق��̃{�^���������Ă���\��������Ȃ�ʏ�
				else {
					mRender->addRectangle( 
						Vector2( static_cast<float>(mX), static_cast<float>(mY) ),
						Vector2( static_cast<float>(mX+mSizeX), static_cast<float>(mY+mSizeY) ),
						Vector2( skU0, skV0 ),
						Vector2( skU1, skV1 ),
						mColor );
				}
			}
		}
		else{
			mRender->addRectangle( 
				Vector2( static_cast<float>(mX), static_cast<float>(mY) ),
				Vector2( static_cast<float>(mX+mSizeX), static_cast<float>(mY+mSizeY) ),
				Vector2( skU0, skV0 ),
				Vector2( skU1, skV1 ),
				mColor );
		}
		
		int startX, startY;
		//������
		calcPosCenterAlign( &startX, &startY, mText );
		mStringRender->add( startX, startY, mText, mFontColor );

		mRender->draw();
		mStringRender->draw();
	}
}//namespace GUI;
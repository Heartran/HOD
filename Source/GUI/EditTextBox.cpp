#include"EditTextBox.h"
#include"Manager.h"
#include<GameLib/Graphics/Texture.h>
#include<GameLib/Graphics/Manager.h>
#include<GameLib/GameLib.h>
#include<string.h>
#include<assert.h>
#include<GameLib/Scene/PrimitiveRenderer.h>
#include<GameLib/Scene/StringRenderer.h>
#include<GameLib/Math/Vector2.h>
#include<GameLib/ImeUi/ImeUi.h>

#include<GameLib/Input/Mouse.h>
#include<GameLib/Input/Keyboard.h>

#include"helper/MyOutputDebugString.h"

#include <mbctype.h>

namespace{
static const unsigned int skSelectedColor = 0x88000044;

//�L���X�g����̂��߂�ǂ�����
GameLib::Vector2 toVec2( int x, int y ){
	return GameLib::Vector2( 
		static_cast<float>(x),
		static_cast<float>(y) );
}

bool isZenkaku( unsigned char code ){
	//ShiftJis�͈͂��H
	return ( code >= 0x81 && code <= 0x9F ) || ( code >= 0xE0 && code <= 0xFC );
}
bool isHankaku( unsigned char code ){
	return !isZenkaku(code);
}

//
//isZenkakuAt
//�󕶎����n�����ꍇfalse��Ԃ�
bool isPositionOfStringZenkaku( const char* str, unsigned int pos ){
	if( strlen(str)==0 ){
		return false;
	}
	assert( strlen(str)>pos );
	bool isAscii = true;
	for (unsigned int i = 0; i < pos+1; ++i ) {
		unsigned int code = static_cast<unsigned int>( *(str+i) );
		
		//�}���`�o�C�g�������ǂ����H
		if(_ismbblead( code ) ){
				isAscii = false;
				++i;
				//\0�̔��������ׂ�����������
				//std::string�n�����ꍇ�r��0�ɂȂ�\�����邵�ǂ�����
				
				//2�o�C�g�ڂ�����ʒu�̏ꍇ
				if(i == pos  ){
					return true;;
				}
		}else{
			isAscii = true;
		}
	}
	return !isAscii;
}

}//end unnamespace
namespace GUI {

using GameLib::Math::Vector2;


EditTextBox::EditTextBox(
	int x, int y,
	int sizeX, int sizeY,
	unsigned int color,
	unsigned int fontColor )
	:ComponentWithRender( 
		x, y,
		sizeX, sizeY,
		color,
		fontColor,
		false, false, GameLib::Graphics::BLEND_LINEAR, GameLib::Graphics::CULL_NONE,
		6*4, 4
	),
	mOnFocus(false),
	mCursolPos(0)
{}

EditTextBox::~EditTextBox(){
}

bool EditTextBox::onFocus()const{
	return mOnFocus;
}

void EditTextBox::update(){
	GameLib::Input::Mouse m;
	if( m.isReleased(m.BUTTON_LEFT) ){
		int x = m.x();
		int y = m.y();
		if( mX <= x && x <= mX+mSizeX &&
			mY <= y && y <= mY+mSizeY )
		{
			mOnFocus=true;
		} else {
			mOnFocus=false;
			GameLib::ImeUi::enableIme(false);
		}
	}

	if( onFocus() ){
		GameLib::ImeUi::enableIme(true);

		GameLib::Input::Keyboard k;
		if( k.isTriggered( k.KEY_LEFT ) ){
			if( mCursolPos <= 0 ){
			} 
			else if( isPositionOfStringZenkaku( mText.c_str(), mCursolPos-1 ) ){
				mCursolPos-=2;			
			} else{
				--mCursolPos;			
			}
		} else if( k.isTriggered( k.KEY_RIGHT ) ){
			if( mCursolPos >= mText.size() ){
			} else if( isPositionOfStringZenkaku( mText.c_str(), mCursolPos ) ){
				mCursolPos+=2;
			} else{
				++mCursolPos;
			}
		} else if( k.isTriggered( k.KEY_BACKSPACE ) ){
			if( mCursolPos!=0 ){
				if( isPositionOfStringZenkaku( mText.c_str(), mCursolPos-1 ) ){
					mText.erase( mCursolPos-2, 2);
					mCursolPos -= 2;
				} else {
					mText.erase( mCursolPos-1, 1 );
					--mCursolPos;

				}
				//TRACE1( "Text: %s\n", mText.c_str() );
			}
		}

		//��������͂��`�F�b�N
		const std::string input = GameLib::ImeUi::getInputString(); 
		if( !input.empty() ){
			mText.insert( mCursolPos, input );
			mCursolPos += input.size();
		}
	}
}

void EditTextBox::draw( Manager* mngr ){

	mRender->enableDepthTest( mIsEnableDepthTest );
	mRender->enableDepthWrite( mIsEnableDepthWrite );
	mRender->setBlendMode( mBlendMode );
	mRender->setCullMode( mCullMode );
	
	drawBG();
	drawText();

	if( onFocus() ){
		drawComposition();
		drawCandidate();
		drawIndicator();
	}

	mRender->draw();
	mStringRender->draw();
}

void EditTextBox::drawBG(){
	mRender->addRectangle( toVec2(mX, mY), toVec2(mX+mSizeX, mY+mSizeY), 0x33002200 );
}
void EditTextBox::drawText(){
	mStringRender->add( mX, mY, mText.c_str(), mFontColor );	
}
void EditTextBox::drawComposition(){
	using namespace GameLib::ImeUi;
	tstring compositionString;
	DWORD cc = getImeCursorChars();
	
	TCHAR* tc= getCompositionString();
	
	if( tc!=0 ){
		//�ϊ����̑I��͈͂����߂�
		BYTE* pAttr;
		const TCHAR* pcComp;
		int lengthSelected=0;
		int numAsciiSelected=0;//�I��͈͒��̔��p(ascii)������
		int numAscii=0;//�S����ascii������
		int numNotAscii=0;

		float startSelected=0.f;
		bool isContinued=true;
		for( pcComp = tc, pAttr = getCompStringAttr();
			*pcComp != TEXT('\0'); ++pcComp, ++pAttr )
		{
			//�I��͈͂�T��
			if( 
				*pAttr == ATTR_TARGET_CONVERTED ||
				*pAttr == ATTR_TARGET_NOTCONVERTED )
			{
				//�J�n�ʒu���Z�b�g
				if( lengthSelected==0 ){
					startSelected=numAscii+2.f*numNotAscii;
				}
				//�I��͈͂̒����𓾂�
				if( isHankaku(*pcComp) ){
					++lengthSelected;
				} else{
					lengthSelected+=2;
				}
			}

			//���p�S�p�̐��𐔂���
			if( isHankaku(*pcComp) ){
				++numAscii;
			} else {
				++numNotAscii;
				//�S�p�Ȃ�Ύ��̕����͔�΂�
				++pcComp;
				assert(*pcComp!=TEXT('\0'));
				++pAttr;
			}
		}

		//�I�𒆂̔w�i��h��
		int left, top, right, bottom;  
		mStringRender->calcTextSize(
			&left, &top, &right, &bottom,
			Vector2( static_cast<float>(mX), static_cast<float>(mY) ),
			tc );

		float hankaku_length = (right-left)/(numAscii+2.f*numNotAscii);
		float start=left + mCursolPos*hankaku_length + hankaku_length*startSelected;
		Vector2 p0(start , static_cast<float>(top) );
		Vector2 p1(start + hankaku_length*lengthSelected, static_cast<float>(bottom ));
		
		mRender->addRectangle( p0, p1, skSelectedColor );
		
		mStringRender->add( static_cast<int>(mX+mCursolPos*hankaku_length), mY, tc, mFontColor, true );

	}

	
	//if( int num = GameLib::ImeUi::getCandidateCount() ){
	//	for( int i=0; i<num; ++i ){
	//		TRACE2( "%d: %s\n", i, GameLib::ImeUi::getCandidate(i) );
	//	}
	//}
}
void EditTextBox::drawCandidate(){
	
	using namespace GameLib;
	assert( !ImeUi::isShowReadingWindow() );
	static const int skMaxCandList=10;
	if( ImeUi::isVerticalCand() )
    {
        // Vertical window
        for( UINT i = 0; i < skMaxCandList; ++i )
        {
            if( *( ImeUi::getCandidate( i ) ) == TEXT('\0') ){
                break;
			}

			const char* candidateText = ImeUi::getCandidate( i );
			//�ꕶ���̑傫�����v�Z
			int left, top, right, bottom;  
			mStringRender->calcTextSize(
				&left, &top, &right, &bottom,
				Vector2( 0.f, 0.f ),
				"a" );

			//�R���|�W�V�����X�g�����O�̊J�n�ʒu��T��
			unsigned int start=0; 
			BYTE* pAttr = ImeUi::getCompStringAttr();
			const char* pcComp = ImeUi::getCompositionString();
			for(
				;
				*pcComp != TEXT('\0');
				++pcComp, ++pAttr )
			{
				//�I��͈͂��J�n�ʒu�Ȃ̂Ńu���C�N
				if( 
					*pAttr == ATTR_TARGET_CONVERTED ||
					*pAttr == ATTR_TARGET_NOTCONVERTED )
				{
					break;
				}
				++start;
			}

			//�\���ʒu�����߂�
			Vector2 p( mX+(mCursolPos+start)*right, mY + bottom*(i+1) );
			mStringRender->add( p, candidateText );
			
			//�I�𒆕�����ɐF��t����
			if( ImeUi::getCandidateSelection() == i )
            {
				mStringRender->calcTextSize(
					&left, &top, &right, &bottom,
					p, candidateText );
				mRender->addRectangle( toVec2(left, top ), toVec2( right, bottom ), skSelectedColor );
            }

        }
    }
	
}

void EditTextBox::drawIndicator(){
	static const unsigned int skIndicatorColor = 0xff000000;
	if(mText.empty()){
		const char *c="a";
		int left, top, right, bottom;  		
		mStringRender->calcTextSize(
			&left, &top, &right, &bottom,
			toVec2( mX, mY ),
			c, true );
		mRender->addRectangle( 
			toVec2(mX, mY),
			toVec2(mX + 3, bottom),
			skIndicatorColor );
	} else {
		int left, top, right, bottom;  
		mStringRender->calcTextSize(
			&left, &top, &right, &bottom,
			toVec2( mX, mY ),
			mText.c_str() );
		float lengthCharacter = static_cast<float>((right-left)/mText.length());
		mRender->addRectangle( 
			Vector2(mX+lengthCharacter*mCursolPos, static_cast<float>(mY) ),
			Vector2(mX+lengthCharacter*mCursolPos + 3, static_cast<float>(bottom) ),
			skIndicatorColor );
	}
}

const std::string & EditTextBox::getText()const{
	return mText;
}

void EditTextBox::clearText(){
	mText.clear();
	mCursolPos=0;
}


}//namespace GUI
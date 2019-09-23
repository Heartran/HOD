#pragma once
#include"IComponent.h"
//#include<string>
//#include<list>
#include<vector>
#include<GameLib/Scene/Font.h>

namespace GUI{
class VerticalScrollBar;
class PushableLabel;
class ListTexts : public BaseComponent {
public:
	ListTexts(
		int x, int y,
		int sizeX, int sizeY
	);
	virtual ~ListTexts();

	void add( const char *text );
	//vector���g���Ă���̂Œx��
	void remove( const char *text );

	const char* currentText()const;
	void setSelectedIndex(unsigned int index);
	int getSelectedIndex()const;

	int count() const;

	virtual void update();
	virtual void draw( Manager* );

	void setFont( GameLib::Scene::Font f );

	GameLib::Scene::Font getFont()const{
		return mFont;
	}

	//int �ŕԂ��ꍇ�A������ĂȂ��ꍇ�[1�A0�Ԗڂ������ꂽ�ꍇ0(false)��Ԃ����삪�C���������̂�bool�ŕԂ���
	//�ǂ̃{�^���������ꂽ����getSelectedIndex()�Ŏ擾����
	bool isPushedSelectedElement()const;

	void setText( unsigned int index, const char* text );

	void setPushable( unsigned int index, bool enable );

private:
	//vector�ɂ����킯
	//�@�����_���A�N�Z�X������
	//	erase��1�t���[�����ɏ��Ȃ���(1��)�������삳���Ȃ��B
	typedef std::vector<PushableLabel*> List;
	List mList;;
	//List::iterator mCurrentIt;
	int mSelectedIndex;
	VerticalScrollBar *mScrollBar;
	
	unsigned int mSelectedItemColor;
	unsigned int mHoverItemColor;//�}�E�X����ɂ���Ƃ��̐F
	unsigned int mItemColor;
	GameLib::Scene::Font mFont;
};
}
#pragma once
#include"IModalDialog.h"
namespace GUI{
	class Window;
	class Button;
	class Label;
//�J���Ă���Ԃ͑��̃R���|�[�l���g�̏�Ԃ͕ύX���Ȃ�

//exsample
//
////���[�_���_�C�A�����O���Ăяo����Ă�Ԃ͑���GUI�̓A�b�v�f�[�g����Ȃ�
//	if( mBtQuit->isPushed() ){
//		mYesNo->show();
//		//yesnodialog
//		if( mYesNo->isYes() ){
//			GameLib::Framework::instance().requestEnd();
//		}else if( mYesNo->isNo() ){
//			mYesNo->hide();
//			mYesNo->init();
//		}
//	}
	
//	if( mBtQuit->isPushed() ){
//		static YesNoDialog sYesNo;//�{���̓|�C���^��������
//		sYesNo.show();//���t���[���Ă΂Ȃ��Ă��I�b�P
//		//yesnodialog
//		if( sYesNo.isYes() ){
//			GameLib::Framework::instance().requestEnd();
//		}
//	}
class YesNoDialog : public IModalDialog {
public:
	YesNoDialog( const char *questionText );
	virtual ~YesNoDialog();

	void update();
	void draw( Manager* );
	void init(){
		update();//�Ƃ肠��������ŗǂ���
	}

	bool isEnabled() const {
		return mIsEnable;
	}
	void setIsEnabled( bool b ){
		mIsEnable = b;
	}

	bool isDrawAble() const {
		return mIsDrawAble;
	}
	void setIsDrawAble( bool b ){
		mIsDrawAble = b;
	}

	bool isYes()const;
	bool isNo()const;

protected:
	bool	mIsEnable;
	bool	mIsDrawAble;

	Window *mWindow;
	Button *mBtYes;
	Button *mBtNo;
	Label  *mQuestionText;
};
}//end namespace GUI
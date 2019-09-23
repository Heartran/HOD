#pragma once
#include"IComponent.h"

namespace GUI{ //tolua_export

// �\�����̓E�C���h�E�̊O�̑��삪�ł��Ȃ��Ȃ�u���[�_���_�C�A���O�v(modal dialog)�ƁA
// ���̃E�C���h�E�̑�����s���Ă��悢�u���[�h���X�_�C�A���O�v(modeless dialog)������
//
//�@�J���Ă���Ԃ͑��̃R���|�[�l���g�̏�Ԃ͕ύX�������Ȃ������̂�show(), hide()��������B
//�@
//  show()������́A�K��hide()���ĂԂ���
//	show()�͖��t���[���]�Ă�ł��ǂ����A��񂾂��ł��ǂ����A�K��hide()�����邱��

class IModalDialog : public IComponent { //tolua_export
public:
	IModalDialog();
	virtual ~IModalDialog();

//tolua_begin
	void show();
	void hide();
};

}//end namespace GUI
//tolua_end
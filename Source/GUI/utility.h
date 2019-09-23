#pragma once
#include<assert.h>
#include<memory>

//���O helper�̂ق����悩�������H
//��ӏ��ɂ܂Ƃ߂��������̂ō����
//�ʂɈႤ�l�A�g�����Ƃ����X����
namespace GUI{
namespace ButtonBasicSize{
	int width();
	int height();
};


namespace SaveLoadListTextPosSize{
	int x();
	int y();
	int width();
	int height();
};

class ListTexts;

std::shared_ptr<ListTexts> createSaveListText();
std::shared_ptr<ListTexts> createLoadListText();

void updateSaveListText( int index, std::shared_ptr<ListTexts> list );


}//end namespace GUI
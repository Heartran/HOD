#ifndef __SEQUENCE_BASE_H__
#define __SEQUENCE_BASE_H__

namespace Sequence{

class Base{
public:
	virtual ~Base(){} //�Ȃɂ����Ȃ��̂ł���ł���
	virtual Base* update( Base* ) = 0;
};

} //namespace Sequence

#endif
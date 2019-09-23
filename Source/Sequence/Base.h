#ifndef __SEQUENCE_BASE_H__
#define __SEQUENCE_BASE_H__

namespace Sequence{

class Base{
public:
	virtual ~Base(){} //なにもしないのでこれでいい
	virtual Base* update( Base* ) = 0;
};

} //namespace Sequence

#endif
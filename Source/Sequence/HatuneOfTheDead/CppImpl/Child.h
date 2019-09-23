#pragma once
#include "Sequence/Base.h"

namespace Sequence{
namespace HatuneOfTheDead{
namespace CppImpl{
class Parent;

class Child : public Base{
public:
	virtual ~Child();
	Base* update( Base* ); //これは実体を作ってやる
	virtual Base* update( Parent* ) = 0;
};


}}} //namespace Sequence::HatuneOfTheDead::CppImpl

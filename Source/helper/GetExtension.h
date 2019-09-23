#pragma once
#include<string>
#include<assert.h>

static std::string getExtension( const std::string &src){
	int index = src.rfind( ".", src.size());
	if( index == std::string::npos){
		assert( false && "�g���q�����݂��Ȃ�");
	}
	std::string str;
	str = src.substr( index + 1);
	return str;
}
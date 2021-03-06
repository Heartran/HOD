#pragma once
#include<string>
#include<assert.h>

static std::string getFolder( const std::string &src){
	int pos = src.rfind('/');
	if( pos == std::string::npos){
		assert( false && "フォルダじゃない");
		return std::string();
	}
	return std::string ( src, 0, pos+1 );
}
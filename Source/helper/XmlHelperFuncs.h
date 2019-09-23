#pragma once
#include<GameLib/GameLib.h>
#include<assert.h>
#include<GameLib/PseudoXml/Document.h>
#include<GameLib/PseudoXml/Element.h>
#include<GameLib/PseudoXml/Attribute.h>
#include<GameLib/Base/RefString.h>

#include"helper/MyOutputDebugString.h"

#include<sstream>
#include<string>

namespace XmlHelperFuncs {
	using namespace GameLib::PseudoXml;
//���܂ŒT���ɂ����Ȃ�
//������Ȃ��ꍇ�͋��ConstElement��Ԃ�
static ConstElement findElement( const ConstElement &e, const char *findElementName ){
	int numChild = e.childNumber();
	for( int i=0; i<numChild; ++i ){
		if( GameLib::RefString(e.child(i).name()) == findElementName ){
			return e.child(i);
		}
	}

	std::string message(findElementName);
	message.append( " didn't found!" );
	TRACE1("%s\n", message.c_str());
	STRONG_ASSERT( false && "element didn't find!" );

	//����ۂ�Ԃ�
	return ConstElement(); 
}

static const char* getVMDFileName( const ConstElement &elem ){
	int num=elem.attributeNumber();
	for( int i=0; i<num; ++i ){
		if( GameLib::RefString("vmdFile") == elem.attribute(i).name() ){
			return elem.attribute(i).value();
		}
	}
	STRONG_ASSERT( false && "vmdFile Attribute didn't find!" );

	return NULL;
}

//������( a.value() )�𕪐͂���"true"�Ȃ��true��Ԃ�
//"false"�Ȃ�false��Ԃ�
//�G���[���N������assert���N�����Afalse��Ԃ��B
static bool setBool(bool *out, const ConstAttribute &a ){
	GameLib::RefString str( a.value() );
	if( str == "true" ){
		*out=true;
		return true;
	} else if( str == "false" ){
		*out=false;
		return true;
	} else {
		assert(false);
		return false;
	}
}

//�o�O����
//stream�����̃^�O�ʒu�܂Ői�߂�
//�������^�O����Ԃ�
//������Ȃ������ꍇ�󕶎����Ԃ�+assert
static std::string advanceToNextTag( std::istream &is ){
	assert(false);
	while( !is.eof() ){
		std::streamoff pos = is.tellg();
		char c;
		is >> c;
		if( c=='<' ){
			std::string tag;
			is >> tag;
			int size = tag.size();
			if( tag[size-1]=='>' ){
				tag.erase( size-1 );
			}
			is.seekg( pos );
			return tag;
		}
	}

	assert(false && "xml file is wrong. not found <");
	return std::string();
}	


} //end namespace XmlHelperFuncs

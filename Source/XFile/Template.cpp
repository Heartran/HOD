#include <GameLib/GameLib.h>
#include "XFile/Template.h"
#include "XFile/Token.h"
#include "XFile/Element.h"
#include <GameLib/Base/Tank.h>
#include "XFile/Parser.h"
#include <sstream>

#include<assert.h>

#include"helper/MyOutputDebugString.h"

namespace XFile{

using namespace GameLib;
using namespace std;

Template::Template( RefString name, const RefString& uuid, int childNumber ) :
mName( name ),
mChildren( childNumber ), 
mUuid( uuid ){

}

Template::~Template(){
	//TRACE1( "size %d\n", mChildren.size() );
}

void Template::setChild( int i, TemplateLine** tl ){
	mChildren[ i ] = *tl;
	*tl = 0;
}

const RefString* Template::getType() const {
	return &mName;
}

const RefString* Template::getChildName( int index ) const {
	if ( index >= mChildren.size() ){ //���傫���ԍ���v������邱�Ƃ�����B�g�������B0��Ԃ��B
		return 0;
	}else{
		return &mChildren[ index ]->mName;
	}
}

const RefString* Template::getChildType( int index ) const {
	if ( index >= mChildren.size() ){ //���傫���ԍ���v������邱�Ƃ�����B�g�������B0��Ԃ��B
		return 0;
	}else if ( mChildren[ index ]->mTemplate ){ //�����^
		return mChildren[ index ]->mTemplate->getType();
	}else{ //�v���~�e�B�u
		return &mChildren[ index ]->mPrimitiveType;
	}
}


int Template::createElement( 
ElementSharedPtr* elementOut, 
const Array< Token >& t,
int p,
const Parser& parser,
const RefString& name,
map< RefString, ElementSharedPtr >* namedElements,
int* level ) const {

//if ( p % 1000 == 0 ){ cout << p << endl; }

	const int n = t.size();
	Tank< ElementSharedPtr > children;
	map< RefString, int > arrayIndices;
	//���M�����[�f�[�^
	for ( int i = 0; i < mChildren.size(); ++i ){
		++( *level );
		ElementSharedPtr c; //�q
		if ( ( p < n ) && ( t[ p ] == Token::LEFT_BRACE ) ){ //�Q�Ɨ��܂�����...
			p = parseReference( &c, t, p, namedElements );
		}else{
			const TemplateLine& line = *mChildren[ i ];
			if ( line.mIndices.size() > 0 ){ //�z��ł�
				p = createArray( &c, line, t, p, arrayIndices, parser, namedElements, level );
			}else if ( line.mTemplate ){ //�����^�ł���
				RefString nullStr;
				//TRACE1( "name:%s\n", std::string( line.mName.get(), line.mName.size() ).c_str() );
				p = line.mTemplate->createElement( &c, t, p, parser, line.mName, namedElements, level );
			}else{ //�v���~�e�B�u�ł�
				c = std::make_shared<PrimitiveElement>( *t[ p ].get(), line.mName, line.mPrimitiveType );
				int size = parser.toInt( *t[ p ].get() ); //�Y�����ɂȂ肤�邩���ׂ܂�
				++p;
				if ( size > 0 ){
					arrayIndices.insert( make_pair( line.mName, size ) );
				}
			}
		}
		children.add( c );
		Parser::dieIf( p >= n );
		if ( t[ p ] == Token::SEMICOLON ){
			++p;
		}else{ //���肦�Ȃ�
#if 0 //���^�Z�R�C�A�Ή��B�Z�~�R�����������X���[
			Parder::dieIf( true );
#endif
		}
		--( *level );
#if 1 //���^�Z�R�C�A�Ή��R�[�h �]���ȃZ�~�R����������
		if ( ( *level == 0 ) && ( p < n ) && ( t[ p ] == Token::SEMICOLON ) ){ //���肦�Ȃ�
			++p;
		}
#endif
	}
	//�ǉ��f�[�^
	while ( p < n ){
		ElementSharedPtr c;
		if ( t[ p ] == Token::LEFT_BRACE ){ //�Q�Ɨ��܂�����...
			p = parseReference( &c, t, p, namedElements );
		}else if ( t[ p ] == Token::IDENTIFIER ){
			const Token &token = t[p];
			//�^�������o���܂�
			const RefString* typeName = t[ p ].get();
			++p;
			//�ϐ������o��
			RefString childName;
			if ( ( p < n ) && ( t[ p ] == Token::IDENTIFIER ) ){
				childName = *t[ p ].get();
				++p;
			}
			//{

			//�z��̋�؂�ɃR���}�ł͂Ȃ����s�̏ꍇ������B
			//���̏ꍇ�A�����ň���������
			//Paser.cpp�ŉ��s�������Ă���ۂ�
			//X�t�@�C���ɃJ���}�����Ηǂ������Ȃ̂�
			//�\�[�X�R�[�h���C����̂́A���Ȃ��B
			//X�t�@�C���̎d�l�Ƃ����܂藝�����ĂȂ��̂Ŏ��Ԃ�������
			//���̃\�[�X�𗝉�����͎̂��Ԃ�������
			//�����̃\�[�X�����s������O��Ƃ��č���Ă���̂ŁA�R�[�h�𗝉����ĂȂ��̂ŉe���͈͂��i�肫��Ȃ�
			//Parser::preprocess, Parser::tokenize��array�̏ꍇ���l����Ηǂ��̂��H�܂�����ȏ�͍l���Ȃ��B�܂��K�v�ɂȂ�����l����B
			Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::LEFT_BRACE ) ); //���肦�Ȃ�
			++p;
			//�e���v���[�g�Ƀf�[�^������Ă��炢�܂��B
			if ( parser.isPrimitiveType( *typeName ) ){ //�v���~�e�B�u
				c = std::make_shared<PrimitiveElement>( *t[ p ].get(), childName, *typeName );
				++p;
			}else{ //�����^
				const Template* tmpl = parser.getTemplate( *typeName );
				int childLevel = 0;
				p = tmpl->createElement( &c, t, p, parser, childName, namedElements, &childLevel );
				namedElements->insert( make_pair( childName, c ) );
			}
			//}
			const Token &token1 = t[p];
			Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::RIGHT_BRACE ) ); //���肦�Ȃ�
			++p;
		}else{
			break; //�Q�Ƃł��f�[�^���̂ł��Ȃ��B�܂�I���B
		}
		children.add( c );
	}
	std::shared_ptr<CompositeElement> e = std::make_shared<CompositeElement>( this, children.size(), name );


	for ( int i = 0; !children.isEnd(); ++i ){
		ElementSharedPtr *c = children.get();
		e->setChild( i, *c );
		children.toNext();
	}
	*elementOut = e;

	return p;
}

int Template::createArray(
ElementSharedPtr* elementOut, 
const TemplateLine& line,
const Array< Token >& t,
int p,
const map< RefString, int >& arrayIndices,
const Parser& parser,
map< RefString, ElementSharedPtr >* namedElements,
int* level ) const {
	//�ŏ��ɂ�邱�Ƃ͔z��̓Y�����̎����Ɗe�����̃T�C�Y���擾���邱�ƁB�����͍œK���̗]�n������
	const int n = t.size();
	const Array< RefString >& indices = line.mIndices;
	int dimension = indices.size();
	Array< int > arraySizes( dimension );
	int k = 0;
	int arraySize = 1;
	for ( int i = 0; i < indices.size(); ++i ){
		const RefString& s = indices[ i ];
		int size = parser.toInt( s );
		if ( size > 0 ){
			arraySizes[ k ] = size;
		}else{
			map< RefString, int >::const_iterator l = arrayIndices.find( s );
			Parser::dieIf( l == arrayIndices.end() );
			arraySizes[ k ] = l->second;
		}
		arraySize *= arraySizes[ k ];
	}
	//�v���~�e�B�u�z�񂩕����^���ŕ���
	if ( line.mTemplate ){ //�����^
		std::shared_ptr<CompositeArrayElement> a = std::make_shared<CompositeArrayElement>( arraySizes, *(line.mTemplate->getType()), line.mName  );
		for ( int i = 0; i < arraySize; ++i ){
			ElementSharedPtr te;
			RefString nullStr;
			p = line.mTemplate->createElement( &te, t, p, parser, nullStr, namedElements, level );
			if ( i < arraySize - 1 ){ //�ŏI�v�f�����̓J���}���Ȃ��B�Ȃ�Ă������I
				Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::COMMA ) );
				++p;
			}
			a->setElement( i, &te );
		}
		*elementOut = a;
	}else{ //�v���~�e�B�u
		std::shared_ptr<PrimitiveArrayElement> a =  std::make_shared<PrimitiveArrayElement>( arraySizes, line.mPrimitiveType, line.mName );
		for ( int i = 0; i < arraySize; ++i ){
			a->setElement( i, *t[ p ].get() );
			++p;
			if ( i < arraySize - 1 ){ //�ŏI�v�f�����̓J���}���Ȃ��B�Ȃ�Ă������I
				Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::COMMA ) );
				++p;
			}
		}
		*elementOut = a;
	}
	return p;
}

//�Q�Ə��� { name | UUID | name UUID }
int Template::parseReference(
ElementSharedPtr* elementOut,
const Array< Token >& t,
int p,
map< RefString, ElementSharedPtr >* namedElements ) const {
	const int n = t.size();

	++p; //�����������΂�
	RefString refName;
	if ( ( p < n ) && ( t[ p ] == Token::IDENTIFIER ) ){
		refName = *t[ p ].get();
		++p;
	}
	if ( ( p < n ) && ( t[ p ] == Token::UUID ) ){ //UUID�͖��O���Ȃ�����������Ă���B
		++p;
		if ( refName.size() == 0 ){
			refName = *t[ p ].get();
		}
	}
	Parser::dieIf( refName.size() == 0 );
	Parser::dieIf( ( p >= n ) || ( t[ p ] != Token::RIGHT_BRACE ) ); //���肦�Ȃ�
	++p;

	//�Q�Ƃ��������݂܂�
	map< RefString, ElementSharedPtr >::iterator it = namedElements->find( refName );
	Parser::dieIf( it == namedElements->end() );
	*elementOut = it->second;

	return p;
}


} //namespace XFile


#ifndef INCLUDED_XFILE_DOCUMENT_H
#define INCLUDED_XFILE_DOCUMENT_H

#include <string>
#include <GameLib/PseudoXml/Document.h>
#include <boost/shared_array.hpp>
#include <tuple>


namespace XFile{

struct Mesh;
typedef boost::shared_array<Mesh> MeshSharedArray;

class Parser;

//XFile�N���X
/*!
XFile����f�[�^�\�����\�z����B
*/
class Document{
public:
	Document( const char* data, int size );
	~Document();
	bool isValid() const;
	///xml������ɕϊ��B
	void convertToString( std::string* ) const;
	GameLib::PseudoXml::Document toXml()const;

	std::tuple<
		MeshSharedArray,
		int // size
	>
	createMeshArray( const std::string & textureFileFolder )const;
private:
	Parser* mParser;
};

} //namespace XFile

#endif

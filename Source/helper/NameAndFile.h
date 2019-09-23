#ifndef __NAMEANDFILE_H__
#define __NAMEANDFILE_H__
#include<GameLib/FileIO/InFile.h>
#include<string>
///�ǂݍ��݃t�@�C���n���h��
class NameAndFile{
public:
	///�t�@�C�����[�h���J�n����
	NameAndFile(){}
	~NameAndFile(){}
	void load( const char* filename ){
		mFileName.assign(filename);
		mFile = GameLib::FileIO::InFile::create( filename );
	}
	///���[�h�e�ʃQ�b�g
	int size() const{
		return mFile.size();
	}
	///���[�h���e�Q�b�g
	const char* data() const{
		return mFile.data();
	}
	///���[�h�I������H
	bool isFinished() const{
		return mFile.isFinished();
	}
	///�G���[�o�Ă܂��H
	bool isError() const{
		return mFile.isError();
	}

	const char* getName()const{
		return mFileName.c_str();
	}
private:
	NameAndFile( const NameAndFile& );
	NameAndFile& operator=( const NameAndFile& );
	std::string mFileName;
	GameLib::FileIO::InFile mFile;
};
#endif

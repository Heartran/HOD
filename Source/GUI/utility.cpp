#include"utility.h"
#include<assert.h>
#include<GameLib/Framework.h>
#include"GUI/ListTexts.h"

#include<boost/filesystem.hpp>
#include<boost/foreach.hpp>
#include<wchar.h>
#include<boost/xpressive/xpressive.hpp>
//#include<boost/regex.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<boost/date_time/local_time_adjustor.hpp>
#include<boost/date_time/c_local_time_adjustor.hpp>

#include<boost/lexical_cast.hpp>
#include<sstream>

#include"helper/MyOutputDebugString.h"

//xml�����[�h�����ē��I�ɃR���g���[���Ƃ����������
//���������A�{�^���̃T�C�Y����ʂɉ����ĕω�����Ƃ����̂��ς��H

namespace GUI{

namespace {
static const int skNumberOfFile = 10;

//"Data/sav/sav\d"�t�@�C���̍ŏI�X�V���Ƒ��݂��邩���`�F�b�N����
//���݂���t�@�C���̂ݍX�V�����̂�file_exsists�ɂ͍ŏ��S��0�����Ă�������
void checkLastUpdateTimesAndExsists( bool file_exsists[], boost::posix_time::ptime last_updates[] ){
	
	namespace fs = boost::filesystem;

	fs::path dir("Data/sav");
	std::string oldfilename;

	//�t�H���_���̑S�Ẵt�@�C����
	BOOST_FOREACH(const fs::path& p, 
		std::make_pair(
		fs::directory_iterator(dir),
		fs::directory_iterator())
		) {
			if (!fs::is_directory(p)){

				//const wchar_t* c= p.filename().string();
				//err = wcstombs_s( p.filename().string., wStrC, 20, wStrW, _TRUNCATE);
				using namespace boost::xpressive;
				//using namespace boost;
				const std::string & filename = p.filename().string();
				assert( oldfilename.empty() | filename>oldfilename );//windows�����̓A���t�@�x�b�g�炵����
				oldfilename=filename;

				sregex rex = as_xpr("sav") >> (s1=_d);// >> _d; 
				smatch what;
				//sav�����̏ꍇ(�ꌅ)
				if (regex_match(filename, what, rex)) {
					int index = boost::lexical_cast<int>(what[1]);
					assert( 0<=index && index<skNumberOfFile );
					file_exsists[index]=true;
					
					//�ŏI�X�V���������Ƃ߂�
					std::time_t last_update = fs::last_write_time(p);
					boost::posix_time::ptime ptime = boost::posix_time::from_time_t(last_update);

					//posix���Ԃ��玩�g�̃^�C���]�[���̎����ɕϊ�
					typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adj;
					last_updates[index] = local_adj::utc_to_local(ptime);	
				}
			}
	}
}

enum Mode{
	Save,
	Load
};
std::shared_ptr<ListTexts> createListText( Mode mode,
	const bool *file_exsists,
	const boost::posix_time::ptime *last_updates )
{
	std::shared_ptr<ListTexts> listTexts = std::make_shared<GUI::ListTexts>(  
		SaveLoadListTextPosSize::x(),
		SaveLoadListTextPosSize::y(),
		SaveLoadListTextPosSize::width(),
		SaveLoadListTextPosSize::height()
	);

	
	//listText�ɏ��Ԃɓ��ꂽ���̂ŁA�ꎞ�ϐ��������œ����
	for( int i=0; i<skNumberOfFile; ++i ){
		std::stringstream ss;
		
		//�����낦����
		if(i+1==skNumberOfFile){
			ss << "file10";
		}else{
			ss << "file0" << i+1;
		}

		if( file_exsists[i] ){
			ss << " " << to_simple_string(last_updates[i]);// << "  �X�e�[�W�N���A�� �N���A���@����ۗL��";
			listTexts->add( ss.str().c_str() );
		} else{
			
				ss << " �f�[�^������܂���";
				listTexts->add( ss.str().c_str() );
				switch(mode){
				case Load:
					//�f�[�^�Ȃ��̂ŉ����Ȃ��悤�ɂ���
					listTexts->setPushable( i, false );
					break;
				case Save:
					break;
				}
		}
	}
	return listTexts;
}

std::shared_ptr<ListTexts> createSaveLoadListText(){
	std::shared_ptr<ListTexts> listTexts = std::make_shared<GUI::ListTexts>(  
		SaveLoadListTextPosSize::x(),
		SaveLoadListTextPosSize::y(),
		SaveLoadListTextPosSize::width(),
		SaveLoadListTextPosSize::height()
	);

	namespace fs = boost::filesystem;

	bool file_exsists[skNumberOfFile] = {0};
	boost::posix_time::ptime last_updates[skNumberOfFile];

	fs::path dir("Data/sav");
	std::string oldfilename;

	//�t�H���_���̑S�Ẵt�@�C����
	BOOST_FOREACH(const fs::path& p, 
		std::make_pair(
		fs::directory_iterator(dir),
		fs::directory_iterator())
		) {
			if (!fs::is_directory(p)){

				//const wchar_t* c= p.filename().string();
				//err = wcstombs_s( p.filename().string., wStrC, 20, wStrW, _TRUNCATE);
				using namespace boost::xpressive;
				//using namespace boost;
				const std::string & filename = p.filename().string();
				assert( oldfilename.empty() | filename>oldfilename );//windows�����̓A���t�@�x�b�g�炵����
				oldfilename=filename;

				sregex rex = as_xpr("sav") >> (s1=_d);// >> _d; 
				smatch what;
				//sav�����̏ꍇ(�ꌅ)
				if (regex_match(filename, what, rex)) {
					int index = boost::lexical_cast<int>(what[1]);
					assert( 0<=index && index<skNumberOfFile );
					file_exsists[index]=true;
					
					//�ŏI�X�V���������Ƃ߂�
					std::time_t last_update = fs::last_write_time(p);
					boost::posix_time::ptime ptime = boost::posix_time::from_time_t(last_update);

					//posix���Ԃ��玩�g�̃^�C���]�[���̎����ɕϊ�
					typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adj;
					last_updates[index] = local_adj::utc_to_local(ptime);
	
				}
			}
	}

	//listText�ɏ��Ԃɓ��ꂽ���̂ŁA�ꎞ�ϐ��������œ����
	for( int i=0; i<skNumberOfFile; ++i ){
		std::stringstream ss;
		
		//�����낦����
		if(i+1==skNumberOfFile){
			ss << "file10";
		}else{
			ss << "file0" << i+1;
		}

		if( file_exsists[i] ){
			ss << " " << to_simple_string(last_updates[i]);// << "  �X�e�[�W�N���A�� �N���A���@����ۗL��";
			listTexts->add( ss.str().c_str() );
		} else{
			ss << " �f�[�^������܂���";
			listTexts->add( ss.str().c_str() );
		}
	}
	return listTexts;
}
}// unnamespace end

namespace ButtonBasicSize{
	int width(){
		return GameLib::Framework::instance().width()/5;

	}
	int height(){
		return GameLib::Framework::instance().height()/10;
	}
}//end namespace ButtonBasicSize

namespace SaveLoadListTextPosSize {
	int width(){
		return GameLib::Framework::instance().width()/3*2;

	}
	int height(){
		return GameLib::Framework::instance().height()/3*2;
	}

	int x(){
		return GameLib::Framework::instance().width()/6;
	}

	int y(){
		return GameLib::Framework::instance().width()/6;
	}
}//end namespace SaveLoadListTextPosSize

void updateSaveListText( int index, std::shared_ptr<ListTexts> list ){
	namespace fs = boost::filesystem;
	
	fs::path path( std::string( "Data/sav/sav"+boost::lexical_cast<std::string>(index) ) );

	std::stringstream ss;

	//�����낦����
	if(index+1==skNumberOfFile){
		ss << "file10";
	}else{
		ss << "file0" << index+1;
	}

	//Setting the modification time to now
	//std::time_t now = std::time( 0 ) ;
	//boost::filesystem::last_write_time( path , now );

	std::time_t last_update = fs::last_write_time(path);
	boost::posix_time::ptime ptime = boost::posix_time::from_time_t(last_update);

	//posix���Ԃ��玩�g�̃^�C���]�[���̎����ɕϊ�
	typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adj;
	boost::posix_time::ptime local = local_adj::utc_to_local(ptime);
	ss << " " << to_simple_string(local);// << "  �X�e�[�W�N���A�� �N���A���@����ۗL��";
	list->setText( index, ss.str().c_str() );
}

std::shared_ptr<ListTexts> createSaveListText(){
	bool file_exsists[skNumberOfFile] = {0};
	boost::posix_time::ptime last_updates[skNumberOfFile];
	checkLastUpdateTimesAndExsists( file_exsists, last_updates );
	return createListText( Save, file_exsists, last_updates );
}
std::shared_ptr<ListTexts> createLoadListText(){
	bool file_exsists[skNumberOfFile] = {0};
	boost::posix_time::ptime last_updates[skNumberOfFile];
	checkLastUpdateTimesAndExsists( file_exsists, last_updates );
	return createListText( Load, file_exsists, last_updates );

}


}//end namespace GUI
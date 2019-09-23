#include"DrawableMaterialNumberRange.h"
#pragma warning(push)
#pragma warning(disable:4819)
#include<boost/numeric/interval.hpp>
#include<boost/numeric/interval/io.hpp>
#pragma warning(pop)
#include<vector>
#include<boost/lexical_cast.hpp>
#include<string.h>
#include<assert.h>
#include<iostream>

namespace MME {

using boost::numeric::interval;

namespace{
	struct In{
		In( unsigned int findingNum )
			:mN(findingNum)
		{
		}

		bool operator()(const boost::numeric::interval<unsigned int> &obj) const{
			return boost::numeric::in( mN, obj );
		}
		unsigned int mN;
	};
}

class DrawableMaterialNumberRange::Impl{
public:
	Impl( const char *text ){
		using boost::lexical_cast;
		const char *p = text;
		const char *first = p;
		int nMin=0;
		bool foundMinus = false;
		while( *(++p)!=NULL ){
			if( *p==','){
				int n = lexical_cast<int>( std::string( first, p ) );
				first =NULL;
				if( foundMinus ){
					push( nMin, n );
					foundMinus = false;
				} else {
					push( n, n );
				}
				assert(
					*(p+1)=='\0' ||
					*(p+1)=='0' ||
					*(p+1)=='1' ||
					*(p+1)=='2' ||
					*(p+1)=='3' ||
					*(p+1)=='4' ||
					*(p+1)=='5' ||
					*(p+1)=='6' ||
					*(p+1)=='7' ||
					*(p+1)=='8' ||
					*(p+1)=='9' && "�L���̎��͐���");
				
			} else if( *p=='-' ){
				assert( !foundMinus );
				foundMinus = true;
				nMin = lexical_cast<unsigned int>( std::string( first, p ) );
				first =NULL;
				assert(
					*(p+1)=='\0' ||
					*(p+1)=='0' ||
					*(p+1)=='1' ||
					*(p+1)=='2' ||
					*(p+1)=='3' ||
					*(p+1)=='4' ||
					*(p+1)=='5' ||
					*(p+1)=='6' ||
					*(p+1)=='7' ||
					*(p+1)=='8' ||
					*(p+1)=='9' && "�L���̎��͐���");
				
			} else if( 
					*p=='0' ||
					*p=='1' ||
					*p=='2' ||
					*p=='3' ||
					*p=='4' ||
					*p=='5' ||
					*p=='6' ||
					*p=='7' ||
					*p=='8' ||
					*p=='9'){
				if( first==NULL ){
					first = p;
				}
			} else {
				// std::cout << "debug:" <<p << std::endl;
				assert(false && "�s���ȕ���" );
			}
		}
		
		if( *(p-1)=='-' ){
			push( nMin, std::numeric_limits<unsigned int>::max() );
		} else if( 
				*(p-1)=='0' ||
				*(p-1)=='1' ||
				*(p-1)=='2' ||
				*(p-1)=='3' ||
				*(p-1)=='4' ||
				*(p-1)=='5' ||
				*(p-1)=='6' ||
				*(p-1)=='7' ||
				*(p-1)=='8' ||
				*(p-1)=='9')
		{
			int n = lexical_cast<unsigned int>( std::string( first, p ) );
			if( foundMinus ){
				push( nMin, n );
			} else {
				push( n, n );
			}
		}

		mRanges.shrink_to_fit();
		debug_print();

	}
	~Impl(){}
	bool canDraw( unsigned int materialIndex ){
		return std::find_if( mRanges.begin(), mRanges.end(), In(materialIndex) ) != mRanges.end();
	}
private:
	void push( unsigned int min, unsigned int max ){
		assert( min <= max );
		mRanges.push_back( interval<unsigned int>(min, max) );
	}
	void debug_print()const{
		for( unsigned int i=0; i<mRanges.size(); ++i ){
			std::cout << mRanges[i] << ", ";
		}
			std::cout << std::endl;
	}

	std::vector< interval< unsigned int > > mRanges;
};

DrawableMaterialNumberRange::DrawableMaterialNumberRange(const char *text)
:mImpl(NULL)
{
	mImpl = new Impl( text );
}

DrawableMaterialNumberRange::~DrawableMaterialNumberRange(){
	delete mImpl;
	mImpl = 0;
}

bool DrawableMaterialNumberRange::canDraw( unsigned int materialIndex ) const {
	return mImpl->canDraw( materialIndex );
}
}

//test
//int main(int argc, char const* argv[])
//{
//	// MME::DrawableMaterialNumberRange( "6-5" );
//	MME::DrawableMaterialNumberRange( "0,3,5" );
//	MME::DrawableMaterialNumberRange( "0,3,5," );
//	MME::DrawableMaterialNumberRange r( "0,3,5-" );
//	MME::DrawableMaterialNumberRange( "0-9," );
//	MME::DrawableMaterialNumberRange( "0-9" );
//	MME::DrawableMaterialNumberRange( "0,3-6," );
//	MME::DrawableMaterialNumberRange( "0,3-6" );
//	MME::DrawableMaterialNumberRange( "12-" );
//	//"0,3,5"�̂悤�ɁA�J���}��؂�Ŕԍ���񋓂��邱�ƂŁA�����̔ԍ����w��ł���B
//	//�܂��A"6-10"�Ȃǂ̂悤�ɁA�ԍ����n�C�t���łȂ����ƂŁA�͈͎w�肪�ł���B
//	//"12-"�̂悤�ɁA�͈͂̊J�n�ԍ��݂̂��w�肵���ꍇ�́A����ȍ~�̑S�Ă̔ԍ����ΏۂƂȂ�B
//
//	assert( !r.canDraw(4) );
//	assert( r.canDraw(40) );
//	assert( r.canDraw(0) );
//	
//	
//	return 0;
//}

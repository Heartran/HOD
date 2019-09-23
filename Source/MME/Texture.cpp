#include"Texture .h"
#include<GameLib/Graphics/Manager.h>
#include<assert.h>
#include"helper/MyOutputDebugString.h"
#include"TextureHelper.h"
#include"helper/ContainerHelper/findByName.h"
namespace MME{


typedef Texture Type;
std::vector< std::weak_ptr<Type> > Type::sCache;
typedef std::vector< std::weak_ptr<Type> >::iterator ItType;

TextureSharedPtr Texture::create(  ID3DXEffect *effect, D3DXHANDLE hTex, const std::string &path ){
	using namespace ContainerHelper;

	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
		
	bool isShared = 0 != (texDesc.Flags & D3DX_PARAMETER_SHARED);

	if( isShared){
		ItType it = sCache|findByName( texDesc.Name );

		//found!
		if( it !=sCache.end() ){
			//not expired
			if(std::shared_ptr<Type> shared = it->lock() ){
				std::shared_ptr<Type> p( new Type( *shared.get(), effect, hTex, path ) );
				return p;
			} else{
				assert(false && "�^�C�~���O�ɂ���Ă̓o�O�肻���ȋC������O�̂���");
				std::shared_ptr<Type> p( new Type( effect, hTex, path ) );
				//�X�V
				*it=p;
				return p;
			}
		}
		//not found! create and push_back to Cache.
		else {
			std::shared_ptr<Type> p( new Type( effect, hTex, path ) );
			sCache.push_back(p);
			return p;
		}
	}
	

	std::shared_ptr<Type> p( new Type( effect, hTex, path ) );
	return p;
}

void Texture::clearCache(){
	sCache.clear();
	sCache.shrink_to_fit();
}
void Texture::shrinkToFitCache(){
	sCache.shrink_to_fit();

}

Texture::~Texture(){
	mTexture.reset();
}

const std::string & Texture::getName()const{
	return mName;
}

bool Texture::isError()const{
	return mTexture==NULL;
}
void Texture::send(ID3DXEffect *effect){
	HRESULT hr;
	hr=effect->SetTexture( mHandle, mTexture.get() );
	assert(SUCCEEDED(hr));
}

Texture::Texture( ID3DXEffect *effect, D3DXHANDLE hTex, const std::string &path ) :
	mTexture(NULL),
	mHandle(NULL)
{
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	HRESULT hr;
	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
		
	//�^��texture, texture2D, texture3D, textureCUBE�̂��������ꂩ�B
	if(
		texDesc.Type==D3DXPT_TEXTURE ||
		//texDesc.Type==D3DXPT_TEXTURE1D ||
		texDesc.Type==D3DXPT_TEXTURE2D ){
		//texDesc.Type==D3DXPT_TEXTURE3D ||
		//texDesc.Type==D3DXPT_TEXTURECUBE ){
	} else{
		assert( false && "not impliment." );
		assert( false && "type must be texture or texture2D, texTure3D, textureCUBE." ); 
		return;
	}
		
	//�Estring ResourceType
	//�e�N�X�`���̎�ނ��w�肷��B"2D", "3D", "CUBE"�̂��������ꂩ���w�肷��B
	//�^�Ɩ��������l�͎w��ł��Ȃ��B
	//
	//�^��"texture"�ł���A����"2D"�ȊO�̃e�N�X�`���𐶐�����ꍇ�ɂ́A
	//���Ȃ炸���̃A�m�e�[�V�������w�肵�Ȃ���΂Ȃ�Ȃ��B����ȊO�ł͏ȗ��\�ł���B
	D3DXHANDLE hResourceType = effect->GetAnnotationByName( hTex, "ResourceType" );
	if( hResourceType ){
		const char *rt=NULL;
		effect->GetString( hResourceType, &rt );
		if( _strcmpi(rt, "2D")==0 ){}
		else if( _strcmpi(rt, "3D")==0 ){
			assert( false && "not impliment." );
			return;
		}
		else if( _strcmpi(rt, "CUBE")==0 ){
			assert( false && "not impliment." );			
			return;
		}
		else {
			assert( false && "ResourceType is wrong." );
			return;
		}
	}

	//�Estring ResourceName
	//�e�N�X�`���̌��ɂȂ�摜�t�@�C�����w�肷��B
	//�T�|�[�g���Ă���t�@�C���t�H�[�}�b�g�́Abmp�A.dds�A.dib�A.jpg�A.png�A����� .tga �ł���B
	//���΃p�X�Ńt�@�C�������w�肵���ꍇ�A�G�t�F�N�g�t�@�C�����i�[����Ă���t�H���_����ƂȂ�B
	D3DXHANDLE hResourceName = effect->GetAnnotationByName( hTex, "ResourceName" );
	if( hResourceName ){
		const char *rname=NULL;
		hr=effect->GetString( hResourceName, &rname );
		assert(SUCCEEDED(hr));
		std::string fileName(path);
		fileName.append(rname);
		IDirect3DTexture9 *pTexture=NULL;			
		if( 
			FAILED( D3DXCreateTextureFromFileA( pDevice, fileName.c_str(), &pTexture ) )
		){
			assert(false && "texture�̍쐬�Ɏ��s���܂����B");
			return;
		}
		mTexture.reset( pTexture, TextureReleaser );
	} else {
		//�Eint Width, int�@Height, int Depth
		//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
		//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
		//	
		//	Dimensions, ViewportRatio �Ƃ͓����Ɏw��ł��Ȃ��B
		//
		//	(Width,Height,Depth), Dimensions, ViewportRatio�̂�������w�肳��Ă��Ȃ��ꍇ�́A
		//	�f�t�H���g�l�́A64�s�N�Z���ł���B
		//	�܂��AResourceName���w�肳��Ă���Ȃ�΁A�摜�t�@�C������T�C�Y�������I�Ɏ擾�����B
		int width=64, height=64, depth=64;

		D3DXHANDLE hWidth=0, hHeight=0, hDepth=0;
		hWidth  = getWidth(	 effect, hTex, &width );
		hHeight = getHeight( effect, hTex, &height );
		hDepth  = getDepth(	 effect, hTex, &depth );

		//�Eint2(or int3) Dimensions
		//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
		//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
		//	
		//	ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
		D3DXHANDLE hDimensions=0;
		hDimensions = getDimensions( effect, hTex, &height, &width, &depth );

		//�Efloat2 ViewportRatio
		//	�e�N�X�`���̕��A�������A�����_�����O�^�[�Q�b�g�̃X�N���[���̃T�C�Y�Ƃ̔�Ŏw�肷��B
		//	�X�N���[���Ɠ����T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {1.0, 1.0};" �Ǝw�肷��B
		//	�X�N���[���̏c��2�{�̃T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {2.0, 2.0};" �Ǝw�肷��B
		//	Dimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
		D3DXHANDLE hViewPortRatio = 0;
		float xRate=1.f, yRate=1.f;

		hViewPortRatio = getViewPortRatio( effect, hTex, &xRate, &yRate );

		if( hViewPortRatio ){
			GameLib::Framework fw = GameLib::Framework::instance();
			width = static_cast<int>(fw.width()*xRate);
			height = static_cast<int>(fw.height()*yRate);
		}

		if( hViewPortRatio && hDimensions ){
			assert( false && "ViewportRatio, hDimensions�Ƃ͓����Ɏw��ł��Ȃ��B" );
			return;
		} else if( hViewPortRatio && (hWidth || hHeight || hDepth) ){
			assert( false && "ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
			return;
		} else if( hDimensions && (hWidth || hHeight || hDepth) ){
			assert( false && "hDimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
			return;
		}
		//�Estring Format
		//	�e�N�X�`���̃t�H�[�}�b�g���w�肷��B
		//	
		//	�ȗ������ꍇ�A"A8R8G8B8"���g�p�����B
		//	ResourceName���w�肳��Ă���Ȃ�΁A�摜�t�@�C������t�H�[�}�b�g���擾����A
		//	���̃A�m�e�[�V�����̐ݒ�͖��������B
		//	
		//	�w��\�ȃt�H�[�}�b�g�́AD3DFORMAT�ihttp://msdn.microsoft.com/ja-jp/library/bb172558(v=VS.85).aspx�j
		//	���Q�ƁB
		//	
		//	"A8R8G8B8"�A"FMT_A8R8G8B8"�A"D3DFMT_A8R8G8B8"�̂�����̏����ł��w��ł���B
		D3DFORMAT format = getFormat( effect, hTex, D3DFMT_A8R8G8B8 );

		//�Eint Miplevels
		//	�~�b�v�}�b�v���w�肵�����x���Ő�������B
		//	�ȗ����邩0���w�肵���ꍇ�A���S�ȃ~�b�v�}�b�v �`�F�[�����쐬�����B
		//	1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��B
		//�Eint Levels
		//	Miplevels�̕ʖ��ł���B
		int miplevels= getMiplevel( effect, hTex, 0 );

		IDirect3DTexture9 *pTexture=NULL;	
		//�e�N�X�`���I�u�W�F�N�g�̍쐬
		if( FAILED(
				D3DXCreateTexture(
				pDevice,
				width, height,
				miplevels,
				0,
				format,
				D3DPOOL_MANAGED,
				&pTexture )
			)
		){
			assert( false && "�e�N�X�`���̍쐬�Ɏ��s���܂���");
		}
		mTexture.reset( pTexture, TextureReleaser );
		assert(false&&"����������΂����C������");
	}

	mName.assign(texDesc.Name);
	mHandle=hTex;

	//D3DSURFACE_DESC desc;

	//mTexture->GetLevelDesc( 0, &desc );
	//desc.
}

Texture::Texture( const Texture &src, ID3DXEffect *effect, D3DXHANDLE hTex, const std::string &path )
	:mTexture(src.mTexture),
	mHandle(hTex),
	mName(src.mName)
{
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	HRESULT hr;
	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
		
	//�^��texture, texture2D, texture3D, textureCUBE�̂��������ꂩ�B
	if(
		texDesc.Type==D3DXPT_TEXTURE ||
		//texDesc.Type==D3DXPT_TEXTURE1D ||
		texDesc.Type==D3DXPT_TEXTURE2D ){
		//texDesc.Type==D3DXPT_TEXTURE3D ||
		//texDesc.Type==D3DXPT_TEXTURECUBE ){
	} else{
		assert( false && "not impliment." );
		assert( false && "type must be texture or texture2D, texTure3D, textureCUBE." ); 
		return;
	}
		
	//�Estring ResourceType
	//�e�N�X�`���̎�ނ��w�肷��B"2D", "3D", "CUBE"�̂��������ꂩ���w�肷��B
	//�^�Ɩ��������l�͎w��ł��Ȃ��B
	//
	//�^��"texture"�ł���A����"2D"�ȊO�̃e�N�X�`���𐶐�����ꍇ�ɂ́A
	//���Ȃ炸���̃A�m�e�[�V�������w�肵�Ȃ���΂Ȃ�Ȃ��B����ȊO�ł͏ȗ��\�ł���B
	D3DXHANDLE hResourceType = effect->GetAnnotationByName( hTex, "ResourceType" );
	if( hResourceType ){
		const char *rt=NULL;
		effect->GetString( hResourceType, &rt );
		if( _strcmpi(rt, "2D")==0 ){}
		else if( _strcmpi(rt, "3D")==0 ){
			assert( false && "not impliment." );
			return;
		}
		else if( _strcmpi(rt, "CUBE")==0 ){
			assert( false && "not impliment." );			
			return;
		}
		else {
			assert( false && "ResourceType is wrong." );
			return;
		}
	}

	//�Estring ResourceName
	//�e�N�X�`���̌��ɂȂ�摜�t�@�C�����w�肷��B
	//�T�|�[�g���Ă���t�@�C���t�H�[�}�b�g�́Abmp�A.dds�A.dib�A.jpg�A.png�A����� .tga �ł���B
	//���΃p�X�Ńt�@�C�������w�肵���ꍇ�A�G�t�F�N�g�t�@�C�����i�[����Ă���t�H���_����ƂȂ�B
	D3DXHANDLE hResourceName = effect->GetAnnotationByName( hTex, "ResourceName" );
	if( hResourceName ){
	} else {
		//�Eint Width, int�@Height, int Depth
		//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
		//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
		//	
		//	Dimensions, ViewportRatio �Ƃ͓����Ɏw��ł��Ȃ��B
		//
		//	(Width,Height,Depth), Dimensions, ViewportRatio�̂�������w�肳��Ă��Ȃ��ꍇ�́A
		//	�f�t�H���g�l�́A64�s�N�Z���ł���B
		//	�܂��AResourceName���w�肳��Ă���Ȃ�΁A�摜�t�@�C������T�C�Y�������I�Ɏ擾�����B
		int width=64, height=64, depth=64;

		D3DXHANDLE hWidth=0, hHeight=0, hDepth=0;
		hWidth  = getWidth(	 effect, hTex, &width );
		hHeight = getHeight( effect, hTex, &height );
		hDepth  = getDepth(	 effect, hTex, &depth );

		//�Eint2(or int3) Dimensions
		//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
		//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
		//	
		//	ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
		D3DXHANDLE hDimensions=0;
		hDimensions = getDimensions( effect, hTex, &height, &width, &depth );

		//�Efloat2 ViewportRatio
		//	�e�N�X�`���̕��A�������A�����_�����O�^�[�Q�b�g�̃X�N���[���̃T�C�Y�Ƃ̔�Ŏw�肷��B
		//	�X�N���[���Ɠ����T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {1.0, 1.0};" �Ǝw�肷��B
		//	�X�N���[���̏c��2�{�̃T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {2.0, 2.0};" �Ǝw�肷��B
		//	Dimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
		D3DXHANDLE hViewPortRatio = 0;
		float xRate=1.f, yRate=1.f;

		hViewPortRatio = getViewPortRatio( effect, hTex, &xRate, &yRate );

		if( hViewPortRatio ){
			GameLib::Framework fw = GameLib::Framework::instance();
			width = static_cast<int>(fw.width()*xRate);
			height = static_cast<int>(fw.height()*yRate);
		}

		if( hViewPortRatio && hDimensions ){
			assert( false && "ViewportRatio, hDimensions�Ƃ͓����Ɏw��ł��Ȃ��B" );
			return;
		} else if( hViewPortRatio && (hWidth || hHeight || hDepth) ){
			assert( false && "ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
			return;
		} else if( hDimensions && (hWidth || hHeight || hDepth) ){
			assert( false && "hDimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
			return;
		}
		//�Estring Format
		//	�e�N�X�`���̃t�H�[�}�b�g���w�肷��B
		//	
		//	�ȗ������ꍇ�A"A8R8G8B8"���g�p�����B
		//	ResourceName���w�肳��Ă���Ȃ�΁A�摜�t�@�C������t�H�[�}�b�g���擾����A
		//	���̃A�m�e�[�V�����̐ݒ�͖��������B
		//	
		//	�w��\�ȃt�H�[�}�b�g�́AD3DFORMAT�ihttp://msdn.microsoft.com/ja-jp/library/bb172558(v=VS.85).aspx�j
		//	���Q�ƁB
		//	
		//	"A8R8G8B8"�A"FMT_A8R8G8B8"�A"D3DFMT_A8R8G8B8"�̂�����̏����ł��w��ł���B
		D3DFORMAT format = getFormat( effect, hTex, D3DFMT_A8R8G8B8 );

		//�Eint Miplevels
		//	�~�b�v�}�b�v���w�肵�����x���Ő�������B
		//	�ȗ����邩0���w�肵���ꍇ�A���S�ȃ~�b�v�}�b�v �`�F�[�����쐬�����B
		//	1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��B
		//�Eint Levels
		//	Miplevels�̕ʖ��ł���B
		int miplevels= getMiplevel( effect, hTex, 0 );

		IDirect3DTexture9 *pTexture=NULL;	
		//�e�N�X�`���I�u�W�F�N�g�̍쐬
		if( FAILED(
				D3DXCreateTexture(
				pDevice,
				width, height,
				miplevels,
				0,
				format,
				D3DPOOL_MANAGED,
				&pTexture )
			)
		){
			assert( false && "�e�N�X�`���̍쐬�Ɏ��s���܂���");
		}
		mTexture.reset( pTexture, TextureReleaser );
		assert(false&&"����������΂����C������");
	}

	assert( mName == texDesc.Name );
}



} // end namespace

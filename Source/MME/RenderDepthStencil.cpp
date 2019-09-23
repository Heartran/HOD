#include"RenderDepthStencil.h"
#include<GameLib/Graphics/Manager.h>
#include<GameLib/Framework.h>
#include<assert.h>
#include"helper/MyOutputDebugString.h"
#include"TextureHelper.h"
#include"helper/ContainerHelper/findByName.h"

namespace MME {

typedef RenderDepthStencil Type;
std::vector< std::weak_ptr<Type> > Type::sCache;
typedef std::vector< std::weak_ptr<Type> >::iterator ItType;

RenderDepthStencilSharedPtr RenderDepthStencil::create(  ID3DXEffect *effect, D3DXHANDLE hTex ){
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
				std::shared_ptr<Type> p( new Type( *shared.get(), effect, hTex ) );
				return p;
			} else{
				assert(false && "�^�C�~���O�ɂ���Ă̓o�O�肻���ȋC������O�̂���");
				std::shared_ptr<Type> p( new Type( effect, hTex ) );
				//�X�V
				*it=p;
				return p;
			}
		}
		//not found! create and push_back to Cache.
		else {
			std::shared_ptr<Type> p( new Type( effect, hTex ) );
			sCache.push_back(p);
			return p;
		}
	}
	

	std::shared_ptr<Type> p( new Type( effect, hTex ) );
	return p;
}

void RenderDepthStencil::clearCache(){
	sCache.clear();
	sCache.shrink_to_fit();
}
void RenderDepthStencil::shrinkToFitCache(){
	sCache.shrink_to_fit();

}
RenderDepthStencil::~RenderDepthStencil(){
}
bool RenderDepthStencil::isError()const{
	return mZBuffer==NULL;
}
	
//IDirect3DSurface9* RenderDepthStencil::getZBuffer(){
//	return mZBuffer;
//}

void RenderDepthStencil::setDepthStencilSurface(){
	HRESULT hr;
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();

	hr=pDevice->SetDepthStencilSurface( mZBuffer.get() );				
	assert(SUCCEEDED(hr));
}

const std::string & RenderDepthStencil::getName()const{
	return mName;
}

void RenderDepthStencil::send(ID3DXEffect *effect){
	HRESULT hr;
	hr=effect->SetTexture( mHandle, mTexture.get() );
	assert(SUCCEEDED(hr));
}

//�f�o�C�X���X�g���̂��߂̊J������
void RenderDepthStencil::backup(){
	mZBuffer.reset();
	mTexture.reset();
}
//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
void RenderDepthStencil::recover(){
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	
	HRESULT hr;
	//1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��B
	int miplevels=1;
	IDirect3DTexture9 *pTexture=NULL;

	//RENDERCOLORTARGET�Ő��������e�N�X�`���Ƃ͈Ⴂ�A���̃Z�}���e�B�N�X�Ő��������e�N�X�`���́A
	//�����_�����O����A���e���Q�Ƃ��邱�Ƃ͂ł��Ȃ��B
	//�ƂȂ��Ă���̂ŁA�T�[�t�F�C�X����Ȃ�������
	if( FAILED(
			D3DXCreateTexture(
			pDevice,
			mWidth, mHeight,
			miplevels,
			D3DUSAGE_DEPTHSTENCIL,
			mFormat,
			D3DPOOL_DEFAULT,
			&pTexture )
		)
	){
		assert( false && "�e�N�X�`���̍쐬�Ɏ��s���܂���");
		return;
	}
	mTexture.reset( pTexture, TextureReleaser );

	IDirect3DSurface9 *pSurface=NULL;
	hr=mTexture->GetSurfaceLevel(0, &pSurface);		 
	assert(SUCCEEDED(hr));
	mZBuffer.reset( pSurface, SurfaceReleaser );
}


RenderDepthStencil::RenderDepthStencil( ID3DXEffect *effect, D3DXHANDLE hTex) :
	mTexture(NULL),
	mZBuffer(NULL),
	mHandle(NULL),
	mWidth(0),
	mHeight(0),
	mFormat(D3DFMT_D24S8)

{
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	GameLib::Framework fw = GameLib::Framework::instance();
	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
	//��RENDERDEPTHSTENCILTARGET
	assert( _strcmpi( texDesc.Semantic, "RENDERDEPTHSTENCILTARGET" )==0 );
	//�^��texture, texture2D�̂��������ꂩ�B
	if(
		texDesc.Type==D3DXPT_TEXTURE ||
		//texDesc.Type==D3DXPT_TEXTURE1D ||
		texDesc.Type==D3DXPT_TEXTURE2D ){
	} else{
		assert( false && "type must be texture or texture2D." ); 
		return;
	}
		
	//�Eint Width, int�@Height, int Depth
	//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
	//	
	//	Dimensions, ViewportRatio �Ƃ͓����Ɏw��ł��Ȃ��B
	//	�ȗ������ꍇ�A"float2 ViewportRatio = {1.0, 1.0};" �Ƃ����ݒ肪�g�p�����B
	int width=fw.width(), height=fw.height();

	D3DXHANDLE hWidth=0, hHeight=0;
	hWidth  = getWidth(	 effect, hTex, &width );
	hHeight = getHeight( effect, hTex, &height );

	//�Eint2 Dimensions
	//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
	//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
	//	
	//	ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
	D3DXHANDLE hDimensions=0;
	int dummy;
	hDimensions = getDimensions( effect, hTex, &height, &width, &dummy );

	//�Efloat2 ViewportRatio
	//	�e�N�X�`���̕��A�������A�����_�����O�^�[�Q�b�g�̃X�N���[���̃T�C�Y�Ƃ̔�Ŏw�肷��B
	//	�X�N���[���Ɠ����T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {1.0, 1.0};" �Ǝw�肷��B
	//	�X�N���[���̏c��2�{�̃T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {2.0, 2.0};" �Ǝw�肷��B
	//	Dimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
	//	"���ʏ�e�N�X�`��"���Q�ƁB
	//	�ȗ������ꍇ�A"float2 ViewportRatio = {1.0, 1.0};" �Ƃ����ݒ肪�g�p�����B
	D3DXHANDLE hViewPortRatio = 0;
	float xRate=1.f;
	float yRate=1.f;
	hViewPortRatio = getViewPortRatio( effect, hTex, &xRate, &yRate );
	
	if( hWidth && hHeight ){
		mWidth=width;
		mHeight=height;
	}else if( hDimensions ){
		mWidth=width;
		mHeight=height;
	}else if( hViewPortRatio ){
		mWidth = static_cast<unsigned int>(static_cast<float>(fw.width())*xRate);
		mHeight = static_cast<unsigned int>(static_cast<float>(fw.height())*yRate);
	}else {
		mWidth=fw.width();
		mHeight=fw.height();
	}	

	if( hViewPortRatio && hDimensions ){
		assert( false && "ViewportRatio, hDimensions�Ƃ͓����Ɏw��ł��Ȃ��B" );
		return;
	} else if( hViewPortRatio && (hWidth || hHeight ) ){
		assert( false && "ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
		return;
	} else if( hDimensions && (hWidth || hHeight ) ){
		assert( false && "hDimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
		return;
	}

	//�Estring Format
	//	�e�N�X�`���̃t�H�[�}�b�g���w�肷��B
	//	�ȗ������ꍇ�A"D24S8"���g�p�����B
	mFormat = getFormatRenderStencil( effect, hTex );
	
	//1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��B
	int miplevels=1;

	recover();

	mName.assign(texDesc.Name);
	mHandle=hTex;
}

RenderDepthStencil::RenderDepthStencil( const RenderDepthStencil &src, ID3DXEffect *effect, D3DXHANDLE hTex)
	:mTexture(src.mTexture),
	mZBuffer(src.mZBuffer),
	mHandle(hTex),
	mWidth(src.mWidth),
	mHeight(src.mHeight),
	mFormat(src.mFormat),
	mName(src.mName)
{
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	GameLib::Framework fw = GameLib::Framework::instance();
	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
	//��RENDERDEPTHSTENCILTARGET
	assert( _strcmpi( texDesc.Semantic, "RENDERDEPTHSTENCILTARGET" )==0 );
	//�^��texture, texture2D�̂��������ꂩ�B
	if(
		texDesc.Type==D3DXPT_TEXTURE ||
		//texDesc.Type==D3DXPT_TEXTURE1D ||
		texDesc.Type==D3DXPT_TEXTURE2D ){
	} else{
		assert( false && "type must be texture or texture2D." ); 
		return;
	}
		
	//�Eint Width, int�@Height, int Depth
	//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
	//	
	//	Dimensions, ViewportRatio �Ƃ͓����Ɏw��ł��Ȃ��B
	//	�ȗ������ꍇ�A"float2 ViewportRatio = {1.0, 1.0};" �Ƃ����ݒ肪�g�p�����B
	int width=fw.width(), height=fw.height();

	D3DXHANDLE hWidth=0, hHeight=0;
	hWidth  = getWidth(	 effect, hTex, &width );
	hHeight = getHeight( effect, hTex, &height );

	//�Eint2 Dimensions
	//	�e�N�X�`���̕��A�����A�[�����s�N�Z���P�ʂŎw�肷��B
	//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
	//	
	//	ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
	D3DXHANDLE hDimensions=0;
	int dummy;
	hDimensions = getDimensions( effect, hTex, &height, &width, &dummy );

	//�Efloat2 ViewportRatio
	//	�e�N�X�`���̕��A�������A�����_�����O�^�[�Q�b�g�̃X�N���[���̃T�C�Y�Ƃ̔�Ŏw�肷��B
	//	�X�N���[���Ɠ����T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {1.0, 1.0};" �Ǝw�肷��B
	//	�X�N���[���̏c��2�{�̃T�C�Y�̃e�N�X�`���𐶐�����ɂ́A"float2 ViewportRatio = {2.0, 2.0};" �Ǝw�肷��B
	//	Dimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B
	//	"���ʏ�e�N�X�`��"���Q�ƁB
	//	�ȗ������ꍇ�A"float2 ViewportRatio = {1.0, 1.0};" �Ƃ����ݒ肪�g�p�����B
	D3DXHANDLE hViewPortRatio = 0;
	float xRate=1.f;
	float yRate=1.f;
	hViewPortRatio = getViewPortRatio( effect, hTex, &xRate, &yRate );
	
	if( hWidth && hHeight ){
	}else if( hDimensions ){
	}else if( hViewPortRatio ){
		width = static_cast<unsigned int>(static_cast<float>(fw.width())*xRate);
		height = static_cast<unsigned int>(static_cast<float>(fw.height())*yRate);
	}else {
		width=fw.width();
		height=fw.height();
	}
	assert( mHeight == height );
	assert( mWidth == width );

	if( hViewPortRatio && hDimensions ){
		assert( false && "ViewportRatio, hDimensions�Ƃ͓����Ɏw��ł��Ȃ��B" );
		return;
	} else if( hViewPortRatio && (hWidth || hHeight ) ){
		assert( false && "ViewportRatio, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
		return;
	} else if( hDimensions && (hWidth || hHeight ) ){
		assert( false && "hDimensions, Width, Height, Depth�Ƃ͓����Ɏw��ł��Ȃ��B" );
		return;
	}

	//�Estring Format
	//	�e�N�X�`���̃t�H�[�}�b�g���w�肷��B
	//	�ȗ������ꍇ�A"D24S8"���g�p�����B
	assert( mFormat == getFormatRenderStencil( effect, hTex ) );
	
	//1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��B
	int miplevels=1;

	assert( mName == texDesc.Name );
}


}// end namespace MME
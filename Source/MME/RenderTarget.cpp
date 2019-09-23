#include"RenderTarget.h"
#include<GameLib/Graphics/Manager.h>
#include<GameLib/Framework.h>
#include<assert.h>
#include"helper/MyOutputDebugString.h"
#include<string>
#include"TextureHelper.h"
#include"BaseEffect.h"
#include"helper/ContainerHelper/findByName.h"



namespace MME{

typedef RenderTarget Type;
std::vector< std::weak_ptr<Type> > Type::sCache;
typedef std::vector< std::weak_ptr<Type> >::iterator ItType;

RenderTargetSharedPtr RenderTarget::create(  ID3DXEffect *effect, D3DXHANDLE hTex ){
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
				RenderTargetSharedPtr p( new Type( *shared.get(), effect, hTex ) );
				return p;
			} else{
				assert(false && "�^�C�~���O�ɂ���Ă̓o�O�肻���ȋC������O�̂���");
				RenderTargetSharedPtr p( new Type( effect, hTex ) );
				//�X�V
				*it=p;
				return p;
			}
		}
		//not found! create and push_back to Cache.
		else {
			RenderTargetSharedPtr p( new Type( effect, hTex ) );
			sCache.push_back(p);
			return p;
		}
	}
	

	RenderTargetSharedPtr p( new RenderTarget(effect, hTex ) );
	return p;
}

RenderTarget::~RenderTarget(){
}
bool RenderTarget::isError()const{
	return mTexture==NULL;
}
void RenderTarget::send(ID3DXEffect *effect){
	HRESULT hr;
	hr=effect->SetTexture( mHandle, mTexture.get() );
	assert(SUCCEEDED(hr));
		
	//D3DXPARAMETER_DESC desc;
	//effect->GetParameterDesc( mHandle, &desc );

}

//IDirect3DSurface9* RenderTarget::getSurfLevel( UINT level ){
//	IDirect3DSurface9 *result=0;
//	HRESULT hr;
//	hr=mTexture->GetSurfaceLevel( level, &result );
//	assert(SUCCEEDED(hr));
//	return result;
//}


//���O���ړI��Ǝ��ł��Ԃ��Ă��邪�킩��₷�����炱�̖��O�ɂ���
//�����Ƃ��Ă�
//	setToDevice
//	setSelf
void RenderTarget::setRenderTarget( int renderTargetIndex, BaseEffect* effect ){
	HRESULT hr;
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	
	//�����_�����O�^�[�Q�b�g���e�N�X�`���ɃZ�b�g
	hr=pDevice->SetRenderTarget( renderTargetIndex, mSurface.get() );				
	assert(SUCCEEDED(hr));
	
	if( renderTargetIndex==0 )
	{
		D3DSURFACE_DESC desc;
		hr=mSurface->GetDesc( &desc );
		assert(SUCCEEDED(hr));
	
		//set viewport
		D3DVIEWPORT9 vp;
		vp.X = vp.Y = 0;
		vp.Width = desc.Width;
		vp.Height = desc.Height;
		vp.MinZ = 0.f;
		vp.MaxZ = 1.f;
		hr=pDevice->SetViewport( &vp );
		assert(SUCCEEDED(hr));
		
		//�����̒l�́AMMD�̃X�N���[���������̓I�t�X�N���[�������_�����O�^�[�Q�b�g�̃T�C�Y���w���B
		//Scripts��"RenderColorTarget"�R�}���h�ŁA�����_�����O�^�[�Q�b�g���ύX���Ă��A
		//���̒l�͕ω����Ȃ��B
		//effect->setViewportPixelSize( static_cast<float>(vp.Width), static_cast<float>(vp.Height) );
	}
}

const std::string & RenderTarget::getName()const{
	return mName;
}

//�f�o�C�X���X�g���̂��߂̊J������
void RenderTarget::backup(){
	mTexture.reset();
	mSurface.reset();
}
//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
void RenderTarget::recover(){
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
		
	IDirect3DTexture9 *pTexture=NULL;
		
	HRESULT hr;
	//�����_�����O�^�[�Q�b�g�ɂȂ�e�N�X�`���I�u�W�F�N�g�̍쐬
	if( FAILED(
			D3DXCreateTexture(
			pDevice,
			mWidth, mHeight,
			mMipLevels,
			D3DUSAGE_RENDERTARGET,
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
	mSurface.reset( pSurface, SurfaceReleaser );
}


RenderTarget::RenderTarget( ID3DXEffect *effect, D3DXHANDLE hTex) :
	mTexture(NULL, TextureReleaser ),
	mHandle(0),
	mSurface(NULL, SurfaceReleaser ),
	mWidth(0),
	mHeight(0),
	mMipLevels(1),
	mFormat(D3DFMT_A8R8G8B8),
	mIsShared(false)
{
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	GameLib::Framework fw = GameLib::Framework::instance();
	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
	
	mName.assign(texDesc.Name);

	mIsShared = 0 != (texDesc.Flags & D3DX_PARAMETER_SHARED);
		
	//��RENDERCOLORTARGET
	assert( _strcmpi( texDesc.Semantic, "RENDERCOLORTARGET" )==0 );
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
	//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
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
	float xRate=1.f, yRate=1.f;
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
	//	
	//	�ȗ������ꍇ�A"A8R8G8B8"���g�p�����B
	//	ResourceName���w�肳��Ă���Ȃ�΁A�摜�t�@�C������t�H�[�}�b�g���擾����A
	//	���̃A�m�e�[�V�����̐ݒ�͖��������B
	//	
	//	�w��\�ȃt�H�[�}�b�g�́AD3DFORMAT�ihttp://msdn.microsoft.com/ja-jp/library/bb172558(v=VS.85).aspx�j
	//	���Q�ƁB
	//	
	//	"A8R8G8B8"�A"FMT_A8R8G8B8"�A"D3DFMT_A8R8G8B8"�̂�����̏����ł��w��ł���B
	mFormat = getFormat( effect, hTex, D3DFMT_A8R8G8B8 );


	//�Eint Miplevels
	//�Eint Levels
	//�@�@"���ʏ�e�N�X�`��"���Q�ƁB
	//�@�@1�܂���0���w��ł���B
	//�@�@0���w�肵���ꍇ�A���S�ȃ~�b�v�}�b�v �`�F�[�����쐬�����B
	//�@�@1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��i�f�t�H���g�j�B
	mMipLevels= getMiplevel( effect, hTex, 1 );

	recover();

	mHandle=hTex;
}

//���ʂ̒l���m���߂�
RenderTarget::RenderTarget( const RenderTarget &src, ID3DXEffect *effect, D3DXHANDLE hTex )
	:mTexture(src.mTexture),
	mHandle(hTex),
	mSurface(src.mSurface ),
	mWidth(src.mWidth),
	mHeight(src.mHeight),
	mMipLevels(src.mMipLevels),
	mFormat(src.mFormat),
	mIsShared(src.mIsShared),
	mName( src.mName )
{
	GameLib::Framework fw = GameLib::Framework::instance();
	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
		
	assert( mIsShared == 0 != (texDesc.Flags & D3DX_PARAMETER_SHARED) );
		
	//��RENDERCOLORTARGET
	assert( _strcmpi( texDesc.Semantic, "RENDERCOLORTARGET" )==0 );
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
	//	�[�����w�肷��̂̓{�����[���e�N�X�`���̏ꍇ�݂̂ł���B
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
	float xRate=1.f, yRate=1.f;
	hViewPortRatio = getViewPortRatio( effect, hTex, &xRate, &yRate );

	if( hWidth && hHeight ){
		assert(	mWidth	==	width);
		assert( mHeight	==	height );
	}else if( hDimensions ){
		assert(	mWidth	==	width);
		assert( mHeight	==	height );
	}else if( hViewPortRatio ){
		width = static_cast<unsigned int>(static_cast<float>(fw.width())*xRate);
		height = static_cast<unsigned int>(static_cast<float>(fw.height())*yRate);
		assert(	mWidth	==	width);
		assert( mHeight	==	height );
	}else {
		width=fw.width();
		height=fw.height();
		assert(	mWidth	==	width);
		assert( mHeight	==	height );
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
	//	
	//	�ȗ������ꍇ�A"A8R8G8B8"���g�p�����B
	//	ResourceName���w�肳��Ă���Ȃ�΁A�摜�t�@�C������t�H�[�}�b�g���擾����A
	//	���̃A�m�e�[�V�����̐ݒ�͖��������B
	//	
	//	�w��\�ȃt�H�[�}�b�g�́AD3DFORMAT�ihttp://msdn.microsoft.com/ja-jp/library/bb172558(v=VS.85).aspx�j
	//	���Q�ƁB
	//	
	//	"A8R8G8B8"�A"FMT_A8R8G8B8"�A"D3DFMT_A8R8G8B8"�̂�����̏����ł��w��ł���B
	assert( mFormat == getFormat( effect, hTex, D3DFMT_A8R8G8B8 ) );

	//�Eint Miplevels
	//�Eint Levels
	//�@�@"���ʏ�e�N�X�`��"���Q�ƁB
	//�@�@1�܂���0���w��ł���B
	//�@�@0���w�肵���ꍇ�A���S�ȃ~�b�v�}�b�v �`�F�[�����쐬�����B
	//�@�@1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��i�f�t�H���g�j�B
	assert( mMipLevels == getMiplevel( effect, hTex, 1 ) );

	assert(mName==texDesc.Name);
}

void RenderTarget::clearCache(){
	sCache.clear();
	sCache.shrink_to_fit();
}
void RenderTarget::shrinkToFitCache(){
	sCache.shrink_to_fit();
}

}// end namespace MME
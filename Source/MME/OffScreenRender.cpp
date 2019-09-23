#include"OffScreenRender.h"
#include<GameLib/Graphics/Manager.h>
#include<GameLib/Framework.h>
#include<assert.h>
#include"helper/MyOutputDebugString.h"
#include"helper/ContainerHelper/findByName.h"
#include"helper/ContainerHelper/safeDeleteAll.h"
#include"BaseEffect.h"
#include"TextureHelper.h"
#include"Manager.h"
#include<boost/xpressive/xpressive.hpp>
#include"helper/parseEquals.h"
#include<assert.h>
#include"helper/ReplaceTxt.h"
#include"Object/IObject.h"

namespace MME {

struct OffScreenRender::DefalutEffect{
	DefalutEffect( const boost::xpressive::sregex &reg, const std::string &str )
		:mModelFile(reg),
		mFxFileName(str)
	{}
	boost::xpressive::sregex mModelFile;
	std::string mFxFileName;
};

typedef OffScreenRender Type;
std::vector< std::weak_ptr<Type> > Type::sCache;
typedef std::vector< std::weak_ptr<Type> >::iterator ItType;

OffScreenRenderSharedPtr OffScreenRender::create(
	ID3DXEffect *effect,
	D3DXHANDLE hTex,
	const std::string& currentPath,
	LPD3DXEFFECTPOOL pool )
{
	using namespace ContainerHelper;

	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
		
	bool isShared = 0 != (texDesc.Flags & D3DX_PARAMETER_SHARED);

	if( isShared){
		assert( false && "�����������ƂȂ��A�o�O�̉\�����X����" );
		//�Ή�����
		ItType it = sCache|findByName( texDesc.Name );

		//found!
		if( it !=sCache.end() ){
			//not expired
			if(std::shared_ptr<Type> shared = it->lock() ){
				std::shared_ptr<Type> p( new Type( *shared.get(), effect, hTex, currentPath, pool ) );
				return p;
			} else{
				assert(false && "�^�C�~���O�ɂ���Ă̓o�O�肻���ȋC������O�̂���");
				std::shared_ptr<Type> p( new Type( effect, hTex, currentPath, pool ) );
				//�X�V
				*it=p;
				return p;
			}
		}
		//not found! create and push_back to Cache.
		else {
			std::shared_ptr<Type> p( new Type( effect, hTex, currentPath, pool ) );
			sCache.push_back(p);
			return p;
		}
	}
	

	std::shared_ptr<Type> p( new Type( effect, hTex, currentPath, pool ) );
	return p;
}

void OffScreenRender::clearCache(){
	sCache.clear();
	sCache.shrink_to_fit();
}
void OffScreenRender::shrinkToFitCache(){
	sCache.shrink_to_fit();
}

OffScreenRender::~OffScreenRender(){
	mDefalutEffects|ContainerHelper::safeDeleteAll();
	SAFE_RELEASE(mTempBackbuffer);
	SAFE_RELEASE(mTempBackZ);
}

bool OffScreenRender::isError()const{
	return mTexture==NULL;
}
void OffScreenRender::send(ID3DXEffect *effect){
	HRESULT hr;
	hr=effect->SetTexture( mHandle, mTexture.get() );
	assert(SUCCEEDED(hr));
		
	//D3DXPARAMETER_DESC desc;
	//effect->GetParameterDesc( mHandle, &desc );

}

void OffScreenRender::draw(){
	//HRESULT hr;
	//GameLib::Graphics::Manager m;
	//LPDIRECT3DDEVICE9 pDevice = m.dxDevice();

	//// �o�b�N�o�b�t�@�[�̑ޔ�p
	////LPDIRECT3DSURFACE9      backbuffer;	// �o�b�N�o�b�t�@�[
	////LPDIRECT3DSURFACE9		backZ;		// �o�b�N�o�b�t�@�[�p�̐[�x�o�b�t�@�[


	//// �r���[�|�[�g���̕ێ�
	////D3DVIEWPORT9 viewport;
	//pDevice->GetViewport(&mTempViewport);

	//// �o�b�N�o�b�t�@�[�ւ̃C���^�[�t�F�[�X�̕ێ�
	//hr=pDevice->GetRenderTarget(0, &mTempBackbuffer);
	//assert( SUCCEEDED(hr) );

	//// �[�x�o�b�t�@�[�̃C���^�[�t�F�[�X�̕ێ�
	//hr=pDevice->GetDepthStencilSurface( &mTempBackZ); 
	//assert( SUCCEEDED(hr) );

	//// �e�N�X�`���������_�����O�^�[�Q�b�g�ɐݒ�
	//hr=pDevice->SetRenderTarget(0, mSurface.get());
	//assert( SUCCEEDED(hr) );

	//// �e�N�X�`���p�̐[�x�o�b�t�@��ݒ�
	//hr=pDevice->SetDepthStencilSurface(mZBuffer.get());
	//assert( SUCCEEDED(hr) );

	//// �����_�����O�^�[�Q�b�g��������
	//hr=pDevice->Clear(0,NULL, 
	//	D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 
	//	D3DXCOLOR(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w),
	//	mClearDepth, 0);
	//assert( SUCCEEDED(hr) );

	//// �r���[�|�[�g���e�N�X�`���摜�̃T�C�Y�ɐݒ�
	//D3DSURFACE_DESC desc;
	//mTexture->GetLevelDesc( 0, &desc );
	//D3DVIEWPORT9 viewport2 = { 0, 0, desc.Width, desc.Height, 0.0f, 1.0f };
	//pDevice->SetViewport(&viewport2);

	//	�`��^�C�~���O�́A�ȉ��̒ʂ�ł��B
	//(1) �Z���t�V���h�E�pZ�o�b�t�@�`��(zplot)
	//(2) OFFSCREENRENDERTARGET�ꎮ�`��
	//(3) �|�X�g�G�t�F�N�g��Script�́A�擪����"ScriptExternal=Color"�܂�
	//(4) �v���G�t�F�N�g
	//(5) ���f��������ɕ`�悳���A�N�Z�T���ꎮ�`��iobject, object_ss�j
	//(6) �n�ʉe�ꎮ�`�ʁishadow�j ���������A�N�Z�̕`�揇���͕s��
	//(7) ���f���ꎮ�`��iobject, object_ss�j
	//(8) �G�b�W�ꎮ�`��iedge�j
	//(9) ���f������ɕ`�ʂ����A�N�Z�T���ꎮ�`�ʁiobject, object_ss�j
	//(10) �|�X�g�G�t�F�N�g��Script�́A"ScriptExternal=Color"����Ō�܂�
	//��(5)�`(9)�ɂ��ẮAMMD���̕`�揇���ɗR�����܂��B

	begin();

	//�I�t�X�N���[���p�̃V���h�E�o�b�t�@���K�v�A�������ĂȂ�
	//beginShadow();
	//for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){
	//	mEffects[i]->drawZPlot(); 
	//}
	//endShadow();

	//offscreen��offscren�͓���Ȃ��C������(���x�I��)	
	for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){
		mEffects[i]->drawOffScreen(); 
	}

	for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){
		StandardEffect *e = mEffects[i].get();
		e->drawObject();
		//e->drawObjectSS();//�e���`��̂ق��������C�����邪

	}

	//�x���̂ŃR�����g�A�E�g
	//for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){
	//	StandardEffect *e = mEffects[i].get();
	//	e->drawEdge();
	//}

	
	end();

	//// �ۑ����Ă������o�b�N�o�b�t�@�������_�����O�^�[�Q�b�g�ɐݒ�
	//hr=pDevice->SetRenderTarget(0, mTempBackbuffer);
	//assert( SUCCEEDED(hr) );
	//SAFE_RELEASE(mTempBackbuffer);

	//// �ۑ����Ă������o�b�N�o�b�t�@�̐[�x�o�b�t�@��ݒ�
	//hr=pDevice->SetDepthStencilSurface(mTempBackZ);
	//assert( SUCCEEDED(hr) );
	//SAFE_RELEASE(mTempBackZ);


	//// �ۑ����Ă������r���[�|�[�g�����ɖ߂�
	//hr=pDevice->SetViewport(&mTempViewport);
	//assert( SUCCEEDED(hr) );
}
LPDIRECT3DTEXTURE9 OffScreenRender::getDxTexture(){
	return mTexture.get();
}


const std::string & OffScreenRender::getName()const{
	return mName;
}

//void OffScreenRender::removeRemovableObjects(){
//	for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){		
//		mEffects[i]->removeRemovableObjects();
//	}
//}

StandardEffectSharedPtr OffScreenRender::findStandardEffect( const std::string &name ){
	using namespace ContainerHelper;
	std::vector<StandardEffectSharedPtr>::iterator it = mEffects|findByName2( name.c_str() );
	if( it==mEffects.end() ) {
		return NULL;
	} else {
		return *it;
	}
}

//�f�o�C�X���X�g���̂��߂̊J������
void OffScreenRender::backup(){
	mTexture.reset();
	mSurface.reset();
	mZBuffer.reset();

	for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){
		mEffects[i]->backup(); 
	}


}
//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
void OffScreenRender::recover(){
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();

	HRESULT hr;

	IDirect3DTexture9 *pTexture=NULL;

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
		assert( false && "offscreen�e�N�X�`���̍쐬�Ɏ��s���܂���");
		return;
	}
	mTexture.reset( pTexture, TextureReleaser );

	IDirect3DSurface9 *pZBuffer;
	if(
		FAILED(
			pDevice->CreateDepthStencilSurface(
				mWidth, mHeight,
				D3DFMT_D24S8,
				D3DMULTISAMPLE_NONE,
				0,
				TRUE,
				&pZBuffer,
				NULL )
		)
	)
	{
		assert( false && "offscreen�p��Z�o�b�t�@�쐬�Ɏ��s���܂���");	
	}
	mZBuffer.reset( pZBuffer, SurfaceReleaser );


	IDirect3DSurface9 *pSurface=NULL;
	hr=mTexture->GetSurfaceLevel(0, &pSurface);		 
	assert(SUCCEEDED(hr));
	mSurface.reset( pSurface, SurfaceReleaser );
	
	for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){
		mEffects[i]->recover(); 
	}
		
}

OffScreenRender::OffScreenRender( ID3DXEffect *effect, D3DXHANDLE hTex, const std::string& currentPath, LPD3DXEFFECTPOOL pool ) :
	mTexture(NULL),
	mHandle(NULL),
	mSurface(NULL),
	mZBuffer(NULL),
	mTempBackbuffer(NULL),
	mTempBackZ(NULL),
	mWidth(0),
	mHeight(0),
	mMipLevels(1),
	mFormat(D3DFMT_A8R8G8B8)
{
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	GameLib::Framework fw = GameLib::Framework::instance();
	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
	//��OFFSCREENRENDERTARGET
	assert( _strcmpi( texDesc.Semantic, "OFFSCREENRENDERTARGET" )==0 );
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

	//D3DFORMAT formatZ = getFormatRenderStencil( effect, hTex );


	//�Eint Miplevels
	//�Eint Levels
	//�@�@"���ʏ�e�N�X�`��"���Q�ƁB
	//�@�@1�܂���0���w��ł���B
	//�@�@0���w�肵���ꍇ�A���S�ȃ~�b�v�}�b�v �`�F�[�����쐬�����B
	//�@�@1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��i�f�t�H���g�j�B
	mMipLevels= getMiplevel( effect, hTex, 1 );

	//	�Efloat4 ClearColor
	//�@�@�����_�����O�^�[�Q�b�g���N���A����F��ݒ肷��B
	//�@�@�����Ŏw�肵���F�ŁA�����_�����O�^�[�Q�b�g�͎����I�ɃN���A�����B
	//
	D3DXHANDLE hClearColor = effect->GetAnnotationByName( hTex, "ClearColor" );
	mClearColor.set( 0.f, 0.f, 0.f, 0.f );
	if( hClearColor ){
		float c[4]={0};
		hr=effect->GetFloatArray( hClearColor, c, 4 );
		assert( SUCCEEDED(hr) );
		mClearColor.set( c[0], c[1], c[2], c[3] );
	}

	//�Efloat ClearDepth
	//�@�@�[�x�X�e���V���T�[�t�F�C�X���N���A����Z�l��ݒ肷��B
	//�@�@�����Ŏw�肵��Z�l�ŁA�[�x�X�e���V���T�[�t�F�C�X�͎����I�ɃN���A�����B
	D3DXHANDLE hClearDepth = effect->GetAnnotationByName( hTex, "ClearDepth" );
	mClearDepth = 0.f;
	if( hClearDepth ){
		hr=effect->GetFloat( hClearDepth, &mClearDepth );
		assert( SUCCEEDED(hr) );
	}

	//�Ebool AntiAlias
	//�@�@�����_�����O�ɃA���`�G�C���A�X���g�p����B�f�t�H���g��false�B
	//�@�@���A���`�G�C���A�X���L���̏ꍇ�A�r�f�I�������͑��������邽�߁A
	//�@�@�@�e�N�X�`���̃T�C�Y�ɂ͒��ӂ��邱��
	D3DXHANDLE hAntiAilias = effect->GetAnnotationByName( hTex, "AntiAlias" );
	mIsAntiAilias = false;
	if( hAntiAilias ){
		TRACE( "not impl offscreen antiailias" );
		hr=effect->GetBool( hAntiAilias, &mIsAntiAilias );
		assert( SUCCEEDED(hr) );
	}

	//�Estring Description
	//�@�@�I�t�X�N���[�������_�����O�^�[�Q�b�g�̐��������w�肷��B
	//�@�@�����Ŏw�肵�������񂪁A�G�t�F�N�g������GUI�_�C�A���O��ŕ\�������B
	D3DXHANDLE hDescription = effect->GetAnnotationByName( hTex, "Description" );
		
	if( hDescription ){
		const char *string;
		hr=effect->GetString( hDescription, &string );
		mDescription.assign( string );
		assert( SUCCEEDED(hr) );
	}


	//�Estring DefaultEffect
	//�@�@�I�t�X�N���[�������_�����O�Ŏg�p����G�t�F�N�g�t�@�C���̊��蓖�ĕ��@���w�肷��B
	//�@�@�P�̊��蓖�ẮA�ȉ��̏����ŋL�q����B
	//�@�@
	//�@�@�@�@"(�I�u�W�F�N�g�t�@�C����)=(�G�t�F�N�g�t�@�C����);"
	//�@�@
	//�@�@�I�u�W�F�N�g���Ɏg�p����G�t�F�N�g�t�@�C����؂�ւ���ɂ́A���̊��蓖�Ă𕡐���L�q����B
	//�@�@������L�q�����ꍇ�A�L�q�������ŃI�u�W�F�N�g�t�@�C��������r����A�ŏ��Ƀ}�b�`�������̂��g�p�����B
	//�@�@
	//�@�@�@��F string DefaultEffect = "self=hide; Mirror*.x=hide; *=MirrorObject.fx;";
	//�@�@
	//�@�@�I�u�W�F�N�g�t�@�C�����ɂ�"*"��"?"�ɂ�郏�C���h�J�[�h���w��ł���B
	//�@�@����ȃI�u�W�F�N�g�t�@�C�����Ƃ���"self"���w��ł��A����́A
	//�@�@����OFFSCREENRENDERTARGET�����G�t�F�N�g�����蓖�Ă�ꂽ�I�u�W�F�N�g���g��\���B
	//�@�@
	//�@�@�G�t�F�N�g�t�@�C�����ɑ��΃p�X�Ńt�@�C�������w�肵���ꍇ�A
	//�@�@�Q�ƌ��̃G�t�F�N�g�t�@�C�����i�[����Ă���t�H���_����ƂȂ�B
	//�@�@
	//�@�@�܂��A����ȃG�t�F�N�g�t�@�C�����Ƃ���"none"��"hide"���w��ł��A
	//�@�@����́A�u�G�t�F�N�g�Ȃ��v�Ɓu��\���v��\���B
	//�@�@�܂��A"main_default"���w�肷��ƁA���C���X�N���[���ł̃f�t�H���g����Ɠ��l�ȁA
	//�@�@�I�u�W�F�N�g�t�@�C���p�X�Ɋ�Â�fx�t�@�C����emd�t�@�C���̎����������s����B
	//�@�@
	D3DXHANDLE hDefaultEffect = effect->GetAnnotationByName( hTex, "DefaultEffect" );
	if( hDefaultEffect ){
		const char *string;
		hr=effect->GetString( hDefaultEffect, &string );
		assert( SUCCEEDED(hr) );

		while ( string && *string ){
			std::string fxfile, model;
			string = parseEquals( string, &model, &fxfile );
				
			if( model == "self" ){
				assert( fxfile=="hide" );
				//�ǂݍ��ނƂ�xfile�ǂݍ��܂Ȃ��������Ȃ��ł���A���"self=hide"����
				TRACE1( "offscreen %s: ignored 'self' in DeffalutEffect annotation.\n", mName.c_str() );
				continue;
			}else{
				assert(false);
			}

			if( fxfile == "main_defalt" ){
				assert( false && "not impl" );
			} else if( fxfile == "none" ){
				assert( false && "not impl" );
			} else if( fxfile == "hide" ){
			}else{
				assert(false && "�����͂������ǃe�X�g���ĂȂ�");
			}


			std::string fxFullPath( currentPath );
			fxFullPath.append( fxfile );
			//�t�@�C�������݂��邩
			if( 
				fxfile == "main_defalt" ||
				fxfile == "none"		||
				fxfile == "hide" )
			{
				fxFullPath = fxfile;

			}
			else if( findStandardEffect( fxFullPath ) )
			{
			
			}
			else{
				GameLib::FileIO::InFile f = GameLib::FileIO::InFile::create( fxFullPath.c_str() );
				StandardEffectSharedPtr effect = std::make_shared<StandardEffect>( fxFullPath, f, pool );
				if( effect->isError() ){
					assert(false);
					return;
				}
				effect->setViewportPixelSize( static_cast<float>(width), static_cast<float>(height) );

				mEffects.push_back( effect );
			}
			
			using namespace boost::xpressive;
			model = replaceText( model, "*", ".*" );
			model = replaceText( model, "?", ".?" );


			mDefalutEffects.emplace_back(
				new DefalutEffect( sregex::compile( model ), fxFullPath )
			);			
		}
	}
	mEffects.shrink_to_fit();

	recover();
		
	mName.assign(texDesc.Name);
	mHandle=hTex;
}

OffScreenRender::OffScreenRender( const OffScreenRender & src, ID3DXEffect *effect, D3DXHANDLE hTex, const std::string& currentPath, LPD3DXEFFECTPOOL pool )
	:mTexture(src.mTexture),
	mHandle(hTex),
	mSurface(src.mSurface),
	mZBuffer(src.mZBuffer),
	mTempBackbuffer(NULL),
	mTempBackZ(NULL),
	mWidth(src.mWidth),
	mHeight(src.mHeight),
	mMipLevels(src.mMipLevels),
	mFormat(src.mFormat),
	mName(src.mName),
	mClearColor(src.mClearColor),
	mClearDepth(src.mClearDepth),
	mIsAntiAilias(src.mIsAntiAilias),
	mDescription(src.mDescription )
{
	IDirect3DDevice9 *pDevice = GameLib::Graphics::Manager::instance().dxDevice();
	GameLib::Framework fw = GameLib::Framework::instance();
	HRESULT hr;

	D3DXPARAMETER_DESC texDesc;
	hr=effect->GetParameterDesc( hTex, &texDesc );
	//��OFFSCREENRENDERTARGET
	assert( _strcmpi( texDesc.Semantic, "OFFSCREENRENDERTARGET" )==0 );
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
	}else if( hDimensions ){
	}else if( hViewPortRatio ){
		width = static_cast<unsigned int>(static_cast<float>(fw.width())*xRate);
		height = static_cast<unsigned int>(static_cast<float>(fw.height())*yRate);
	}else {}

	assert( mWidth==width );
	assert( mHeight==height );


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

	//D3DFORMAT formatZ = getFormatRenderStencil( effect, hTex );


	//�Eint Miplevels
	//�Eint Levels
	//�@�@"���ʏ�e�N�X�`��"���Q�ƁB
	//�@�@1�܂���0���w��ł���B
	//�@�@0���w�肵���ꍇ�A���S�ȃ~�b�v�}�b�v �`�F�[�����쐬�����B
	//�@�@1���w�肷��ƁA�~�b�v�}�b�v�͐�������Ȃ��i�f�t�H���g�j�B
	assert( mMipLevels == getMiplevel( effect, hTex, 1 ) );

	//	�Efloat4 ClearColor
	//�@�@�����_�����O�^�[�Q�b�g���N���A����F��ݒ肷��B
	//�@�@�����Ŏw�肵���F�ŁA�����_�����O�^�[�Q�b�g�͎����I�ɃN���A�����B
	//
	D3DXHANDLE hClearColor = effect->GetAnnotationByName( hTex, "ClearColor" );
	if( hClearColor ){
		float c[4]={0};
		hr=effect->GetFloatArray( hClearColor, c, 4 );
		assert( SUCCEEDED(hr) );
		assert(
			mClearColor.x == c[0] &&
			mClearColor.y == c[1] &&
			mClearColor.z == c[2] &&
			mClearColor.w == c[3] 
		);
	}

	//�Efloat ClearDepth
	//�@�@�[�x�X�e���V���T�[�t�F�C�X���N���A����Z�l��ݒ肷��B
	//�@�@�����Ŏw�肵��Z�l�ŁA�[�x�X�e���V���T�[�t�F�C�X�͎����I�ɃN���A�����B
	D3DXHANDLE hClearDepth = effect->GetAnnotationByName( hTex, "ClearDepth" );
	if( hClearDepth ){
		float clearDepth;
		hr=effect->GetFloat( hClearDepth, &clearDepth );
		assert( SUCCEEDED(hr) );
		assert( clearDepth==mClearDepth );
	}

	//�Ebool AntiAlias
	//�@�@�����_�����O�ɃA���`�G�C���A�X���g�p����B�f�t�H���g��false�B
	//�@�@���A���`�G�C���A�X���L���̏ꍇ�A�r�f�I�������͑��������邽�߁A
	//�@�@�@�e�N�X�`���̃T�C�Y�ɂ͒��ӂ��邱��
	D3DXHANDLE hAntiAilias = effect->GetAnnotationByName( hTex, "AntiAlias" );
	if( hAntiAilias ){
		int isAntiAilias;
		TRACE( "not impl offscreen antiailias" );
		hr=effect->GetBool( hAntiAilias, &isAntiAilias );
		assert( SUCCEEDED(hr) );
		assert( isAntiAilias==mIsAntiAilias );
	}

	//�Estring Description
	//�@�@�I�t�X�N���[�������_�����O�^�[�Q�b�g�̐��������w�肷��B
	//�@�@�����Ŏw�肵�������񂪁A�G�t�F�N�g������GUI�_�C�A���O��ŕ\�������B
	D3DXHANDLE hDescription = effect->GetAnnotationByName( hTex, "Description" );
		
	if( hDescription ){
		const char *string;
		hr=effect->GetString( hDescription, &string );
		assert( SUCCEEDED(hr) );
		assert( mDescription== string );
	}


	//�Estring DefaultEffect
	//�@�@�I�t�X�N���[�������_�����O�Ŏg�p����G�t�F�N�g�t�@�C���̊��蓖�ĕ��@���w�肷��B
	//�@�@�P�̊��蓖�ẮA�ȉ��̏����ŋL�q����B
	//�@�@
	//�@�@�@�@"(�I�u�W�F�N�g�t�@�C����)=(�G�t�F�N�g�t�@�C����);"
	//�@�@
	//�@�@�I�u�W�F�N�g���Ɏg�p����G�t�F�N�g�t�@�C����؂�ւ���ɂ́A���̊��蓖�Ă𕡐���L�q����B
	//�@�@������L�q�����ꍇ�A�L�q�������ŃI�u�W�F�N�g�t�@�C��������r����A�ŏ��Ƀ}�b�`�������̂��g�p�����B
	//�@�@
	//�@�@�@��F string DefaultEffect = "self=hide; Mirror*.x=hide; *=MirrorObject.fx;";
	//�@�@
	//�@�@�I�u�W�F�N�g�t�@�C�����ɂ�"*"��"?"�ɂ�郏�C���h�J�[�h���w��ł���B
	//�@�@����ȃI�u�W�F�N�g�t�@�C�����Ƃ���"self"���w��ł��A����́A
	//�@�@����OFFSCREENRENDERTARGET�����G�t�F�N�g�����蓖�Ă�ꂽ�I�u�W�F�N�g���g��\���B
	//�@�@
	//�@�@�G�t�F�N�g�t�@�C�����ɑ��΃p�X�Ńt�@�C�������w�肵���ꍇ�A
	//�@�@�Q�ƌ��̃G�t�F�N�g�t�@�C�����i�[����Ă���t�H���_����ƂȂ�B
	//�@�@
	//�@�@�܂��A����ȃG�t�F�N�g�t�@�C�����Ƃ���"none"��"hide"���w��ł��A
	//�@�@����́A�u�G�t�F�N�g�Ȃ��v�Ɓu��\���v��\���B
	//�@�@�܂��A"main_default"���w�肷��ƁA���C���X�N���[���ł̃f�t�H���g����Ɠ��l�ȁA
	//�@�@�I�u�W�F�N�g�t�@�C���p�X�Ɋ�Â�fx�t�@�C����emd�t�@�C���̎����������s����B
	//�@�@
	D3DXHANDLE hDefaultEffect = effect->GetAnnotationByName( hTex, "DefaultEffect" );
	assert( false && "����ŗǂ��Ǝv�����ǎ��M�Ȃ�" );
	mEffects.assign(src.mEffects.begin(), src.mEffects.end() );
	
	//	
	//if( hDefaultEffect ){
	//	const char *string;
	//	hr=effect->GetString( hDefaultEffect, &string );
	//	assert( SUCCEEDED(hr) );

	//	while ( string && *string ){
	//		std::string fxfile, model;
	//		string = parseEquals( string, &model, &fxfile );
	//			
	//		if( model == "self" ){
	//			TRACE1( "offscreen %s: ignored self in DeffalutEffect annotation.\n", mName.c_str() );
	//			continue;
	//		}else{
	//			assert(false);
	//		}

	//		if( fxfile == "main_defalt" ){
	//			assert( false && "not impl" );
	//			continue;
	//		} else if( fxfile == "none" ){
	//			assert( false && "not impl" );
	//			continue;
	//		} else if( fxfile == "hide" ){
	//			continue;
	//		}else{
	//			assert(false);
	//		}


	//		std::string fxFullPath( currentPath );
	//		fxFullPath.append( fxfile );
	//		//�t�@�C�������݂��邩���m���߂��ق�������
	//		GameLib::FileIO::InFile f = GameLib::FileIO::InFile::create( fxFullPath.c_str() );
	//		StandardEffectSharedPtr effect = std::make_shared<StandardEffect>( fxFullPath, f, pool );
	//		if( effect->isError() ){
	//			assert(false);
	//			return;
	//		}
	//		effect->setViewportPixelSize( static_cast<float>(width), static_cast<float>(height) );

	//		mEffects.push_back( effect );
	//			
	//		std::vector<Object::IObjectSharedPtr> objects;
	//		using namespace boost::xpressive;
	//		model = replaceText( model, "*", ".*" );
	//		sregex rexModel = sregex::compile( model );
	//			
	//		Manager *m = Manager::instance();
	//		m->matchByModelFileName( &objects, rexModel );

	//		for( unsigned int i=0, size=objects.size(); i<size; i++ ){
	//			effect->add( objects[i] );
	//		}
	//		
	//	}
	//}
	mEffects.shrink_to_fit();

	assert(mName==texDesc.Name);
}

//void OffScreenRender::add( const Object::IObjectSharedPtr &obj ){
//	for( int i=0,size=mDefalutEffects.size(); i<size; ++i ){
//		std::string effectName;
//		if( findProperEffectName( &effectName, obj ) ){
//			StandardEffectSharedPtr ef = findStandardEffect( effectName );
//			if(ef){
//				ef->add( obj );
//			}
//			else {
//				assert(
//					mDefalutEffects[i]->mFxFileName == "hide" ||
//					mDefalutEffects[i]->mFxFileName == "main_defalut" ||
//					mDefalutEffects[i]->mFxFileName == "none"  &&
//					"�o�^����ĂȂ��G�t�F�N�g��T�����Ƃ��������H" );
//
//			}
//			return;
//		}
//		
//	}
//}

bool OffScreenRender::findProperEffectName( std::string* found, const Object::IObjectSharedPtr &obj ){
	for( int i=0,size=mDefalutEffects.size(); i<size; ++i ){
		using namespace boost::xpressive;
		smatch what;
		if (regex_match( obj->getModelFileName(), what, mDefalutEffects[i]->mModelFile)) {
			*found = mDefalutEffects[i]->mFxFileName;
			return true;
		}
	}
	return false;
}

void OffScreenRender::begin(){
	HRESULT hr;
	GameLib::Graphics::Manager m;
	LPDIRECT3DDEVICE9 pDevice = m.dxDevice();

	// �r���[�|�[�g���̕ێ�
	pDevice->GetViewport(&mTempViewport);

	// �o�b�N�o�b�t�@�[�ւ̃C���^�[�t�F�[�X�̕ێ�
	hr=pDevice->GetRenderTarget(0, &mTempBackbuffer);
	assert( SUCCEEDED(hr) );

	// �[�x�o�b�t�@�[�̃C���^�[�t�F�[�X�̕ێ�
	hr=pDevice->GetDepthStencilSurface( &mTempBackZ); 
	assert( SUCCEEDED(hr) );

	// �e�N�X�`���������_�����O�^�[�Q�b�g�ɐݒ�
	hr=pDevice->SetRenderTarget(0, mSurface.get());
	assert( SUCCEEDED(hr) );

	// �e�N�X�`���p�̐[�x�o�b�t�@��ݒ�
	hr=pDevice->SetDepthStencilSurface(mZBuffer.get());
	assert( SUCCEEDED(hr) );

	// �����_�����O�^�[�Q�b�g��������
	hr=pDevice->Clear(0,NULL, 
		D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 
		D3DXCOLOR(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w),
		mClearDepth, 0);
	assert( SUCCEEDED(hr) );

	// �r���[�|�[�g���e�N�X�`���摜�̃T�C�Y�ɐݒ�
	D3DSURFACE_DESC desc;
	mTexture->GetLevelDesc( 0, &desc );
	D3DVIEWPORT9 viewport2 = { 0, 0, desc.Width, desc.Height, 0.0f, 1.0f };
	pDevice->SetViewport(&viewport2);

}

void OffScreenRender::end(){
	GameLib::Graphics::Manager m;
	LPDIRECT3DDEVICE9 pDevice = m.dxDevice();
	
	// �ۑ����Ă������o�b�N�o�b�t�@�������_�����O�^�[�Q�b�g�ɐݒ�
	HRESULT hr=pDevice->SetRenderTarget(0, mTempBackbuffer);
	assert( SUCCEEDED(hr) );
	SAFE_RELEASE(mTempBackbuffer);

	// �ۑ����Ă������o�b�N�o�b�t�@�̐[�x�o�b�t�@��ݒ�
	hr=pDevice->SetDepthStencilSurface(mTempBackZ);
	assert( SUCCEEDED(hr) );
	SAFE_RELEASE(mTempBackZ);


	// �ۑ����Ă������r���[�|�[�g�����ɖ߂�
	hr=pDevice->SetViewport(&mTempViewport);
	assert( SUCCEEDED(hr) );
}

unsigned int OffScreenRender::getNumberOfStandardEffects()const
{
	return mEffects.size();
}
StandardEffectSharedPtr OffScreenRender::getStandardEffectAt( unsigned int i){
	return mEffects[i];
}

//void OffScreenRender::clearAllObjects(){
//	for(unsigned int i=0, size=mEffects.size(); i<size; ++i ){		
//		mEffects[i]->clearAllObjects();
//	}
//}


}// end namespace MME
#include "Light.h"
#include "MME/Manager.h"
#include "MME/Batch/StandardEffectBatch.h"
#include "MME/Batch/PostEffectBatch.h"
#include <GameLib/GameLib.h>//SAFE_DELETE
#include "helper/FileLoader.h"
#include "GameObject/AccessoryUtils/MME/Model.h"
#include "MME/Object/ModelKai.h"
#include "MME/Object/Accessory.h"
#include "MME/Object/PointLight.h"

#include <GameLib/Graphics/Manager.h>


namespace MME {
namespace {
using namespace Batch;
static const char *skDrawGBufferFxFileName = "ShaderTest/Basic/deffered_shading_luminous/basic.fx";
//static const char *skDrawGBufferFxFileName = "ShaderTest/Basic/full.fx";//defalut
//static const char *skDrawGBufferFxFileName = "ShaderTest/Basic/deffered_shading/basic.fx";
//static const char *skDrawGBufferFxFileName = "ShaderTest/Basic/deffered_shading_shadow/basic.fx";

static const char *skLuminousFxFileName = "ShaderTest/Basic/deffered_shading_luminous/postEffect.fx";

static const char *skFXAAFileName = "ShaderTest/Fxaa/fxaa.fx";

static const char *skPointLightFxFileName="ShaderTest/Basic/DSPoinghtLight.fx";

StandardEffectBatch* createStandardEffectBatchIf( const char* filename ){
	MME::Manager* mme= MME::Manager::instance();
	StandardEffectSharedPtr effect = mme->findStandardEffect( filename );
	if( effect ){
		return new StandardEffectBatch( effect );
	}
	return nullptr;	
}

PostEffectBatch* createPostEffectBatchIf( const char* filename ){
	MME::Manager* mme= MME::Manager::instance();
	PostEffectSharedPtr effect = mme->findPostEffect( filename );
	if( effect ){
		return new PostEffectBatch( effect );
	}
	return nullptr;
}

bool canRemove( const Object::IObjectWeakPtr & wp ){
	if( Object::IObjectSharedPtr sp = wp.lock() ){
		return sp->canRemove();
	} else{
		return true;
	}
}

}//end unnamespace

namespace Drawer { namespace CutSceneImpl { namespace Light {
Impl::Uptr Impl::mThis;

Impl* Impl::instance(){
	if( !mThis )
	{
		mThis.reset(  new Impl() );
	}
	return mThis.get();
}

void Impl::destroy(){
	
	mThis.reset( nullptr );
}

Impl::Impl()
	:mDrawGBuffer(nullptr),
	mLuminous(nullptr),
	mFxaa(nullptr),
	mPointLight(nullptr),
	mFileLoader(nullptr)
{
	mObjectsPtr.reset( new MME::Batch::DrawObjects );
	mLightsPtr.reset( new MME::Batch::DrawObjects );
	
	MME::Manager* mme= MME::Manager::instance();
	
	mDrawGBuffer = createStandardEffectBatchIf( skDrawGBufferFxFileName );
	if( mDrawGBuffer ){
		mDrawGBuffer->setObjectsPtr( mObjectsPtr );
	}
	
	if( mDrawGBuffer )
	{
		return;
	}
	
	mFileLoader = new FileLoader();
	
	if( !mDrawGBuffer ){
		mFileLoader->insert( skDrawGBufferFxFileName );
	}
}

Impl::~Impl(){
	SAFE_DELETE( mFileLoader );
	SAFE_DELETE( mDrawGBuffer );
	SAFE_DELETE( mLuminous );
	SAFE_DELETE( mFxaa );
	SAFE_DELETE( mPointLight );
}

bool Impl::isReady(){
	if( mFileLoader ){
		if(mFileLoader->isFinished()){
			
			MME::Manager* mme= MME::Manager::instance();
			if( !mDrawGBuffer ){
				mDrawGBuffer = new StandardEffectBatch(
					mme->registStandardEffect( 
							skDrawGBufferFxFileName,
							mFileLoader->get( skDrawGBufferFxFileName ) ) );
				mDrawGBuffer->setObjectsPtr( mObjectsPtr );
			}
					
			SAFE_DELETE(mFileLoader);
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

void Impl::draw(){
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
	GameLib::Graphics::Manager m;
	LPDIRECT3DDEVICE9 pDevice = m.dxDevice();

	using namespace GameLib::Graphics;
	m.setBlendMode( BLEND_LINEAR );
	//mmd�͍�����W
	m.setCullMode( CULL_FRONT );
	m.enableDepthTest( true );
	m.enableDepthWrite( true );

	m.enableMME(true);

	MME::Manager* mme= MME::Manager::instance();

	DrawObjects::iterator it = std::remove_if( mObjectsPtr->begin(), mObjectsPtr->end(), canRemove );
	mObjectsPtr->erase( it, mObjectsPtr->end());

	//offscreen�Ȃ�����R�����g�A�E�g���Ă��ǂ���
	mDrawGBuffer->drawOffScreens();

	mDrawGBuffer->drawObject();		
	
	m.dxDevice()->SetRenderTarget( 1, NULL );
	m.dxDevice()->SetRenderTarget( 2, NULL );
	m.dxDevice()->SetRenderTarget( 3, NULL );

	mme->setCurrentEffect(nullptr);
	
	//��n���@�����I��VertexBlend�𖳌��ɂ��Ă���
	//����̂�GameLib::Graphics::Manager��VertexBlendEnable�������Ȃ̂ɁA�L���Ȓl�������Ă���ꍇ�Ȃ̂�
	//GameLib::Graphics::Manager��VertexBlendEnable���L���̏ꍇ�͂����ɐ������l��������͂��Ȃ̂Ŗ��Ȃ�
	//GameLib::Graphics::Manager��VertexBlendEnable�������ł�����VertexBlendEnable�������Ȃ�Ζ��Ȃ�
	m.enableMME(false);
	m.enableVertexBlend(true);
	m.enableVertexBlend(false);

	//mme->drawShadowMapBuffer();

	mme->sendParamsToGameLibGraphicsManager();
}


void Impl::add( const Object::ModelKaiSharedPtr &obj){
	mObjectsPtr->push_back( obj );
}
void Impl::add( const Object::PointLightSharedPtr &obj){
}
	
void Impl::add( const Object::AccessorySharedPtr &obj){
	mObjectsPtr->push_back( obj );
}
void Impl::add( const GameObject::AccessoryUtils::MME::ModelSharedPtr &obj){
	mObjectsPtr->push_back( obj );
}

} } } }// namespace MME::Drawer::CutSceneImpl::Light

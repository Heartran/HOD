#include "GameDrawer.h"
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

namespace Drawer {
GameDrawer::Uptr GameDrawer::mThis;

GameDrawer* GameDrawer::instance(){
	if( !mThis )
	{
		mThis.reset(  new GameDrawer() );
	}
	return mThis.get();
}

void GameDrawer::destroy(){
	mThis.reset( nullptr );
}

GameDrawer::GameDrawer()
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
	mLuminous = createPostEffectBatchIf( skLuminousFxFileName );
	mFxaa = createPostEffectBatchIf( skFXAAFileName );
	mPointLight = createPostEffectBatchIf( skPointLightFxFileName );
	if(mPointLight){
		mPointLight->setObjectsPtr( mLightsPtr );
	}
	
	if( mDrawGBuffer &&
			mLuminous &&
			mFxaa &&
			mPointLight )
	{
		return;
	}
	
	mFileLoader = new FileLoader();
	
	if( !mDrawGBuffer ){
		mFileLoader->insert( skDrawGBufferFxFileName );
	}
	if( !mLuminous ){
		mFileLoader->insert( skLuminousFxFileName );
	}
	if( !mFxaa ){
		mFileLoader->insert( skFXAAFileName );
	}
			
	if( !mPointLight ){
		mFileLoader->insert( skPointLightFxFileName );
	}
}

GameDrawer::~GameDrawer(){
	SAFE_DELETE( mFileLoader );
	SAFE_DELETE( mDrawGBuffer );
	SAFE_DELETE( mLuminous );
	SAFE_DELETE( mFxaa );
	SAFE_DELETE( mPointLight );
}

bool GameDrawer::isReady(){
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
		
			if( !mLuminous ){
				mLuminous = new PostEffectBatch(
					mme->registPostEffect( 
							skLuminousFxFileName ,
							mFileLoader->get( skLuminousFxFileName ) ) );
			}
			if( !mFxaa ){
				mFxaa =  new PostEffectBatch(
						mme->registPostEffect( 
							skFXAAFileName ,
							mFileLoader->get( skFXAAFileName ) ) );
			}

			if( !mPointLight ){
				mPointLight = new PostEffectBatch(
					mme->registPostEffect( 
						skPointLightFxFileName ,
						mFileLoader->get( skPointLightFxFileName ) ) );
				mPointLight->setObjectsPtr( mLightsPtr );
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

void GameDrawer::draw(){
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
	bool isGraphicsHigh=true;
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

	it = std::remove_if( mLightsPtr->begin(), mLightsPtr->end(), canRemove );
	mLightsPtr->erase( it, mLightsPtr->end());


	
	if(isGraphicsHigh){
		mme->beginShadow();
		mDrawGBuffer->drawZPlot();
		mme->endShadow();
	}

	//offscreen�Ȃ�����R�����g�A�E�g���Ă��ǂ���
	mDrawGBuffer->drawOffScreens();

	
	//mFxaa->preDraw();//�o�O����
	//mPointLight->preDraw();
	//mLuminous->preDraw();//�o�O����


	//mDrawGBuffer->drawZPrePass();
	
	//selfshadow
	if(isGraphicsHigh){
		mDrawGBuffer->drawObjectSS();
	} else {
		mDrawGBuffer->drawObject();		
	}
	
	
	//�G�b�W���P�s�N�Z���ȉ��ł������肷��ƁAFXAA�Ńm�C�Y�̂悤�ɂȂ��Ă��܂�
	mDrawGBuffer->drawEdge();		
	//
	//mLuminous->postDraw();
	//mPointLight->postDraw();


	//mFxaa->postDraw();//�o�O����


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

	//drawOffScreen( "ShaderTest/AutoLuminous4/AutoLuminous.fx", "AL_EmitterRT" );
	//drawOffScreen( "ShaderTest/AutoLuminous4/AutoLuminousBasic.fx", "ALB_EmitterRT" );

	mme->sendParamsToGameLibGraphicsManager();
}


void GameDrawer::add(const Object::ModelKaiSharedPtr &obj) {

	auto func = [&obj](const Object::IObjectWeakPtr &wp)-> bool
	{
		auto sp = wp.lock();
		if (sp)
		{
			return sp == obj;
		}
		else {
			return false;
		}
	};

	auto it = std::find_if(mObjectsPtr->begin(), mObjectsPtr->end(), func);

	if ( it== mObjectsPtr->end()) {
		obj->setCanRemove(false);
		mObjectsPtr->push_back(obj);
	}
	else {
		obj->setCanRemove(false);
		//TRACE("�o�^�ς݁A���t���[������add(),remove()�����ꍇ�����ɗ���\n");//�o�O�ł͂Ȃ����d�l������������������̂ŏo��
	}
}
void GameDrawer::add( const Object::PointLightSharedPtr &obj){
	auto func = [&obj](const Object::IObjectWeakPtr &wp)-> bool
	{
		auto sp = wp.lock();
		if (sp)
		{
			return sp == obj;
		}
		else {
			return false;
		}
	};

	auto it = std::find_if(mLightsPtr->begin(), mLightsPtr->end(), func);
	if (it == mLightsPtr->end()) {
		obj->setCanRemove(false);

		mLightsPtr->push_back(obj);
	}
	else {
		obj->setCanRemove(false);
		//TRACE("�o�^�ς݁A���t���[������add(),remove()�����ꍇ�����ɗ���\n");//�o�O�ł͂Ȃ����d�l������������������̂ŏo��
	}
}
	
void GameDrawer::add( const Object::AccessorySharedPtr &obj){
	auto func = [&obj](const Object::IObjectWeakPtr &wp)-> bool
	{
		auto sp = wp.lock();
		if (sp)
		{
			return sp == obj;
		}
		else {
			return false;
		}
	};

	auto it = std::find_if(mObjectsPtr->begin(), mObjectsPtr->end(), func);
	if (it == mObjectsPtr->end()) {
		obj->setCanRemove(false);

		mObjectsPtr->push_back(obj);
	}
	else {
		obj->setCanRemove(false);
		//TRACE("�o�^�ς݁A���t���[������add(),remove()�����ꍇ�����ɗ���\n");//�o�O�ł͂Ȃ����d�l������������������̂ŏo��
	}	
}
void GameDrawer::add( const GameObject::AccessoryUtils::MME::ModelSharedPtr &obj){

	auto func = [&obj](const Object::IObjectWeakPtr &wp)-> bool
	{
		auto sp = wp.lock();
		if (sp)
		{
			return sp == obj;
		}
		else {
			return false;
		}
	};
	auto it = std::find_if(mObjectsPtr->begin(), mObjectsPtr->end(), func);
	if (it == mObjectsPtr->end()) {
		obj->setCanRemove(false);
		mObjectsPtr->push_back(obj);
	}
	else {
		obj->setCanRemove(false);
		//TRACE("�o�^�ς݁A���t���[������add(),remove()�����ꍇ�����ɗ���\n");//�o�O�ł͂Ȃ����d�l������������������̂ŏo��
	}	
}


void GameDrawer::remove(const Object::ModelKaiSharedPtr &obj) {
	obj->setCanRemove(true);
}
void GameDrawer::remove(const Object::PointLightSharedPtr &obj) {
	obj->setCanRemove(true);
}
void GameDrawer::remove(const Object::AccessorySharedPtr &obj) {
	obj->setCanRemove(true);
}
void GameDrawer::remove(const GameObject::AccessoryUtils::MME::ModelSharedPtr &obj) {
	obj->setCanRemove(true);
}


} }//end namespace MME::Drawer

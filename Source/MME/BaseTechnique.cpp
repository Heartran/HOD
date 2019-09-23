#include"BaseTechnique.h"
#include"helper/UseClass.h"
#include<assert.h>
#include<tuple>
#include<GameLib/GameLib.h>
#include"Script/Scripts.h"
#include"Pass.h"
#include"helper/ContainerHelper/findByName.h"
#include"helper/MyOutputDebugString.h"
#include"DrawableMaterialNumberRange.h"
#include"BaseEffect.h"

namespace MME{
BaseTechnique::BaseTechnique( ID3DXEffect *effect, D3DXHANDLE hTech, BaseEffect *mme ) :
mEffect(effect),
mHandle(hTech),
mMode( MP_OBJECT ),//�A�m�e�[�V�����ȗ����ɂ́A"object"���w�肵���Ƃ݂Ȃ����B
mUseTexture( new UseClass( either() ) ),//�A�m�e�[�V�����ȗ����ɂ́A�e�N�X�`���̗L���͖��������B
//�A�m�e�[�V�����ȗ����A��������邪�A�V�F�[�_����object_ss���g���Ă���AmmdPass��object�̏ꍇ��false���ݒ肳���B
mUseSelfShadow( new UseClass( either() ) ),
mUseSphereMap( new UseClass( either() ) ),
mUseToon( new UseClass( either() ) ),
mDrawableMaterials(NULL)
{
	HRESULT hr;

	D3DXHANDLE hSubset = effect->GetAnnotationByName( hTech, "Subset" );
	if( hSubset ){
		const char *text;
		hr=effect->GetString( hSubset, &text );
		assert( SUCCEEDED(hr) );

		mDrawableMaterials = new DrawableMaterialNumberRange( text );
	}

	D3DXHANDLE hMode = effect->GetAnnotationByName( hTech, "MMDPass" );
	if( hMode ){
		const char *textMode = NULL;
		effect->GetString( hMode, &textMode );
		if( _strcmpi( textMode, "object" )==0 ){
			mMode=MP_OBJECT;
		} else if( _strcmpi( textMode, "zplot" )==0 ){
			mMode=MP_ZPLOT;
		} else if( _strcmpi( textMode, "object_ss" )==0 ){
			mMode=MP_OBJECT_SS;
		} else if( _strcmpi( textMode, "shadow" )==0 ){
			mMode=MP_SHADOW;
		} else if( _strcmpi( textMode, "edge" )==0 ){
			mMode=MP_EDGE;
		} else if( _strcmpi( textMode, "zprepass" )==0 ){		
			mMode=MP_ZPREPASS;
		} else {
			assert( false && "MMDPass is wrong. seted object" );
		}
	}

	{
		D3DXHANDLE hUseTex = effect->GetAnnotationByName( hTech, "UseTexture" );
		if(hUseTex){
			BOOL b;
			hr=effect->GetBool( hUseTex, &b );
			assert( SUCCEEDED(hr) );
			mUseTexture->set( static_cast<bool>(b!=0) );
		}
	}

	{
		D3DXHANDLE hUseSphereMap = effect->GetAnnotationByName( hTech, "UseSphereMap" );
		if(hUseSphereMap){
			BOOL b;
			hr=effect->GetBool( hUseSphereMap, &b );
			assert( SUCCEEDED(hr) );
			mUseSphereMap->set( static_cast<bool>(b!=0) );
		}
	}

	{
		D3DXHANDLE hUseToon = effect->GetAnnotationByName( hTech, "UseToon" );
		if(hUseToon){
			BOOL b;
			hr=effect->GetBool( hUseToon, &b );
			assert( SUCCEEDED(hr) );
			mUseToon->set( static_cast<bool>(b!=0) );
		}
	}

	{
		D3DXHANDLE hUseSelfShadow = effect->GetAnnotationByName( hTech, "UseSelfShadow" );
		if(hUseSelfShadow){
			BOOL b;
			hr=effect->GetBool( hUseSelfShadow, &b );
			assert( SUCCEEDED(hr) );
			mUseSelfShadow->set( static_cast<bool>(b!=0) );
		}
		//MMD�̏ꍇ�͏ȗ����ꂽ�ꍇobject_ss������̂�false
		else if( mMode==MP_OBJECT && mme->isThereObject_SSInShader() ){
			mUseSelfShadow->set(false);
		}
	}

	if(mMode==MP_OBJECT_SS){
		mMode=MP_OBJECT;
		mUseSelfShadow->set( true );
	}

	//Pass�̍쐬
	D3DXTECHNIQUE_DESC techDesc;
	hr=effect->GetTechniqueDesc( hTech, &techDesc );
	assert(SUCCEEDED(hr));
	for( UINT iPass=0; iPass<techDesc.Passes; ++iPass ){
		//����������Ȃ�new[techDesc.Passes];
		//���̌�init
		//�܂��債�ĕς��Ȃ��Ǝv���B���F1���Ƃ��e�N�j�b�N���Ȃ���
		D3DXHANDLE hPass = effect->GetPass( hTech, iPass );
		Pass *pass = new Pass( effect, hPass, iPass, mme );
		mPasses.push_back(pass);
		if(pass->isError()){
			mHandle=NULL;
			return;
		}
	}

	mPasses.shrink_to_fit();

}
BaseTechnique::~BaseTechnique(){

	for( int i=0, size=mPasses.size(); i<size; ++i ){
		SAFE_DELETE( mPasses[i] );
	}
	mPasses.clear();
	mPasses.shrink_to_fit();

	SAFE_DELETE( mDrawableMaterials );
	SAFE_DELETE( mUseTexture );
	SAFE_DELETE( mUseSphereMap );
	SAFE_DELETE( mUseSelfShadow);
	SAFE_DELETE( mUseToon );
}

bool BaseTechnique::canUse( MMDPass mmdPass, bool useTexture, bool useSphereMap, bool useToon, bool useSelfShadow )const{
	//if(
	//	std::make_tuple( mMode, *mUseTexture, *mUseSphereMap, *mUseToon, *mUseSelfShadow ) ==
	//	std::make_tuple( mmdPass, useTexture, useSphereMap, useToon, useSelfShadow ) )
	//{
	//	D3DXTECHNIQUE_DESC desc;
	//	mEffect->GetTechniqueDesc( mHandle, &desc );
	//	TRACE1( "set Technique %s\n", desc.Name );
	//	return true;
	//}
	//
	//return false;
	return 
		std::make_tuple( mMode, *mUseTexture, *mUseSphereMap, *mUseToon, *mUseSelfShadow ) ==
		std::make_tuple( mmdPass, useTexture, useSphereMap, useToon, useSelfShadow );
}



Pass* BaseTechnique::findPass( const std::string &name ){
	using namespace ContainerHelper;
	
	//�g�������o�[�֐�
	std::vector<Pass*>:: iterator it = mPasses|findByName( name.c_str() );
	if( it==mPasses.end()){
		return NULL;
	}
	return *it;
}

bool BaseTechnique::canDraw( unsigned int materialIndex ) const{
	if(mDrawableMaterials){
		assert(false && "�����������ǈ�x�������ĂȂ�" );
		return mDrawableMaterials->canDraw( materialIndex );
	} else {
		//�ȗ����ꂽ�ꍇ
		return true;
	}
}


}//end namespace MME
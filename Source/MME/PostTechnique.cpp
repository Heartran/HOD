#include"PostTechnique.h"
#include<assert.h>
#include"Script/Scripts.h"
#include<GameLib/GameLib.h>
#include"helper/ContainerHelper/safeDeleteAll.h"
namespace MME {

PostTechnique::PostTechnique( ID3DXEffect *effect, D3DXHANDLE hTech, BaseEffect *mme )
	:BaseTechnique( effect, hTech, mme )
{
	HRESULT hr;
	D3DXHANDLE hScript = effect->GetAnnotationByName( hTech, "Script" );

	if(hScript){
		const char *scriptText = NULL;
		hr=effect->GetString( hScript, &scriptText );
		assert(SUCCEEDED(hr));

		const char *next=scriptText;
		bool isPre = true;

		assert(next!='\0');
		
		while( Script::isValid( next ) ){
			Script::IScript* s = Script::createScript(  next, &next, effect, mme, this );
			if(s==NULL){
				assert( false && "postTech script was failed." );
				mHandle=NULL;
				return;
			}
			if( isPre ){
				//post�Ɉڍs
				if( dynamic_cast<Script::ScriptExternal*>(s) ){
					isPre=false;
					SAFE_DELETE(s);
					continue;
				}
				mPreScripts.push_back( s );
			} else {
				if( dynamic_cast<Script::ScriptExternal*>(s) ){
					assert( false && "ScriptExternal�͈�񂾂��ɂ��Ă��������B�������܂�" );
					SAFE_DELETE(s);
					continue;
				}
				mPostScripts.push_back(s);
			}
		}
	}
	else {
		assert(false && "PostProcess�ŃX�N���v�g������܂���B" );
		mHandle=0;
	}

	mPreScripts.shrink_to_fit();
	mPostScripts.shrink_to_fit();

}
PostTechnique::~PostTechnique(){
	using namespace ContainerHelper;
	mPreScripts|safeDeleteAll();
	mPostScripts|safeDeleteAll();
}


void PostTechnique::preExec(){
	//HRESULT hr;
	//begin end����ˁ[���O�̂���
	//hr=mEffect->SetTechnique( mHandle );
	//assert(SUCCEEDED(hr));

	//hr=mEffect->Begin( 0, 0 );
	//assert(SUCCEEDED(hr));
	// scripts ���s
	for( int i=0, size=mPreScripts.size(); i<size; ++i ){
		mPreScripts[i]->exec();
	}

	//assert( !mPreScripts.empty() );

	//hr=mEffect->End();
	//assert(SUCCEEDED(hr));
}

void PostTechnique::postExec(){
	HRESULT hr;
	hr=mEffect->SetTechnique( mHandle );
	assert(SUCCEEDED(hr));

	hr=mEffect->Begin( 0, 0 );
	assert(SUCCEEDED(hr));

	// scripts ���s
	for( int i=0, size=mPostScripts.size(); i<size; ++i ){
		//MME�͂��̎d�l�����ǂ��������������Ƃ͎v���Ȃ��B
		//state�̐؂�ւ��̃��O�I��
		//state�̐؂�ւ��̎d�l��MME�̈قȂ邪�������̕��������C������̂�
		//hr=mEffect->Begin( 0, 0 );
		//assert(SUCCEEDED(hr));
		mPostScripts[i]->exec();
		//hr=mEffect->End();
		//assert(SUCCEEDED(hr));
	}

	assert( !mPostScripts.empty() );

	hr=mEffect->End();
	assert(SUCCEEDED(hr));
}
}
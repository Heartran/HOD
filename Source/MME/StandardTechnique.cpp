#include"StandardTechnique.h"
#include<assert.h>
#include"Script/Scripts.h"
#include<GameLib/GameLib.h>
#include"BaseEffect.h"
namespace MME {

StandardTechnique::StandardTechnique( ID3DXEffect *effect, D3DXHANDLE hTech, BaseEffect *mme )
	:BaseTechnique( effect, hTech, mme )
{
	HRESULT hr;
	D3DXHANDLE hScript = effect->GetAnnotationByName( hTech, "Script" );

	if(hScript){
		const char *scriptText = NULL;
		hr=effect->GetString( hScript, &scriptText );
		assert(SUCCEEDED(hr));

		const char *next=scriptText;
		assert(next!='\0');
		
		while( Script::isValid( next ) ){
			Script::IScript* s = Script::createScript(  next, &next, effect, mme, this );
			if( s==NULL && next!='\0' ){
				mHandle=NULL;
				return;
			}
			mScripts.push_back( s );
		}
	}
	//�e�N�j�b�N�̃X�N���v�g���ȗ������ꍇ�A�P�Ƀe�N�j�b�N�̃p�X�����Ɏ��s�����B
	else {
		for( int i=0, size=mPasses.size(); i<size; ++i ){
			Script::IScript *s = new Script::PassScript( mPasses[i] );
			mScripts.push_back(s);
		}
	}

	mScripts.shrink_to_fit();

}
StandardTechnique::~StandardTechnique(){
	for( int i=0, size=mScripts.size(); i<size; ++i ){
		SAFE_DELETE( mScripts[i] );
	}
	mScripts.clear();
	mScripts.shrink_to_fit();
}


void StandardTechnique::exec(){
	HRESULT hr;
	hr=mEffect->SetTechnique( mHandle );
	assert(SUCCEEDED(hr));

	hr=mEffect->Begin( 0, 0 );
	assert(SUCCEEDED(hr));

	for( int i=0, size=mScripts.size(); i<size; ++i ){
		mScripts[i]->exec();
	}

	//����̃e�N�j�b�N

	//�ȉ��̂悤�ɁA�e�N�j�b�N����ɂ���ƁA���̃e�N�j�b�N�ł͈�ؕ`�悪�s���Ȃ��Ȃ�B
	//����𗘗p����ƁA�I�u�W�F�N�g�̈ꕔ�܂��͑S���̕`���}�~���邱�Ƃ��ł���B

	//�@/*********************************************************/
	//     technique ShadowTech < string MMDPass = "shadow";  > {
	//        
	//     }
	//�@/*********************************************************/

	hr=mEffect->End();
	assert(SUCCEEDED(hr));
}


}
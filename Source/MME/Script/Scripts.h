#pragma once

//#include<iostream>
#include<vector>
#include<d3dx9.h>
#include<string>
#include<assert.h>

namespace MME {
	class BaseEffect;
	class BaseTechnique;
	class Pass;
namespace Script{

class IScript {
public:
	IScript(){}
	virtual ~IScript(){}
	//������Ȃ�true��Ԃ�
	virtual void exec()=0;
};

class PassScript : public IScript {
public:
	PassScript( BaseTechnique *tech, const std::string &passName );
	
	PassScript( Pass *pass )
		:mPass(pass)
	{}
	virtual ~PassScript(){}
	virtual void exec();

	virtual bool isValid()const{
		return mPass!=NULL;
	}
private:
	Pass *mPass;
};


class DrawGeometry : public IScript{
public:
	DrawGeometry( BaseEffect* mme )
		:mEffect(mme)
	{}
	virtual ~DrawGeometry(){}
	virtual void exec();
private:
	BaseEffect *mEffect;
};

class ScriptExternal : public IScript {
public:
	ScriptExternal(){}
	virtual ~ScriptExternal(){}
	virtual void exec(){}
};


//�X�N���v�g���Ԉ���Ă�����NULL��Ԃ���
//�I������ppNext=='\0'
//����new���Ă�̂�delete�����邱��
//���L�|�C���^�ɂ���ׂ���
IScript* createScript(
	const char *pExpression,
	const char **ppNextExpression,
	ID3DXEffect *effect,
	BaseEffect *mme,
	BaseTechnique *technique );

static bool isValid( const char *script ){
	bool result = strchr( script, ';' ) != 0;
	assert( result || strlen(script)==0 && "there is not ;.");
	return result;
}

}//end namespace Script
}//end namespace MME

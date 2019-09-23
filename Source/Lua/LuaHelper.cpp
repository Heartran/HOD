#ifdef _WIN32
#include <windows.h>
#endif

#include <string>
#include <boost/utility/string_view.hpp>
#include <memory>

#include<assert.h>

#include "helper/MyOutputDebugString.h"

#include "LuaHelper.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include"tolua++.h"
#include"tolua_bind/luafunc_glue.h"



typedef std::unique_ptr<Lua::LuaHelper> UptrLua;

static UptrLua gLua;

//�t�@�C�������[�h������
//lua��startfunc�֐����ĂԂ��Ă�
void LoadLuaFiles( const char* arg );


void LuaCreate(const char* classname){
	if(gLua==nullptr){
		gLua.reset( new Lua::LuaHelper );
		// �����[�h�Ə������[�h���܂Ƃ߂�
		if (!gLua->DoFile("Script/common/explicit_globals.lua")) {
			TRACE1( "%s\n Exit!", gLua->GetErr() );
			exit(1);
		}

		std::string loadType(classname);
		assert(
			loadType == "luaTest" ||
			loadType == "HatuneOfTheDead" );


		loadType.append("/load");

		//�������common�Ƃ��ɂ��ċ��ʕ����̂݃��[�h���Ă����H
		LoadLuaFiles(loadType.c_str());
	}
	else {
		assert(false);
	}
}

void LuaDestroy(){
	gLua.reset(nullptr);
}


//��ԑJ�ڂ��S�ă��A�ɔC���Ă���킯�ł͂Ȃ��̂�
//reload.lua�ASequence�ŗL��reload�t�@�C�������ׂ���
//
//�����[�h�����Ƃ�c++��Sequence�͂ǂ�����̂��H
//Title�܂Ŗ߂��Ă��܂����H
//
//���Ƃ���
//ReloadLuaFiles( "all" );���Ă񂾂Ƃ���
//���������lua�X�N���v�g��framefunc��Title:framefunc���ĂԂ悤�ɂȂ�B
//���������ꍇ�^�C�g���ȊO����ReloadLuaFiles( "all" );���Ă񂾂Ƃ��o�O�ɂȂ�
//ReloadLuaFileAll()������Ă��̒��Ő�������ԂɈڍs������
//arg���Œ�p�����[�^�ɂ���͕̂�
//��������AReloadLuaFile( const char* fileName, const char* arg );�̂ق����܂�����
//
//���낢��l�������ʈ�ԍŏ��̐݌v�ɂ��悤
//arg�Ń����[�h������̂�ς���
//�����[�h����t�@�C���͈���
void ReloadLuaFiles( const char* arg ){
	using namespace Lua;
	LuaFuncParam param;
	param.String(arg);
	LuaFuncParam result;
	while (true) {
		if (gLua->DoFile("Script/game/reload.lua", &result, 2, &param)) {
			if (result.GetBool(0)) {
				// �X�N���v�g�����[�h����
				break;
			}
			WaitInputOnError(result.GetString(1), "Reload OK?");
		}
		else {
			WaitInputOnError(gLua->GetErr(), "Reload OK?");
		}
	}
}
void LoadLuaFiles(const char* arg ){
	ReloadLuaFiles(arg);

	// �������pLua�֐����Ă�
	Lua::LuaFuncParam result;
	if (!gLua->CallFunc("startfunc", &result, 1)) {
		TRACE1( "%s\n Exit!", gLua->GetErr() );
		exit(1);
	}
	else {
		// �֐��̕Ԃ�l
		const char *str = result.GetString(0);
	}
}

bool DoLuaFile(const char* path){
	std::string err;
	if (!gLua->DoFile(path, NULL, 0, NULL)) {
		// �G���[�������Ԃ�
		err = gLua->GetErr();
		TRACE1("DoLuaFile: %s", err.c_str());
		return false;
	}
	return true;
}

// Lua�̃G���[���擾����
const char * GetLuaError() {
	return gLua->GetErr();
}

bool CallLuaFunc(const char *funcname, Lua::LuaFuncParam* result, int result_count, Lua::LuaFuncParam* params){
	return gLua->CallFunc( funcname, result, result_count, params );
}

// �G���[���̓��͑҂��֐�
void WaitInputOnError(const char *mes, const char *postfix)
{
#ifdef _WIN32
	char text[1000];
	strcpy(text, mes);
	strcat(text, ".\r\n");
	strcat(text, postfix);
	TRACE(mes);
	MessageBoxA(NULL, text, "Error", MB_OK);
#else
	char text[1000];
	strcpy(text, mes);
	strcat(text, ".\n ");
	strcat(text, postfix);
	strcat(text, "\n>");
	printf(text);
	getchar();
#endif
}	


//
//  class LuaFuncParamItem
//
namespace Lua {
// �o�b�t�@���
void LuaFuncParamItem::ReleaseValue() {
	if (m_type == LUA_TSTRING) {
		if (m_str) delete [] m_str;
		m_str = NULL;
	}
}

// ���l���Z�b�g
void LuaFuncParamItem::SetNumber(double number) {
	ReleaseValue();
	m_type = LUA_TNUMBER;
	m_number = number;
}
// ������l���Z�b�g
void LuaFuncParamItem::SetString(const char * str) {
	ReleaseValue();
	m_type = LUA_TSTRING;
	size_t len = strlen(str);
	m_str = new char[len+1];
	if (m_str) {
		strncpy(m_str, str, len);
		m_str[len] = '\0';
	}
}
// nil�l���Z�b�g
void LuaFuncParamItem::SetNil() {
	ReleaseValue();
	m_type = LUA_TNIL;
}
// bool�l���Z�b�g
void LuaFuncParamItem::SetBool(bool value) {
	ReleaseValue();
	m_type = LUA_TBOOLEAN;
	m_bool = value;
}

// �i�[���Ă���l��Lua�X�^�b�N�ɐς�
void LuaFuncParamItem::PushToStack(lua_State *L) {
	switch(m_type) {
	case LUA_TNUMBER: lua_pushnumber(L, m_number); break;
	case LUA_TSTRING: lua_pushstring(L, m_str); break;
	case LUA_TNIL: lua_pushnil(L); break;
	case LUA_TBOOLEAN: lua_pushboolean(L, m_bool); break;
	}
}

//
//  class LuaFuncParam
//

// �p�����[�^��S�N���A
void LuaFuncParam::Clear() {
	for(int i=0 ; i<m_params_count ; i++) {
		m_params[i].ReleaseValue();
	}
	m_params_count = 0;
}

// ���l�p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::Number(double number) {
	m_params[m_params_count].SetNumber(number);
	m_params_count++;
	return *this;
}
// ������p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::String(const char *str) {
	m_params[m_params_count].SetString(str);
	m_params_count++;
	return *this;
}
// nil�p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::Nil() {
	m_params[m_params_count].SetNil();
	m_params_count++;
	return *this;
}
// �u�[���l�p�����[�^�̒ǉ�
LuaFuncParam & LuaFuncParam::Bool(bool value) {
	m_params[m_params_count].SetBool(value);
	m_params_count++;
	return *this;
}

// �w��C���f�b�N�X�̃p�����[�^��NULL�`�F�b�N
// (�C���f�b�N�X�͂O�x�[�X�j
bool LuaFuncParam::IsNil(int index) {
	if (index < 0 || index >= m_params_count ) return true;
	return m_params[index].IsNil();
}
// �w��C���f�b�N�X�̃p�����[�^���l�擾
// (�C���f�b�N�X�͂O�x�[�X�j
double LuaFuncParam::GetNumber(int index) {
	if (index < 0 || index >= m_params_count ) return 0;
	if (m_params[index].GetType() != LUA_TNUMBER) {
		return 0;
	}
	return m_params[index].GetNumber();
}
// �w��C���f�b�N�X�̃p�����[�^������擾
// (�C���f�b�N�X�͂O�x�[�X�j
const char * LuaFuncParam::GetString(int index) {
	if (index < 0 || index >= m_params_count ) return NULL;
	if (m_params[index].GetType() != LUA_TSTRING) {
		return NULL;
	}
	return m_params[index].GetString();
}
// �w��C���f�b�N�X�̃u�[���l�擾
// (�C���f�b�N�X�͂O�x�[�X�j
const bool LuaFuncParam::GetBool(int index) {
	if (index < 0 || index >= m_params_count ) return NULL;
	if (m_params[index].GetType() != LUA_TBOOLEAN) {
		return NULL;
	}
	return m_params[index].GetBool();
}

// Lua�X�^�b�N�Ɉ������v�b�V�����āA�v�b�V�����������̐���Ԃ�
int LuaFuncParam::PushToStack(lua_State *L) {
	for (int i=0 ; i<m_params_count ; i++) {
		m_params[i].PushToStack(L);
	}
	return m_params_count;
}

// Lua�X�^�b�N����l���擾
// �X�^�b�N�g�b�v����result_count�̒l���擾���Ċi�[
void LuaFuncParam::GetFromStack(lua_State *L, int result_count) {
	for ( int i=0 ; i<result_count ; i++) {
		int index = -result_count+i;
		int type = lua_type(L, index);
		switch(type) {
		case LUA_TNIL: this->Nil(); break;
		case LUA_TSTRING: this->String(lua_tostring(L, index)); break;
		case LUA_TNUMBER: this->Number(lua_tonumber(L, index)); break;
		case LUA_TBOOLEAN: this->Bool(lua_toboolean(L, index)? true:false); break;
		default: this->Nil(); break;
		}
	}
}



//
//  class LuaHelper
//


LuaHelper::LuaHelper()
	: m_L(NULL),
	m_pGetStackTraceFunc(NULL)
{
	m_L =luaL_newstate();
	luaL_openlibs(m_L);
	// tolua��C�̊֐���N���X���C���|�[�g
	tolua_luafunc_open(m_L);
	SetLua( m_L );

	// print�֐���ݒ�
	InitPrintFunc();

	// tolua��C�̊֐���N���X���C���|�[�g
	tolua_luafunc_open(m_L);

}

// Lua�X�e�[�g�����
void LuaHelper::Close() {
	if (m_L)
	{
		lua_close(m_L);
		m_L = NULL;
	}
}

// Lua�X�e�[�g���Z�b�g����
// �����ɁAdebug.traceback�̎����ւ̃|�C���^�𓾂�
// �i���̂��߁ALua���C�u�����I�[�v���オ�]�܂����j
void LuaHelper::SetLua(lua_State *L) {
	m_L = L;
	lua_getglobal(L, "debug");
	if (!lua_isnil(L, -1)) {
		lua_getfield(L, -1, "traceback");
		m_pGetStackTraceFunc = lua_tocfunction(L, -1);
	}
}

// print�֐���ݒ�
void LuaHelper::InitPrintFunc() {
	// Windows�ȊO�Ȃ�W���̂���(stdout�o�́j�ŗǂ�
#ifdef _WIN32
	lua_register(m_L, "print", LuaHelper::LuaPrintWindows);
	lua_atpanic(m_L, LuaHelper::LuaPrintWindows);
#endif
}

#ifdef _WIN32
// print�֐������FVC++�̃��b�Z�[�W�Ƃ��ďo��
int LuaHelper::LuaPrintWindows(lua_State *L) {
	int count = lua_gettop(L);
	lua_getglobal(L, "tostring");
	for (int i=0 ; i<count ; i++) {
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i+1);
		lua_call(L, 1, 1);
		const char *str = lua_tostring(L,-1);
		OutputDebugStringA((str) ? str : "");
		if (i != 0) OutputDebugStringA("\t");
		lua_pop(L, 1);
	}
	OutputDebugStringA("\n");
	return 0;
}
#endif


// ���^�[���l�ƃX�^�b�N�̒l����G���[���b�Z�[�W��ݒ�
void LuaHelper::AnalyzeError(int res_call, const char *location) {
	const char * reason = "";
	switch( res_call ) {
	case LUA_ERRRUN: reason = "SCRIPT RUNTIME ERROR"; break;
	case LUA_ERRSYNTAX: reason = "SCRIPT SYNTAX ERROR"; break;
	case LUA_ERRMEM: reason = "SCRIPT MEMORY ERROR"; break;
	case LUA_ERRFILE: reason = "SCRIPT FILE ERROR"; break;
	default: break;
	}
	// �G���[���b�Z�[�W�擾
	const char *mes = lua_tostring(m_L, -1);
	char err_mes[1000];
	sprintf(err_mes, "%s : %s", reason, mes);
	SetErr(location, err_mes);
}

// �֐��R�[��
// result,params��NULL�ł��ǂ�
bool LuaHelper::CallFunc(const char *funcname, LuaFuncParam* result, int result_count, LuaFuncParam* params) {
	int old_top = lua_gettop(m_L);

	// �Ԃ�l�̓N���A���Ă���
	if (result) result->Clear();

	// �����^�C���G���[�����֐���ς�
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// �֐����݂���
	lua_getglobal(m_L, funcname);
	if (!lua_isfunction(m_L, -1)) {
		char location[300] = "";
		sprintf(location, "calling function<%s>", funcname);
		SetErr(location, "the function not found.");
		return false;
	}

	// params�ɂ���Ďw�肳�ꂽ�ό̈������X�^�b�N�ɒu��
	int params_count = 0;
	if (params) {
		params_count = params->PushToStack(m_L);
	}

	// Lua���̊֐����R�[��
	int res_call = lua_pcall(m_L, params_count, result_count, old_top);

	// �G���[����
	if (res_call != 0) {
		// �G���[���b�Z�[�W����
		char location[300] = "";
		sprintf(location, "calling function<%s>", funcname);
		AnalyzeError(res_call, location);
		// �X�^�b�N�����ɖ߂�
		lua_settop(m_L, old_top);
		return false;
	}

	// �Ԃ�l���
	if (result) result->GetFromStack(m_L, result_count);

	// �X�^�b�N�����ɖ߂�
	lua_settop(m_L, old_top);

	return true;
}

// �t�@�C�����s
// result,params��NULL�ł��ǂ�
bool LuaHelper::DoFile(const char *path, LuaFuncParam* result, int result_count, LuaFuncParam* params) {
	int old_top = lua_gettop(m_L);

	// �Ԃ�l�̓N���A���Ă���
	if (result) result->Clear();

	// �����^�C���G���[�����֐���ς�
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// �t�@�C�������[�h
	int res_load = luaL_loadfile(m_L, path);
	// �G���[����
	if (res_load != 0) {
		// �G���[���b�Z�[�W����
		char location[300] = "";
		sprintf(location, "loading file<%s>", path);
		AnalyzeError(res_load, location);
		lua_settop(m_L, old_top); // �X�^�b�N�����ɖ߂�
		return false;
	}

	// params�ɂ���Ďw�肳�ꂽ�ό̈������X�^�b�N�ɒu��
	int params_count = 0;
	if (params) {
		params_count = params->PushToStack(m_L);
	}

	// ���[�h���ꂽ�`�����N���R�[��
	int res_call = lua_pcall(m_L, params_count, result_count, old_top+1);

	// �G���[����
	if (res_call != 0) {
		// �G���[���b�Z�[�W����
		char location[300] = "";
		sprintf(location, "executing file<%s>", path);
		AnalyzeError(res_call, location);
		lua_settop(m_L, old_top); // �X�^�b�N�����ɖ߂�
		return false;
	}

	// �Ԃ�l���
	if (result) result->GetFromStack(m_L, result_count);

	// �X�^�b�N�����ɖ߂�
	lua_settop(m_L, old_top);

	return true;
}

// �G���[���b�Z�[�W���Z�b�g����
void LuaHelper::SetErr(const char *location, const char *message) {
	sprintf(m_err, "%s : %s", location, message);
}


bool LuaHelper::ChangeScene() {
	const char *funcname="change_scene";
	LuaFuncParam* result=NULL;
	int result_count=0;
	LuaFuncParam* params=NULL;
	int old_top = lua_gettop(m_L);

	// �Ԃ�l�̓N���A���Ă���
	if (result) result->Clear();

	// �����^�C���G���[�����֐���ς�
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// �֐����݂���
	lua_getglobal(m_L, funcname);
	if (!lua_isfunction(m_L, -1)) {
		char location[300] = "";
		sprintf(location, "calling function<%s>", funcname);
		SetErr(location, "the function not found.");
		return false;
	}

	// params�ɂ���Ďw�肳�ꂽ�ό̈������X�^�b�N�ɒu��
	int params_count = 0;
	if (params) {
		params_count = params->PushToStack(m_L);
	}

	// Lua���̊֐����R�[��
	int res_call = lua_pcall(m_L, params_count, result_count, old_top);

	// �G���[����
	if (res_call != 0) {
		// �G���[���b�Z�[�W����
		char location[300] = "";
		sprintf(location, "calling function<%s>", funcname);
		AnalyzeError(res_call, location);
		// �X�^�b�N�����ɖ߂�
		lua_settop(m_L, old_top);
		return false;
	}

	// �Ԃ�l���
	if (result) result->GetFromStack(m_L, result_count);

	// �X�^�b�N�����ɖ߂�
	lua_settop(m_L, old_top);

	return true;
}
}
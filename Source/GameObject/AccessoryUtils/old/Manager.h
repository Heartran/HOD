#ifndef __GAMEOBJECT_ACCESSORYUTILS_MANAGER_H__
#define __GAMEOBJECT_ACCESSORYUTILS_MANAGER_H__
#include<memory>
//#include<functional>
#include<map>
//#include"Accessory.h"
//#include"TemplateAccessory.h"
#include<string>

class btTransform;
class btCollisionObject;
class CBone;

namespace GameLib{
	namespace PseudoXml{
		class ConstElement;
	}
}

namespace GameObject {
class Accessory;

namespace AccessoryUtils {
class TemplateAccessory;
	namespace MME{
		class TemplateModel;
		typedef std::shared_ptr<TemplateModel> SptrTemplateModel;
	}
class Manager {
public:
	static Manager* instance();
	static void destroy();

	
	//@param
	//	name : GameObjectSetting/Accessory/TemplateAccessory.xml�ł����j�b�N�l�[��
	//	�킴�킴�t���p�X������̂��߂�ǂ������̂ƌ`�����L�������̂ō����
	//	
	//	owner : �����������肷��Ɩ���A�N�Z�T���[(��)�Ǝ�����(owner)���Փ˂��Ă��܂��̂ŏՓ˂𖳎����邽�߂ɓ����B
	//	bone : �Ǐ]����{�[�� 
	//std::shared_ptr<Accessory> createAccessory( const char *name, btCollisionObject *owner,  CBone *bone, const char* effectFile=NULL  );

	//@param
	//	name : GameObjectSetting/Accessory/TemplateAccessory.xml�ł����j�b�N�l�[��
	//	�킴�킴�t���p�X������̂��߂�ǂ������̂ƌ`�����L�������̂ō����
	//	
	//	owner : �����������肷��Ɩ���A�N�Z�T���[(��)�Ǝ�����(owner)���Փ˂��Ă��܂��̂ŏՓ˂𖳎����邽�߂ɓ����B
	//	bone : �Ǐ]����{�[�� 
	std::shared_ptr<Accessory> createAccessory( const char *name, btCollisionObject *owner,  const btTransform &startTrans, const char* effectFile=NULL );

	//std::shared_ptr<Model> getAccessoryModel( const char *name );
private:
	typedef std::pair< float, MME::SptrTemplateModel > ScaleAndSptrTemplateModel;

	ScaleAndSptrTemplateModel createTemplateModel( const GameLib::PseudoXml::ConstElement &e );

	//singleton
	Manager();
	~Manager();

	//disallow assign
	Manager( const Manager & );
	//disallow copy
	void operator=( const Manager & );
	struct Deleter
	{
		void operator()( Manager const* const p ) const
		{
			delete p;
		}
	};

	typedef std::unique_ptr< Manager, Deleter > UptrManager;
	static UptrManager mThis;
	
	typedef std::shared_ptr<TemplateAccessory> SptrTemplateAccessory;

	typedef std::map< const std::string, SptrTemplateAccessory > TemplateAccessoryMapType;
	TemplateAccessoryMapType mNameTemplateAccessoryMap;
	typedef std::map< const std::string, MME::SptrTemplateModel > TemplateModelMapType;
	TemplateModelMapType mNameTemplateModelMap;
};

}// end namespace AccessoryUtils
}// end namespace GameObject
#endif

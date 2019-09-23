#pragma once
#include <memory>
#include <vector>
class FileLoader;

namespace GameObject { namespace AccessoryUtils { namespace MME {
class Model;
typedef std::shared_ptr<Model>  ModelSharedPtr;
} } } //end namespace GameObject::AccessoryUtils::MME

namespace MME {

namespace Object{
class IObject;
typedef std::shared_ptr<IObject> IObjectSharedPtr;
	typedef std::weak_ptr<IObject> IObjectWeakPtr;
class ModelKai;
typedef std::shared_ptr<ModelKai> ModelKaiSharedPtr;
class PointLight;
typedef std::shared_ptr<PointLight> PointLightSharedPtr;
class Accessory;
typedef std::shared_ptr<Accessory> AccessorySharedPtr;
}//namespace Object

namespace Batch {
class StandardEffectBatch;
class PostEffectBatch;
typedef std::vector<Object::IObjectWeakPtr> DrawObjects;
typedef std::shared_ptr< DrawObjects > DrawObjectsSharedPtr;
} // namespace Batch

//manager drawer batch �X�̖������悭�l���Đ݌v���ׂ���
//Drawer��������̂͏_��ȕ`��������������̂�
//
namespace Drawer {
class GameDrawer {
public:
	static GameDrawer* instance();
	static void destroy();
	bool isReady();
	
	void draw();
	
	void add( const Object::ModelKaiSharedPtr &);
	void add( const Object::PointLightSharedPtr &);
	void add( const Object::AccessorySharedPtr &);
	void add( const GameObject::AccessoryUtils::MME::ModelSharedPtr&);
	
	//void remove( obj )�͑��p���͂��Ȃ�
	//�폜�ɂȂ邽�т�remove�������
	//�R���e�i��vector�Ȃ̂Œx��(�폜�����Ƃ�����߂Ȃ���΂Ȃ�Ȃ�)
	//�Ȃ��obj->setCanRemove(true);�����邾���ɂ��Ă���
	//���Ƃł܂Ƃ߂č폜�������ق�������
	//forceRemove�����ׂ���
	//���ƍ폜���Y��Ă��Ă�weak_ptr�Ȃ̂Ń��\�[�X���폜���ꂽ��update()�������폜����
	//add��remove�ł킩��₷���悤�ɑΏ̐����~���������̂�remove���������B
	//���ۂȂ���
	//obj->setCanRemove(true);����

	void remove(const Object::ModelKaiSharedPtr &);
	void remove(const Object::PointLightSharedPtr &);
	void remove(const Object::AccessorySharedPtr &);
	void remove(const GameObject::AccessoryUtils::MME::ModelSharedPtr&);


private:
	Batch::StandardEffectBatch*	mDrawGBuffer;
	Batch::PostEffectBatch*			mLuminous;
	Batch::PostEffectBatch*			mFxaa;
	Batch::PostEffectBatch*			mPointLight;
	FileLoader *mFileLoader;
	Batch::DrawObjectsSharedPtr mObjectsPtr;
	Batch::DrawObjectsSharedPtr mLightsPtr;

	GameDrawer();
	~GameDrawer();

	//disallow assign
	GameDrawer( const GameDrawer & );
	//disallow copy
	void operator=( const GameDrawer & );
	struct Deleter
	{
		void operator()( GameDrawer const* const p ) const
		{
			delete p;
		}
	};

	typedef std::unique_ptr< GameDrawer, Deleter > Uptr;
	static Uptr mThis;

};
} }  // namespace MME::Drawer

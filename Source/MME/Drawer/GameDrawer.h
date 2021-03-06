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

//manager drawer batch 個々の役割をよく考えて設計すべきだ
//Drawerを作ったのは柔軟な描画をしたかったので
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
	
	//void remove( obj )は即廃棄はしない
	//削除になるたびにremoveさせると
	//コンテナがvectorなので遅い(削除したところをつめなければならない)
	//ならばobj->setCanRemove(true);させるだけにしている
	//あとでまとめて削除させたほうが早い
	//forceRemoveを作るべきか
	//あと削除し忘れていてもweak_ptrなのでリソースが削除されたらupdate()字自動削除する
	//addとremoveでわかりやすいように対称性が欲しかったのでremoveをつくった。
	//実際なかは
	//obj->setCanRemove(true);だけ

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

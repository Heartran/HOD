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


namespace Drawer{ namespace CutSceneImpl { namespace Light {
//�f�o�b�O�p�ɃC���^�[�t�F�C�X�͓����ŁA���[�h�A�`����y������
class Impl {
public:
	static Impl* instance();
	static void destroy();
	bool isReady();
	
	void draw();
	
	void add( const Object::ModelKaiSharedPtr &);
	void add( const Object::PointLightSharedPtr &);
	void add( const Object::AccessorySharedPtr &);
	void add( const GameObject::AccessoryUtils::MME::ModelSharedPtr&);
	//void remove( obj )���Ȃ����R
	//�폜�ɂȂ邽�т�remove�������
	//�R���e�i��vector�Ȃ̂Œx��(�폜�����Ƃ�����߂Ȃ���΂Ȃ�Ȃ�)
	//�Ȃ��obj->setCanRemove(true);������
	//���Ƃł܂Ƃ߂č폜�������ق�������
private:
	Batch::StandardEffectBatch*	mDrawGBuffer;
	Batch::PostEffectBatch*			mLuminous;
	Batch::PostEffectBatch*			mFxaa;
	Batch::PostEffectBatch*			mPointLight;
	FileLoader *mFileLoader;
	Batch::DrawObjectsSharedPtr mObjectsPtr;
	Batch::DrawObjectsSharedPtr mLightsPtr;

	Impl();
	~Impl();

	//disallow assign
	Impl( const Impl & )=delete;
	//disallow copy
	void operator=( const Impl & )=delete;
	struct Deleter
	{
		void operator()( Impl const* const p ) const
		{
			delete p;
		}
	};

	typedef std::unique_ptr< Impl, Deleter > Uptr;
	static Uptr mThis;

};
} } } }// namespace MME::Drawer::CutSceneImpl::Light

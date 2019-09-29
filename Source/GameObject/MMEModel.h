#pragma once
#include"IGameObject.h"
#include"RTTI/RTTI.h"
#include"MMD/AnimationModel.h"
#include"MMD/AnimationState/Default.h"
#include<memory>
#include<GameLib/GameLib.h>
#include<d3dx9.h>
#include"MME/Manager.h"
#include"MME/Object/Model.h"
#include<assert.h>
#include"Accessory.h"
#include"AccessoryUtils/Manager.h"
#include"BulletPhysics/BulletPhysicsHelper.h"

namespace GameObject{
class MMEModel;
typedef std::shared_ptr<MMEModel> MMEModelSharedPtr;
//MME�\���e�X�g�p���f��
class MMEModel : public IGameObject  {
public:
	//make_shared��private�@�R���X�g���N�^�ĂԂ̂ɕK�v
	friend class std::_Ref_count_obj<MMEModel>;


	static std::shared_ptr<MMEModel> create(){
		std::shared_ptr<MMEModel> result = std::make_shared<MMEModel>();
		//MME::Manager::instance()->regist( result, "Shader/Basic/full_before.fx" );
		//MME::Manager::instance()->regist( result, "Shader/Basic/full_toon.fx" );
		//MME::Manager::instance()->regist( result, NULL );

		 return result;
	}
	virtual ~MMEModel(){
	}

	RTTI_HEADER_IMPL(MMEModel);


	virtual const btTransform & getWorldTransform() const {
		return mTransform; 
	}

	virtual btTransform & getWorldTransform() {
		return mTransform;
	}

	void update( float elapsedTime ){
		mModel->update( toDxMatrix(mTransform), elapsedTime );
	}


private:
	MMEModel()
		:mModel(0),
		mTransform(btTransform::getIdentity()),
		mState(0)
	{
		mModel = MME::Object::Model::create();
		const char *vmdName = "Data/MMDData/vmd/H17_SO_�����~�N_p20_repeatEnd100_allFrames104.vmd";
		//int i = mModel->loadVMD("Data/MMDData/vmd/nothing.vmd");
		mState = new MMD::AnimationState::Default( mModel->getAnimationModel(), vmdName, true, true, true );
		mModel->init( toDxMatrix(mTransform), mState );

	}

	MME::Object::ModelSharedPtr mModel;
	MMD::AnimationState::Default* mState;
	btTransform mTransform;
};
}

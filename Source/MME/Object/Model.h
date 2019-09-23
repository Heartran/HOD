#pragma once
#include"IObject.h"
#include<memory>
#include<string>
struct D3DXMATRIX;

namespace MMD{
	class Bone;

	class AnimationModel;
	namespace AnimationState{
		class IAnimationState;
	}
}


namespace MME{
	namespace Object{

class Model;
typedef std::shared_ptr<Model> ModelSharedPtr;

//MMD::AnimationModel�̃��b�p�[�N���X
class Model : public IObject{
public:
	//make_shared��private�@�R���X�g���N�^�ĂԂ̂ɕK�v
	//vc2015����tr1���ʏ�̃l�[���X�y�[�X�ֈڍs
	friend class std::_Ref_count_obj<Model>;


	//shared_from_this�̓R���X�g���N�^���Ŏg���Ȃ��̂ł������MME::Manager�ɓo�^
	static ModelSharedPtr create();

	virtual ~Model();

	virtual void draw()const;

	virtual float getScale()const;

	virtual float getFaceValue(const std::string &faceName )const;

	virtual const D3DXMATRIX & getBoneMatrix( const std::string &boneName )const;

	virtual const std::string& getModelFileName()const;

	void update( const D3DXMATRIX & world, float elapsed );

	bool isReady();

	void loadPMDFromDB( const char *fileName, bool enablePhysics );

	//�����ʒu���Z�b�g(RigidBody�̂���).
	//warp�̕������O����?setInitWorldTransform();reset
	void init( const D3DXMATRIX &world, MMD::AnimationState::IAnimationState* anim );
	
	bool isAnimOver() const;
	
	MMD::AnimationState::IAnimationState* getCurrentAnimationState();
	void changeAnimation( MMD::AnimationState::IAnimationState* state );
	float getFrame() const;

	MMD::Bone* getBone( const char* boneName );

	MMD::AnimationModel *getAnimationModel(){
		return mModel;
	}
private:
	Model();
	//disallow assign
	Model( const Model& )=delete;
	//disallow copy
	void operator=( const Model& )=delete;

private:
	MMD::AnimationModel *mModel;
};

}// end namespace Object
}// end namespace MME


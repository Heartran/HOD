#ifndef __MMD_ANIMATIONMODEL_H__
#define __MMD_ANIMATIONMODEL_H__
#include<boost/unordered_map.hpp>

struct D3DXMATRIX;


namespace GameLib{
	namespace FileIO {
		class InFile;
	}
}
namespace GameObject{
	class Accessory;
}

namespace MMD {


	class CModel;

	class TemplatePMD;
	typedef std::shared_ptr<TemplatePMD> TemplatePMDSharedPtr;

	class Bone;
	class Animation;

	namespace AnimationState{
		class IAnimationState;
		typedef std::shared_ptr<IAnimationState> IAnimationStateSharedPtr;
		namespace Hook{
			class IHook;
			typedef std::shared_ptr<IHook> IHookSharedPtr;
		}
	}


class AnimationModel{
public:
	AnimationModel();
	AnimationModel( const TemplatePMDSharedPtr &pmd, bool enablePhysics ); 
	virtual ~AnimationModel();	

	void loadPMD( const char *pmdName );
	void loadPMD( const GameLib::FileIO::InFile &inFile );
	void loadPMDFromDB( const char *fileName, bool enablePhysics );

	bool isReady();

	//Animation��Ԃ��B���s�������̃|�C���^��Ԃ�
	std::shared_ptr<Animation> loadVMD( const char *vmdName );
	//index��Ԃ��B���s������-1��Ԃ��B
	//int loadVMD( const GameLib::FileIO::InFile &inFile );	

	//������Ȃ��Ȃ�[1��Ԃ�
	//int getIndex( const char *vmdName ) const;

	//�����ʒu���Z�b�g(RigidBody�̂���).
	//warp�̕������O����?setInitWorldTransform();reset
	void init( const D3DXMATRIX &world, AnimationState::IAnimationState* anim );
	//�����ʒu���Z�b�g(RigidBody�̂���).
	void init( const D3DXMATRIX &world, const std::string & animName );

	//using gamelib
	void draw() const;
	void drawByMME()const;
	void debugDraw() const;
	void update( const D3DXMATRIX &world, float elapsedTime );
	bool isAnimOver() const;
	
	std::shared_ptr<Animation> getCurrentAnimation() const;
	AnimationState::IAnimationState* getCurrentAnimationState();
	void changeAnimation( AnimationState::IAnimationState* state );
	float getFrame() const;

	//const char getPmdName() const;

	Bone* getBone( const char* boneName );
	const std::string& getModelFileName()const;

	void changeAnimation( const std::string& animName );

	std::shared_ptr<GameObject::Accessory> getAccessory( const std::string &name );

	void addHookToAnim( const std::string &toAnimName, const MMD::AnimationState::Hook::IHookSharedPtr &hook );
	void addAnimState( const std::string &animationName, const MMD::AnimationState::IAnimationStateSharedPtr &anim );
	void addAccessory( const std::string &name, const std::shared_ptr<GameObject::Accessory>& accessory  );

	const std::string& getCurrentAnimationName()const;

	//�������E�ɒǉ�
	void addToWorld();
	//�������E����폜
	void removeFromWorld();

private:
	//disallow assingn
	AnimationModel( const AnimationModel& )=delete;
	//disallow copy
	void operator=( const AnimationModel& )=delete;
	
	//�A�N�Z�T���̈ʒu��Bone�̈ʒu�ɍX�V����
	void updateAccessories()const;

	CModel *mModel;

	AnimationState::IAnimationState *mState;

	//�A�j���[�V������Ԃ͂����Ŏ��K�v����̂��H
	//���ɓK���ȏꏊ���Ȃ��̂łƂ肠��������
	typedef boost::unordered_map<std::string, MMD::AnimationState::IAnimationStateSharedPtr> AnimationMap;
	AnimationMap mAnimationMap;
	typedef boost::unordered_map< const std::string, std::shared_ptr<GameObject::Accessory> > AccessoryMap;
	//�����ɃA�N�Z�T���[�u���̂͂悭�Ȃ�?�AAcessory�̎����Ɉˑ����Ă��܂�
	//�������f�����X�V������A�N�Z�T���[���X�V������
	//
	AccessoryMap mAccessoryMap;

};
}
#endif
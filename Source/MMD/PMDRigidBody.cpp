#include <memory.h>
#include "PMDRigidBody.h"
#include "BulletPhysics/Manager.h"
#include <helper/MyOutputDebugString.h>
#include <tchar.h>
#include <btBulletCollisionCommon.h>
#include "Component/RigidBodyComponent.h"




namespace{
	const int kFollowUpBone=0; 
}

namespace MMD {
	RTTI_IMPL(PMDRigidBody, GameObject::BaseObject);

	class PMDRigidBody::CanCollideCallback : public Component::ICanCollideCallback {
	public:
		CanCollideCallback(
			unsigned short filterGroup,
			unsigned short filterMask)
			:mCollisionFilterGroup(filterGroup),
			mCollisionFilterMask(filterMask)
		{}
		virtual ~CanCollideCallback() = default;

		virtual bool needCollision(const btCollisionObject *obj) const override {
			auto c = static_cast<Component::RigidBodyComponent*>(obj->getUserPointer());
			assert(c);
			if (c) {
				GameObject::BaseObject *b = c->getOwner();
				auto pmd = dynamic_cast<PMDRigidBody*>(b);
				assert(pmd);

				return
					pmd &&
					((mCollisionFilterGroup & (pmd->getFilterMask())) != 0) &&
					((pmd->getFilterGroup() & mCollisionFilterMask) != 0);
			}
			return false;
		}

		unsigned short mCollisionFilterGroup;
		unsigned short mCollisionFilterMask;
	};

	// Kinematic�p���[�V�����X�e�[�g
	class PMDRigidBody::btKinematicMotionState : public btMotionState
	{
	public:
		//btTransform bttrGraphicsWorldTrans_;
		btTransform	mBoneOffsetTransform;
		btTransform mStartWorldTransform;
		Bone	*mBone;

		btKinematicMotionState(const btTransform& startTrans, const btTransform& boneOffset, Bone *pBone)
			: mBoneOffsetTransform(boneOffset), mStartWorldTransform(startTrans), mBone(pBone)
		{}

		virtual ~btKinematicMotionState() {}

		///synchronizes world transform from user to physics
		virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const
		{
			btTransform		bttrBoneTransform;

			bttrBoneTransform = toBtTransform(mBone->getSkinningMatrix());
			//btVector3 v = bttrBoneTransform.getOrigin();
			//v += toBtVector3(mBone->initPos_);
			//bttrBoneTransform.setOrigin(v);

			centerOfMassWorldTrans = bttrBoneTransform * mStartWorldTransform;
		}

		///synchronizes world transform from physics to user
		///Bullet only calls the update of worldtransform for active objects
		virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans)
		{
			//mBone->skinningMatrix_ = toDXMatrix( centerOfMassWorldTrans );
			//bttrGraphicsWorldTrans_ = centerOfMassWorldTrans;
		}
	};


	PMDRigidBody::PMDRigidBody()
		:mRigidBodyComponent(nullptr),
		mCanCollideCallback(nullptr),
		mBone(NULL),
		mCollisionFilterGroup(0),
		mCollisionFilterMask(0)
	{

	}


	PMDRigidBody::~PMDRigidBody()
	{
		release();
	}

	bool PMDRigidBody::initialize(const PMD_RigidBody *pPMDRigidBody, Bone *pBone)
	{
		assert(
			!mRigidBodyComponent &&
			!mCanCollideCallback );

		if (mRigidBodyComponent ||
			mCanCollideCallback )
		{
			release();
		}


		BtCollisionShapeSharedPtr shape;
		BtMotionStateSharedPtr motionState;
		BtRigidBodySharedPtr rigidBody;


		// �V�F�C�v�̍쐬
		switch (pPMDRigidBody->cbShapeType)
		{
		case 0:	// ��
			shape.reset( new btSphereShape(pPMDRigidBody->fWidth) );
			break;

		case 1:	// ��
			shape.reset( new btBoxShape(btVector3(pPMDRigidBody->fWidth, pPMDRigidBody->fHeight, pPMDRigidBody->fDepth)) );
			break;

		case 2:	// �J�v�Z��
			shape.reset( new btCapsuleShape(pPMDRigidBody->fWidth, pPMDRigidBody->fHeight) );
			break;
		}

		// ���ʂƊ����e���\���̐ݒ�
		btScalar	btsMass(0.0f);
		btVector3	btv3LocalInertia(0.0f, 0.0f, 0.0f);

		// �{�[���Ǐ]�łȂ��ꍇ�͎��ʂ�ݒ�
		if (pPMDRigidBody->cbRigidBodyType != 0)	btsMass = pPMDRigidBody->fMass;

		// �����e���\���̌v�Z
		if (btsMass != 0.0f)	shape->calculateLocalInertia(btsMass, btv3LocalInertia);

		// �{�[���̈ʒu�擾
		D3DXVECTOR3 vec3BonePos;
		vec3BonePos = pBone->initPos_;

		// �{�[���I�t�Z�b�g�p�g�����X�t�H�[���쐬
		btMatrix3x3	btmRotationMat;
		btmRotationMat.setEulerZYX(pPMDRigidBody->vec3Rotation[0], pPMDRigidBody->vec3Rotation[1], pPMDRigidBody->vec3Rotation[2]);

		btTransform		bttrBoneOffset;

		bttrBoneOffset.setIdentity();
		bttrBoneOffset.setOrigin(btVector3(pPMDRigidBody->vec3Position[0], pPMDRigidBody->vec3Position[1], pPMDRigidBody->vec3Position[2]));
		bttrBoneOffset.setBasis(btmRotationMat);

		// ���̂̏����g�����X�t�H�[���쐬
		btTransform bttrStartTransform;
		bttrStartTransform.setIdentity();
		bttrStartTransform.setOrigin(btVector3(vec3BonePos.x, vec3BonePos.y, vec3BonePos.z));
		bttrStartTransform = bttrStartTransform * bttrBoneOffset;


		//�^�C�v(0:Bone�Ǐ]�A1:�������Z�A2:�������Z(Bone�ʒu����)) 
		mBoneType = pPMDRigidBody->cbRigidBodyType;
		switch (pPMDRigidBody->cbRigidBodyType)
		{
		case 0:
			motionState.reset( new btKinematicMotionState(bttrStartTransform, bttrBoneOffset, pBone) );
			break;
		case 1:
			motionState.reset( new btDefaultMotionState(bttrStartTransform) );
			break;
		case 2:
			motionState.reset( new btDefaultMotionState(bttrStartTransform) );
			break;
			//case 2 :pbtMotionState = new btKinematicMotionState( bttrTransform, bttrBoneOffset, pBone );	break;
		default: assert(false && "failed PMDRigidBody::initialize."); break;
		}

		// ���̂̃p�����[�^�̐ݒ�
		btRigidBody::btRigidBodyConstructionInfo	btRbInfo(btsMass, motionState.get(), shape.get(), btv3LocalInertia);
		btRbInfo.m_linearDamping = pPMDRigidBody->fLinearDamping;	// �ړ���
		btRbInfo.m_angularDamping = pPMDRigidBody->fAngularDamping;	// ��]��
		btRbInfo.m_restitution = pPMDRigidBody->fRestitution;	// ������
		btRbInfo.m_friction = pPMDRigidBody->fFriction;		// ���C��
		btRbInfo.m_additionalDamping = true;

		// ���̂̍쐬
		rigidBody.reset( new btRigidBody(btRbInfo) );
		mRigidBodyComponent = new Component::RigidBodyComponent(
			this,
			shape,
			motionState,
			rigidBody );


		// Kinematic�ݒ�
		if (pPMDRigidBody->cbRigidBodyType == kFollowUpBone)
		{
			mRigidBodyComponent->setIsKinematic(true);
		}
		else {
			//TRACE1( "%s isn't Kinematic.\n", pBone->getName() );
		}

		//pRigidBody_->setSleepingThresholds( 0.0f, 0.0f );v

		//if(btsMass==0){ TRACE1( "%s is static.\n", pBone->getName() ); }

		// ���̂��V�~�����[�V�������[���h�ɒǉ�
		BulletPhysics::Manager *mngr = BulletPhysics::Manager::instance();

		
		//MMD�œƎ��̃t�B���^�����O�̂���
		mCollisionFilterGroup = 0x0001 << (pPMDRigidBody->cbColGroupIndex);
		mCollisionFilterMask = pPMDRigidBody->unColGroupMask;

		mCanCollideCallback = new CanCollideCallback(mCollisionFilterGroup, mCollisionFilterMask);
		mRigidBodyComponent->setCanCollideCallback(mCanCollideCallback);

		mBoneOffsetTransform = bttrBoneOffset;
		mInvStartTransform = bttrStartTransform.inverse();

		mBone = pBone;


		if (strncmp(pBone->getName(), "�Z���^�[", 20) == 0) {
			m_bNoCopyToBone = true;
		}
		else {
			m_bNoCopyToBone = false;
		}

		return true;
	}



	// �{�[���̎p�������̂̎p���ƈ�v������(���̃t���[���̃V�~�����[�V�����I����ɌĂ�)
	void PMDRigidBody::updateBoneTransform(void)
	{
		if (mBoneType != kFollowUpBone && !m_bNoCopyToBone)
		{
			btTransform	bttrBoneTransform = mRigidBodyComponent->getRigidBody()->getCenterOfMassTransform() * mInvStartTransform;
			mBone->skinningMatrix_ = toDxMatrix(bttrBoneTransform);
		}
	}

	void PMDRigidBody::release(void)
	{
		if (mRigidBodyComponent)
		{
			removeFromWorld();

		}
		SAFE_DELETE(mRigidBodyComponent);
		SAFE_DELETE(mCanCollideCallback);
	}

	void PMDRigidBody::debugDraw() {
		mBone->debug_draw();
	}
	//���̂�bone�̎p���Ɉ�v������
	void PMDRigidBody::fitBone(void)
	{
		btTransform bttrBoneTransform = toBtTransform(mBone->getSkinningMatrix());
		btTransform bttrStratTransform = mBoneOffsetTransform;
		btVector3 v = mBoneOffsetTransform.getOrigin();
		bttrStratTransform.setOrigin(v + toBtVector3(mBone->initPos_));
		mInvStartTransform = bttrStratTransform.inverse();

		btTransform bttrTransform = bttrBoneTransform*bttrStratTransform;
		auto rb = mRigidBodyComponent->getRigidBody();

		rb->setCenterOfMassTransform(bttrTransform);

		rb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		rb->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		rb->setInterpolationLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		rb->setInterpolationAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		rb->setInterpolationWorldTransform(rb->getCenterOfMassTransform());
		rb->clearForces();

		for (int i = rb->getNumConstraintRefs() - 1; i > -1; --i) {
			btGeneric6DofSpringConstraint *pC = dynamic_cast<btGeneric6DofSpringConstraint*>(rb->getConstraintRef(i));
			assert(pC);
			if (pC) {
				pC->calculateTransforms();
			}
		}

	}

	void PMDRigidBody::addToWorld() {
		assert(mRigidBodyComponent);

		if (mRigidBodyComponent) {
			mRigidBodyComponent->addToWorld(
				BulletPhysics::MMDObjectFilter,
				BulletPhysics::MMDObjectFilter);
		}
	}

	bool PMDRigidBody::removeFromWorld() {
		assert(mRigidBodyComponent);
		if (mRigidBodyComponent) {
			return mRigidBodyComponent->removeFromWorld();
		}
		return false;
	}

	void PMDRigidBody::setVelocityZero() {
		assert(mRigidBodyComponent);
		auto rb = mRigidBodyComponent->getRigidBody();

		rb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		rb->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

		rb->clearForces();

		for (int i = rb->getNumConstraintRefs() - 1; i > -1; --i) {
			btGeneric6DofSpringConstraint *pC = dynamic_cast<btGeneric6DofSpringConstraint*>(rb->getConstraintRef(i));
			assert(pC);
			if (pC) {
				pC->calculateTransforms();
			}
		}
	}

	btRigidBody* PMDRigidBody::getRigidBody() {
		return mRigidBodyComponent->getRigidBody().get();
	}


	int PMDRigidBody::calculateDamage()
	{
		assert(false && 
			"�������Ƃ��邱�Ƃ͂Ȃ��͂��A�݌v�I�ɌĂԂ͂��Ȃ�\
			mmd�I�u�W�F�N�g�͑��̂��̂ɐG����Ȃ��̂�\
			�����������ɐ݌v���Ȃ������̂Ȃ炱�����ς���K�v������");
		return 0;
	}

	bool PMDRigidBody::canDamage()
	{
		assert(false &&
			"�������Ƃ��邱�Ƃ͂Ȃ��͂��A�݌v�I�ɌĂԂ͂��Ȃ�\
			mmd�I�u�W�F�N�g�͑��̂��̂ɐG����Ȃ��̂�\
			�����������ɐ݌v���Ȃ������̂Ȃ炱�����ς���K�v������");
		return false;
	}

	const GameObjectData::WeaponStatus * PMDRigidBody::getWeaponStatus()
	{
		assert(false &&
			"�������Ƃ��邱�Ƃ͂Ȃ��͂��A�݌v�I�ɌĂԂ͂��Ȃ�\
			mmd�I�u�W�F�N�g�͑��̂��̂ɐG����Ȃ��̂�\
			�����������ɐ݌v���Ȃ������̂Ȃ炱�����ς���K�v������");
		return nullptr;
	}



}
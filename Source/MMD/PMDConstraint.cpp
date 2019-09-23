// PMD�R���X�g���C���g(�W���C���g)


#include "PMDConstraint.h"
#include "BulletPhysics/Manager.h"
#include "Pmd.h"
#include "PMDRigidBody.h"

namespace MMD {
	PMDConstraint::PMDConstraint(void)
		: pConstraint_(NULL),
		mIsInWorld(false),
		mIsInCutScene(false)
	{
	}

	PMDConstraint::~PMDConstraint(void)
	{
		release();
	}

	bool PMDConstraint::initialize(const PMD_Constraint *pPMDConstraint, PMDRigidBody *pRigidBodyA, PMDRigidBody *pRigidBodyB)
	{
		// ��]�s��쐬
		btMatrix3x3	btmRotationMat;

		btmRotationMat.setEulerZYX(pPMDConstraint->vec3Rotation[0], pPMDConstraint->vec3Rotation[1], pPMDConstraint->vec3Rotation[2]);

		// �R���X�g���C���g�̃g�����X�t�H�[�����쐬
		btTransform		bttrTransform;

		bttrTransform.setIdentity();
		bttrTransform.setOrigin(btVector3(pPMDConstraint->vec3Position[0], pPMDConstraint->vec3Position[1], pPMDConstraint->vec3Position[2]));
		bttrTransform.setBasis(btmRotationMat);

		// ����A,B���猩���R���X�g���C���g�̃g�����X�t�H�[�����쐬 
		btTransform		bttrRigidAInvTransform = pRigidBodyA->getRigidBody()->getWorldTransform().inverse(),
			bttrRigidBInvTransform = pRigidBodyB->getRigidBody()->getWorldTransform().inverse();

		bttrRigidAInvTransform = bttrRigidAInvTransform * bttrTransform;
		bttrRigidBInvTransform = bttrRigidBInvTransform * bttrTransform;

		pConstraint_ = new btGeneric6DofSpringConstraint(*pRigidBodyA->getRigidBody(), *pRigidBodyB->getRigidBody(),
			bttrRigidAInvTransform, bttrRigidBInvTransform, true);

		// �e�퐧���p�����[�^�̃Z�b�g
		pConstraint_->setLinearLowerLimit(btVector3(pPMDConstraint->vec3PosLimitL[0], pPMDConstraint->vec3PosLimitL[1], pPMDConstraint->vec3PosLimitL[2]));
		pConstraint_->setLinearUpperLimit(btVector3(pPMDConstraint->vec3PosLimitU[0], pPMDConstraint->vec3PosLimitU[1], pPMDConstraint->vec3PosLimitU[2]));

		pConstraint_->setAngularLowerLimit(btVector3(pPMDConstraint->vec3RotLimitL[0], pPMDConstraint->vec3RotLimitL[1], pPMDConstraint->vec3RotLimitL[2]));
		pConstraint_->setAngularUpperLimit(btVector3(pPMDConstraint->vec3RotLimitU[0], pPMDConstraint->vec3RotLimitU[1], pPMDConstraint->vec3RotLimitU[2]));


		if (pPMDConstraint->vec3SpringPos[0] != 0.0f)
		{
			//index 0 : translation X
			pConstraint_->enableSpring(0, true);
			pConstraint_->setStiffness(0, pPMDConstraint->vec3SpringPos[0]);
		}

		if (pPMDConstraint->vec3SpringPos[1] != 0.0f)
		{
			//index 1 : translation Y
			pConstraint_->enableSpring(1, true);
			pConstraint_->setStiffness(1, pPMDConstraint->vec3SpringPos[1]);
		}

		if (pPMDConstraint->vec3SpringPos[2] != 0.0f)
		{
			//index 2 : translation Z
			pConstraint_->enableSpring(2, true);
			pConstraint_->setStiffness(2, pPMDConstraint->vec3SpringPos[2]);
		}

		if (pPMDConstraint->vec3SpringRot[0] != 0.0f)
		{
			// 3 : rotation X (3rd Euler rotational around new position of X axis, range [-PI+epsilon, PI-epsilon] )
			pConstraint_->enableSpring(3, true);
			pConstraint_->setStiffness(3, pPMDConstraint->vec3SpringRot[0]);
		}

		if (pPMDConstraint->vec3SpringRot[1] != 0.0f)
		{
			// 4 : rotation Y (2nd Euler rotational around new position of Y axis, range [-PI/2+epsilon, PI/2-epsilon] )
			pConstraint_->enableSpring(4, true);
			pConstraint_->setStiffness(4, pPMDConstraint->vec3SpringRot[1]);
		}

		if (pPMDConstraint->vec3SpringRot[2] != 0.0f)
		{
			// 5 : rotation Z (1st Euler rotational around Z axis, range [-PI+epsilon, PI-epsilon] )
			pConstraint_->enableSpring(5, true);
			pConstraint_->setStiffness(5, pPMDConstraint->vec3SpringRot[2]);
		}

		return true;
	}

	//rigidBody���Q�Ƃ��Ă���̂ł�����J������O�ɊJ�����邱��
	void PMDConstraint::release(void)
	{
		if (pConstraint_)
		{
			removeFromWorld();
			removeFromCutScene();
			SAFE_DELETE(pConstraint_);
		}
	}

	void PMDConstraint::addToWorld() {
		if (!mIsInWorld) {
			auto bullet = BulletPhysics::Manager::instance(BulletPhysics::GameWorldIndex);
			bullet->addToWorld(pConstraint_);
			mIsInWorld = true;
		}
	}

	void PMDConstraint::removeFromWorld() {
		if (mIsInWorld) {
			auto bullet = BulletPhysics::Manager::instance(BulletPhysics::GameWorldIndex);
			bullet->removeFromWorld(pConstraint_);
			mIsInWorld = false;
		}
	}

	void PMDConstraint::addToCutScene() {
		if (!mIsInCutScene) {
			auto bullet = BulletPhysics::Manager::instance(BulletPhysics::CutSceneWorldIndex);
			bullet->addToWorld(pConstraint_);
			mIsInCutScene = true;
		}
	}

	void PMDConstraint::removeFromCutScene() {
		if (mIsInCutScene) {
			auto bullet = BulletPhysics::Manager::instance(BulletPhysics::CutSceneWorldIndex);
			bullet->removeFromWorld(pConstraint_);
			mIsInCutScene = false;
		}
	}


}
#include"BoneCollisionObject.h"
#include"BulletPhysics/Manager.h"
#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionDispatch/btGhostObject.h>
#include"MMD/Bone.h"
#include"BulletPhysics/BulletPhysicsHelper.h"
#include"helper/PrintBtVector3.h"
#include"GameObject/CharacterControllerGhostObject.h"
#include<GameLib/Scene/Container.h>
#include<GameLib/GameLib.h>
#include"SegaTypeHelper.h"
#include"helper/PrimitiveDrawer.h"
#include<GameLib/Scene/Batch.h>
#include"GameObjectData/Status.h"
#include"GameObjectData/WeaponStatus.h"
#include"Enemy.h"

using BulletPhysicsHelper::toBtTransform;
namespace GameObject {

RTTI_IMPL( BoneCollisionObject, ICollider );
//RTTI_IMPL( GameObject::BoneCollisionObject, GameObject::ICollider );


struct BoneCollisionObject::Model {
	Model()
		:mBatchs(NULL),
		mScale(1.f),
		mNumBatchs(0)
	{}
	~Model(){
		SAFE_DELETE_ARRAY( mBatchs );
		mScale=0.f;
		mNumBatchs=0;
	}
	void setScale( const float & s ){
		mScale=s;
	}
	void createBatchsArray( unsigned int numBatchs ){
		mNumBatchs=numBatchs;
		mBatchs = new GameLib::Scene::Batch[mNumBatchs];
	}
	void setBatch( unsigned int index, GameLib::Scene::Batch b ){
		assert( index < mNumBatchs );
		mBatchs[index]=b;
	}
	void draw( const btTransform &bttr ){
		using namespace GameLib::Graphics;

		GameLib::Math::Matrix34 w34= toMatrix34( bttr );
		w34.scale(mScale);
		Manager::instance().setWorldMatrix( w34 );
		Manager::instance().setAmbientColor( GameLib::Math::Vector3(1.f) ); 

		for( unsigned int i=0; i<mNumBatchs; ++i){
			mBatchs[i].draw();
		}
	}
	GameLib::Scene::Batch *mBatchs;
	unsigned int mNumBatchs;
	float mScale;
};

struct BoneCollisionObject::KinematicMotionState : public btMotionState
{
	CBone	*mBone;

	KinematicMotionState(CBone *pBone )
		: mBone( pBone )
	{}
	virtual ~KinematicMotionState(){}

	///synchronizes world transform from user to physics
	virtual void getWorldTransform(btTransform& centerOfMassWorldTrans ) const
	{
		//fix�@���[�񂢂낢�떳�ʂ���ȁ[bone�N���X���̂�������������
		centerOfMassWorldTrans=toBtTransform( mBone->getSkinningMatrix() );
		centerOfMassWorldTrans.setOrigin( toBtVector3(mBone->getPos()) );
	}

	///synchronizes world transform from physics to user
	///Bullet only calls the update of worldtransform for active objects
	virtual void setWorldTransform( const btTransform& centerOfMassWorldTrans )
	{}
};

BoneCollisionObject::BoneCollisionObject(
	float mass,
	float start, float end,
	std::shared_ptr<btCollisionShape> shape,
	btCollisionObject *owner,
	CBone *bone,
	GameObjectData::WeaponStatus *weaponStatus )
	:
	//mGhost(NULL),
	mRigidBody(NULL),
	mShape(shape),
	mBtObjOfOwner(owner),
	mBone(bone),
	mIsInWorld(false),
	mStart(start), mEnd(end),
	mModel(NULL),
	//mContainer(NULL),
	mWeaponStatus(weaponStatus)
{
	btTransform bttr= toBtTransform( bone->getSkinningMatrix() );

	// �����e���\���̌v�Z
	btVector3 btv3LocalInertia(0.f, 0.f, 0.f);
	if( mass != 0.0f ) mShape->calculateLocalInertia( mass, btv3LocalInertia );

	// MotionState���쐬����B���̂̎p��������������	
	btMotionState *pMotionState = new KinematicMotionState( bone );

	// ���̂��쐬����
	// ���� 0.0�A�����e���\�� 0.0 �Ȃ炱�̍��͓̂����Ȃ�
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, pMotionState, mShape.get(), btv3LocalInertia );
	mRigidBody = new btRigidBody( rbInfo );

	//mRigidBody = BulletPhysicsHelper::createRigidBody( mass, bttr, mShape );
	
	//mRigidBody->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT );
	mRigidBody->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE );
	mRigidBody->setActivationState( DISABLE_DEACTIVATION );
	if( owner ){
		ignoreCollision(owner);
	}
	mRigidBody->setUserPointer( this );
}
BoneCollisionObject::~BoneCollisionObject(){
	removeFromWorld();
	SAFE_DELETE(mModel);
	//SAFE_DELETE(mContainer);

	btMotionState* pM = mRigidBody->getMotionState();
	SAFE_DELETE( pM );
	SAFE_DELETE(mRigidBody);
	//mShape.reset( (void*)0 );
}

void BoneCollisionObject::addToWorld(){
	if(!mIsInWorld){
		//�����ʒu�������������Ă���
		mRigidBody->getMotionState()->getWorldTransform( mRigidBody->getWorldTransform() );
		//const int mmdFilter = 0x7fc0;
		//BulletPhysics::Manager::instance()->addToWorld( mRigidBody, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter & ~mmdFilter );
		BulletPhysics::Manager::instance()->addToWorld( mRigidBody, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter & ~BulletPhysics::Manager::MMDObjectFilter );
		mIsInWorld=true;
	}
}
void BoneCollisionObject::removeFromWorld(){
	if(mIsInWorld){
		BulletPhysics::Manager::instance()->removeFromWorld( mRigidBody );
		mIsInWorld=false;
	}
}
	
//float �͂����Ă��̏ꍇ�@elapsedTime=1.f/30.f sec
//�����ABoneCollisionObject�̏ꍇ�̂� currentFrame
//�����t���[���ƏI���t���[���œ����蔻����o�����ߌ��݂̃t���[�����~����
void BoneCollisionObject::update( float currentFrame ){
	if( mStart <= currentFrame && currentFrame <= mEnd ){
		addToWorld();
	}
	else {
		removeFromWorld();
	}
}
	
void BoneCollisionObject::draw() const{
	using namespace GameLib::Math;
	using namespace GameLib::Graphics;
	
	if( mIsInWorld ){
//#ifdef _DEBUG
		//1�t���[���x�ꂽ�\��
		//�����蔻��̕\��
		PrimitiveDrawer::draw( mRigidBody, 0xffff0000 );
//#endif
		//�{�[���X�V��ŕ����X�V�O�Ȃ̂�mRigidBody->getWorldTransform()�ł�1�t���[���x���
		//���ۂɕ\������̂�model(�~�N����)�Ɉ�v���ė~�����̂�kinematicMotionState���璼�ڂ���Ă���
		if(mModel){
			btTransform w;
			mRigidBody->getMotionState()->getWorldTransform(w);
			
			mModel->draw(w);
		}
	}
		
	
}

//btTransform const & BoneCollisionObject::getWorldTransform() const{
//	return mRigidBody->getWorldTransform();
//}
//btTransform & BoneCollisionObject::getWorldTransgorm(){
//	return mRigidBody->getWorldTransform();
//}

btCollisionObject* BoneCollisionObject::getBtCollisionObject(){
	return mRigidBody;
}

void BoneCollisionObject::onCollisionStay( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj ) {
	assert(me==mRigidBody);
	
	ICollider *gameObj = static_cast<ICollider*>( obj->getUserPointer() );
	//if (gameObj->getRTTI().isExactly( BoneCollisionObject::rtti ) )
	//{
	//	//btVector3 vel = mBtObjOfOwner->getWorldTransform().getOrigin() - obj->getWorldTransform().getOrigin();
	//	btVector3 vel = me->getWorldTransform().getOrigin() - obj->getWorldTransform().getOrigin();
	//		
	//	float l = vel.length();
	//	if( l== 0.f ){
	//		return;
	//	}
	//	vel /= l;
	//	
	//	ICollider *gameOwnerObj = static_cast<ICollider*>( mBtObjOfOwner->getUserPointer() );

	//	float knocbackLength = 2.f;

	//	//�e���m�b�N�o�b�N
	//	if( gameOwnerObj->getRTTI().isExactly( CharacterControllerGhostObject::rtti ) ){
	//		//CharacterControllerGhostObject* cco = static_cast<CharacterControllerGhostObject*>(gameOwnerObj);
	//		//cco->strafe( mBtObjOfOwner->getWorldTransform().getOrigin() + knocbackLength*vel  );
	//		//�A�j���[�V�����~�����H
	//	} else if( mBtObjOfOwner->getInternalType() ==btCollisionObject::CO_RIGID_BODY ){
	//		//btRigidBody::upcast(mBtObjOfOwner)->applyCentralForce( vel );
	//	}
	//}
}

//float BoneCollisionObject::getMass() const {
//	return 1.f/mRigidBody->getInvMass();
//}

void BoneCollisionObject::calcDamage( GameObjectData::Status *st ){
	mWeaponStatus->attack( st );
}

//void BoneCollisionObject::setWeaponStatus( GameObjectData::WeaponStatus *weaponSt ){
//	mWeaponStatus=weaponSt;
//}

void BoneCollisionObject::attack( Enemy *enemy, GameObjectData::Status *enemyStatus ){
	calcDamage( enemyStatus );
	btVector3 vec2Enemy = enemy->getWorldTransform().getOrigin() - mRigidBody->getWorldTransform().getOrigin();
	float length = vec2Enemy.length();
	if(length > 0 ){
		vec2Enemy /= length;
	} else {
		length=1.f;
		vec2Enemy.setValue( 0.f, 1.f, 0.f );
	}
	//const float *normalVel = mWeaponStatus->knockBackNormalVel;
	//const float *velocity = mWeaponStatus->knockBackVelSpeed;
	//const float upDownWardOffset = mWeaponStatus->upDownwardOffset
	
	//�������߂��قǏՌ���傫������B
	//length*=0.003f;
	//length*=length;
	//vec2Enemy/=length;

	float power = 10.f;
	enemy->hitback( vec2Enemy*enemy->getMass()*power );
}

void BoneCollisionObject::createModel( unsigned int numBatchs, float scale ){
	if(mModel){
		SAFE_DELETE(mModel);
	} 
	mModel=new Model;
	mModel->createBatchsArray( numBatchs );
	mModel->setScale( scale );
}

void BoneCollisionObject::setBatch( unsigned int index, GameLib::Scene::Batch b ){
	assert(mModel);
	mModel->setBatch( index, b );
}

}//end namespace GameObject
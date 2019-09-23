#ifndef __CHARACTER_CONTROLLER_PROTOTYPE_H__
#define __CHARACTER_CONTROLLER_PROTOTYPE_H__
#include"GameObject/Character/old/OldCharacterControllerObject.h"
#include"helper/SphereMesh.h"
#include<btBulletDynamicsCommon.h>
#include<GameLib/Input/Keyboard.h>
#include<BulletCollision/CollisionDispatch/btGhostObject.h>
#include"MMD/MMDMikuModel.h"
#include"BulletPhysics/BulletPhysics_Manager.h"
#include"BulletPhysics/BulletPhysicsHelper.h"
#include"helper/MyOutputDebugString.h"
#include<tchar.h>

#include"MouseWrapper.h"
#include"helper/SphereMesh.h"
#include"Camera/SpringCamera.h"
#include"Camera/SpringCameraWithoutYAxis.h"


class CCharacterContorllerProtoType{
public:
	CCharacterContorllerProtoType()
		:mVelocity(10.f),
		mAngulerVelocity( 2.f*3.14f * 0.01 * 30.f ),//2PI * ��]/�b * fps
		mController(
			btTransform( 
				btMatrix3x3(	1,0,0,
								0,1,0,
								0,0,1 ) ,
				btVector3(0, 60 , 0)
			) ,
			14.f, 2.f,//height, width
			10.f,//vel
			2.f*3.14f * 0.01 * 30.f//angVel
		),
		mUpLength(15.f),
		mBackwardLength(30.f),
		mHeight(9.f),
		mWidth(3.f)
	{
		mMousePosition = mController.getWorldTransform().getOrigin();
		const btTransform &world = mController.getWorldTransform();
		D3DXMATRIX dxW = toDxMatrix(world);
		dxW._42 -= mHeight;
		mModel.init( dxW );
		mModel.changeAnimation( mModel.ANIM_RUN );
		Vector3 cameraTarget( toVector3(world.getOrigin()) );
		Vector3 cameraPos;
		cameraPos.setAdd( cameraTarget, Vector3( 0.f, mUpLength, mBackwardLength ) );
		mCamera.reset( cameraPos, cameraTarget, cameraPos );
		mController.setUserPtr(this);
	}
	virtual ~CCharacterContorllerProtoType()
	{}

	const btTransform& getWorldTransform() const
	{
		return mController.getWorldTransform();
	}

	void draw(){
		using namespace GameLib::Graphics;
		mController.draw();
		mModel.draw();
		
		//debug�p�Ɍ��ݒn�\��
		//{
		//	const btTransform &w = mController.getWorldTransform();
		//	TRACE3( "pos( %f, %f, %f )\n", w.getOrigin().x(), w.getOrigin().y(), w.getOrigin().z() ); 
		//}

		//�}�E�X�J�[�\�������[���h��Ԃɕ\��
		if(true){
			SphereMesh mesh;
			btTransform bttr;
			bttr.setIdentity();
			bttr.setOrigin(mMousePosition);
			mesh.draw( bttr, 5 );
		}
		//�O�̕�����\��
		if(true){
			const btTransform &world = mController.getWorldTransform();
			btVector3 z=world.getBasis()[2];
			btVector3 z0(0.f, 0.f, 1.f);
			float rad = acosf ( z.dot(z0) );
			float s = sinf(-rad);
			float c = cosf(-rad);
			//TRACE2("sin: %f, cos: %f \n", s, c);
			//TRACE2("x  : %f, z  : %f \n", z.x(), z.z() );
			btVector3 pos = world.getOrigin();
			VertexBuffer line = VertexBuffer::create(2);
			Vertex *pV=line.lock();
			{
				pV[0].mPosition.set( pos.x(), pos.y(), pos.z() );
				//pos- rotMat( axis(0,1,0), -acos(z�Ez0) )�Ez0
				pV[1].mPosition.set( pos.x()+z.x()*10, pos.y()-z.y()*10, pos.z()-z.z()*10 );
				pV[0].mNormal.set(0.f, 0.f, 0.f);
				pV[1].mNormal.set(0.f, 0.f, 0.f);
				pV[0].mColor = 0xff00ffff;
				pV[1].mColor = 0xff00ffff;
			}
			line.unlock( &pV );
			Matrix34 wm;
			wm.setIdentity();
			GameLib::Graphics::Manager mngr = GameLib::Graphics::Manager::instance();
			mngr.setWorldMatrix(wm);
			mngr.setTexture(0);
			mngr.setVertexBuffer( line );
			mngr.enableVertexBlend(false);
			mngr.draw( 0, 1, PRIMITIVE_LINE );
		}
	}
	//elapsedTime
	//start : �X�N���[�����W�����[���h���W�ɂ����}�E�X�̊J�n�_
	//end   : �X�N���[�����W�����[���h���W�ɂ����}�E�X�̏I���_
	void update( float elapsedTime, const btVector3 &start, const btVector3 &end ){
		CMouseWrapper::update();
		//Mouse m;
		float pitch = CMouseWrapper::velocityX();
		float yaw = CMouseWrapper::velocityY();
		bool isJump = CMouseWrapper::isTriggeredDoubleClick(Mouse::BUTTON_LEFT);
		bool isDash = CMouseWrapper::isOnClick(Mouse::BUTTON_LEFT);
		bool isAttack = false; //m.isTriggered(Mouse::BUTTON_LEFT);
		//assert( !CMouseWrapper::isReleased(Mouse::BUTTON_LEFT) );

		const btTransform &world = mController.getWorldTransform();
		
		//�I�u�W�F�N�g��"�㕽��"��Ƀ}�E�X�J�\�[�����ˉe
		{
			//btVector3 normalPlane(0.f,1.f,0.f);
			btVector3 normalPlane = world.getBasis()[1];
			normalPlane.normalize();
			btVector3 posPlane=world.getOrigin();
			btVector3 line = end-start;
			//���_�Ɉړ�			
			float startToPlaneLength = (start-posPlane).dot(normalPlane);
			float length = line.normalize().dot( -normalPlane )*startToPlaneLength;
			//�O�p�`�̑������
			float rate=startToPlaneLength*startToPlaneLength/length;

			mMousePosition=line*rate+start;
			//TRACE1("startToPlaneLength:%f\n", startToPlaneLength );
			//TRACE3("(%f, %f, %f)\n", mMousePosition.x(), mMousePosition.y(), mMousePosition.z() );

			//�J�����̐ݒ肪����Ȃ�A�@����line�������Ȃ����Ƃ͂Ȃ��͂�
		}
		mController.update( elapsedTime, pitch, yaw, isDash, isJump, isAttack, mMousePosition );

		//�A�j���[�V�����֘A
		{
			 const btVector3& vel = mController.getVelocity();
			 if(vel.length()==0.f){
				 //mModel.changeAnimation(mModel.ANIM_STAND);
			 } else{
				 //mModel.changeAnimation( mModel.ANIM_RUN );
			 }
		}

		D3DXMATRIX dxW = toDxMatrix(world);
		dxW._42 -= mHeight;
		mModel.update( dxW, elapsedTime );

		//if(mController.onGround()){
		//	//TRACE0(_T("onGroud!\n"));
		//	mModel.update( dxW, elapsedTime );
		//} else {
		//	TRACE0(_T("onAir!\n"));
		//	mController.onGroundPerhaps();
		//	//mModel.update( dxW, elapsedTime, mController.getWorldTransform().getOrigin().y() - mHeight*2 );
		//}
		mCamera.update( elapsedTime );
	}

	void update( float elapsedTime ){
		CMouseWrapper::update();
		//Mouse m;
		Keyboard k;
		mController.update( elapsedTime, k.isOn(k.KEY_UP), k.isOn(k.KEY_DOWN), k.isOn(k.KEY_LEFT), k.isOn(k.KEY_RIGHT) );

		const btTransform &world = mController.getWorldTransform();

		D3DXMATRIX dxW = toDxMatrix(world);
		dxW._42 -= mHeight;
		mModel.update( dxW, elapsedTime );
	
	}

	void getView( btVector3 *pTarget, btVector3 *pCameraPos ){
		//look at the vehicle
		const btTransform &characterWorldTrans = mController.getWorldTransform();
		btVector3 up = characterWorldTrans.getBasis()[1];
		up.normalize ();

		//mmd��������W�Ƃ��č���Ă���̂ŋt�������Ă���̂Ŕ��Ε�����O�Ƃ���B
		btVector3 backward = characterWorldTrans.getBasis()[2];
		backward.setX(-backward.x());
		backward.normalize ();

		btVector3 forward = -backward;

		btVector3 cameraTargetPosition = characterWorldTrans.getOrigin()+10.f*forward;
		btVector3 cameraPosition = characterWorldTrans.getOrigin() + up * mUpLength + backward * mBackwardLength;	

		//use the convex sweep test to find a safe position for the camera (not blocked by static geometry)
		btSphereShape cameraSphere(0.2f);
		btTransform cameraFrom,cameraTo;
		cameraFrom.setIdentity();
		cameraFrom.setOrigin(characterWorldTrans.getOrigin());
		cameraTo.setIdentity();
		cameraTo.setOrigin(cameraPosition);
	
		btCollisionWorld::ClosestConvexResultCallback cb( characterWorldTrans.getOrigin(), cameraTo.getOrigin() );
		//�ÓI���̂̂ݓ����蔻�肠���mask
		cb.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
		
		BulletPhysics::Manager::instance()->getWorldPtr()->convexSweepTest( &cameraSphere, cameraFrom, cameraTo, cb );
		
		if (cb.hasHit())
		{
			btScalar minFraction  = cb.m_closestHitFraction;//btMax(btScalar(0.3),cb.m_closestHitFraction);
			cameraPosition.setInterpolate3(cameraFrom.getOrigin(),cameraTo.getOrigin(),minFraction);
			
			//�����A�����~�N���ɃJ�����������Ă���Ȃ�`�悵�Ȃ��Ƃ����ϐ����~��������
			//�������Ƃ�
		}

		if(pTarget){
			*pTarget=cameraTargetPosition;
		}
		if(pCameraPos){
			*pCameraPos=cameraPosition;
		}
	}

	//springCamera�g����
	void getView2( Vector3 *pTarget, Vector3 *pCameraPos ){
		//look at the vehicle
		const btTransform &characterWorldTrans = mController.getWorldTransform();
		btVector3 up = characterWorldTrans.getBasis()[1];
		up.normalize ();

		//mmd��������W�Ƃ��č���Ă���̂ŋt�������Ă���̂Ŕ��Ε�����O�Ƃ���B
		btVector3 backward = characterWorldTrans.getBasis()[2];
		backward.setX(-backward.x());
		backward.normalize ();

		btVector3 forward = -backward;

		btVector3 cameraTargetPosition = characterWorldTrans.getOrigin()+10.f*forward;
		btVector3 cameraPosition = characterWorldTrans.getOrigin() + up * mUpLength + backward * mBackwardLength;	

		//use the convex sweep test to find a safe position for the camera (not blocked by static geometry)
		btSphereShape cameraSphere(0.2f);
		btTransform cameraFrom,cameraTo;
		cameraFrom.setIdentity();
		cameraFrom.setOrigin(characterWorldTrans.getOrigin());
		cameraTo.setIdentity();
		cameraTo.setOrigin(cameraPosition);
	
		btCollisionWorld::ClosestConvexResultCallback cb( characterWorldTrans.getOrigin(), cameraTo.getOrigin() );
		//�ÓI���̂̂ݓ����蔻�肠���mask
		cb.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
		
		BulletPhysics::Manager::instance()->getWorldPtr()->convexSweepTest( &cameraSphere, cameraFrom, cameraTo, cb );
		
		if (cb.hasHit())
		{
			btScalar minFraction  = cb.m_closestHitFraction;//btMax(btScalar(0.3),cb.m_closestHitFraction);
			cameraPosition.setInterpolate3(cameraFrom.getOrigin(),cameraTo.getOrigin(),minFraction);
			
			//���삵�ɂ����B
			//mCamera.setPos( toVector3( cameraPosition ) );

			//�����A�����~�N���ɃJ�����������Ă���Ȃ�`�悵�Ȃ��Ƃ����ϐ����~��������

		}

		mCamera.setDesiredPos( toVector3(cameraPosition) );

		if(pTarget){
			*pTarget = toVector3( cameraTargetPosition );
		}
		if(pCameraPos){
			*pCameraPos = mCamera.getPos();
		}
	}
	/*
	void attack(){
		mModel.changeAnimation( CMMD_MikuModel::ANIM_DOUBLE_LARIAT );
		
		float radius;
		
		//���Ƙr�ɓ����蔻����o��
		//std::vector<btRigidBody*> &rbs=mModel.getBone( "rightFoot" )->getRigidBodes();
		
		foreach(btRigidBody *rb, rbs) {
			btCollisionShape *s= new btSphereShape( radius );
			btCollisionObject *co= new btCollisionObject();
			co->setCollisionShape( s );
			co->setCollisionFlags( btCollisionObject::CF_NO_CONTACT_RESPONSE );
			co->setWorldTransform( rb->getWorldTransform() );
			BulletPhysics::Manager::instance()->getWorldPtr()->addCollisionObject(co);

			rb->getUserPointer();
			rb->setUserPointer();
		}
	}*/

	//�J�����̑��Έʒu
	//�J�����̈ʒu���^�[�Q�b�g�̏���̂ǂ̂��炢�̒����ɂ�����
	//�v�Z���͈ȉ��ɂȂ��Ă���
	//btVector3 cameraPosition = cameraTargetPosition + up * mUpLength + backward * mBackwardLength;	
	void setUpLength( float upLength ){
		mUpLength=upLength;
	}
	
	//�J�����̑��Έʒu
	//�J�����̈ʒu���^�[�Q�b�g�̌���̂ǂ̂��炢�̒����ɂ�����
	//btVector3 cameraPosition = cameraTargetPosition + up * mUpLength + backward * mBackwardLength;	
	//�v�Z���͈ȉ��ɂȂ��Ă���
	void setBackwardLength( float backLength ){
		mBackwardLength=backLength;
	}

	void reset(){
		mController.warp( btVector3( 0.f, 100.f, 0.f ) );
	}
private:
	Old::CharacterControllerObject mController;
	CMMD_MikuModel mModel;
	//btVector3 mPos;
	float mVelocity;
	float mAngulerVelocity;

	float mHeight;
	float mWidth;

	float mUpLength;//�J�����̑��Ώ���ʒu
	float mBackwardLength;//�J�����̑��Ό���ʒu

	float mBoostPower;
	btVector3 mMousePosition;//���E�̒��ł̃}�E�X�̈ʒu

	SpringCameraWithoutYAxis mCamera;
};
#endif//__CHARACTER_CONTROLLER_PROTOTYPE_H__
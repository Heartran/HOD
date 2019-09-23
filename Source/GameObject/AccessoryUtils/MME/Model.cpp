#include"Model.h"
#include"XFile/Model.h"
#include<d3dx9.h>
#include"GameObject/Accessory.h"
#include"MME/Enums.h"
#include"MMD/Bone.h"
#include"BulletPhysics/BulletPhysicsHelper.h"
#include"SegaTypeHelper.h"
#include<boost/flyweight.hpp>
#include<boost/flyweight/key_value.hpp>



namespace GameObject {
namespace AccessoryUtils {
namespace MME {

Model::Model( const FlyWeightModel &model, float scale )
	:mModel(0),
	mAccessory(NULL),
	mScale(scale)
{
	mModel = new FlyWeightModel(model);
	//*mModel=model;
	setCanDraw(false);
}

Model::~Model(){
	delete mModel;
	mModel=0;
}

void Model::setScale( float s ){
	mScale = s;
}

float Model::getScale() const{
	return mScale;
}

bool Model::isFinished(){
	return const_cast<XFile::Model&>(mModel->get()).isFinished();
}
bool Model::isError()const{
	return mModel->get().isError();
}

GameLib::Graphics::VertexBuffer Model::getVertexBuffer(){
	return const_cast<XFile::Model&>(mModel->get()).getVertexBuffer();
}
GameLib::Graphics::IndexBuffer Model::getIndexBuffer(){
	return const_cast<XFile::Model&>(mModel->get()).getIndexBuffer();
}

void Model::fitTransformToBone(){
	//	�����X�V
	//		��
	//	pmdModel->update( elapsedTime );//bone�X�V
	//		��
	//	this->fitBone();//�`��p�����{�[���Ɉ�v������
	//		��
	//	���낢�돑��


	//�{�[���X�V��ŕ����X�V�O(�X�V�����͎̂��t���[��)�Ȃ̂�mRigidBody->getWorldTransform()�ł�1�t���[���x���
	//���ۂɕ\������̂�model(�~�N����)�Ɉ�v���ė~�����̂�kinematicMotionState���璼�ڂ���Ă���
	
	//�{�[���Ǐ]�������蔻�肠��@or ������rigidbody
	if( const btRigidBody *pRb = btRigidBody::upcast(mAccessory->getBtCollisionObject()) ){
		btTransform w;
		pRb->getMotionState()->getWorldTransform(w);
		btMatrix3x3 &r = w.getBasis();
		r[0][0] *=mScale; 
		r[1][1] *=mScale; 
		r[2][2] *=mScale;
		mTransform = toDxMatrix(w);
	}
	//�{�[���Ǐ]�����蔻��Ȃ�
	else if( const MMD::Bone *pBone = mAccessory->getBone() ){
		mTransform = pBone->getSkinningMatrix();
		D3DXVECTOR3 v = pBone->getPos();
		mTransform._41=v.x;
		mTransform._42=v.y;
		mTransform._43=v.z;
		mTransform._11 *= mScale;
		mTransform._22 *= mScale;
		mTransform._33 *= mScale;
	} else {
		assert(false);
	}
}

void Model::draw() const {
	::MME::Manager * m = ::MME::Manager::instance();
	::MME::MMDPass pass;
	::MME::UseTexture tex;
	::MME::UseSphereMap sphere;
	::MME::UseToon toon;
	::MME::UseSelfShadow selfShadow;

	m->getCurrentPass( &pass, &tex, &sphere, &toon, &selfShadow );

	//Xfile�ɂ�toon��sphereMap�͂Ȃ��̂ŁA����
	if(  toon || sphere  ){
		return;
	}
	assert(mAccessory);

	if( mCanDraw && mCanDrawMMDPass[pass] ) {
		mModel->get().drawByMME( mTransform );
	}
}

void Model::drawUsingGameLib()const{
	if( mCanDraw ) {
		if( const btRigidBody *pRb = btRigidBody::upcast(mAccessory->getBtCollisionObject()) ){
			btTransform w;
			pRb->getMotionState()->getWorldTransform(w);
			//w.getBasis().scaled( btVector3( mScale, mScale, mScale ) );
			btMatrix3x3 &r = w.getBasis();
			r[0][0] *=mScale; 
			r[1][1] *=mScale; 
			r[2][2] *=mScale; 
			mModel->get().draw( toMatrix34(w) );
		} else if( const MMD::Bone *pBone = mAccessory->getBone() ){
			D3DXMATRIX m = pBone->getSkinningMatrix();
			D3DXVECTOR3 v = pBone->getPos();
			m._11 *= mScale;
			m._22 *= mScale;
			m._33 *= mScale;
			m._41=v.x;
			m._42=v.y;
			m._43=v.z;
			mModel->get().draw( toMatrix34(m) );
		} else {
			assert(false);
		}
	}
}


const std::string & Model::getModelFileName()const{
	return mModel->get().getFileName();
}

float Model::getFaceValue(const std::string &faceName )const
{
	return 0.0;
}


const D3DXMATRIX & Model::getBoneMatrix( const std::string &boneName )const
{
	const static D3DXMATRIX matI( 
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
	return matI;
}

ModelSharedPtr Model::create( const FlyWeightModel &model, float scale ){
	return std::shared_ptr<Model>( new Model( model, scale ) );
}


}//end namespace MME

}//end namespace AccessoryUtils
}//end namespace GameObject

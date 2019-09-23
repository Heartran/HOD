#ifndef __IKIMPL_H__
#define __IKIMPL_H__
#include <vector>
#include "MMDMacro.h"
#include "WrapperIK.h"
#include "Pmd.h"
#include "Bone.h"
#include <assert.h>
#include <d3dx9.h>
#include <boost/lexical_cast.hpp>

namespace MMD {

class IK_Impl : public IIK_Impl
{
public:
	IK_Impl()
	:index_(0),
	topIndex_(0),
	effectBoneNum_(0),
	calcNum_(0),
	weight_(0.f),
	isEnable_(true)
	{};
	virtual ~IK_Impl(){};
	virtual const char* load( const char *pData );
	virtual void solve(Bone *pBones);
	virtual void setEnable(bool enabled);
	virtual bool isEnable();
	virtual unsigned short getIndex();
	virtual unsigned short getTopIndex(){
		return topIndex_;
	}
private:
	DISALLOW_COPY_AND_ASSIGN(IK_Impl);
private:
	unsigned short index_;//ik�̑Ή�����t���[����index
	unsigned short topIndex_;//��[��index
	unsigned char effectBoneNum_;
	unsigned short calcNum_;
	float weight_;
	std::vector<unsigned short> effectBoneIndeces_;
	bool isEnable_;
};


const char* IK_Impl::load( const char *pData ) {
	const PMD_IK *pIK = reinterpret_cast<const PMD_IK *>(pData);
	index_=pIK->index;
	topIndex_=pIK->target;
	effectBoneNum_=pIK->effectBoneNum;
	calcNum_=pIK->calcNum;
	weight_=pIK->weight;
	pData +=  sizeof(unsigned short)*3 + sizeof(unsigned char) + sizeof(float);

	effectBoneIndeces_.reserve(effectBoneNum_);
	for( unsigned char i=0; i<effectBoneNum_; ++i) {
		unsigned short effectBoneIndex= *(reinterpret_cast<const unsigned short *>(pData));
		effectBoneIndeces_.push_back(effectBoneIndex);
		pData += sizeof( unsigned short);
	}
	return pData;
}

void IK_Impl::solve(Bone *pBones) {
	
	if( !isEnable_ ){
		return;
	}
	//IK(�ڕW�ʒu)�֐�
	Bone* ikBone = &pBones[index_];
	//�ڕW�ʒu(IK)
	D3DXVECTOR4 posIk;
	//���[���h���W��ɕϊ�
	D3DXVec3Transform(&posIk,&ikBone->initPos_,&ikBone->skinningMatrix_);
				
	//��[�̊֐߁@
	Bone* topBone = &pBones[topIndex_];
	//��[�̈ʒu
	D3DXVECTOR4 posTop;
	D3DXVec3Transform(&posTop,&topBone->initPos_,&topBone->skinningMatrix_);
		
	float firstLen;
	firstLen=D3DXVec4LengthSq(&(posTop-posIk));

	//���łɏ\���ɋ߂��ꍇ
	if( D3DXVec4LengthSq(&(posTop-posIk)) < 0.001f ) { 
		return;
	}

	for(int i=0; i<calcNum_; ++i)
	{
		for(int j=0; j<effectBoneNum_; ++j)
		{
			//��֐�
			Bone* basicBone = &pBones[effectBoneIndeces_[j]];
			//��֐߂̈ʒu
			D3DXVECTOR4 posBasic;

			//���[���h���W��ɕϊ�		
			D3DXVec3Transform(&posTop,&topBone->initPos_,&topBone->skinningMatrix_);
			D3DXVec3Transform(&posBasic,&basicBone->initPos_,&basicBone->skinningMatrix_);

			float len;
			len=D3DXVec4LengthSq(&(posTop-posIk));
				
			//���łɏ\���ɋ߂��ꍇ
			if( len<0.001f ) { 
				break;
			}

			//��֐߁�IK(�ڕW�ʒu)�̃x�N�g��
			D3DXVECTOR3 vBasic2Ik = static_cast<D3DXVECTOR3>(posIk) - static_cast<D3DXVECTOR3>(posBasic);
			//��֐߁���[�̃x�N�g��
			D3DXVECTOR3 vBasic2Top  = static_cast<D3DXVECTOR3>(posTop) - static_cast<D3DXVECTOR3>(posBasic);
				
			//��]�p
			D3DXVec3Normalize(&vBasic2Top,&vBasic2Top);
			D3DXVec3Normalize(&vBasic2Ik,&vBasic2Ik);
			float theata = acosf( D3DXVec3Dot(&vBasic2Top,&vBasic2Ik) );
			//assert(theata>=0);
			if( theata<0 ){
				break;
			}

			if( theata>weight_*(j+1)*2) {
				theata=weight_*(j+1)*2;
			}


			//��]��
			D3DXVECTOR3 rotationAxis;
			D3DXVec3Cross(&rotationAxis, &vBasic2Top, &vBasic2Ik);
			D3DXVec3Normalize(&rotationAxis, &rotationAxis);
				
			//������LengthSq�K���v���X�����assert()�Œ��ׂĂ������
			//�O�ς�2�̓��̓x�N�g���������������ꍇ�΂����Ȃ�̂ł���
			if( D3DXVec3LengthSq(&rotationAxis) > 0.f && abs(theata) > 1.0e-5f)
			{
				//���[���h���W���烍�[�J�����W�ɕϊ����邽�ߋt�s����v�Z����
				D3DXMATRIX matInv;
				float dat;
				D3DXMatrixInverse(&matInv, &dat, &basicBone->skinningMatrix_);
				D3DXVec3TransformNormal(&rotationAxis, &rotationAxis, &matInv);
				D3DXVec3Normalize( &rotationAxis, &rotationAxis );
				
				//������
				if( ( (strncmp(basicBone->name_,"�E�Ђ�",6)==0) | (strncmp(basicBone->name_,"���Ђ�",6)==0) ) )
				{
					if(rotationAxis.x<0){
						rotationAxis.x=-1.f;
					}
					else{
						rotationAxis.x=1.f;
					}
					rotationAxis.y=0.f;
					rotationAxis.z=0.f;
				}

				
				D3DXQUATERNION quatR;
				D3DXQuaternionRotationAxis(&quatR, &rotationAxis, theata);
				//D3DXQuaternionNormalize(&quatR, &quatR);

				D3DXQUATERNION quatTemp = basicBone->qRotation_;
				//basicBone->qRotation_ *= quatR;
				D3DXQuaternionMultiply(&basicBone->qRotation_, &quatR, &basicBone->qRotation_ );
				D3DXQuaternionNormalize(&basicBone->qRotation_, &basicBone->qRotation_);
				
				if( ( (strncmp(basicBone->name_,"�E�Ђ�",6)==0) | (strncmp(basicBone->name_,"���Ђ�",6)==0) ) )
				{
					//�Ђ��͐��̕����ɂ����Ȃ��Ȃ��B
					//�Ђ���X����]�̂�
					//X�������̃}�C�i�X�����Ă�ꍇ��(���肦�Ȃ��ق��ɋȂ����Ă���ꍇ�́j�␳
					//if( atan2f(basicBone->skinningMatrix_._32, basicBone->skinningMatrix_._33)<0.0f ){
					if( basicBone->qRotation_.x > 0 ) {
						basicBone->qRotation_.x = -basicBone->qRotation_.x; 
					}
				}
				//�t���[�����X�V
				basicBone->updateSkinningMatrix( basicBone->pParent_->skinningMatrix_ );
			}
		}
	}
}
/*
void IK_Impl::solve(std::vector<Bone*> *pBones) {
	
	if( !isEnable_ ){
		return;
	}
	//IK(�ڕW�ʒu)�֐�
	Bone* ikBone = (*pBones)[index_];
	//�ڕW�ʒu(IK)
	D3DXVECTOR4 posIk;
	//���[���h���W��ɕϊ�
	D3DXVec3Transform(&posIk,&ikBone->initPos_,&ikBone->skinningMatrix_);
				
	//��[�̊֐߁@
	Bone* topBone = (*pBones)[topIndex_];
	//��[�̈ʒu
	D3DXVECTOR4 posTop;
	D3DXVec3Transform(&posTop,&topBone->initPos_,&topBone->skinningMatrix_);
		
	float firstLen;
	firstLen=D3DXVec4LengthSq(&(posTop-posIk));

	//���łɏ\���ɋ߂��ꍇ
	if( D3DXVec4LengthSq(&(posTop-posIk)) < 0.001f ) { 
		return;
	}

	for(int i=0; i<calcNum_; ++i)
	{
		for(int j=0; j<effectBoneNum_; ++j)
		{
			//��֐�
			Bone* basicBone = (*pBones)[effectBoneIndeces_[j]];
			//��֐߂̈ʒu
			D3DXVECTOR4 posBasic;

			//���[���h���W��ɕϊ�		
			D3DXVec3Transform(&posTop,&topBone->initPos_,&topBone->skinningMatrix_);
			D3DXVec3Transform(&posBasic,&basicBone->initPos_,&basicBone->skinningMatrix_);

			float len;
			len=D3DXVec4LengthSq(&(posTop-posIk));
				
			//���łɏ\���ɋ߂��ꍇ
			if( len<0.001f ) { 
				break;
			}

			//��֐߁�IK(�ڕW�ʒu)�̃x�N�g��
			D3DXVECTOR3 vBasic2Ik = static_cast<D3DXVECTOR3>(posIk) - static_cast<D3DXVECTOR3>(posBasic);
			//��֐߁���[�̃x�N�g��
			D3DXVECTOR3 vBasic2Top  = static_cast<D3DXVECTOR3>(posTop) - static_cast<D3DXVECTOR3>(posBasic);
				
			//��]�p
			D3DXVec3Normalize(&vBasic2Top,&vBasic2Top);
			D3DXVec3Normalize(&vBasic2Ik,&vBasic2Ik);
			float theata = acosf( D3DXVec3Dot(&vBasic2Top,&vBasic2Ik) );	
			//��]��
			D3DXVECTOR3 rotationAxis;
			D3DXVec3Cross(&rotationAxis, &vBasic2Top, &vBasic2Ik);
			D3DXVec3Normalize(&rotationAxis, &rotationAxis);
				
			//������LengthSq�K���v���X�����assert()�Œ��ׂĂ������
			//�O�ς�2�̓��̓x�N�g���������������ꍇ�΂����Ȃ�̂ł���
			if( D3DXVec3LengthSq(&rotationAxis) > 0.f && abs(theata) > 1.0e-5f)
			{
				//���[���h���W���烍�[�J�����W�ɕϊ����邽�ߋt�s����v�Z����
				D3DXMATRIX matInv;
				float dat;
				D3DXMatrixInverse(&matInv, &dat, &basicBone->skinningMatrix_);
				D3DXVec3TransformNormal(&rotationAxis, &rotationAxis, &matInv);
				D3DXVec3Normalize( &rotationAxis, &rotationAxis );
					
				D3DXQUATERNION quatR;
				D3DXQuaternionRotationAxis(&quatR, &rotationAxis, theata*weight_);
				//D3DXQuaternionNormalize(&quatR, &quatR);

				D3DXQUATERNION quatTemp = basicBone->qRotation_;
				//basicBone->qRotation_ *= quatR;
				D3DXQuaternionMultiply(&basicBone->qRotation_, &basicBone->qRotation_, &quatR);
				D3DXQuaternionNormalize(&basicBone->qRotation_, &basicBone->qRotation_);
				//�t���[�����X�V
				basicBone->updateSkinningMatrix( basicBone->pParent_->skinningMatrix_ );
					
				//�����Ȃ��Ă�����
				D3DXVec3Transform(&posTop,&topBone->initPos_,&topBone->skinningMatrix_);
				float lenAfter = D3DXVec4LengthSq(&(posTop-posIk));
				if( lenAfter>len ) {
					basicBone->qRotation_=quatTemp;
					D3DXQuaternionRotationAxis(&quatR, &rotationAxis, -theata*weight_);
					//basicBone->qRotation_ *= quatR;
					D3DXQuaternionMultiply(&basicBone->qRotation_, &basicBone->qRotation_, &quatR);
					D3DXQuaternionNormalize(&basicBone->qRotation_, &basicBone->qRotation_);

					//�t���[�����X�V
					basicBone->updateSkinningMatrix( basicBone->pParent_->skinningMatrix_ );
				}
			}
		}
	}
}
*/

bool IK_Impl::isEnable() {
	return isEnable_;
}

void IK_Impl::setEnable( bool enabled) {
	isEnable_ = enabled;
}

unsigned short IK_Impl::getIndex() {
	return index_;
}
/*
class FootIK_Impl : public IIK_Impl
{
public:
	FootIK_Impl()
	:index_(0),
	topIndex_(0),
	rootIndex_(0),
	kneeIndex_(0),
	lengthToKnee_(0.f),
	lengthFromKneeToTop_(0.f),
	isEnable_(true)
	{};
	virtual ~FootIK_Impl(){};
	virtual const char* load( const char *pData );
	virtual void solve(std::vector<Bone*> *pBones);
	void initLength(const std::vector<Bone*> &bones);
	virtual void setEnable(bool enabled);
	virtual bool isEnable();
	virtual unsigned short getIndex();
private:
	DISALLOW_COPY_AND_ASSIGN(FootIK_Impl);
private:
	unsigned short index_;//ik�̑Ή�����t���[����index
	unsigned short topIndex_;//��[��index
	unsigned short rootIndex_;//���̕t����
	unsigned short kneeIndex_;
	float lengthToKnee_;
	float lengthFromKneeToTop_;
	bool isEnable_;
};


const char* FootIK_Impl::load( const char *pData ) {
	const PMD_IK *pIK = reinterpret_cast<const PMD_IK *>(pData);
	index_=pIK->index;
	topIndex_=pIK->target;
	//effectBoneNum_=pIK->effectBoneNum;
	//calcNum_=pIK->calcNum;
	//weight_=pIK->weight;
	pData +=  sizeof(unsigned short)*3 + sizeof(unsigned char) + sizeof(float);

	//�A����2�̏ꍇ�����l�����ĂȂ�
	assert(pIK->effectBoneNum==2);
	kneeIndex_= *(reinterpret_cast<const unsigned short *>(pData));
	pData += sizeof( unsigned short);
	rootIndex_= *(reinterpret_cast<const unsigned short *>(pData));
	pData += sizeof( unsigned short);
	return pData;
}

void FootIK_Impl::initLength(const std::vector<Bone*> &bones) {
	D3DXVECTOR3 top=bones[topIndex_]->initPos_;
	D3DXVECTOR3 root=bones[rootIndex_]->initPos_;
	D3DXVECTOR3 knee=bones[kneeIndex_]->initPos_;
	lengthToKnee_ = D3DXVec3Length(&(knee-root));
	lengthFromKneeToTop_ = D3DXVec3Length(&(top-knee));
}

void FootIK_Impl::solve(std::vector<Bone*> *pBones) {
	if( !isEnable_ ){
		return;
	}

	//IK(�ڕW�ʒu)�֐�
	Bone* ikBone = (*pBones)[index_];
	//�ڕW�ʒu(IK)
	D3DXVECTOR4 posIk;
	//���[���h���W��ɕϊ�
	D3DXVec3Transform(&posIk, &ikBone->initPos_, &ikBone->skinningMatrix_);
				
	//��[�̊֐߁@
	Bone* topBone = (*pBones)[topIndex_];
	//��[�̈ʒu
	D3DXVECTOR4 posTop;
	D3DXVec3Transform(&posTop, &topBone->initPos_, &topBone->skinningMatrix_);
		
	float lengthSqIK2Top;
	lengthSqIK2Top=D3DXVec4LengthSq(&(posTop-posIk));

	//���łɏ\���ɋ߂��ꍇ
	if( D3DXVec4LengthSq(&(posTop-posIk)) < 0.001f ) { 
		return;
	}
	D3DXVECTOR4 posRoot;
	Bone* rootBone = (*pBones)[rootIndex_];
	D3DXVec3Transform(&posRoot, &rootBone->initPos_, &rootBone->skinningMatrix_);

	D3DXVECTOR4 posKnee;
	Bone* kneeBone = (*pBones)[kneeIndex_];
	D3DXVec3Transform(&posKnee, &kneeBone->initPos_, &kneeBone->skinningMatrix_);

	float lengthToIK=D3DXVec4Length( &(posIk-posRoot) );
	//��֐߁�IK(�ڕW�ʒu)�̃x�N�g��
	D3DXVECTOR3 vRoot2Ik = static_cast<D3DXVECTOR3>(posIk) - static_cast<D3DXVECTOR3>(posRoot);
	//��֐߁��q�U�̃x�N�g��
	D3DXVECTOR3 vRoot2Knee  = static_cast<D3DXVECTOR3>(posKnee) - static_cast<D3DXVECTOR3>(posRoot);
				
	//��]�p
	D3DXVec3Normalize( &vRoot2Knee, &vRoot2Knee);
	D3DXVec3Normalize( &vRoot2Ik, &vRoot2Ik);
	float theta0 = acosf( D3DXVec3Dot( &vRoot2Knee, &vRoot2Ik ) );	
	//��]��
	D3DXVECTOR3 rotationAxis0;
	D3DXVec3Cross( &rotationAxis0, &vRoot2Knee, &vRoot2Ik);
	D3DXVec3Normalize( &rotationAxis0, &rotationAxis0 );

	//���[���h���W���烍�[�J�����W�ɕϊ����邽�ߋt�s����v�Z����
	D3DXMATRIX matInv;
	float dat;
	D3DXMatrixInverse(&matInv, &dat, &rootBone->skinningMatrix_);
	D3DXVec3TransformNormal(&rotationAxis0, &rotationAxis0, &matInv);
	D3DXVec3Normalize( &rotationAxis0, &rotationAxis0 );

	D3DXQUATERNION q0;
	D3DXQuaternionRotationAxis(&q0, &rotationAxis0, theta0 );

	//posIK���͈͓��ɂ���ꍇ
	if( lengthToKnee_+lengthFromKneeToTop_ >= lengthToIK ) {
		//������̉�]�����
		float theta1 = acos( (lengthToKnee_*lengthToKnee_-lengthFromKneeToTop_*lengthFromKneeToTop_+lengthToIK*lengthToIK) / 
			(2.f*lengthToKnee_*lengthToIK) );

		//��]�������@���[�J�����W�Ŏw��
		D3DXVECTOR3 rotationAxis1( 1.f, 0.f, 0.f );
		//D3DXVec3TransformNormal(&rotationAxis1, &(rotationAxis1), &rootBone->skinningMatrix_);

		D3DXQUATERNION q1;
		D3DXQuaternionRotationAxis(&q1, &rotationAxis1, theta1 );
		D3DXQUATERNION qR=q1*q0;
		D3DXQuaternionNormalize( &qR, &qR );
		rootBone->qRotation_=qR;
		rootBone->updateSkinningMatrix( rootBone->pParent_->skinningMatrix_ );

		//�Ђ��̊֐߂̉�]�����
		D3DXVec3Transform(&posKnee, &kneeBone->initPos_, &kneeBone->skinningMatrix_);
		D3DXVec3Transform(&posTop, &topBone->initPos_, &topBone->skinningMatrix_);

		//�q�U�֐߁�IK(�ڕW�ʒu)�̃x�N�g��
		D3DXVECTOR3 vKnee2Ik = static_cast<D3DXVECTOR3>(posIk) - static_cast<D3DXVECTOR3>(posKnee);
		//�q�U�֐߁���[�̃x�N�g��
		D3DXVECTOR3 vKnee2Top  = static_cast<D3DXVECTOR3>(posTop) - static_cast<D3DXVECTOR3>(posKnee);
		
		D3DXVec3Normalize( &vKnee2Ik, &vKnee2Ik );
		D3DXVec3Normalize( &vKnee2Top, &vKnee2Top );

		//��]��
		D3DXVECTOR3 kneeRotationAxis;
		D3DXVec3Cross( &kneeRotationAxis, &vKnee2Top, &vKnee2Ik);
		D3DXVec3Normalize( &kneeRotationAxis, &kneeRotationAxis );

		//��]�p
		float cos = D3DXVec3Dot( &vKnee2Top, &vKnee2Ik );
		float thetaKnee = acosf( cos );		
		
		//���[���h���W���烍�[�J�����W�ɕϊ����邽�ߋt�s����v�Z����
		D3DXMATRIX matInv;
		float dat;
		D3DXMatrixInverse(&matInv, &dat, &kneeBone->skinningMatrix_);
		D3DXVec3TransformNormal(&kneeRotationAxis, &kneeRotationAxis, &matInv);

		D3DXQUATERNION qRKnee;
		D3DXQuaternionRotationAxis(&qRKnee, &kneeRotationAxis, thetaKnee );
		kneeBone->qRotation_=qRKnee;
		kneeBone->updateSkinningMatrix( kneeBone->pParent_->skinningMatrix_ );
	} else {
		rootBone->qRotation_=q0;
		rootBone->updateSkinningMatrix( rootBone->pParent_->skinningMatrix_ );
	}
}


bool FootIK_Impl::isEnable() {
	return isEnable_;
}

void FootIK_Impl::setEnable( bool enabled) {
	isEnable_ = enabled;
}

unsigned short FootIK_Impl::getIndex() {
	return index_;
}

class AnkleIK_Impl : public IIK_Impl
{
public:
	AnkleIK_Impl()
	:index_(0),
	topIndex_(0),
	rootIndex_(0),
	isEnable_(true)
	{};
	virtual ~AnkleIK_Impl(){};
	virtual const char* load( const char *pData );
	virtual void solve(std::vector<Bone*> *pBones);
	virtual void setEnable(bool enabled);
	virtual bool isEnable();
	virtual unsigned short getIndex();
private:
	DISALLOW_COPY_AND_ASSIGN(AnkleIK_Impl);
private:
	unsigned short index_;//ik�̑Ή�����t���[����index
	unsigned short topIndex_;//�ܐ��index
	unsigned short rootIndex_;//����
	unsigned short kneeIndex_;
	bool isEnable_;
};

const char* AnkleIK_Impl::load( const char *pData ) {
	const PMD_IK *pIK = reinterpret_cast<const PMD_IK *>(pData);
	index_=pIK->index;
	topIndex_=pIK->target;
	//effectBoneNum_=pIK->effectBoneNum;
	//calcNum_=pIK->calcNum;
	//weight_=pIK->weight;
	pData +=  sizeof(unsigned short)*3 + sizeof(unsigned char) + sizeof(float);

	//�A����1�̏ꍇ�����l�����ĂȂ�
	assert(pIK->effectBoneNum==1);
	rootIndex_= *(reinterpret_cast<const unsigned short *>(pData));
	pData += sizeof( unsigned short);
	return pData;
}

void AnkleIK_Impl::solve( std::vector<Bone*> *pBones ) {
	if( !isEnable_ ) {
		return;
	}

	//IK(�ڕW�ʒu)�֐�
	Bone* ikBone = (*pBones)[index_];
	//�ڕW�ʒu(IK)
	D3DXVECTOR4 posIk;
	//���[���h���W��ɕϊ�
	D3DXVec3Transform(&posIk, &ikBone->initPos_, &ikBone->skinningMatrix_);
				
	//��[�̊֐߁@
	Bone* topBone = (*pBones)[topIndex_];
	//��[�̈ʒu
	D3DXVECTOR4 posTop;
	D3DXVec3Transform(&posTop, &topBone->initPos_, &topBone->skinningMatrix_);
		
	float lengthSqIK2Top;
	lengthSqIK2Top=D3DXVec4LengthSq(&(posTop-posIk));

	//���łɏ\���ɋ߂��ꍇ
	if( D3DXVec4LengthSq(&(posTop-posIk)) < 0.001f ) { 
		return;
	}
	D3DXVECTOR4 posRoot;
	Bone* rootBone = (*pBones)[rootIndex_];
	D3DXVec3Transform(&posRoot, &rootBone->initPos_, &rootBone->skinningMatrix_);

	//����IK(�ڕW�ʒu)�̃x�N�g��
	D3DXVECTOR3 vRoot2Ik = static_cast<D3DXVECTOR3>(posIk) - static_cast<D3DXVECTOR3>(posRoot);
	//���񁨂ܐ�̃x�N�g��
	D3DXVECTOR3 vRoot2Top  = static_cast<D3DXVECTOR3>(posTop) - static_cast<D3DXVECTOR3>(posRoot);
				
	//��]�p
	D3DXVec3Normalize( &vRoot2Top, &vRoot2Top);
	D3DXVec3Normalize( &vRoot2Ik, &vRoot2Ik);
	float theta = acosf( D3DXVec3Dot( &vRoot2Top, &vRoot2Ik ) );	
	//��]��
	D3DXVECTOR3 rotationAxis;
	D3DXVec3Cross( &rotationAxis, &vRoot2Top, &vRoot2Ik);
	D3DXVec3Normalize( &rotationAxis, &rotationAxis );

	//���[���h���W���烍�[�J�����W�ɕϊ����邽�ߋt�s����v�Z����
	D3DXMATRIX matInv;
	float dat;
	D3DXMatrixInverse(&matInv, &dat, &rootBone->skinningMatrix_);
	D3DXVec3TransformNormal(&rotationAxis, &rotationAxis, &matInv);
	D3DXVec3Normalize( &rotationAxis, &rotationAxis );

	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis(&q, &rotationAxis, theta );

	rootBone->qRotation_=q;
	rootBone->updateSkinningMatrix( rootBone->pParent_->skinningMatrix_ );

}

bool AnkleIK_Impl::isEnable() {
	return isEnable_;
}

void AnkleIK_Impl::setEnable( bool enabled) {
	isEnable_ = enabled;
}

unsigned short AnkleIK_Impl::getIndex() {
	return index_;
}
*/
}// namespace MMD
#endif //__IKIMPL_H__
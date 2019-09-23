#include "Animation.h"
#include "Vmd.h"
#include "FrameKey.h"
#include "FaceKey.h"
#include "Bone.h"
#include "Face.h"
#include "ContainerHelper.h"
#include <algorithm>
#include"MMDHelperFunc.h"
#include<boost/lexical_cast.hpp>
#include<string>
#include<GameLib/FileIO/InFile.h>

static const D3DXVECTOR3& getRefPos( const std::pair<D3DXVECTOR3, D3DXQUATERNION> &inPair ) {
	return inPair.first;
}

static const D3DXQUATERNION& getRefQuat( const std::pair<D3DXVECTOR3, D3DXQUATERNION> &inPair ) {
	return inPair.second;
}


CAnimation::CAnimation()
	:name_(NULL),maxFrame_(0.f)
	{};
CAnimation::~CAnimation()
{
	SAFE_DELETE_ARRAY(name_);
	frameKeys_ | destroy();
	faceKeys_ | destroy();
}

bool CAnimation::load( const CVmd &vmd, CBone *pBones, const unsigned short &boneSize  ){
	bool hasPrinted=false;
	
	frameKeys_.reserve(boneSize);
	for( unsigned int i=0; i<vmd.motionLength_ ; ++i)
	{	
		std::vector<CFrameKey*>::iterator it;
		it = frameKeys_ | find_name(vmd.pMotions_[i].BoneName);
		//it = std::find_if( frameKeys_.begin(), frameKeys_.end(), name_is(vmd.pMotions_[i].BoneName) );

		//�������łɃt���[����������Ȃ炻���ɒǉ�
		if( it!=frameKeys_.end() ) {
			CFrameKey* key= *it;
			D3DXQUATERNION q(vmd.pMotions_[i].Rotate[0],vmd.pMotions_[i].Rotate[1],vmd.pMotions_[i].Rotate[2],vmd.pMotions_[i].w);
			D3DXVECTOR3 pos(vmd.pMotions_[i].Location[0],vmd.pMotions_[i].Location[1],vmd.pMotions_[i].Location[2]);
			float frame = static_cast<float>(vmd.pMotions_[i].flameNo);
			key->addFrameKey( frame, pos, q );
			if(maxFrame_ < frame ) maxFrame_=frame;
		}
		//�V�K�쐬
		else {
			//�����A�j���[�V�������g���܂킵�����Ȃ�A�����ŏ������K�v�B�Ƃ������K�v�Ȃ�
			//�����Y���{�[���Ȃ��Ȃ�Ƃ肠��������Ă����āA���ƂŖ�������
			//�~�N�ɂ̓l�N�^�C�����邪�A�J�C�g�ɂ͂Ȃ�
			CBone*  bone = findNameInArray( pBones, boneSize, vmd.pMotions_[i].BoneName );
			//���O��������Ȃ�������
			if( bone==NULL ){
				if( !hasPrinted ){
					//assert(false && "���[�V�����ƃ��f������v���܂���B���Ԃ�A���̃��[�V�����͈Ⴄ���f���ō�������̂ł��B");
					OutputDebugStringA("���[�V�����ƃ��f������v���܂���B\n");
					OutputDebugStringA("vmd�w�b�_:");
					OutputDebugStringA( vmd.vmdHeader_.vmdHeader );
					OutputDebugStringA("\n");
					OutputDebugStringA("�쐬���f����:");
					OutputDebugStringA( vmd.vmdHeader_.vmdModelName );
					OutputDebugStringA("\n");
					hasPrinted=true;
				}
				continue;
			}

			//CBone* bone=*(*pBones | find_name(vmd.pMotions_[i].BoneName));
			//CBone* bone=*find_if(pBones->begin(), pBones->end(), name_is(vmd.pMotions_[i].BoneName));
			CFrameKey* key=new CFrameKey(vmd.pMotions_[i].BoneName, bone->getIndex() );
			frameKeys_.push_back(key);

			D3DXQUATERNION q(vmd.pMotions_[i].Rotate[0],vmd.pMotions_[i].Rotate[1],vmd.pMotions_[i].Rotate[2],vmd.pMotions_[i].w);
			D3DXVECTOR3 pos(vmd.pMotions_[i].Location[0],vmd.pMotions_[i].Location[1],vmd.pMotions_[i].Location[2]);
			float frame = static_cast<float>(vmd.pMotions_[i].flameNo);
			key->addFrameKey( frame, pos, q );
			if(maxFrame_ < frame ) maxFrame_=frame;
		}

		//�⊮�Ȑ�(�x�W�F�Ȑ�)�̏�����
		interpolationX_.init(	static_cast<float>(vmd.pMotions_[i].Interpolation1[0]),
								static_cast<float>(vmd.pMotions_[i].Interpolation1[4]),
								static_cast<float>(vmd.pMotions_[i].Interpolation1[8]),
								static_cast<float>(vmd.pMotions_[i].Interpolation1[12]) );

		interpolationY_.init(	static_cast<float>(vmd.pMotions_[i].Interpolation2[0]),
								static_cast<float>(vmd.pMotions_[i].Interpolation2[4]),
								static_cast<float>(vmd.pMotions_[i].Interpolation2[8]),
								static_cast<float>(vmd.pMotions_[i].Interpolation2[12]) );

		interpolationZ_.init(	static_cast<float>(vmd.pMotions_[i].Interpolation3[0]),
								static_cast<float>(vmd.pMotions_[i].Interpolation3[4]),
								static_cast<float>(vmd.pMotions_[i].Interpolation3[8]),
								static_cast<float>(vmd.pMotions_[i].Interpolation3[12]) );

		interpolationQ_.init(	static_cast<float>(vmd.pMotions_[i].Interpolation4[0]),
								static_cast<float>(vmd.pMotions_[i].Interpolation4[4]),
								static_cast<float>(vmd.pMotions_[i].Interpolation4[8]),
								static_cast<float>(vmd.pMotions_[i].Interpolation4[12]) );
		
	}
	//FaceKey������
	for( unsigned int i=0; i<vmd.faceLength_ ; ++i)
	{
		std::vector<CFaceKey*>::iterator it;
		it = faceKeys_ | find_name(vmd.pFaces_[i].FaceName);
		//it = find_if( faceKeys_.begin(), faceKeys_.end(), name_is(vmd.pFaces_[i].FaceName) );

		//�������łɃt���[����������Ȃ炻���ɒǉ�
		if( it!=faceKeys_.end() )
		{
			CFaceKey* key = *it;
			float t=static_cast<float>(vmd.pFaces_[i].flameNo);
			key->insert( t, vmd.pFaces_[i].weight );
			if(maxFrame_ < t ) maxFrame_=t;
		}
		//�V�K�쐬
		else
		{
			CFaceKey* key = new CFaceKey(vmd.pFaces_[i].FaceName);
			faceKeys_.push_back( key );
			float t=static_cast<float>(vmd.pFaces_[i].flameNo);
			key->insert( t, vmd.pFaces_[i].weight );
			if(maxFrame_ < t ) maxFrame_=t;
		}
	}


	return true;
}

bool CAnimation::load(const char *pFileName, CBone *pBones, const unsigned short &boneSize ) {
	GameLib::FileIO::InFile inFile = GameLib::FileIO::InFile::create( pFileName );
	
	if(inFile.isError()){
		return false;
	}
	while(!inFile.isFinished()){}
	
	CVmd vmd( inFile.data() );

	//�����Ńt�@�C���𐳏�Ƀ��[�h�ł������m���߂��ق�������
	name_=new char[strlen(pFileName)+1];
	strcpy(name_, pFileName);

	return load( vmd, pBones, boneSize );

}



bool CAnimation::load( const GameLib::FileIO::InFile &inFile, CBone *bones, const unsigned short &boneSize ){
	if(inFile.isError()){
		return false;
	}
	while(!inFile.isFinished()){}
	
	CVmd vmd( inFile.data() );

	
	return load( vmd, bones, boneSize );

}


//�A�j���[�V�������E�F�C�g�ō����Ďg�����Ƃ�z�肵�Ă�
void CAnimation::resetRelativeBones( CBone *pBones, const float &frame, const float &weight ) {
	if (weight==0) return;
	
	//������
	for (unsigned int i=0, end=frameKeys_.size(); i<end; ++i) {
		CFrameKey* key=frameKeys_[i];
		pBones[key->boneIndex_].reset();
	}
}
//�A�j���[�V�������E�F�C�g�ō����Ďg�����Ƃ�z�肵�Ă�
//���O��ς��܂���
//upDateBoneUsingWeight
//updataBoneMixingAnimation
//updateBoneCurrentAnimation
//blend����Ȃ�A������
//( CBone *pBones, std::vector< std::pair<animationIndex, const float &frame, const float &weight> > )�ɂ����3�ȏ�̃u�����h�\��
void CAnimation::updateBones( CBone *pBones, const float &frame, const float &weight ) {
	if (weight==0) return;
	
	for (unsigned int i=0,max=frameKeys_.size(); i<max; ++i) {
		CFrameKey* key=frameKeys_[i];
		CBone* pBone=&pBones[key->boneIndex_];
		if (key->map_.empty()) continue;
			
		// �L�[�t���[���̕��
		if ( frame <= key->map_.begin()->first ) {
			if(weight==1.f){
				//pair�g�������Č������
				//const D3DXVECTOR3 &pos= getRefPos(key->map_.begin()->second);
				const D3DXVECTOR3 &pos= key->map_.begin()->second.first;
				const D3DXQUATERNION &q=key->map_.begin()->second.second;
				pBone->qRotation_*= q;
				pBone->pos_+= pos;
			} else {
				/*
				float theta;
				const D3DXVECTOR3 &pos= key->map_.begin()->second.first;
				const D3DXQUATERNION &q1=key->map_.begin()->second.second;
				D3DXVECTOR3 axis;
				D3DXQUATERNION q2;
				D3DXQuaternionToAxisAngle(&q1, &axis, &theta);
				theta *= weight;
				D3DXQuaternionRotationAxis( &q2, &axis, theta );
				pBone->qRotation_*=q2;
				pBone->pos_+= pos*weight;
				*/
				const D3DXVECTOR3 &pos= key->map_.begin()->second.first;
				pBone->pos_+= pos*weight;
				
				const D3DXQUATERNION &q=key->map_.begin()->second.second;
				//�A�j���[�V�������u�����h����ꍇ
				//���̎������Ə�肭�����Ȃ��P�[�X������B
				//3�ȏ�̃u�����h�͏�肭�����Ȃ��B
				
				//1��ڂ�blend�͂���
				if( pBone->qRotation_==D3DXQUATERNION(0.f,0.f,0.f,1.f) ){
					pBone->qRotation_=q*weight;
				}
				//2��ڂ̃u�����h�͂���
				else {
					D3DXQUATERNION qOut;
					D3DXQUATERNION qb=pBone->qRotation_/(1.f-weight);
					float qDot = D3DXQuaternionDot( &q, &qb );
					
					if(qDot<0){
						qOut= q*weight - pBone->qRotation_;
					} else {
						qOut= q*weight + pBone->qRotation_;
					}
					D3DXQuaternionNormalize( &pBone->qRotation_, &qOut );
				}
			}
		} else if (frame >= key->map_.rbegin()->first ) {
			if(weight==1.f){
				const D3DXVECTOR3 &pos= key->map_.rbegin()->second.first;
				const D3DXQUATERNION &q=key->map_.rbegin()->second.second;
				
				pBone->qRotation_*= q;
				pBone->pos_+= pos;
			} else {
				/*
				const D3DXVECTOR3 &pos= key->map_.rbegin()->second.first;
				const D3DXQUATERNION &q1=key->map_.rbegin()->second.second;
				
				float theta;
				D3DXVECTOR3 axis;
				D3DXQUATERNION q2;
				D3DXQuaternionToAxisAngle(&q1, &axis, &theta);
				theta *= weight;
				D3DXQuaternionRotationAxis( &q2, &axis, theta );
				pBone->qRotation_*=q2;
				pBone->pos_+= pos*weight;
				*/
				const D3DXVECTOR3 &pos= key->map_.begin()->second.first;
				pBone->pos_+= pos*weight;
				
				const D3DXQUATERNION &q=key->map_.begin()->second.second;
				//�A�j���[�V�������u�����h����ꍇ
				//���̎������Ə�肭�����Ȃ��P�[�X������B
				//3�ȏ�̃u�����h�͏�肭�����Ȃ��B
				
				//1��ڂ�blend�͂���
				if( pBone->qRotation_==D3DXQUATERNION(0.f,0.f,0.f,1.f) ){
					pBone->qRotation_=q*weight;
				}
				//2��ڂ̃u�����h�͂���
				else {
					D3DXQUATERNION qOut;
					D3DXQUATERNION qb=pBone->qRotation_/(1.f-weight);
					float qDot = D3DXQuaternionDot( &q, &qb );
					
					if(qDot<0){
						qOut= q*weight - pBone->qRotation_;
					} else {
						qOut= q*weight + pBone->qRotation_;
					}
					D3DXQuaternionNormalize( &pBone->qRotation_, &qOut );
				}
			}
		} else {
			std::map<float, std::pair<D3DXVECTOR3, D3DXQUATERNION> >::iterator it= key->map_.begin();
			
			float time0 = it->first;
			D3DXVECTOR3 pos0 = it->second.first;
			D3DXQUATERNION q0 = it->second.second;
			++it;
			for( std::map<float, std::pair<D3DXVECTOR3, D3DXQUATERNION> >::iterator end= key->map_.end();
				it!=end;
				++it)
			{	
				float time1 = it->first;
				D3DXVECTOR3 pos1 = it->second.first;
				D3DXQUATERNION q1 = it->second.second;
				assert(time0<time1);
				// time0 < frame <time1�̏��������ǃ\�[�g���Ă���̂ł���ŏ\��
				if ( frame<time1 ) {
				
					float r=(time1-frame)/(time1-time0);
					
					D3DXVECTOR3 vOut;

					float rate = interpolationX_.getInterValue( r );
					float a = pos0.x;
					float b = pos1.x;
					vOut.x = a*rate + b*(1.f-rate);

					rate = interpolationY_.getInterValue( r );
					a = pos0.y;
					b = pos1.y;
					vOut.y = a*rate + b*(1.f-rate);

					rate = interpolationZ_.getInterValue( r );
					a = pos0.z;
					b = pos1.z;
					vOut.z = a*rate + b*(1.f-rate);

					
					//��]�����̕��
					D3DXQUATERNION qOut;
					rate = interpolationQ_.getInterValue( r );
					float qDot = D3DXQuaternionDot( &q0, &q1 );
					
					if(qDot<0){
						qOut= q0*rate - q1*(1-rate);
					} else {
						qOut= q0*rate + q1*(1-rate);
					}
					//��]�����͋��ʕ�Ԃ̂ق��������H
					//D3DXQuaternionSlerp(&qOut,&q1,&q2,r);
					D3DXQuaternionNormalize( &qOut, &qOut );
					
					//if(strcmp( pBone->getName(), "�Z���^�[" )==0 ) {
					//	OutputDebugStringA( "q0\n" );
					//	debugPrintQuaternion( q0 );
					//	OutputDebugStringA( "q1\n" );
					//	debugPrintQuaternion( q1 );
					//	
					//	OutputDebugStringA("t=");
					//	OutputDebugStringA( boost::lexical_cast<std::string>(rate).c_str() );
					//	OutputDebugStringA(";\n");

					//	OutputDebugStringA( "��Ԍ�@qOut\n" );
					//	debugPrintQuaternion( qOut );
					//}

					if(weight==1.f){
						pBone->qRotation_ = qOut;
						pBone->pos_ = vOut;
					} else {
						/*
						float theta;
						D3DXVECTOR3 axis;
						D3DXQUATERNION q1 = qOut,q2;
						D3DXQuaternionToAxisAngle(&q1, &axis, &theta);
						theta *= weight;
						D3DXQuaternionRotationAxis( &q2, &axis, theta );
						pBone->qRotation_*=q2;
						pBone->pos_+= vOut*weight;
						*/
						const D3DXVECTOR3 &pos= key->map_.begin()->second.first;
						pBone->pos_+= pos*weight;
				
						const D3DXQUATERNION &q=key->map_.begin()->second.second;
						//�A�j���[�V�������u�����h����ꍇ
						//���̎������Ə�肭�����Ȃ��P�[�X������B
						//3�ȏ�̃u�����h�͏�肭�����Ȃ��B
				
						//1��ڂ�blend�͂���
						if( pBone->qRotation_==D3DXQUATERNION(0.f,0.f,0.f,1.f) ){
							pBone->qRotation_=q*weight;
						}
						//2��ڂ̃u�����h�͂���
						else {
							D3DXQUATERNION qOut;
							D3DXQUATERNION qb=pBone->qRotation_/(1.f-weight);
							float qDot = D3DXQuaternionDot( &q, &qb );
					
							if(qDot<0){
								qOut= q*weight - pBone->qRotation_;
							} else {
								qOut= q*weight + pBone->qRotation_;
							}
							D3DXQuaternionNormalize( &pBone->qRotation_, &qOut );
						}
					}
					break;
				}
				time0=time1;
				pos0=pos1;
				q0=q1;
			}
		}
	}
}

void CAnimation::updateFace( CFace *pFaces, const short &faceSize, const float &frame, const float &weight ) {
	//weight��weight������̂͂ǂ��Ȃ́H
	if (weight==0) return;//�A�j���[�V�����R���g���[����Weight_���O���Ă��Ƃ͉������Ȃ��̂ŃX���[
	
	// �A�j���[�V�����L�[���֌W����Face��weight_��0�ŏ�����
	
	//for (unsigned int i=0, imax=faceKeys_.size(); i<imax; ++i) {
	//	CFaceKey* key=faceKeys_[i];
	//	//facekey��index��������,�T���̂�߂������ق��������Ǝv��
	//	CFace *pFace= *( *pFaces | find_name(key->name_) );
	//	//CFace *pFace= *std::find_if(pFaces->begin(), pFaces->end(), name_is(key->name_) );
	//	pFace->weight_=0.f;
	//}
	{
		CFace *pFace = pFaces;
		for (unsigned int i=0, imax=faceSize; i<imax; ++i) {
			pFace[i].weight_=0.f;
		}
	}


	//�\��A�j���[�V�������Đ�����\��̏d�݂��v�Z����
	for(unsigned j=0, jmax=faceKeys_.size(); j<jmax; ++j)
	{
		CFaceKey* key=faceKeys_[j];
		
		CFace *face =  findNameInArray( pFaces, faceSize, key->name_.c_str() );
		if( face==NULL )
		{
			//assert���ׂ�������
			continue;
		}
		//CFace* face=*std::find_if(pFaces->begin(), pFaces->end(), name_is(key->name_) );
		if (key->map_.empty()){ continue; }
				
		// �L�[�t���[���̕��
		if ( frame <= key->map_.begin()->first ) {
			face->weight_ += key->map_.begin()->second*weight;
		} 
		else if(frame>=key->map_.rbegin()->first )
		{
			face->weight_ += key->map_.rbegin()->second*weight;
		} 
		else 
		{	
			float time0, weight0;
			std::map<float, float>::iterator it = key->map_.begin();
			time0 = it->first;
			weight0 = it->second;
			++it;
			for( std::map<float, float>::iterator end = key->map_.end();
				 it!=end;
				 ++it)
			{
				float time1, weight1;
				time1 = it->first;
				weight1 = it->second;

				assert(time0<time1);
				// time0 < frame <time1�̏��������ǃ\�[�g���Ă���̂ł���ŏ\��
				if ( frame < time1 )
				{
					float r=(time1-frame) / (time1-time0);
					face->weight_ += ( weight0*r + weight1*(1-r) )*weight;
					break;
				}	
				time0=time1;
				weight0=weight1;
			}
		}
	}
}
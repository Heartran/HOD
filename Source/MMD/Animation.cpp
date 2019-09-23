#include "Animation.h"
#include "BoneKey.h"
#include "FaceKey.h"
#include "ContainerHelper.h"
#include <algorithm>
#include"MMDHelperFunc.h"
#include<string>
#include"MMDDataBase.h"
#include"TemplateVMD.h"

namespace MMD{
Animation::Animation()
	:maxFrame_(0.f),
	numBoneKeys_(0),
	numFaceKeys_(0)
{};

Animation::~Animation()
{}

//�A�j���[�V�������E�F�C�g�ō����Ďg�����Ƃ�z�肵�Ă�
//void Animation::resetRelativeBones( ) {
//	assert(false &&"����Ȃ��Ȃ�");
//	////������
//	//BoneKey *bk = boneKeys_.get();
//	//for (unsigned int i=0; i<numBoneKeys_; ++i) {
//	//	bk[i].bone()->reset();
//	//}
//}

void Animation::updateBones(Bone *bones, float frame) {
	BoneKey *bk = boneKeys_.get();	
	
	for (unsigned int i=0; i<numBoneKeys_; ++i) {
		BoneKey* key=&bk[i];		
		assert(!key->empty());
		key->updateBone(frame, bones);
	}
}

void Animation::updateFace( float frame ) {
	
	//Face *pFace = pFaces;
	//for (unsigned int i=0, imax=faceSize; i<imax; ++i) {
	//	pFace[i].weight_=0.f;
	//}
	

	FaceKey* keys=faceKeys_.get();
	//�\��A�j���[�V�������Đ�����\��̏d�݂��v�Z����
	for(unsigned j=0; j<numFaceKeys_; ++j)
	{
		FaceKey* key=&keys[j];
		key->updateFace(frame);
	}
}

void Animation::init(
		const TemplateVMD &vmd,
		Bone* bones, const unsigned long &numBones,
		Face* faces, const unsigned long &numFaces )
{
	name_ = vmd.getFileName();

	std::pair< unsigned long, boost::shared_array<BoneKey> > 
		numAndBoneKeys = vmd.createBoneKeyArray( bones, numBones );
	numBoneKeys_ = numAndBoneKeys.first;
	boneKeys_ = numAndBoneKeys.second;
	
	std::pair< unsigned long, boost::shared_array<FaceKey> >
		numAndFaceKeys= vmd.createFaceKeyArray( faces, numFaces );
	numFaceKeys_ = numAndFaceKeys.first;
	faceKeys_ = numAndFaceKeys.second;

	BoneKey *bka=boneKeys_.get();
	for(unsigned long i=0; i<numBoneKeys_; ++i ){
		float maxFrame = bka[i].maxFrame();
		if( maxFrame > maxFrame_){
			maxFrame_=maxFrame;
		}
	}

	FaceKey *fka=faceKeys_.get();
	for(unsigned long i=0; i<numFaceKeys_; ++i ){
		float maxFrame = fka[i].maxFrame();
		if( maxFrame > maxFrame_){
			maxFrame_=maxFrame;
		}
	}
}
}
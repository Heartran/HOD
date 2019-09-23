#define _CRT_SECURE_NO_WARNINGS
#include "Model.h"
#include "Pmd.h"
//#include "VertexData.h"
//#include "FastVertexData.h"
#include "VertexDataUsingVertexBlendShader.h"
#include "Bone.h"
#include "WrapperIK.h"
#include "Face.h"
#include "Animation.h"
#include "PMDRigidBody.h"
#include "PMDConstraint.h"
#include "ContainerHelper.h"
#include <stdio.h>
#include <algorithm>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>

#include"MMDDataBase.h"
#include"TemplatePMD.h"
#include"TemplateVMD.h"

#include<GameLib/FileIO/InFile.h>
#include"helper/PrimitiveDrawer.h"
#include"helper/MyOutputDebugString.h"

#include<GameLib/Graphics/Manager.h>

#ifdef _DEBUG
#define MODEL_TIME
#endif

namespace{

static const char skErrorIndex=-1;


template<typename T, std::size_t N> 
char (&lengthof_helper_char_array(T (&a)[N]))[N]; 
#define LENGTHOF(a) (sizeof(lengthof_helper_char_array(a)))
}

namespace MMD {

	//============================================================================
	// ���f���̍쐬
	CModel::CModel()
		:pVertexData_(NULL),
		//rightAnkleIkBone_(NULL),
		//rightToeIkBone_(NULL),
		//leftAnkleIkBone_(NULL),
		//leftToeIkBone_(NULL),
		numMaterials_(0),
		numSubsets_(0),
		numBones_(0),
		numIks_(0),
		numFaces_(0),
		numRigidBodies_(0),
		rigidBodies_(nullptr),
		numConstraints_(0),
		frame_(0.f),
		loadingTemplatePMD_(0),
		isEnablePhysics_(false),
		weight_(0.f)
	{
	}
	CModel::~CModel() {
		removeFromWorld();
		SAFE_DELETE_ARRAY(rigidBodies_);
	}

	bool CModel::loadPMD(const char *fileName, bool isLoadBulletPysics) {
		assert(false && "use loadPMDFromDB");

		if (bones_) {
			assert(false && "dobled allocate!");
			return false;
		}

		GameLib::FileIO::InFile inFile = GameLib::FileIO::InFile::create(fileName);

		if (inFile.isError()) {
			return false;
		}
		while (!inFile.isFinished()) {}
		//GameLib���g��Ȃ��ǂݍ���
		//FILE	*pFile;
		//fpos_t	fposFileSize;
		//char *pData;

		//pFile = fopen( fileName, "rb" );
		//if( !pFile )	return false;	// �t�@�C�����J���Ȃ�

		// �t�@�C���T�C�Y�擾
		//fseek( pFile, 0, SEEK_END );
		//fgetpos( pFile, &fposFileSize );

		// �������m��
		//pData = (char *)malloc( (size_t)fposFileSize );

		// �ǂݍ���
		//fseek( pFile, 0, SEEK_SET );
		//fread( pData, 1, (size_t)fposFileSize, pFile );

		//fclose( pFile );


		// ���f���f�[�^������
		//bool	bRet = initPMD( pData, fposFileSize, isLoadBulletPysics );
		bool	bRet = initPMD(inFile.data(), inFile.size(), isLoadBulletPysics);
		assert(bRet && "���f���f�[�^���������s");
		//free( pData );

		return bRet;
	}

	bool CModel::initPMD(const char *pData, unsigned long size, bool isLoadBulletPhiscs) {
		const char *pNext = pData;
		const char * const pDataTop = pData;
		// �w�b�_�̃`�F�b�N
		const PMD_HEADER *pHeader = reinterpret_cast<const PMD_HEADER *>(pData);
		if (strcmp(pHeader->magic, "Pmd") != 0 || pHeader->version != 1.0f) {
			return false;	// �t�@�C���`�����Ⴄ
		}
		// ���f�����̃R�s�[
		modelName_.reset(new std::string(pHeader->name));

		pNext += sizeof(PMD_HEADER);
		//���_�f�[�^�̏�����
		pVertexData_.reset(new VertexDataUsingVertexBlendShader());
		pNext = pVertexData_->load(pNext);

		//�^�����e�N�X�`������
		//GameLib::Graphics::Texture whiteTexture = GameLib::Graphics::Texture::create( 1, 1, false );
		//unsigned* textureData;
		//int pitch;
		//whiteTexture.lock( &textureData, &pitch, 0 );
		//*textureData = 0xffffffff;
		//whiteTexture.unlock( &textureData );
		//textureData = 0;

		//material�̏�����
		numMaterials_ = *(reinterpret_cast<const unsigned long*>(pNext));
		pNext += sizeof(unsigned long);
		materials_.reset(new Material[numMaterials_]);

		for (unsigned long i = 0; i < numMaterials_; ++i) {
			const PMD_MATERIAL *pPMD = reinterpret_cast<const PMD_MATERIAL*>(pNext);
			Material *pM = &(materials_[i]);

			pM->diffuse_.set(
				pPMD->diffuse.r,
				pPMD->diffuse.g,
				pPMD->diffuse.b,
				pPMD->diffuse.a);

			pM->ambient_.set(
				pPMD->ambient.r,
				pPMD->ambient.g,
				pPMD->ambient.b,
				1.f);

			pM->specular_.set(
				pPMD->specular.r,
				pPMD->specular.g,
				pPMD->specular.b,
				1.f);

			pM->emissive_ = pM->ambient_;

			pM->power_ = pPMD->shininess;

			pM->isSphereTextureAdditive_ = false;

			// �e�N�X�`��
			if (pPMD->textureFileName[0]) {
				assert(20 == sizeof(pPMD->textureFileName));
				assert(27 == strlen(MMD_DATA_PATH));
				assert(27 == LENGTHOF(MMD_DATA_PATH) - 1);//test���ĂȂ����Ԃ�\0�͂�����28

				//\0�����ė~�����Ȃ��̂Ł[1
				char filePath[LENGTHOF(pPMD->textureFileName) + LENGTHOF(MMD_DATA_PATH) - 1] = MMD_DATA_PATH;
				for (int i = 0; i < LENGTHOF(pPMD->textureFileName); ++i) {
					filePath[LENGTHOF(MMD_DATA_PATH) - 1 + i] = pPMD->textureFileName[i];
				}
				pM->texture_.reset(filePath);
				while (!pM->texture_.isReady())
				{
				}
			}
			else {
				pM->texture_.resetWhiteTexture();
			}

			pM->numEdges_ = pPMD->nEdges;
			pNext += sizeof(PMD_MATERIAL);
		}

		// �T�u�Z�b�g�̍쐬
		numSubsets_ = numMaterials_;
		subsets_.reset(new Subset[numSubsets_]);
		//���v�̃|���S�������i�[���邽�߂Ɏg�p
		DWORD numFace = 0;
		for (unsigned long i = 0, end = numMaterials_; i < end; ++i) {
			Subset* subset = &(subsets_[i]);
			subset->faceCount_ = materials_[i].numEdges_ / 3;
			subset->faceIndex_ = numFace;//�J�n�ʔԍ�
			subset->materialIndex_ = i;
			numFace += subset->faceCount_;
		}

		//�{�[���̏�����
		numBones_ = *(reinterpret_cast<const unsigned short*>(pNext));
		pNext += sizeof(unsigned short);

		bones_.reset(new Bone[numBones_]);
		rootBoneIndecies_.reset(new std::vector<int>);

		for (unsigned short i = 0; i < numBones_; ++i) {
			const PMD_BONE* pPMD = reinterpret_cast<const PMD_BONE*>(pNext);
			//�ǂ��ɓ����Ă��邩(�z��)���ق����̂Œx��������
			bones_[i].init(bones_.get(), *pPMD, static_cast<int>(i));
			if (pPMD->parent == -1) {
				rootBoneIndecies_->push_back(i);
			}
			pNext += sizeof(PMD_BONE);
		}

		//IK�̏�����
		numIks_ = *(reinterpret_cast<const unsigned short*>(pNext));
		pNext += sizeof(unsigned short);
		iks_.reset(new WrapperIK[numIks_]);

		for (unsigned short i = 0; i < numIks_; ++i) {
			//const PMD_IK* pPMD = reinterpret_cast<const PMD_IK*>(pNext);
			WrapperIK *pIK = &(iks_[i]);
			pNext = pIK->load(pNext, bones_.get(), numBones_);
		}

		//Face�̏�����
		numFaces_ = *(reinterpret_cast<const unsigned short*>(pNext));
		pNext += sizeof(unsigned short);
		faces_.reset(new Face[numFaces_]);

		Face *pBase = &(faces_[0]);
		pNext = pBase->load(pNext);
		for (unsigned short i = 1; i < numFaces_; ++i) {
			Face *pFace = &faces_[i];
			pNext = pFace->load(pNext);
			pFace->initalize(*pBase);
		}
#ifdef MODEL_TIME
		using namespace boost;
		boost::timer t; // �^�C�}�[�̊J�n
#endif
	//�T�u�Z�b�g, Face���g�����\�[�g�����邽�߂ɂ����ŏ�����
		pVertexData_->init(subsets_.get(), numSubsets_, faces_.get(), numFaces_);
#ifdef MODEL_TIME
		OutputDebugStringA("VertexDataUsingVertexBlendShader::load() : ");
		OutputDebugStringA(lexical_cast<std::string>(t.elapsed()).c_str());
		OutputDebugStringA("\n");
		t.restart();
#endif


		enablePhysics(isLoadBulletPhiscs);
		if (isLoadBulletPhiscs) {
			unsigned long leftedDataSize = size - (pNext - pDataTop);
			bool b = skipPMDData(&pNext, pNext, leftedDataSize);
			assert(b && "");
			if (!b) { return false; }
			unsigned long bulletDataSize = size - (pNext - pDataTop);
			return loadBulletPhysics(pNext, bulletDataSize);
		}

		return true;

	}
	std::shared_ptr<Animation>  CModel::loadVMD(const char *fileName) {

		while (!isReady()) {}

		assert(bones_);
		// �A�j���[�V����������
		std::shared_ptr<Animation> anim =
			MMDDataBase::createAnimationVMD(
				fileName_->c_str(),
				fileName,
				bones_.get(), numBones_,
				faces_.get(), numFaces_
			);
		return anim;
	}

	void CModel::startWeightBlend()
	{
		weight_ = 1.f;
		for (std::vector<int>::iterator it = rootBoneIndecies_->begin(); it != rootBoneIndecies_->end(); ++it) {
			bones_[*it].saveWeightBlendMatrix();
		}

	}

	//�A�j���[�V�������E�F�C�g�ō����Ďg�����Ƃ�z�肵�Ă�
	//�Ȃ̂őS�ẴA�j���[�V�������܂킷��������
	//if���ő��U��߂Ă�̂ł����܂Ō����͈����Ȃ������F�A�j���[�V�������u�����h����@������Ƃ��v��Ȃ�
	//���̎����̂����_�̓C���^�[�t�F�C�X���u�����h����Ƃ������Ȃ��Ƃ����ЂƂōς�ł邱�Ƃ���
	void CModel::updateBone(const D3DXMATRIX &world ) {

		// �A�j���[�V�����L�[���֌W����t���[���̕ό`�s����v�Z
		if (currentAnimation_) {
			currentAnimation_->updateBones(bones_.get(), frame_);
		}
		// �t���[���̍����s����v�Z
		for (std::vector<int>::iterator it = rootBoneIndecies_->begin(); it != rootBoneIndecies_->end(); ++it) {
			bones_[*it].updateSkinningMatrix(world);
		}
		
	}

	void CModel::updateWeightBlend() {
		for (std::vector<int>::iterator it = rootBoneIndecies_->begin(); it != rootBoneIndecies_->end(); ++it) {
			// �A�j���[�V�����؂�ւ����
			bones_[*it].exeWeightBlend(weight_);
		}
		if (weight_ > 0) {
			weight_ -= 0.1f;
			if (weight_ < 0) {
				weight_ = 0.f;
			}
		}

	}

	void CModel::updateVertex() {
	}

	void CModel::updateIK() {
		for (unsigned short i = 0; i < numIks_; ++i)
		{
			WrapperIK* ik = &iks_[i];
			ik->solve(bones_.get());
		}
	}

	//�\��𕡐��̃A�j���[�V�������狁�߂�K�v����́H
	//�ЂƂ̕\����ŗǂ��Ȃ����H�΂��Ȃ���N����Ƃ������̃A�j���[�V��������������Ə΂��Ȃ���{��Ƃ��ł���
	void CModel::updateFace() {
		if (currentAnimation_) {
			currentAnimation_->updateFace(frame_);
		}
	}

	void CModel::updateFaceVertex() {
		pVertexData_->updateFaceVertex(faces_.get(), numFaces_);
	}

	void CModel::draw() {
		//using namespace boost;
		//boost::timer t; // �^�C�}�[�̊J�n
		using namespace GameLib::Graphics;
		Manager m = Manager::instance();
		m.setBlendMode(BLEND_LINEAR);
		//MMD�I�u�W�F�N�g�̓J����������W�n�ŏ����ꂽ���̂�
		//�J�����E����W�n�ŏ����ꍇ��CULL_BACK
		m.setCullMode(CULL_FRONT);
		m.enableDepthTest(true);
		m.enableDepthWrite(true);

		GameLib::Math::Matrix34 matW;
		matW.setIdentity();
		m.setWorldMatrix(matW);
		pVertexData_->draw(
			bones_.get(), numBones_,
			subsets_.get(), numSubsets_,
			materials_.get(), numMaterials_);
		//OutputDebugStringA( "draw(): " );
		//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
		//OutputDebugStringA( "\n" );
		//t.restart();
	}

	void CModel::drawByMME() {
		//using namespace boost;
		//using namespace GameLib::Graphics;
		//Manager m = Manager::instance();
		////boost::timer t; // �^�C�}�[�̊J�n
		//m.setBlendMode( BLEND_LINEAR );
		//m.setCullMode( CULL_FRONT );
		//m.enableDepthTest( true );
		//m.enableDepthWrite( true );

		pVertexData_->drawByMME(
			bones_.get(), numBones_,
			subsets_.get(), numSubsets_,
			materials_.get(), numMaterials_);
		//OutputDebugStringA( "draw(): " );
		//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
		//OutputDebugStringA( "\n" );
		//t.restart();
	}
	void CModel::debugDraw() {
		//BulletPhysics::Manager::instance()->drawWorld();
		//BOOST_FOREACH(PMDConstraint* pC, constraints_){ pC->draw(); }
		//BOOST_FOREACH(PMDRigidBody *pR, rigidBodies_){
			//pR->debugDraw();
		//}
		for (unsigned short i = 0; i < numIks_; ++i) {
			WrapperIK *ik = &iks_[i];
			Bone *bonearray = bones_.get();;
			D3DXVECTOR3 pos = bonearray[ik->getIndex()].getPos();
			PrimitiveDrawer::drawSphere(pos, 1.f, 0xffff8888);
		}
	}
	void CModel::update(const D3DXMATRIX &world, bool enablePhiscs) {

		//using namespace boost;
		//boost::timer t; // �^�C�}�[�̊J�n

		if (enablePhiscs) {
			//using namespace boost;
			//boost::timer t; // �^�C�}�[�̊J�n
			updateBone(world);
			//OutputDebugStringA( "updateBone(): " );
			//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
			//OutputDebugStringA( "\n" );
			//t.restart();

			updateIK();
			//OutputDebugStringA( "updateIK(): " );
			//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
			//OutputDebugStringA( "\n" );
			//t.restart();

			updateWeightBlend();


			updateFace();
			//OutputDebugStringA( "updateFace(): " );
			//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
			//OutputDebugStringA( "\n" );
			//t.restart();

			updateFaceVertex();
			//OutputDebugStringA( "updateFaceVertex(): " );
			//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
			//OutputDebugStringA( "\n" );
			//t.restart();

			updateRigidBody();
			//OutputDebugStringA( "updateRigidBody(): " );
			//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
			//OutputDebugStringA( "\n" );
			//t.restart();

			//updateVertex();
			//OutputDebugStringA( "updateVertex(): " );
			//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
			//OutputDebugStringA( "\n" );
			//t.restart();

		}
		else {
			updateBone(world);
			updateIK();
			updateFace();
			updateFaceVertex();
			updateVertex();
		}

		//OutputDebugStringA( "CModel::update(): " );
		//OutputDebugStringA( lexical_cast<std::string>(t.elapsed()).c_str() );
		//OutputDebugStringA( "\n" );
		//t.restart();

	}

	bool CModel::skipPMDData(const char **next, const char *pData, unsigned long size) {
		const char * const pDataTop = pData;

		//-----------------------------------------------------
		// �������獄�̏��܂œǂݔ�΂�

		// �\��g�p�\�����X�g
		unsigned char	cbFaceDispNum = *((unsigned char *)pData);
		pData += sizeof(unsigned char);

		pData += sizeof(unsigned short) * cbFaceDispNum;

		// �{�[���g�p�g�����X�g
		unsigned char	cbBoneDispNum = *((unsigned char *)pData);
		pData += sizeof(unsigned char);

		pData += sizeof(char) * 50 * cbBoneDispNum;

		// �{�[���g�p�\�����X�g
		unsigned long	ulBoneFrameDispNum = *((unsigned long *)pData);
		pData += sizeof(unsigned long);

		pData += 3 * ulBoneFrameDispNum;

		// �㑱�f�[�^�̗L�����`�F�b�N
		if ((unsigned long)pData - (unsigned long)pDataTop >= size) {
			return false;
		}

		// �p�ꖼ�Ή�
		unsigned char	cbEnglishNameExist = *((unsigned char *)pData);
		pData += sizeof(unsigned char);

		if (cbEnglishNameExist)
		{
			// ���f�����ƃR�����g(�p��)
			pData += sizeof(char) * 276;

			// �{�[����(�p��)
			pData += sizeof(char) * 20 * numBones_;

			// �\�(�p��)
			pData += sizeof(char) * 20 * (numFaces_ - 1);	// "base"�͉p�ꖼ���X�g�ɂ͊܂܂�Ȃ�

			// �{�[���g�p�g�����X�g(�p��)
			pData += sizeof(char) * 50 * (cbBoneDispNum);
		}

		// �㑱�f�[�^�̗L�����`�F�b�N(�����̃`�F�b�N�͕s�v����)
		if ((unsigned long)pData - (unsigned long)pDataTop >= size) {
			return false;
		}

		// �g�D�[���e�N�X�`�����X�g
		pData += 100 * 10;		// �t�@�C���� 100Byte * 10��

		// �����܂œǂݔ�΂�
		//-----------------------------------------------------
		*next = pData;
		return true;
	}

	bool CModel::loadBulletPhysics(const char *pData, unsigned long size) {
		const char * const pDataTop = pData;

		// �㑱�f�[�^�̗L�����`�F�b�N
		if ((unsigned long)pData - (unsigned long)pDataTop >= size) {
			return false;
		}

		//-----------------------------------------------------
		// ���̐��擾
		numRigidBodies_ = *((unsigned long *)pData);
		pData += sizeof(unsigned long);

		// ���̔z����쐬
		if (numRigidBodies_)
		{

			rigidBodies_ = new std::shared_ptr<PMDRigidBody>[numRigidBodies_];

			for (unsigned long i = 0; i < numRigidBodies_; ++i) {
				rigidBodies_[i] = PMDRigidBody::create();
				PMDRigidBody *pRbody = rigidBodies_[i].get();


				const PMD_RigidBody *pPMDRigidBody = (const PMD_RigidBody *)pData;

				Bone *pBone = NULL;
				if (pPMDRigidBody->unBoneIndex == 0xFFFF) {
					pBone = findNameInArray(bones_.get(), numBones_, "�Z���^�[");
				}
				else {
					pBone = &(bones_[pPMDRigidBody->unBoneIndex]);
				}
				pRbody->initialize(pPMDRigidBody, pBone);
				pData += sizeof(PMD_RigidBody);
			}
		}

		//-----------------------------------------------------
		// �R���X�g���C���g���擾
		numConstraints_ = *((unsigned long *)pData);
		pData += sizeof(unsigned long);

		// �R���X�g���C���g�z����쐬
		if (numConstraints_)
		{
			constraints_.reset(new PMDConstraint[numConstraints_]);
			for (unsigned long i = 0; i < numConstraints_; ++i) {
				PMDConstraint *pC = &constraints_[i];

				const PMD_Constraint *pPMDConstraint = (const PMD_Constraint *)pData;

				PMDRigidBody *pRigidBodyA = rigidBodies_[pPMDConstraint->ulRigidA].get();
				PMDRigidBody *pRigidBodyB = rigidBodies_[pPMDConstraint->ulRigidB].get();

				pC->initialize(pPMDConstraint, pRigidBodyA, pRigidBodyB);
				pData += sizeof(PMD_Constraint);
			}
		}

		return true;
	}
	void CModel::updateRigidBody() {

		for (unsigned long i = 0; i < numRigidBodies_; ++i)
		{
			rigidBodies_[i]->updateBoneTransform();
		}
	}

	void CModel::enablePhysics(bool enabled) {
		//�v�Z�ʌ��炷����
		//ik��S���v�Z���Ă����܂�Ȃ������ʂɂȂ邽��
		//mmd�̎������ƑS��true���H
		for (unsigned long i = 0; i < numIks_; ++i)
		{
			if (enabled) {
				WrapperIK *pIK = &iks_[i];
				unsigned short bi = pIK->getIndex();
				const char *name = bones_[bi].getName();
				if (strcmp(name, "�E���h�j") == 0) {
					pIK->setEnable(true);
					//TRACE1("%s enable ik\n",name);
				}
				else if (strcmp(name, "�E���܂����h�j") == 0) {
					pIK->setEnable(true);
					//TRACE1("%s enable ik\n",name);
				}
				else if (strcmp(name, "�����h�j") == 0) {
					pIK->setEnable(true);
					//TRACE1("%s enable ik\n",name);
				}
				else if (strcmp(name, "�����܂����h�j") == 0) {
					pIK->setEnable(true);
					//TRACE1("%s enable ik\n",name);
				}
				else {
					pIK->setEnable(false);
					//TRACE1("%s disable ik\n",name);
				}
			}
			else {
				WrapperIK *pIK = &iks_[i];
				pIK->setEnable(true);
			}
		}
	}


	bool CModel::isFrameOver() const {
		return  currentAnimation_ && frame_ >= currentAnimation_->getMaxFrame();
	}

	void CModel::fitRigidBodeisToBone(const D3DXMATRIX &world) {
		updateBone(world);

		for (unsigned long i = 0; i < numRigidBodies_; ++i) {
			rigidBodies_[i]->fitBone();
		}
	}

	void CModel::resetBones(const D3DXMATRIX &world) {
		Bone* barray = bones_.get();
		for (unsigned long i = 0; i < numBones_; ++i) {
			barray[i].reset();
		}
		updateBone(world);
	}

	Bone* CModel::getBone(const char *name) {
		return findNameInArray(bones_.get(), numBones_, name);
	}
	Bone* CModel::getBone(int index) {
		return &bones_[index];
	}


	//void CModel::setHeightFootIKBones( float groundHeight ){
	//	//assert( rightAnkleIkBone_ && rightToeIkBone_ && leftAnkleIkBone_ && leftToeIkBone_ 
	//	//	&& 
	//	//	"rightAnkleIkBone_ and etc must be not NULL, \
	//	//	Please set right bone name Using setRightFootIKBone(), \
	//	//	Or Use update( world, enablePhysics) instead Of update( world, groundHeight, enablePhsics)." );
	//	if( rightAnkleIkBone_  )
	//	{
	//		rightAnkleIkBone_->getSkinningMatrix()._42 = groundHeight;
	//	}
	//	
	//	if( rightToeIkBone_  )
	//	{
	//		rightToeIkBone_->getSkinningMatrix()._42 = groundHeight;
	//	}
	//	
	//	if( leftAnkleIkBone_  )
	//	{
	//		leftAnkleIkBone_->getSkinningMatrix()._42 = groundHeight;
	//	}
	//
	//	if( leftToeIkBone_  )
	//	{
	//		leftToeIkBone_->getSkinningMatrix()._42 = groundHeight;
	//	}
	//}
	//
	//
	//bool CModel::setRightFootIKBone(const char* boneName )
	//{
	//	std::vector<Bone*>::iterator end=bones_.end();
	//	std::vector<Bone*>::iterator it= bones_ | find_name( boneName );
	//	if( it==end ){
	//		assert( false && boneName && " is not found." );
	//		return false;
	//	}
	//	rightAnkleIkBone_ = *it;
	//	return true;
	//}
	//
	//bool CModel::setLeftFootIKBone(const char* boneName )
	//{
	//	std::vector<Bone*>::iterator end=bones_.end();
	//	std::vector<Bone*>::iterator it= bones_ | find_name( boneName );
	//	if( it==end ){
	//		assert( false && boneName && " is not found." );
	//		return false;
	//	}
	//	leftAnkleIkBone_ = *it;
	//	return true;
	//}
	//bool CModel::setRightToeIKBone(const char* boneName )
	//{
	//	std::vector<Bone*>::iterator end=bones_.end();
	//	std::vector<Bone*>::iterator it= bones_ | find_name( boneName );
	//	if( it==end ){
	//		assert( false && boneName && " is not found." );
	//		return false;
	//	}
	//	rightToeIkBone_ = *it;
	//	return true;
	//}
	//
	//bool CModel::setLeftToeIKBone(const char* boneName )
	//{
	//	std::vector<Bone*>::iterator end=bones_.end();
	//	std::vector<Bone*>::iterator it= bones_ | find_name( boneName );
	//	if( it==end ){
	//		assert( false && boneName && " is not found." );
	//		return false;
	//	}
	//	leftToeIkBone_ = *it;
	//	return true;
	//}

	//�E��(����)�Ƃܐ��IK�̖ڕW�ʒu��y���w��̍����ɂ���update()
	//void CModel::update(  const D3DXMATRIX &world, float groundHeight, bool enablePhiscs ){
	//	if( enablePhiscs ) {
	//		updateBone(world);
	//		
	//		setHeightFootIKBones(groundHeight);
	//		
	//		updateIK();
	//
	//		updateFace();
	//
	//		updateFaceVertex();
	//			
	//		updateRigidBody();
	//
	//		updateVertex();
	//		
	//	}
	//	else {
	//		updateBone(world);
	//		setHeightFootIKBones(groundHeight);		
	//		updateIK();
	//		updateFace();
	//		updateFaceVertex();
	//		updateVertex();
	//	}
	//
	//}


	bool CModel::loadPMD(const GameLib::FileIO::InFile &inFile, bool isLoadBulletPysics) {
		assert(false && "use loadPMDFromDB");
		if (bones_) {
			assert(false && "dobled allocate!");
			return false;
		}

		if (inFile.isError()) {
			assert(false);
			return false;
		}

		while (!inFile.isFinished()) {}

		// ���f���f�[�^������
		bool	bRet = initPMD(inFile.data(), inFile.size(), isLoadBulletPysics);
		assert(bRet && "���f���f�[�^���������s");

		return bRet;
	}

	void CModel::build(const TemplatePMDSharedPtr & pmd, bool isEnableBulletPysics) {
		fileName_ = pmd->fileName_;
		modelName_ = pmd->modelName_;

		pVertexData_ = pmd->pVertexData_;

		numMaterials_ = pmd->numMaterials_;
		materials_ = pmd->materials_;
		numSubsets_ = pmd->numSubsets_;
		subsets_ = pmd->subsets_;

		//�{�[���̍쐬�A������
		numBones_ = pmd->numBones_;
		bones_.reset(new Bone[numBones_]);
		rootBoneIndecies_.reset(new std::vector<int>);

		for (unsigned short i = 0; i < numBones_; ++i) {
			const PMD_BONE &pmdbone = pmd->bonesData_[i];
			//�ǂ��ɓ����Ă��邩(�z��)���ق����̂Œx��������
			bones_[i].init(bones_.get(), pmdbone, static_cast<int>(i));
			if (pmdbone.parent == -1) {
				rootBoneIndecies_->push_back(i);
			}
		}
		rootBoneIndecies_->shrink_to_fit();

		numIks_ = pmd->numIks_;
		iks_ = pmd->iks_;

		numFaces_ = pmd->numFaces_;
		faces_ = pmd->faces_;

		toonColors_ = pmd->toonColors_;
		toonTextures_ = pmd->toonTextures_;

		if (isEnableBulletPysics) {
			numRigidBodies_ = pmd->numRigidBodies_;
			// ���̔z����쐬
			if (numRigidBodies_)
			{
				Bone *pCenterBone = findNameInArray(bones_.get(), numBones_, "�Z���^�[");
				rigidBodies_ = new std::shared_ptr<PMDRigidBody>[numRigidBodies_];
				for (unsigned long i = 0; i < numRigidBodies_; ++i) {
					rigidBodies_[i] = PMDRigidBody::create();
					PMDRigidBody *pRbody = rigidBodies_[i].get();

					const PMD_RigidBody &refPMDRigidBody = pmd->rigidBodiesData_[i];

					Bone *pBone = NULL;
					if (refPMDRigidBody.unBoneIndex == 0xFFFF) {
						pBone = pCenterBone;
					}
					else {
						pBone = &(bones_[refPMDRigidBody.unBoneIndex]);
					}
					pRbody->initialize(&refPMDRigidBody, pBone);
				}
			}

			numConstraints_ = pmd->numConstraints_;

			// �R���X�g���C���g�z����쐬
			if (numConstraints_)
			{
				constraints_.reset(new PMDConstraint[numConstraints_]);
				for (unsigned long i = 0; i < numConstraints_; ++i) {
					PMDConstraint *pC = &constraints_[i];

					const PMD_Constraint *pPMDConstraint = &pmd->constraintsData_[i];

					PMDRigidBody *pRigidBodyA = rigidBodies_[pPMDConstraint->ulRigidA].get();
					PMDRigidBody *pRigidBodyB = rigidBodies_[pPMDConstraint->ulRigidB].get();

					pC->initialize(pPMDConstraint, pRigidBodyA, pRigidBodyB);
				}
			}
		}
		enablePhysics(isEnableBulletPysics);
	}


	void CModel::loadPMDFromDB(const char *fileName, bool isEnableBulletPysics) {
		isEnablePhysics_ = isEnableBulletPysics;
		loadingTemplatePMD_ = MMDDataBase::loadPMD(fileName);
		assert(loadingTemplatePMD_);
		if (loadingTemplatePMD_) {
			if (loadingTemplatePMD_->isReady()) {
				build(loadingTemplatePMD_, isEnableBulletPysics);
				loadingTemplatePMD_.reset();
			}
		}
		return;
	}

	const std::string & CModel::getFileName()const {
		return *fileName_;
	}

	bool CModel::isReady() {
		if (loadingTemplatePMD_) {
			if (loadingTemplatePMD_->isReady()) {
				//load�����Ȃ̂ŏ�����
				build(loadingTemplatePMD_, isEnablePhysics_);
				loadingTemplatePMD_.reset();
				return true;
			}
			else {
				//load��
				return false;
			}
		}
		else {
			return true;
		}
	}

	void CModel::addToWorld() {

		for (unsigned long i = 0; i < numRigidBodies_; ++i) {
			rigidBodies_[i]->addToWorld();
		}

		for (unsigned long i = 0; i < numConstraints_; ++i) {
			constraints_[i].addToWorld();
		}
	}

	void CModel::removeFromWorld() {
		for (unsigned long i = 0; i < numRigidBodies_; ++i) {
			rigidBodies_[i]->removeFromWorld();
		}

		for (unsigned long i = 0; i < numConstraints_; ++i) {
			constraints_[i].removeFromWorld();
		}
	}






	void CModel::setVelocityZeroRigidBodies() {
		for (unsigned long i = 0; i < numRigidBodies_; ++i) {
			rigidBodies_[i]->setVelocityZero();
		}

	}

}
#include"TemplatePMD.h"
#include"Pmd.h"
#include"VertexDataUsingVertexBlendShader.h"
#include"WrapperIK.h"
#include"Face.h"
#include<GameLib/FileIO/InFile.h>
#include<GameLib/FileIO/Manager.h>
#include<boost/timer.hpp>
#include<boost/lexical_cast.hpp>
#include<string>

#include"helper/MyOutputDebugString.h"

#include <boost/filesystem.hpp>


//#include<stdlib.h>  //_countof

//template<typename T, std::size_t N> 
//char (&lengthof_helper_char_array(T (&a)[N]))[N]; 
//#define LENGTHOF(a) (sizeof(lengthof_helper_char_array(a)))
namespace{
	std::string getExtension( const std::string &src){
    int index = src.rfind( ".", src.size());
    if( index == std::string::npos){
        assert( false && "�g���q�����݂��Ȃ�");
    }
    std::string str;
    str = src.substr( index + 1);
    return str;
}
}
namespace MMD {

	TemplatePMD::TemplatePMD(const char * name)
		:numMaterials_(0),
		numSubsets_(0),
		numBones_(0),
		numIks_(0),
		numFaces_(0),
		numRigidBodies_(0),
		numConstraints_(0),
		file_(0)
	{
		fileName_.reset(new std::string(name));
		toonColors_.reset(new unsigned int[10]);
		toonTextures_.reset(new FlyWeightTexture[10]);
		for (int i = 0; i < 10; ++i) {
			toonColors_[i] = 0;
		}
		file_ = new GameLib::FileIO::InFile();
		*file_ = GameLib::FileIO::InFile::create(name);

		if (file_->isError()) {
			assert(false && "pmd file not found.");
		}
		isReady();
	}
	TemplatePMD::~TemplatePMD() {
		delete file_;
		file_ = 0;
	}

	bool TemplatePMD::init(const char *pData, unsigned long size) {
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
		materials_.reset(new CModel::Material[numMaterials_]);

		for (unsigned long i = 0; i < numMaterials_; ++i) {
			const PMD_MATERIAL *pPMD = reinterpret_cast<const PMD_MATERIAL*>(pNext);
			CModel::Material *pM = &(materials_[i]);

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

			pM->toonIndex_ = pPMD->toonIndex;

			pM->isSphereTextureAdditive_ = false;

			//toon����
			//assert( pM->toonIndex_ != 0xff);

			// �e�N�X�`��
			if (pPMD->textureFileName[0]) {
				int pos = fileName_->rfind('/');
				std::string filePath(*fileName_, 0, pos + 1);

				//�e�N�X�`���t�@�C���ɃX�t�B�A�t�@�C������Z�܂��͉��Z����ꍇ
				//(MMD 5.12�ȍ~)
				//"�e�N�X�`����.bmp*�X�t�B�A��.sph" �ŏ�Z
				//"�e�N�X�`����.bmp*�X�t�B�A��.spa" �ŉ��Z
				const char* asteriskPos = strrchr(pPMD->textureFileName, '*');

				if (asteriskPos == NULL) {
					std::string extension = getExtension(pPMD->textureFileName);
					filePath.append(pPMD->textureFileName);

					if (extension == "sph") {
						pM->texture_.resetWhiteTexture();
						pM->sphereTexture_.reset(filePath);
						pM->isSphereTextureAdditive_ = false;
						while (!pM->sphereTexture_.isReady()) {}
						assert(!pM->sphereTexture_.isError());

					}
					else if (extension == "spa") {
						pM->texture_.resetWhiteTexture();
						pM->sphereTexture_.reset(filePath);
						pM->isSphereTextureAdditive_ = true;
						while (!pM->sphereTexture_.isReady()) {}
						assert(!pM->sphereTexture_.isError());
					}
					else {
						pM->texture_.reset(filePath);
						while (!pM->texture_.isReady()) {}
						assert(!pM->texture_.isError());
						pM->isSphereTextureAdditive_ = false;//�܂�true�ł�false�ǂ����ł������A�Ƃ肠����������
					}


				}
				else {
					std::string textureFileName = filePath + std::string(pPMD->textureFileName, asteriskPos);
					std::string spherTextureFileName = filePath + std::string(asteriskPos + 1);

					pM->texture_.reset(textureFileName);
					pM->sphereTexture_.reset(spherTextureFileName);
					std::string extension = getExtension(spherTextureFileName);
					if (extension == "sph") {
						pM->isSphereTextureAdditive_ = false;
					}
					else if (extension == "spa") {
						pM->isSphereTextureAdditive_ = true;
					}
					else {
						assert(false);
					}
					while (!pM->texture_.isReady()) {}
					while (!pM->sphereTexture_.isReady()) {}
					assert(!pM->texture_.isError());
					assert(!pM->sphereTexture_.isError());
				}
			}
			else {
				pM->texture_.resetWhiteTexture();
			}

			pM->numEdges_ = pPMD->nEdges;
			pM->edgeFlag_ = pPMD->edgeFlag;
			//TRACE1("%d\n",pPMD->edgeFlag);
			//toon����
			assert(
				pM->toonIndex_ != 0xff &&
				static_cast<const GameLib::Graphics::Texture &>(pM->texture_) &&
				static_cast<const GameLib::Graphics::Texture &>(pM->sphereTexture_) &&
				"�`�捂�����̂��� VertexDataUsingVertexBlendShader::drawByMME��������������,�e�N�X�`�����Ȃ��ꍇ�͋����Ȃ��B");


			pNext += sizeof(PMD_MATERIAL);
		}

		// �T�u�Z�b�g�̍쐬
		numSubsets_ = numMaterials_;
		subsets_.reset(new CModel::Subset[numSubsets_]);
		//���v�̃|���S�������i�[���邽�߂Ɏg�p
		DWORD numFace = 0;
		for (unsigned long i = 0, end = numMaterials_; i < end; ++i) {
			CModel::Subset* subset = &(subsets_[i]);
			subset->faceCount_ = materials_[i].numEdges_ / 3;
			subset->faceIndex_ = numFace;//�J�n�ʔԍ�
			subset->materialIndex_ = i;
			numFace += subset->faceCount_;
		}

		//�{�[���̏�����
		numBones_ = *(reinterpret_cast<const unsigned short*>(pNext));
		pNext += sizeof(unsigned short);

		bonesData_.reset(new PMD_BONE[numBones_]);
		memcpy(bonesData_.get(), pNext, sizeof(PMD_BONE)*numBones_);

		pNext += sizeof(PMD_BONE)*numBones_;

		//IK�̏�����
		numIks_ = *(reinterpret_cast<const unsigned short*>(pNext));
		pNext += sizeof(unsigned short);
		iks_.reset(new WrapperIK[numIks_]);

		for (unsigned short i = 0; i < numIks_; ++i) {
			//const PMD_IK* pPMD = reinterpret_cast<const PMD_IK*>(pNext);
			WrapperIK *pIK = &(iks_[i]);
			pNext = pIK->load(pNext);
		}

		//Face�̏�����
		numFaces_ = *(reinterpret_cast<const unsigned short*>(pNext));
		pNext += sizeof(unsigned short);
		if (numFaces_ != 0) {
			faces_.reset(new Face[numFaces_]);

			Face *pBase = &(faces_[0]);
			pNext = pBase->load(pNext);
			for (unsigned short i = 1; i < numFaces_; ++i) {
				Face *pFace = &faces_[i];
				pNext = pFace->load(pNext);
				pFace->initalize(*pBase);
			}
		}

		boost::timer t; // �^�C�}�[�̊J�n
		//�T�u�Z�b�g, Face���g�����\�[�g�����邽�߂ɂ����ŏ�����
		pVertexData_->init(subsets_.get(), numSubsets_, faces_.get(), numFaces_);

		OutputDebugStringA("vertex init: ");
		OutputDebugStringA(boost::lexical_cast<std::string>(t.elapsed()).c_str());
		OutputDebugStringA("\n");

		unsigned long bulletDataSize = size - (pNext - pDataTop);
		return loadBulletPhysics(pNext, bulletDataSize);
	}


	bool TemplatePMD::loadBulletPhysics(const char *pData, unsigned long size) {
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
			if (numFaces_ == 0) {
			}
			else {
				pData += sizeof(char) * 20 * (numFaces_ - 1);	// "base"�͉p�ꖼ���X�g�ɂ͊܂܂�Ȃ�
			}

			// �{�[���g�p�g�����X�g(�p��)
			pData += sizeof(char) * 50 * (cbBoneDispNum);
		}

		// �㑱�f�[�^�̗L�����`�F�b�N(�����̃`�F�b�N�͕s�v����)
		if ((unsigned long)pData - (unsigned long)pDataTop >= size) {
			return false;
		}

		//�܂��͎��g�̃t�H���_����T��
		int pos = fileName_->rfind('/');
		std::string filePath(fileName_->c_str(), 0, pos + 1);
		// �g�D�[���e�N�X�`�����X�g
		for (int i = 0; i < 10; ++i) {
			std::string fullFileName(filePath);
			const char *fileName = pData;
			fullFileName.append(fileName);

			//toon�����H
			assert(strcmp(fileName, "toon0.bmp") != 0);
			assert(strcmp(fileName, "toon00.bmp") != 0);


			//file exist?
			namespace fs = boost::filesystem;
			const fs::path path(fullFileName);

			boost::system::error_code error;
			const bool result = fs::exists(path, error);

			//file dont exist.		
			if (!result || error) {
				//�t�@�C�������݂��Ȃ��ꍇ�A�f�t�H���g�̃t�H���_����T��
				static const char *defaultFilePath = "Data/MMDData/pmd/toonTexture/";
				fullFileName.assign(defaultFilePath);
				fullFileName.append(fileName);
				toonTextures_[i].reset(fullFileName);
			}
			else {
				toonTextures_[i].reset(fullFileName);
			}

			while (!toonTextures_[i].isReady()) {}
			pData += 100;
			GameLib::Graphics::Texture t = toonTextures_[i];
			int h = t.height();
			int w = t.width();

			unsigned int *address;
			int pitch;
			t.lock(&address, &pitch);

			//�������Q�Ƃ���
			address += (pitch / 4)*(h - 1);// + 4*(w-1);
			toonColors_[i] = *address;

			t.unlock(&address);
		}



		for (unsigned int i = 0; i < numMaterials_; ++i) {
			if (materials_[i].toonIndex_ == 0xff) {
				materials_[i].toonColor_[0] = 1.f;
				materials_[i].toonColor_[1] = 1.f;
				materials_[i].toonColor_[2] = 1.f;
				materials_[i].toonColor_[3] = 1.f;
				continue;
			}
			materials_[i].toonTexture_ = toonTextures_[materials_[i].toonIndex_];
			unsigned int c = toonColors_[materials_[i].toonIndex_];
			materials_[i].toonColor_[0] = static_cast<float>((c >> 16) & 0xff) / 255.f;
			materials_[i].toonColor_[1] = static_cast<float>((c >> 8) & 0xff) / 255.f;
			materials_[i].toonColor_[2] = static_cast<float>((c) & 0xff) / 255.f;
			materials_[i].toonColor_[3] = static_cast<float>((c >> 24) & 0xff) / 255.f;
		}

		//pData += 100 * 10;		// �t�@�C���� 100Byte * 10��



		// �����܂œǂݔ�΂�
		//-----------------------------------------------------

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
			rigidBodiesData_.reset(new PMD_RigidBody[numRigidBodies_]);
			memcpy(rigidBodiesData_.get(), pData, sizeof(PMD_RigidBody)*numRigidBodies_);

			pData += sizeof(PMD_RigidBody)*numRigidBodies_;
		}

		//-----------------------------------------------------
		// �R���X�g���C���g���擾
		numConstraints_ = *((unsigned long *)pData);
		pData += sizeof(unsigned long);

		// �R���X�g���C���g�z����쐬
		if (numConstraints_)
		{
			constraintsData_.reset(new PMD_Constraint[numConstraints_]);
			memcpy(constraintsData_.get(), pData, sizeof(PMD_Constraint)*numConstraints_);

			pData += sizeof(PMD_Constraint)*numRigidBodies_;
		}

		return true;
	}

	bool TemplatePMD::isReady() {
		if (file_) {
			//load����
			if (file_->isFinished()) {
				bool initOk = init(file_->data(), file_->size());
				assert(initOk);
				delete file_;
				file_ = 0;
				return true;
			}
			else//load�܂�
			{
				return false;
			}
		}
		else {
			return true;
		}
	}
}
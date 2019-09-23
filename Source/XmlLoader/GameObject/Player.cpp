#include"Player.h"

#include<GameLib/GameLib.h>
#include<GameLib/Framework.h>

#include"XmlLoader/Shape.h"
#include"XmlLoader/MME/Object/ModelKai.h"
#include"XmlLoader/Shape.h"
#include"GameObjectData/Status.h"
#include"GameObjectData/WeaponStatus.h"

#include<GameLib/FileIO/InFile.h>
#include<sstream>
#include"helper/BoostSerializationMacros.h"

#include"GameObject/Player.h"
#include"Input/PlayerMouse.h"

#include"Camera/FixedCamera.h"
#include"Camera/HodCamera.h"

#include"MMD/CameraAnimation.h"

#include<btBulletDynamicsCommon.h>

namespace XmlLoader { namespace GameObject {
		Player::Player()
			:mXmlFile(nullptr),
			mShape(nullptr),
			mStatus(nullptr),
			mModel(nullptr)
		{
			
		}

		Player::~Player() {
			SAFE_DELETE(mXmlFile);
			SAFE_DELETE(mModel)
			SAFE_DELETE(mShape);
			mStatus.reset();
		}

		void Player::loadXml(const std::string &filename) {
			SAFE_DELETE(mXmlFile);
			mXmlFile = new GameLib::FileIO::InFile();
			*mXmlFile = GameLib::FileIO::InFile::create(filename.c_str());
		}

		bool Player::isReady() {
			if (mXmlFile) {
				if (mXmlFile->isFinished()) {
					SAFE_DELETE(mShape);
					mStatus.reset();
					SAFE_DELETE(mModel);

					mShape = new Shape();
					mStatus = std::make_shared<GameObjectData::Status>();
					mModel = new MME::Object::ModelKai();

					std::istringstream in_file(
						std::string(mXmlFile->data(), mXmlFile->size()),
						std::ios::binary || std::ios::in);
					boost::archive::xml_iarchive in_archive(in_file, boost::archive::no_header);
					in_archive >> boost::serialization::make_nvp("Player", *this);
					SAFE_DELETE(mXmlFile);//�����g��Ȃ��̂Ń������J��
					return true;
				}
				else {
					return false;
				}
			}
			else {
				return mShape && mModel &&mModel->isReady();
			}
			return false;
		}

		::GameObject::PlayerSharedPtr Player::instance() {
			assert(isReady() && "�����������Ă���ǂݏo������");
			while (!isReady()) {}

			std::shared_ptr<btCylinderShape> cylinder = std::dynamic_pointer_cast<btCylinderShape>(mShape->instanceBtCollisionShape().first);
			assert(cylinder);

			//rigidbody�̍쐬
			// �����e���\���̌v�Z
			//��]���Ȃ�
			btVector3 btv3LocalInertia(0.f, 0.f, 0.f);
			//mShape->calculateLocalInertia( mMass, btv3LocalInertia );

			//static const btVector3 kDefalutInitPos( 0.f, 20.f, 0.f );
			static const btVector3 kDefalutInitPos(-80.f, 50.f, 0.f);
			
			// MotionState���쐬����B���̂̎p��������������	
			std::shared_ptr<btDefaultMotionState> pMotionState( 
				new btDefaultMotionState(btTransform(btMatrix3x3::getIdentity(), kDefalutInitPos)));
			//�Ȃ�ƂȂ��o�O�̌������ȂƎv�����̂Ŏ��ۂ͕ς��Ȃ�
			//���ǃo���b�g�֘A��makeshard�͐�΂�߂��ق�������
			//std::make_shared<btDefaultMotionState>(btTransform(btMatrix3x3::getIdentity(), kDefalutInitPos));
			
			// ���̂��쐬����
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mMass, pMotionState.get(), cylinder.get(), btv3LocalInertia);
			//mRigidBody = std::make_shared<btRigidBody>( rbInfo );//bug��܂�,�����͂��Ԃ�align
			std::unique_ptr<btRigidBody> pRigidBody(new btRigidBody(rbInfo));
			
			//Mouse�̍쐬
			std::shared_ptr<Input::PlayerMouse> pMouse =
				std::make_shared<Input::PlayerMouse>(pRigidBody.get(), mHeight*0.5f);

			
			//FixedCamera�̍쐬
			//GameLib::Framework framework;
			//auto pCamera = std::make_shared<::Camera::FixedCamera>(
			//	D3DXVECTOR3(0.f, 70.f, 40.f),
			//	D3DXVECTOR3(0.f, 55.f, 0.f),
			//	D3DXVECTOR3(0.f, 1.f, 0.f),
			//	45.f,
			//	0.1f,
			//	30000.f);
			//std::shared_ptr<::Camera::ICamera> pICamera =
			//	std::dynamic_pointer_cast<::Camera::ICamera, ::Camera::FixedCamera>(pCamera);
			//
			//assert(pICamera);


			auto model = mModel->instance();

			//�������GameObject����鎞������g��Ȃ��ق��������Ǝv��
			//�����Ƀ��[�h������ق�����������
			//����̃����b�g�̓v���C���[�Ɋւ��Ĉꂩ���ɂ܂Ƃ߂���Ƃ�������
			//�m���ɂ��̃����b�g�̓A�j�������蔻��X�e�[�^�X���ꂩ���ɂ܂Ƃ߂���
			//�ꂩ���ɂ���Β������₷����
			//�d�l����ł͉���ł���C������
			//���������X�e�[�^�X���ǂ��g�������܂��ĂȂ��̂�����
			
			typedef ::GameObject::Player InstancePlayer;
			return InstancePlayer::create(
				mHeight,
				model,
				cylinder,
				pMotionState,
				std::move(pRigidBody),
				nullptr,
				pMouse,
				mStatus->clone() );
		}



		template<class Archive>
		void Player::serialize(Archive& ar, const unsigned int version) {
			using boost::serialization::make_nvp;
			ar & make_nvp("Name", mName);
			ar & make_nvp("Height", mHeight);
			ar & make_nvp("Mass", mMass);
			ar & make_nvp("Shape", *mShape);
			ar & make_nvp("Status", *mStatus);
			ar & make_nvp("ModelKai", *mModel);
		}

		EXPLICITY_INSTANTIATE_SERIALIZE_FUNCTION(Player);

} }//end XmlLoader::GameObject

BOOST_CLASS_IMPLEMENTATION(XmlLoader::GameObject::Player, boost::serialization::object_serializable);
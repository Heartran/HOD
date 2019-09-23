#pragma once
#include<string>
#include<memory>
#include<boost/serialization/serialization.hpp>

namespace GameLib {
	namespace FileIO {
		class InFile;
	}
}

namespace GameObject {
	namespace Utils {
		class Character;
		typedef std::shared_ptr<Character> CharacterSharedPtr;
	}
}

namespace XmlLoader{
	namespace MME {
		namespace Object {
			class ModelKai;
		}
	}

	class Shape;

namespace GameObject {
	namespace Utils {
		//�e�L�����ɑΉ������l�X�Ȑݒ��ێ�����N���X
		//�l�ԂłȂ��ꍇ�͂Ƃ��A�ق��ɒǉ��p�����[�^���������ꍇ�����肻��
		class Character {
		private:
			typedef ::GameObject::Utils::CharacterSharedPtr InstancedCharacterSharedPtr;
		public:
			Character();
			~Character();

			InstancedCharacterSharedPtr instance();

			void loadStart(const std::string  &filenam);

			bool isReady();

		private:
			//disallow
			Character(const Character &) = delete;
			//disallow
			void operator=(const Character &) = delete;

			friend boost::serialization::access;
			//���N���X��selialize, save, load�֐��𒼐ڌĂяo���Ȃ����ƁI�I
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version);


			GameLib::FileIO::InFile *mFile;

			std::string mName;
			float mHeight;
			float mMass;
			Shape *mShape;
			MME::Object::ModelKai *mModel;

		};

}}}//end XmlLoader::GameObject::Utils
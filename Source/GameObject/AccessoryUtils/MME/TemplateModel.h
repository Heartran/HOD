#pragma once
#include<vector>
#include<GameLib/Graphics/VertexBuffer.h>
#include<GameLib/Graphics/IndexBuffer.h>
#include<memory>

namespace XFile{
	class Model;
}

struct D3DXMATRIX;

namespace GameLib{
	namespace Scene {
		class Batch;
	}
	namespace Math {
		class Matrix34;
	}
}

class btTransform;

namespace GameObject {
	class Accessory;
namespace AccessoryUtils {
namespace MME {

class Model;
//XFile::Model??�����~���������Ȃ񂾂�???!!!����
//XFile::Model�̃��b�p�[�Ȃ񂾂Ǝv�����������C��������
//std::shared_ptr<XFile::Model>���ق����̂ł��̃N���X���쐬����
//XFile::Model*���ƊǗ����ɂ������A���L���ɂ����̂�
//XFile::Model + scale = GameObject::AccessoryUtils::MME::Model
class TemplateModel {
public:
	explicit TemplateModel(const std::string &xFileName );
	virtual ~TemplateModel();

	bool isFinished()const;
	bool isError()const;

	GameLib::Graphics::VertexBuffer getVertexBuffer()const;
	GameLib::Graphics::IndexBuffer getIndexBuffer()const;

	const std::string& getModelFileName()const;

	std::shared_ptr<Model> createModel(float scale)const;

private:
	void operator=( const TemplateModel & );
	TemplateModel( const TemplateModel & );
	std::shared_ptr<XFile::Model> mModel;
};
}//end namespace MME
}//end namespace AccessoryUtils
}//end namespace GameObject

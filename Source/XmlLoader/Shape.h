#pragma once
#include<string>
#include<memory>
namespace boost{
	namespace serialization{
		class access;
	}
}

namespace GameLib{
	namespace PseudoXml{
		class ConstElement;
	}
}
class btCollisionShape;
typedef std::shared_ptr<btCollisionShape> BtCollisionShapeSharedPtr;
class btTriangleMesh;
typedef std::shared_ptr<btTriangleMesh> BtTriangleMeshSharedPtr;
//if btCollisionShape don't use btTriangleMesh, pair.second returns NULL. 
//���̌^��btTiangleMesh�̍폜�̐ӔC�𑼂̃N���X�ɈϏ����邽�߂ɕK�v
typedef std::pair<BtCollisionShapeSharedPtr, BtTriangleMeshSharedPtr> BtCollisionShapeAndBtTriangleMesh; 

namespace XmlLoader {
class Shape;
typedef std::shared_ptr<Shape> ShapeSharedPtr;

class Shape {
public:
	//make_shared��protected�R���X�g���N�^�ĂԂ̂ɕK�v
	//friend class std::_Ref_count_obj<Shape>;
	//static ShapeSharedPtr create(
	//	const GameLib::PseudoXml::ConstElement &shapeElement,
	//	const std::string ModelFileName );//meshShape����邽�߂ɗ~����
	//
	//static ShapeSharedPtr create( const std::string modelFileName="" );//meshShape����邽�߂ɗ~����
	Shape(){}

	virtual ~Shape(){}

	bool isReady();

	//trimesh���g��Ȃ��ꍇ��NULL��Ԃ�
	BtCollisionShapeAndBtTriangleMesh instanceBtCollisionShape();

	//meshShape����邽�߂ɗ~����
	void setModelFile( const std::string & file ){
		mModelFile=file;
	}
protected:
	Shape(
		const std::string &type,
		const std::string &arguments,
		const std::string &file )
		:mType(type),
		mArguments(arguments),
		mModelFile(file)
	{}
private:	
	//disallow
	Shape(const Shape& )=delete;
	//disallow
	void operator=(const Shape& )=delete;

	friend boost::serialization::access;		
	//���N���X��serialize�֐��𒼐ڌĂяo���Ȃ����ƁI�I
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int ver) {
		ar & boost::serialization::make_nvp("type", mType );
		ar & boost::serialization::make_nvp("arguments", mArguments );
	}

	std::string mType;
	//<arguments>"
	//	halfExtentX=1.0;
	//	halfExtentY=1.0;
	//	halfExtentZ=1.0;
	//	radius=0.0;
	//"</arguments>
	std::string mArguments;
	std::string mModelFile;

};

}//end namespace XmlLoader
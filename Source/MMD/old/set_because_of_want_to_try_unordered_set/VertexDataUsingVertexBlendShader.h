#ifndef __VERTEXDATA_USING_VERTEX_BLEND_SHADER_H__
#define __VERTEXDATA_USING_VERTEX_BLEND_SHADER_H__
#include <d3dx9.h>
#include <vector>
#include <map>
#include <assert.h>
#include "Model.h"
#include "MMDMacro.h"
#include <set>

namespace GameLib{
	namespace Graphics{
		class VertexBuffer;
		class IndexBuffer;
		class AnimatedVertexBoneBuffer;
	}
}

class CFace;
class CBone;

//�ǂ����Ȃ�n�[�t�G�b�W�\�����g�ݍ���ł����ׂ����������H
//�~�����Ȃ�������
class VertexDataUsingVertexBlendShader {
public:
	enum{ SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE = 32, };//device�ɑ�����ő�̔z��
	enum{ MODEL_BLEND_COUNT = 4, };
	VertexDataUsingVertexBlendShader();
	~VertexDataUsingVertexBlendShader();
	const char* load(const char *pData);
	void draw(
		const CBone *bones, const unsigned short &boneSize,
		const CModel::Subset *subsets, const unsigned long &subsetSize,
		const CModel::Material *materials, const unsigned long &materialSize );

	void drawByMME(
		const CBone *bones, const unsigned short &boneSize,
		const CModel::Subset *subsets, const unsigned long &subsetSize,
		const CModel::Material *materials, const unsigned long &materialSize );

	void updateFaceVertex(const CFace *faces, const short &size );

	//indexBuffer���œK������B
	//���_�u�����h�����邽�߂�IndexBuffer���œK������
	//void sortIndexBuffer();
	void init(
		const CModel::Subset *subsets,	const unsigned long &subsetSize,
		CFace* faces, const unsigned long &faceSize );

	//static void initStaticObject();
	//static void releaseStaticObject();
	
	//short��int��char�ǂ�ɂ��邩�������̂�
	typedef unsigned char TypeBoneIndex;

private:
	DISALLOW_COPY_AND_ASSIGN(VertexDataUsingVertexBlendShader);
	
	//�O���[�v���Ƃɒ��_���X�V���邽�߂Ɏg�p����WorldMatrixTable���쐬���ăV�F�[�_�ɑ���
	void sendMatrixArrayToShader( 
		const CBone *bones,
		unsigned int subsetIndex,
		unsigned int groupIndex,
		unsigned int startIndexBufferIdx );

	//MME�͍�����W
	void VertexDataUsingVertexBlendShader::sendMatrixArrayToMMEShader(
		const CBone *bones,
		unsigned int subsetIndex,
		unsigned int groupIndex,
		unsigned int startIndexBufferIdx );


	GameLib::Graphics::VertexBuffer *vertexBuffer_;
	GameLib::Graphics::AnimatedVertexBoneBuffer* vbBone_;
	unsigned long numVertices_;
	GameLib::Graphics::IndexBuffer *indexBuffer_;
	unsigned long numIndices_;


	//�N���X�ɂ���ׂ����� PrimitevePart? ModelPart
	//���I�Ƀp�[�c������N���X
	GameLib::Graphics::VertexBuffer **vbOfGroupInSubset_;
	GameLib::Graphics::IndexBuffer **ibOfGroupInSubset_;
	GameLib::Graphics::AnimatedVertexBoneBuffer **vbBoneOfGroupInSubset_;
	//std::vector<IDirect3DVertexBuffer9*> *vbBoneOfGroupInSubset_;

	//subsets��index�Ƌ���
	//�{����CModel::Subset�Ɏ�������ق������R
	std::map<unsigned short, unsigned short> *usingBoneIndicesMap_;//key boneIndex, value boneindexInShader

	struct Vertex;
	struct VertexBone;
	class BoneIndex{
	public:
		explicit BoneIndex( const TypeBoneIndex bi_array[] ){
			for( int i=0; i<MODEL_BLEND_COUNT; ++i ){
				value[i]=bi_array[i];
			}
		}
		BoneIndex( const BoneIndex & bi ) {
			for( int i=0; i<MODEL_BLEND_COUNT; ++i ){
				value[i]=bi.value[i];
			}
		}
		void operator=( const BoneIndex & bi ) {
			for( int i=0; i<MODEL_BLEND_COUNT; ++i ){
				value[i]=bi.value[i];
			}
		}
		bool operator==( const BoneIndex & bi ) const {
			for( int i=0; i<MODEL_BLEND_COUNT; ++i ){
				if(value[i]!=bi.value[i]){
					return false;
				}
			}
			return true;
		}
		TypeBoneIndex operator[](int i) const {
			assert( i<MODEL_BLEND_COUNT );
			assert( i>=0 );
			return value[i];
		}
	private:
		TypeBoneIndex value[MODEL_BLEND_COUNT];
	};
	//�{����boneIndex��ۑ����邽�߂̔z��
	std::vector<BoneIndex> originalVertexBufferBone_;


	//subset(�ގ�)�̐�
	unsigned int numSubsets_;

	//subset i�ɂ�����O���[�v�̐�
	//subsets[i]�ɂ́@numGroups_[i]�O���[�v������B
	unsigned int *numGroups_;
	//subset i�ɂ�����O���[�v���̃C���f�b�N�X�̐�
	//numIndeicesOfGroupInSubset[i][j]�C���f�b�N�X������B
	//�ǂꂾ���`���΂������m��Ɏg��
	unsigned int **numIndeicesOfGroupInSubset_;

	
	//typedef std::set<unsigned short> BoneIndexPattern;
	//subset i �ɂ�����O���[�v���̎O�p�`�P�ʂł̎g���Ă���{�[���C���f�b�N�X�̃p�^�[��
	//for( int j=0; j<numGroups[i]; j++ ){
	//	BoneIndexPattern	it  = usingBoneIndexPatternOfGroupInSubset_[i][j].begin(),
	//						end = usingBoneIndexPatternOfGroupInSubset_[i][j].end;
	//}
	//set�ł���K�v�Ȃ��R�s�[�ƃ��j�[�N���ǂ������ׂ�̂߂�ǂ���������set�g���Ă�
	std::vector<TypeBoneIndex>** usingBoneIndexPatternOfGroupInSubset_;

	
	//static ID3DXEffect *sEffect;
	//static IDirect3DVertexDeclaration9 *sVertexDeclaration;
	//static IDirect3DVertexShader9 *sVertexShader;
	//static IDirect3DPixelShader9 *sPixelShader;


};


#endif//__VERTEXDATA_USING_VERTEX_BLEND_SHADER_H__
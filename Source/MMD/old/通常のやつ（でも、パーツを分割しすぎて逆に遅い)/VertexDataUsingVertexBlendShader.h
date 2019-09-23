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
	}
}

class CFace;
class CBone;

class VertexDataUsingVertexBlendShader {
public:
	enum{ MODEL_BLEND_COUNT = 4, };
	VertexDataUsingVertexBlendShader();
	~VertexDataUsingVertexBlendShader();
	const char* load(const char *pData);
	void draw( 
		const std::vector<CBone*> &bones,
		const std::vector<CModel::Subset*> &subsets,
		const std::vector<CModel::Material*> &materials );

	void update(const std::vector<CBone*> &bones);	
	void updateFaceVertex(const std::vector<CFace*> &faces);

	//indexBuffer���œK������B
	//���_�u�����h�����邽�߂�IndexBuffer���œK������
	//void sortIndexBuffer();
	void init( const std::vector<CModel::Subset*> &subsets );

	static void initStaticObject();
	static void releaseStaticObject();
	
	//short��int��char�ǂ�ɂ��邩�������̂�
	typedef unsigned char TypeBoneIndex;

private:
	DISALLOW_COPY_AND_ASSIGN(VertexDataUsingVertexBlendShader);
	//�T�u�Z�b�g���Ƃ�Vertex���X�V���邽�߂ɂ��悤
	//indexInShader���X�V����B
	void sendMatrixArrayToShader( 
		const std::vector<CBone*> &bones,
		int subsetIndex,
		int groupIndex,
		int startIndexBufferIdx );

	GameLib::Graphics::VertexBuffer *vertexBuffer_;
	IDirect3DVertexBuffer9* vbBone_;
	unsigned long numVertices_;
	GameLib::Graphics::IndexBuffer *indexBuffer_;
	unsigned long numIndices_;

	GameLib::Graphics::VertexBuffer **vbOfGroupInSubset_;
	GameLib::Graphics::IndexBuffer **ibOfGroupInSubset_;
	std::vector<IDirect3DVertexBuffer9*> *vbBoneOfGroupInSubset_;



	//subsets��index�Ƌ���
	//�{����CModel::Subset�Ɏ�������ق������R
	std::map<unsigned short, unsigned short> *usingBoneIndicesMap_;//key boneIndex, value boneindexInShader
	enum{ MATRIX_ARRAY_MAX_SIZE = MODEL_BLEND_COUNT*3, };

	struct VERTEX;
	struct VERTEX_BONE;
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
	int numSubsets_;

	//subset i�ɂ�����O���[�v�̐�
	//subsets[i]�ɂ́@numGroups_[i]�O���[�v������B
	int *numGroups_;
	//subset i�ɂ�����O���[�v���̃C���f�b�N�X�̐�
	//numIndeicesOfGroupInSubset[i][j]�C���f�b�N�X������B
	//�ǂꂾ���`���΂������m��Ɏg��
	int **numIndeicesOfGroupInSubset_;


	//typedef std::set<unsigned short> BoneIndexPattern;
	//subset i �ɂ�����O���[�v���̎O�p�`�P�ʂł̎g���Ă���{�[���C���f�b�N�X�̃p�^�[��
	//for( int j=0; j<numGroups[i]; j++ ){
	//	BoneIndexPattern	it  = usingBoneIndexPatternOfGroupInSubset_[i][j].begin(),
	//						end = usingBoneIndexPatternOfGroupInSubset_[i][j].end;
	//}
	std::set<TypeBoneIndex>** usingBoneIndexPatternOfGroupInSubset_;

	//
	static ID3DXEffect *sEffect;
	static IDirect3DVertexDeclaration9 *sVertexDeclaration;
	static IDirect3DVertexShader9 *sVertexShader;
	static IDirect3DPixelShader9 *sPixelShader;


};


#endif//__VERTEXDATA_USING_VERTEX_BLEND_SHADER_H__
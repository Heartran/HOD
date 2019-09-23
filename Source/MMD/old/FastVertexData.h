#ifndef __FAST_VERTEX_DATA_H__
#define __FAST_VERTEX_DATA_H__
#include <d3dx9.h>
#include <vector>
#include "Model.h"
#include "MMDMacro.h"

class CFace;
class CBone;

class CFastVertexData {
public:
	enum{ MODEL_BLEND_COUNT = 4, };
	CFastVertexData();
	~CFastVertexData();
	const char* load(const char *pData);
	void draw( const std::vector<CModel::Subset*> &subsets, const std::vector<CModel::Material*> &materials );
	void updateFaceVertex(const std::vector<CFace*> &faces);
	void update(const std::vector<CBone*> &bones);

private:
	DISALLOW_COPY_AND_ASSIGN(CFastVertexData);

	struct ORIGINAL_VERTEX;

	IDirect3DVertexBuffer9* originalVertexBuffer_;//�I���W�i���̒��_�f�[�^�B�����ʒu�B�ҏW���Ȃ��B
	IDirect3DVertexBuffer9* animatedVertexBuffer_;//�v�Z�p�̃f�[�^�B�A�j���[�V��������Ƃ��ۑ����Ă����B
	unsigned long vertexCount_;
	IDirect3DIndexBuffer9* indexBuffer_;
	unsigned long numIndices_;
};


#endif//__FAST_VERTEX_DATA_H__
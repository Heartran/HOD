#ifndef __FACE_H__
#define __FACE_H__

#include <vector>
#include <d3dx9.h>
#include <set>
#include <assert.h>
#include "MMDMacro.h"

namespace MMD {
	class Face;
	class Animation;
	class VertexData;
	class VertexDataUsingVertexBlendShader;
	class FaceKey;

	class CFaceVertex
	{
	public:
		CFaceVertex()
			:index_(), vec3_(D3DXVECTOR3(0.f, 0.f, 0.f)), replacedIndices_(NULL), numReplacedIndices_(0)
		{}
		explicit CFaceVertex(const DWORD &index, const D3DXVECTOR3 &vec3)
			:index_(index), vec3_(vec3), replacedIndices_(NULL), numReplacedIndices_(0)
		{};
		virtual ~CFaceVertex()
		{
			SAFE_DELETE_ARRAY(replacedIndices_);
		};
		//vertexBlend�̂��߂̊g��
		void initVertexBlendIndices(const std::set<unsigned long> &indices) {
			numReplacedIndices_ = indices.size();
			assert(numReplacedIndices_ != 0);
			replacedIndices_ = new unsigned long[numReplacedIndices_];
			int i = 0;
			for (
				std::set<unsigned long>::iterator it = indices.begin(), end = indices.end();
				it != end;
				++it, ++i
				)
			{
				replacedIndices_[i] = *it;
			}
		}

		//vertexBlend�̂��߂̊g��
		void initVertexBlendIndices(const std::vector<unsigned long> &indices) {
			numReplacedIndices_ = indices.size();
			assert(numReplacedIndices_ != 0);
			replacedIndices_ = new unsigned long[numReplacedIndices_];
			int i = 0;
			for (
				std::vector<unsigned long>::const_iterator it = indices.cbegin(), end = indices.cend();
				it != end;
				++it, ++i
				)
			{
				replacedIndices_[i] = *it;
			}
		}

	private:
		DISALLOW_COPY_AND_ASSIGN(CFaceVertex);
		void init(const DWORD &index, const D3DXVECTOR3 &vec3)
		{
			index_ = index;
			vec3_ = vec3;
		};
		DWORD index_;
		D3DXVECTOR3 vec3_;
		//vertexBlend�̂��߂Ɋg��
		DWORD *replacedIndices_;
		int numReplacedIndices_;
		friend class Face;
		friend class VertexData;
		friend class CFastVertexData;
		friend class VertexDataUsingVertexBlendShader;
	};

	class Face
	{
	public:
		explicit Face();
		virtual ~Face();

		const char* load(const char *pData);

		const char* getName() const { return name_; }
		//���΍��W���΍��W�ɕϊ�
		//���΃C���f�b�N�X����΃C���f�b�N�X�ɕϊ�
		void initalize(const Face &base);
	private:
		DISALLOW_COPY_AND_ASSIGN(Face);

	private:
		char name_[21];		// �\� (0x00 �I�[�C�]���� 0xFD)

		unsigned long numVertices_;	// �\��_��
		unsigned char type_;			// ����

		float weight_;
		std::vector<CFaceVertex*> vertices_;	// �\��_�f�[�^
		friend class Animation;
		friend class VertexData;
		friend class CFastVertexData;
		friend class VertexDataUsingVertexBlendShader;
		friend class FaceKey;
	};
}
#endif //__FACE_H__
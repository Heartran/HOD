#ifndef __BONE_H__
#define __BONE_H__
#include<vector>
#include <d3dx9.h>
#include"MMDMacro.h"
#include<btBulletCollisionCommon.h>

namespace MMD {
	struct PMD_BONE;
	class Animation;
	class VertexData;
	//class IK;
	class IK_Impl;
	class FootIK_Impl;
	class AnkleIK_Impl;
	class PMDRigidBody;
	class VertexDataUsingVertexBlendShader;

	class Bone {
	public:
		Bone();
		virtual ~Bone();

		void init(
			Bone* pBones,
			const PMD_BONE &pmd,
			const int &index);
		const char* getName()const { return name_; }
		void debug_draw();
		int getIndex()const { return index_; }
		void reset() {
			D3DXQuaternionIdentity(&qRotation_);
			pos_ = D3DXVECTOR3(0.f, 0.f, 0.f);
			D3DXMatrixIdentity(&skinningMatrix_);
		}
		//friend class �Ɓ@�Q�b�^�[�Z�b�^�[�ǂ������������H
		//friend�̂ق����͈͍͂i����B�ǂ�����ł��Ă΂��Ƃ����킯�ł͂Ȃ��B
		//�J�v�Z����
		//void updatePosQuat(pos,q);
		//getQuat();
		//getPos()//getPos�͒P����pos��Ԃ��̂ł͂Ȃ���skinngMatrix��41, 42, 43���������ق�������;
		//setQuat();
		//setPos();
		const D3DXMATRIX& getSkinningMatrix() const {
			return skinningMatrix_;
		}
		D3DXMATRIX& getSkinningMatrix() {
			return skinningMatrix_;
		}
		D3DXVECTOR3 getPos() const {
			D3DXVECTOR3 v;
			D3DXVec3TransformCoord(&v, &initPos_, &skinningMatrix_);

			return v;
		}

		void updateSkinningMatrix(const D3DXMATRIX& matParent);
		//btRigidBody* getRigidBodies(int id);
		//int getNumRigidBodies()const;
		void setPosAndQuat(const D3DXVECTOR3 &p, const D3DXQUATERNION &q) {
			pos_ = p;
			qRotation_ = q;
		}
		void saveWeightBlendMatrix();
		void exeWeightBlend( float weight);
	private:
		DISALLOW_COPY_AND_ASSIGN(Bone);
	private:
		D3DXQUATERNION qRotation_;
		//D3DXMATRIXA16 TransformMatrix_;
		//D3DXMATRIXA16 CombinedMatrix_;
		D3DXMATRIX skinningMatrix_;
		D3DXMATRIX weightBlendMatrix_;
		//D3DXMATRIXA16 InitialPosMatrix_;//�{�[���̏����ʒu

		D3DXVECTOR3 initPos_;
		D3DXVECTOR3 pos_;//�ړ������ʁB���[�J�����W�B�Z���^�[��2�ړ����Ă���Ύq��pos_��0�ł�2�ړ����Ă�BskinningMatrix�������p���ł���
		//���O����move_�̕��������Ă�

		char name_[21];
		int index_;
		std::vector<Bone*> children_;
		Bone* pParent_;

		friend class Animation;
		friend class VertexData;
		friend class CFastVertexData;
		//friend class IK;
		friend class IK_Impl;
		friend class FootIK_Impl;
		friend class AnkleIK_Impl;
		friend class PMDRigidBody;
		//friend class VertexDataUsingVertexBlendShader;
	};
}
#endif//__BONE_H__
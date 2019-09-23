#ifndef __PMD_H__
#define __PMD_H__
#include <d3dx9.h>

//�œK��(�p�b�e�B���O)��h��
#pragma pack( push, 1 )

namespace MMD {

	struct PMD_HEADER
	{
		char magic[3];//Pmd
		float version;
		char name[20]; // ���f�����D0x00 �I�[�C�]���� 0xFD �Ŗ��߂�D
		char comment[256]; // �R�����g (���쌠�\���Ȃ�)�C0x00 �I�[�C�]���� 0xFD �Ŗ��߂�D
	};
	struct DIFFUSE { float r, g, b, a; };
	struct SPECULAR { float r, g, b; };
	struct AMBIENT { float r, g, b; };


	struct PMD_VERTEX
	{
		// ���̕����� MMD2 �Ɠ����DDirect3D �� D3DVERTEX �^�D
		float x, y, z;// ���W
		float nx, ny, nz;// �P�ʖ@���x�N�g��
		float u, v;// �e�N�X�`�����W
		// ���̕����� PMD �Œǉ����ꂽ�C�W�I���g���E�u�����f�B���O�p�f�[�^�D
		// (MMD2 �ł� *.osm ���ɂ������D)
		unsigned short bone[2];// �{�[���ԍ�
		unsigned char weight;// �u�����h�̏d�� (0 �` 100��)
		unsigned char	cbEdge;			// �G�b�W�t���O
	};

	struct PMD_MATERIAL
	{
		DIFFUSE diffuse;
		float shininess;
		SPECULAR specular;
		AMBIENT ambient;
		unsigned char toonIndex;
		unsigned char edgeFlag;//�֊s�A�e
		unsigned long nEdges;// ���̍ގ��ɑΉ�����G�b�W��
		char textureFileName[20];// �e�N�X�`���t�@�C���� (MMD2 �ł� 256 �o�C�g)
	};

	struct PMD_BONE
	{
		char name[20]; // �{�[���� (0x00 �I�[�C�]���� 0xFD)
		short parent; // �e�{�[���ԍ� (�Ȃ���� -1)
		//std::vector<short> childrenIndexs;//�q������Ύq�̔z��ԍ�
		unsigned short to;
		unsigned char kind;
		unsigned short ikTarget;
		float x, y, z;
	};

	struct PMD_IK
	{
		unsigned short index;
		unsigned short target;
		unsigned char effectBoneNum;
		unsigned short calcNum;
		float weight;
		unsigned short *pEffectBoneIndeces;
	};

	//�\��̒��_�f�[�^
	struct PMD_FACE_VERTEX
	{
		unsigned long index;//�e�����钸�_�C���f�b�N�X
		float x, y, z;
	};

	struct PMD_FACE
	{
		char name[20];		// �\� (0x00 �I�[�C�]���� 0xFD)

		unsigned long nVertices;	// �\��_��
		unsigned char type;			// ����

		PMD_FACE_VERTEX* pVertices;	// �\��_�f�[�^
	};
	// ���̃f�[�^
	struct PMD_RigidBody
	{
		char			szName[20];		// ���̖�

		unsigned short	unBoneIndex;	// �֘A�{�[���ԍ�
		unsigned char	cbColGroupIndex;// �Փ˃O���[�v
		unsigned short	unColGroupMask;	// �Փ˃O���[�v�}�X�N

		unsigned char	cbShapeType;	// �`��  0:�� 1:�� 2:�J�v�Z��

		float			fWidth;			// ���a(��)
		float			fHeight;		// ����
		float			fDepth;			// ���s

		float			vec3Position[3];	// �ʒu(�{�[������)
		float			vec3Rotation[3];	// ��](radian)

		float			fMass;			// ����
		float			fLinearDamping;	// �ړ���
		float			fAngularDamping;// ��]��
		float			fRestitution;	// ������
		float			fFriction;		// ���C��

		unsigned char	cbRigidBodyType;// �^�C�v 0:Bone�Ǐ] 1:�������Z 2:�������Z(Bone�ʒu����)
	};

	// �R���X�g���C���g(�W���C���g)�f�[�^
	struct PMD_Constraint
	{
		char			szName[20];		// �R���X�g���C���g��

		unsigned long	ulRigidA;		// ����A
		unsigned long	ulRigidB;		// ����B

		float			vec3Position[3];	// �ʒu(���f�����_���S)
		float			vec3Rotation[3];	// ��](radian)

		float			vec3PosLimitL[3];	// �ړ�����1
		float			vec3PosLimitU[3];	// �ړ�����2

		float			vec3RotLimitL[3];	// ��]����1
		float			vec3RotLimitU[3];	// ��]����2

		float			vec3SpringPos[3];	// �΂ˈړ�
		float			vec3SpringRot[3];	// �΂ˉ�]
	};
}
#pragma pack( pop )

#endif
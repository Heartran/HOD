#pragma once
#include<d3dx9.h>



//�_�Ǝ�����Ƃ̏Փ˔���𒲂ׂ�N���X
class Frustrum {
public:
	Frustrum(const D3DXMATRIX &viewprojection);

	Frustrum(const Frustrum&) = delete;
	void operator=(const Frustrum&) = delete;

	//�_��������̒��ɂ��邩
	bool isInside(const D3DXVECTOR3 &point);

	//�_��������̒��ɂȂ��Ȃ�A
	//�ŋߐړ_��outClosestPoint�ɑ��
	//�܂��ł��߂����ʂ�outClosestPlane�������
	//nullptr�Ȃ牽�����Ȃ�
	bool calculateClosestPoint(
		D3DXVECTOR3 *outClosestPoint,
		D3DXPLANE *outClosestPlane,
		const D3DXVECTOR3 &point);


	//���Lindex���e���ʂɑΉ�
	//	LEFT=0,
	//	RIGHT=1,
	//	UP=2,
	//	DOWN=3,
	//	NEAR=4,
	//	FAR=5
	//
	D3DXPLANE mPlane[6];
};


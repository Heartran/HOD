#include "Frustrum.h"
#include<cmath>
#include<assert.h>
#include<limits>

namespace {
	//�ʂƓ_�Ƃ̒���
	//�ʂ̌����ɉ����Đ�Βl���Ƃ�Ȃ���΂Ȃ�Ȃ����Ƃ��Ă��Ȃ�
	//4�̕��ʂ���̍ŒZ���������߂����̂�
	//t�����߂�
	float calcT(const D3DXPLANE &plane, const D3DXVECTOR3 &point) {
		float t = plane.a * point.x + plane.b * point.y + plane.c * point.z + plane.d;
		t /= std::powf(plane.a * plane.a + plane.b * plane.b + plane.c * plane.c, 0.5f);
		return t;
	}
}

Frustrum::Frustrum(const D3DXMATRIX & viewprojection)
{


	const D3DXMATRIX &m = viewprojection;

	mPlane[0] = D3DXPLANE(m._14 + m._11, m._24 + m._21, m._34 + m._31, m._44 + m._41);
	mPlane[1] = D3DXPLANE(m._14 - m._11, m._24 - m._21, m._34 - m._31, m._44 - m._41);
	mPlane[2] = D3DXPLANE(m._14 + m._12, m._24 + m._22, m._34 + m._32, m._44 + m._42);
	mPlane[3] = D3DXPLANE(m._14 - m._12, m._24 - m._22, m._34 - m._32, m._44 - m._42);
	mPlane[4] = D3DXPLANE(m._14 + m._13, m._24 + m._23, m._34 + m._33, m._44 + m._43);
	mPlane[5] = D3DXPLANE(m._14 - m._13, m._24 - m._23, m._34 - m._33, m._44 - m._43);


	D3DXPlaneNormalize(&mPlane[0], &mPlane[0]);
	D3DXPlaneNormalize(&mPlane[1], &mPlane[1]);
	D3DXPlaneNormalize(&mPlane[2], &mPlane[2]);
	D3DXPlaneNormalize(&mPlane[3], &mPlane[3]);
	D3DXPlaneNormalize(&mPlane[4], &mPlane[4]);
	D3DXPlaneNormalize(&mPlane[5], &mPlane[5]);


}

bool Frustrum::isInside(const D3DXVECTOR3 & point)
{
	//�_�ƕ��ʂ̋���
	//�_�Ƃ��ׂĂ̕��ʂɂ��āA�ʂ���_�ւ̒��������ʂ̌����Ɠ��������ł���Ύ�����̒��ɂ���
	//�����̒��ɂȂ���΍ŒZ�̓_�����߂�
	float t[6] = { 0.f };

	t[0] = calcT(mPlane[0], point);
	t[1] = calcT(mPlane[1], point);
	t[2] = calcT(mPlane[2], point);
	t[3] = calcT(mPlane[3], point);
	t[4] = calcT(mPlane[4], point);
	t[5] = calcT(mPlane[5], point);


	//���ׂăv���X�Ȃ王����̒��ɂ���
	if (
		t[0] >= 0.f &&
		t[1] >= 0.f &&
		t[2] >= 0.f &&
		t[3] >= 0.f &&
		t[4] >= 0.f &&
		t[5] >= 0.f)
	{
		return true;
	}
	return false;
}

bool Frustrum::calculateClosestPoint(D3DXVECTOR3 * outClosestPoint, D3DXPLANE * outClosestPlane, const D3DXVECTOR3 & point)
{
	//�_�ƕ��ʂ̋���
	//�_�Ƃ��ׂĂ̕��ʂɂ��āA�ʂ���_�ւ̒��������ʂ̌����Ɠ��������ł���Ύ�����̒��ɂ���
	//�����̒��ɂȂ���΍ŒZ�̓_�����߂�
	float t[6] = { 0.f };

	t[0] = calcT(mPlane[0], point);
	t[1] = calcT(mPlane[1], point);
	t[2] = calcT(mPlane[2], point);
	t[3] = calcT(mPlane[3], point);
	t[4] = calcT(mPlane[4], point);
	t[5] = calcT(mPlane[5], point);


	//���ׂăv���X�Ȃ王����̒��ɂ���
	if (
		t[0] >= 0.f &&
		t[1] >= 0.f &&
		t[2] >= 0.f &&
		t[3] >= 0.f &&
		t[4] >= 0.f &&
		t[5] >= 0.f)
	{
		return true;
	}
	else {
		//0�����Ő�Βl���ł����������̂�I��
		int index = -1;
		float d = std::numeric_limits<float>::max();
		for (int i = 0; i < 6; ++i) {
			if (
				t[i] < 0.f &&
				abs(t[i]) < d)
			{
				d = abs(t[i]);
				index = i;
			}
		}
		
		const auto &pl = mPlane[index];
		float tt = (pl.a*point.x + pl.b*point.y + pl.c*point.z + pl.d) / (pl.a*pl.a + pl.b*pl.b + pl.c*pl.c);
		if (outClosestPoint) {
			outClosestPoint->x = point.x - tt*pl.a;
			outClosestPoint->y = point.y - tt*pl.b;
			outClosestPoint->z = point.z - tt*pl.c;
		}

		if (outClosestPlane) {
			*outClosestPlane = pl;
		}

		return false;

	}
}


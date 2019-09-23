#pragma once
#include<d3dx9.h>
#include<memory>

namespace MMD {

	class BezierCurve;

	class CameraOneFrameData {
	public:
		friend class CameraAnimation;
		CameraOneFrameData(
			float frame,
			float distance,
			const D3DXVECTOR3 &pos,
			const D3DXQUATERNION &q,
			float viewAngle,
			bool parth,
			const std::shared_ptr< BezierCurve > &ix,
			const std::shared_ptr< BezierCurve > &iy,
			const std::shared_ptr< BezierCurve > &iz,
			const std::shared_ptr< BezierCurve > &iq,
			const std::shared_ptr< BezierCurve > &id,
			const std::shared_ptr< BezierCurve > &iv
		);
		~CameraOneFrameData();

		bool operator<(const CameraOneFrameData & right)const;
		bool operator<(const float & rightFrame)const;
	private:
		float frame_;			// �t���[���ԍ�
		float distance_; 	// �ڕW�_�ƃJ�����̋���(�ڕW�_���J�����O�ʂŃ}�C�i�X)
		D3DXVECTOR3 pos_;
		D3DXQUATERNION rot_;
		std::shared_ptr< BezierCurve > interpolationX_;
		std::shared_ptr< BezierCurve > interpolationY_;
		std::shared_ptr< BezierCurve > interpolationZ_;
		std::shared_ptr< BezierCurve > interpolationQ_;
		std::shared_ptr< BezierCurve > interpolationD_;//Distance(����)�̕⊮
		std::shared_ptr< BezierCurve > interpolationV_;//����p(viewAngle)�̕⊮
		float viewAngle_; // ����p(deg)
		bool parth_; // �p�[�X�y�N�e�B�u, 0:ON, 1:OFF//�������ĂȂ�
	};
}



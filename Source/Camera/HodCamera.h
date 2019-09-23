#include<d3dx9.h>
#include<MMD/Fwd.h>
#include"ICamera.h"

namespace Camera {
	//Hatune Of the Dead�p�J����
	//�܂����O���ʂɃA�j���[�V�����J�����ł������
	class HodCamera : public ICamera
	{
	public:
		HodCamera(MMD::CameraAnimationSharedPtr camera);
		~HodCamera();
		void update(float dt);
		//������nullptr�Ȃ牽�����Ȃ�
		void getCameraParameter(
			D3DXVECTOR3	*pCameraPos,
			D3DXVECTOR3	*pLookAt,
			D3DXVECTOR3 *pUp,
			float		*pViewAngle)const;
		float getTime()const;
		const D3DXMATRIX& getViewProj()const;
		void setTime(float time);
		bool isAnimeOver()const;
		bool isReady();

	private:
		void sendParamToShader();
		MMD::CameraAnimationSharedPtr mCameraAnim;
		float		mTime;
		D3DXVECTOR3	mCameraPos;
		D3DXVECTOR3	mLookAt;
		D3DXVECTOR3 mUp;
		float		mViewAngle;
		D3DXMATRIX	mViewProj;
	};

}//namespace Camera


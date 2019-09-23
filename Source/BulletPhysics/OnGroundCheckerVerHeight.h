#pragma once
#include"BulletPhysics/Manager.h"

class btRigidBody;

namespace BulletPhysics {


	
	//10�t���[���Ԉʒu���̍���y��ێ����A�ڒn���������N���X
	//��_ ��
	//�}�C�t���[��update()���邱��
	class OnGroundCheckerVerHeight {
	public:
		OnGroundCheckerVerHeight(
			btRigidBody* rb,
			short filterGroup = BulletPhysics::Manager::DefaultFilter,
			short filterMask = BulletPhysics::Manager::StaticFilter);
		virtual ~OnGroundCheckerVerHeight();
		void update();
		bool isOnGround()const;
	private:
		//disallow assign
		OnGroundCheckerVerHeight(const OnGroundCheckerVerHeight &);
		//disallow copy
		void operator =(const OnGroundCheckerVerHeight &);

		enum {
			kArraySize = 10,
		};
		
		btRigidBody *mRigidbody;
		float mHeights[kArraySize];
		const short mGroup;
		const short mMask;

		bool mIsOnGround;
	};


}
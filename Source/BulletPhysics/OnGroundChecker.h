#pragma once
#include"BulletPhysics/Manager.h"

class btRigidBody;

namespace BulletPhysics {


	//�ڒn��Ԃ�ێ�����N���X
	//������Փ˔�������Ȃ��悤�ɂ��邽�߂̃N���X
	//���t���[��update()���邱��
	class OnGroundChecker {
	public:
		OnGroundChecker(
			btRigidBody* rb,
			short filterGroup = BulletPhysics::DefaultFilter,
			short filterMask = BulletPhysics::StaticFilter);
		virtual ~OnGroundChecker();
		void update();
		bool isOnGround()const;
	private:
		//disallow assign
		OnGroundChecker(const OnGroundChecker &) = delete;
		//disallow copy
		void operator =(const OnGroundChecker &) = delete;

		
		btRigidBody *mRigidbody;
		const short mGroup;
		const short mMask;

		bool mIsOnGround;
	};


}
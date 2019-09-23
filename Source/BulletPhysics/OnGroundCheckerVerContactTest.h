#pragma once
#include"BulletPhysics/Manager.h"

class btRigidBody;

namespace BulletPhysics {


	//���[��A����_�����ۂ�
	//�ڒn�����contactTest���g���Ă���̂����A
	//�ǂ����Ă��M���M���̏��Őڒn���ĂȂ��Ɣ��肷��悤��(���Ԃ�A�����G���W���ɂƂ��Ă͂��̂ق����s�����悢�̂��낤)
	//����contactTest���g�����@�͂���

	//10�t���[���Ԑڒn�����ێ����A�ڒn���������N���X
	//�ڒn���Ă��kFilterSize�ȏ�ڒn���Ă���ΐݒu���Ă���Ƃ݂Ȃ�
	//�}�C�t���[��update()���邱��
	class OnGroundCheckerVerContactTest {
	public:
		OnGroundCheckerVerContactTest(
			btRigidBody* rb,
			short filterGroup = BulletPhysics::Manager::DefaultFilter,
			short filterMask = BulletPhysics::Manager::StaticFilter);
		virtual ~OnGroundCheckerVerContactTest();
		void update();
		bool isOnGroundAll()const;
		bool isOnGround()const;
	private:
		//disallow assign
		OnGroundCheckerVerContactTest(const OnGroundCheckerVerContactTest &);
		//disallow copy
		void operator =(const OnGroundCheckerVerContactTest &);

		enum {
			kArraySize = 10,
			kFilterSize=1
		};
		
		btRigidBody *mRigidbody;
		bool mIsOnGrounds[kArraySize];
		const short mGroup;
		const short mMask;
	};


}

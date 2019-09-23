#ifndef __OLDCHARACTERCONTOROLEROBJECT_H__
#define __OLDCHARACTERCONTOROLEROBJECT_H__
#include<btBulletDynamicsCommon.h>
namespace kztr{
	class CharacterController2;
}

class btCharacterControllerInterface;
class btKinematicCharacterController;
class btPairCachingGhostObject;
class btCollisionShape;
class btGhostPairCallback;
namespace Old {
//�J�v�Z���^�̃L�����N�^�[�𑀍삷��I�u�W�F�N�g
class CharacterControllerObject {
public:
	//�J�v�Z���`��̓����蔻������
	CharacterControllerObject(
		const btTransform &startTransform,
		float characterHeight,
		float characterWidth,
		float velocity,
		float angularVelocity ); // 2.f*3.14f * 0.01 * 30.f  2PI * (1��]/�b) * (frame/sec) 1��]��100frame������

	virtual ~CharacterControllerObject();

	void update(
		float elapsedTime,
		bool isForward,
		bool isBackforward,
		bool isLeft,
		bool isRight );

	void update(
		float elapsedTime,
		float deltaPitch,
		float deltaYaw,
		bool isDash,
		bool isJump,
		bool isAprochTarget,
		const btVector3 &targetPos );

	void update2(
		float elapsedTime,
		float deltaPitch,
		float deltaYaw,
		bool isDash,
		bool isJump,
		bool isAprochTarget,
		const btVector3 &targetPos );

	void draw()const;

	const btTransform & getWorldTransform() const;

	void warp( const btVector3 &pos );
	
	const btVector3& getVelocity() const;

	void setVelocity( const float &v ){
		mVelocity=v;
	}
	float getAngularVelocity(){
		return mAngularVelocity;
	}
	void setAngularVelocity( const float &v ){
		mAngularVelocity=v;
	}

	void setUserPtr( void *ptr );

	btCollisionObject* getBtCollisionObject();

	bool onGround() const;
	bool onGroundPerhaps() const;

	//bool checkCollisions();

	//btPairCachingGhostObject* getGhostObjPtr(){
	//	return m_ghostObject;
	//}
private:
	kztr::CharacterController2		*m_character;//btKinematicCharacterController	*m_character;
	btPairCachingGhostObject		*m_ghostObject;
	float							mVelocity;
	float							mAngularVelocity;
	float							mMaxVelocity;
	float							mMaxAngularVelocity;
	
	//time��frame�ɂ��ׂ��H
	//�N���X�ɂ����ق�������
	float							mDashVel;
	float							mDashMaxVel;
	float							mDashAccel;
	float							mDashDeltaTime;//�o�ߎ���
	float							mDashEnableTime;//�L������
	float							mDashMaxEnableTime;//�ő�L������
	float							mDashAddTime;//�������
	
	float							mAirResistance;//��C��R�W��
	float							mCoefficientOfFriction;//���C�W���@�����C�W���Ƃ��Î~���C�W�����H

	float							mJumpVel;
	float							mJumpMaxVel;
	float							mJumpAccel;
	float							mJumpDeltaTime;//�o�ߎ���
	float							mJumpEnableTime;//�L������
	float							mJumpMaxEnableTime;//�ő�L������
	float							mJumpAddTime;//�������
};

}//end namespace Old
#endif //__OLDCHARACTERCONTOROLEROBJECT_H__
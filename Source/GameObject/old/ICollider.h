#pragma once

#include"IGameObject.h"
#include<btBulletCollisionCommon.h>
#include<GameLib/Math/Matrix34.h>
#include<GameLib/Math/Vector3.h>
#include<vector>
#include<algorithm>
#include<set>
#include"BulletPhysics/Manager.h"
#include"MME/Enums.h"

class RTTI;
namespace GameLib{
	namespace Math{
		class Vector3;
		class Matrix34;
	}
}
namespace GameObject {
typedef unsigned int GameObjectId;

enum GameObjectType{
	PLAYER,
	ENEMY,
	BULLET,
	LASER,
	CHARACTER_CONTROLLER_GHOST_OBJECT,
	CHARACTER_CONTROLLER_RIGIDBODY,
	BONE_COLLISION_OBJECT,
	RIGIDBODY,
	GHOST,
	MMD,
	STAGE,
	WEAPON,
	CHARACTER,
	CHARACTER_AND_WEAPON,
	EXPLOSION,
	BARRIER,
	ACCESSORY,
	MME_Model,
};

struct ResponseInfo;
struct ResponseInfo{
	enum DamageType{
		NOTHING		= 0x0000,
		FIRE		= 0x0001,
		ICE			= 0x0002,
		BEAM		= 0x0004,
		CLOSE_RANGE	= 0x0008,
		EXPLOSION	= 0x0010,
		POISON		= 0x0020,
		PARALYSIS	= 0x0040,
		SPECIAL		= 0x0080,
		PHYSICS		= 0x0100,
		REPEIAR		= 0x0200,
		SUPLLY		= 0x0400,
		ELECTRICITY	= 0x0800,
	};
	//exvs�ƃ{�[�_�[�u���C�N�Q�l
	float damage;
	short group;
	short correctedRate;//�_���[�W�␳��
	short downValue;//�_�E���l
	short staggerValue;//��낯�l
	btVector3 knockBackVel;
	//btVector3 contactPosistions[4];
	//int contactNums;
	float unControlableTime;//����s�\���ԁA�d������

	float upDownwardOffset;//�ł��グ�A���������U���̂��߂̒l
	btVector3 knockBackVelRate;//�Ԃ���є{�� //���邩�H

	DamageType damageType;

	//���������ǂ�����
	//�ڐG�������_�Ōv�Z

	//�ړ����x�ቺ���ǂ���������H
	//�h��͒ቺ
	//ECM

	//���������ŏ��������Ƃ�
	//ICollisionObject *me;
};


/*�@�U���͂̎��@��{
<a href="http://www10.atwiki.jp/g-vs/pages/88.html">�@����m�K���_�� �K���_��VS.�K���_�� Wiki No.2 - �_���[�W�v�Z</a>
    �_���[�W�@���@�P���ڂ̍U���̈З́@�{�@�P���ڂ̍U���̕␳���~�o�Q���ڂ̍U���̈З́@�{�@�Q���ڂ̍U���̕␳���~�i�R���ڂ́c�j�p
	�P���_���[�W�@�� �З� �~ �_���[�W���x���␳ �~ �␳�� �~ �L�����Z���␳ �~ �_�E���ǌ��␳ �~ ��˕␳
					�~ �c���̓Q�[�W�␳ �~ �U�����́i�����␳�~�A���␳�j�~�H�炤���́i�����␳�~�A���␳�j 

�yPSP�z�K���_��NEXT PLUS �����X�� 6
	�З͒l�����X�g
�S8�s
1�s��0xaaaabbbb:16bit��؂�B+bb���З́B+aa����낯�l�B�~�ς�낯�l100�ł�낯
2�s��0xccccdddd:16bit��؂�B+dd���_�E���l�B100�Ŋ����wiki���ł̐��l�B+cc���^�␳�l�B5�Ŋ����wiki���ł̐��l
3�s��0x00eeffgg:8bit�Bgg,���@�q�b�g�X�g�b�v ff,�G�@�q�b�g�X�g�b�v ee,�q�b�g�G�t�F�N�g(��1)
4�s��0x3Fq00000:float�Bq�q�b�g�G�t�F�N�g�̑傫���B���܂�ł����ƃt���[�Y�B
5�s��0xzzhh00ii:8bit�Bii,�q�b�g�� hh�_�E����Ԃ̓G�ɓ��Ă����̂ӂ���т����B zz�_�E���ȊO�̏�Ԃ̓G�ɓ��Ă����̂ӂ���ѕ��B0x20920002�œ���L�����~
6�s��0x00jj00kk:8bit�Bkk���ӂ��Ƃє{���Bjj���x�N�g���Ɋւ��鉽��?�傫������ƁA�������ɂ�����΂Ȃ��Ȃ�B
7�s��0xmm00unrl:8bit�Brl,�ӂ���ѕ����̉����̕␳�Bun,�ӂ���т̏c���̕␳�B+,2�S�̓I�Ȋp�x�̕␳�Bmm,�A��hit�̍ۂ�hit�ԊuF�B
8�s��0xyy100000:8bit�Byy,�Ǝ��␳�̓K�p�t���O(�����Ȃ�) ���s��
9�s��0xtt010000:8bit�Btt,�i�������B9�Ŏˌ�? 

�q�b�g�X�g�b�v�Ƃ�
	�U�����肪�����������ɋN����A�݂��̎��Ԓ�~���ԁB�܂��͂��̍d����t���[���̂��Ƃ������B

void damage( const damageInfo &di ) {
	float damage=0;
	//di damageInfo
	btVector3 knockBackVel = di.knockBackVel;

	if(di.group==mGroup){
		return;
	}

	float def = mDef;
	if( (mResistDamageType & di.damageType) !=0 )
		def=256.f;
		knockBackVel.setValue(0.f, 0.f, 0.f);
	}
	float defRate=1.f-def/256.f;
	defRate<0 ? defRate=0 : ;
	damage=damage * ( defRate );

	mController->setVelocity(  knockBackVel );

*/
//�R�s�y�p
/*
	RTTI_DECL;
	virtual btCollisionObject* getBtCollisionObject();
	virtual void removeFromWorld();
	virtual void addToWorld();

#define GAME_OBJECT_INTERFACE_DECL \
	RTTI_DECL;
	virtual btCollisionObject* getBtCollisionObject(); \
	virtual const std::vector< const btCollisionObject* > & getNotCollideBtCollisionObjects() const; \
	virtual void removeFromWorld(); \
	virtual void addToWorld();
*/

class ICollider;
typedef std::shared_ptr<ICollider> IColliderSharedPtr;
typedef std::weak_ptr<ICollider> IColliderWeakPtr;

//list or vector or set or sort�ς�vector �̂ǂꂪ�������H
//��ʂ̃f�[�^�����킯�ł͂Ȃ��̂�, insert, find������set�ɂ���B
//��ʂ̃f�[�^�����Ȃ�sorted_vector�ɂ���B
//const btCollisionObject*�ɂ����̂�shared_ptr���Əz�Q�ƂɂȂ��Ă��܂��\������������
//���ۂɃA�N�Z�X����킯�ł͂Ȃ��̂ŊJ�����Ă��Ƃ��Ă����͂����ɂ����Ǝv����
//���ۍ���̂��ė��p����ꍇ�A���Z�b�g���Ȃ���Γ�����Ȃ�
typedef std::set< const btCollisionObject* > BtCollisionObjects;


//interface����Ȃ�Base?
class ICollider : public IGameObject
{
public:
	virtual ~ICollider() {}

	static ICollider* cast(void *vp){
		return static_cast<ICollider*>( vp );
	}

	std::shared_ptr<ICollider> getIColliderSharedPtr(){
		std::shared_ptr<ICollider> r = std::dynamic_pointer_cast<ICollider>(shared_from_this());
		return r;
	}

	RTTI_DECL;
	
	//tofix
	//filtermask��filterid��ǉ��������ق����g��������
	bool isMMDObject() const{
		return mIsMMDObject;
	}

	//�g���p
	bool enableCollidedMMDObject() const{
		return false;
	}

	//���_���̃N���X�͕����Փ˃N���X
	//btCollisionObject��user_ptr�ɂ͕K�����̃N���X���p�������̂�����
	//����ĕK��btCollisionObject�������Ȃ���΂Ȃ�Ȃ�
	//	virtual btCollisionObject* getBtCollisionObject() = 0;
	//��null��Ԃ��݌v�͋�����Ȃ�

	//�l����
	//MME�ƃA�N�Z�T���Apmd
	//	�p���V�����N���XCollider������A�ȉ��̊֐���V�����N���X�Ɉړ�����
	//	�o���b�g���E�ɓ���N���X�͑S��Collider���g���悤�ɂ���
	//	virtual btCollisionObject* getBtCollisionObject() = 0;
	//	virtual const btCollisionObject* getConstBtCollisionObject()const{
	//	const BtCollisionObjects & getNotCollideBtCollisionObjects() const
	//	void ignoreCollision( const btCollisionObject* obj ){
	//	bool canCollide( const btCollisionObject *obj  )const{
	//	void clearNotCollideObjects(){
	//	virtual void removeFromWorld() = 0;
	//	
	//	virtual void addToWorld() = 0;
	//	//btCollisionObject���ڐG���Ă���ԁA�Ă΂ꑱ����
	//	virtual void onCollisionStay( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj){}
	//	
	//	//btCollisionObject���Փ˂����Ƃ��Ă΂��
	//	virtual void onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj){}
	//	
	//	//btCollisionObject�����ꂽ�Ƃ��Ă΂��
	//	virtual void onCollisionExit( btCollisionObject *me, btCollisionObject *obj ){}
	//�H�H���₻������Ƃ��̃N���X�͈�Ή��̂��߂ɂ���񂾁H
	//

	//NULL�͂���//��{��
	//NULL�����e����悤�Ȑ݌v�ɂ�����
	//�\�������̃I�u�W�F�N�g�̂���(������)
	//MME::ControlObjectSelf���l����
	//btCollisionObject�������Ȃ��ꍇ�킯���ǂ����邩
	virtual btCollisionObject* getBtCollisionObject() = 0;
	
	//�p������ƌp�����ĂȂ��ق����B���Ă��܂�����( ������const�����o�[�֐�)�A���O��Const�t����
	virtual const btCollisionObject* getConstBtCollisionObject()const{
		return const_cast<ICollider*>(this)->getBtCollisionObject();
	}

	//btCollisionObject�������Ȃ��ꍇ�킯���ǂ����邩
	//btCollisionObject�������Ȃ��ꍇ�A�I�[�o�[���C�h���邱��
	//�p���V�����N���XCollider
	virtual const btTransform & getWorldTransform() const {
		return getConstBtCollisionObject()->getWorldTransform();
	}

	virtual btTransform & getWorldTransform() {
		return getBtCollisionObject()->getWorldTransform();
	}


	//BoneCollisionObject�̎����̃p���`�������ɓ�����Ȃ��悤�ɂ��邽�ߕK�v
	//�����蔻���2�ȏ㎝�ꍇ���ꍇ�i�n�`�p�A���̑��j�A�������g��������Ȃ��悤�ɂ��邽�ߕK�v�B
	//
	const BtCollisionObjects & getNotCollideBtCollisionObjects() const
	{
		return mNotCollideObjects;
	}
	//void addNotCollideBtCollisionObject( const btCollisionObject* );
	//void removeNotCollideBtCollisionObject( const btCollisionObject* );
	
	
	void ignoreCollision( const btCollisionObject* obj ){
		//assert(!canCollide(obj) && " has hited already. do you want object to hit only once?" );
		mNotCollideObjects.insert( obj );
	}

	bool canCollide( const btCollisionObject *obj  )const{
		GameObject::BtCollisionObjects::const_iterator it = std::find( mNotCollideObjects.begin(), mNotCollideObjects.end(), obj );
		//����������A�Փ˂��Ȃ�
		if( it!=mNotCollideObjects.end() ){
			return false;
		}
		return true;
	}
	
	void clearNotCollideObjects(){
		mNotCollideObjects.clear();
	}

	//�킴�킴�C���^�[�t�F�C�X���������Ȃ��Ă��ǂ��Ǝv��
	//���E�������ɂȂ����̂�
	//�ǉ����鐢�E���w�肵�Ȃ��Ⴂ���Ȃ����ǂ���Ȃ̂߂�ǂ��������
	//������mNotCollideObjects�̏��������s��
	//��1
	//	mNotCollideObjects���N���A���A������Ȃ��I�u�W�F�N�g������������Ȃ���
	//	���������Ă���l�ƌ��͓�����Ȃ��悤�ɐݒ肵�Ȃ���
	//��2
	//	���f���̏�����
	//	mModel->init( mRigidBody->getWorldTransform() );
	//
	//��3
	//	�C�x���g�̔���
	//	Event::Manager::instance()->queueEvent( Event::EventData::AddToWorld::create( getIColliderSharedPtr() ) );
	//virtual void removeFromWorld() = 0;
	//virtual void addToWorld() = 0;

	//btCollisionObject���ڐG���Ă���ԁA�Ă΂ꑱ����
	virtual void onCollisionStay( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj){}

	//btCollisionObject���Փ˂����Ƃ��Ă΂��
	virtual void onCollisionEnter( btPersistentManifold *manifold, btCollisionObject *me, btCollisionObject *obj){}

	//btCollisionObject�����ꂽ�Ƃ��Ă΂��
	virtual void onCollisionExit( btCollisionObject *me, btCollisionObject *obj ){}
	
protected:
	//shared_from_this���g������protected
	//�K���Ashared_ptr���g���Ashared_ptr�ŊǗ����Ȃ���΂Ȃ�Ȃ�
	//�I�u�W�F�N�g���쐬����̂�create();
	ICollider();

	//static const std::vector< const btCollisionObject* > & getEmptyBtCollisionObjects(){
	//	return sEmptyCollisionObject;
	//}
protected:	
	const bool mIsMMDObject;
private:
	BtCollisionObjects mNotCollideObjects;
	//getNotCollideBtCollisionObjects()�ŉ�������Ȃ��ꍇ�A���vector��Ԃ������̂�
	//static const std::vector< const btCollisionObject* > sEmptyCollisionObject;
};

/*
class CPlayerActor : public GameObject {
public:
	CPlayerActor(){}
	virtual ~CPlayerActor(){}
	virtual GameObjectType getType(){ return PLAYER; }
	virtual bool isPhysical(){ return true; }
	virtual bool isGeometrical(){ return false; }
	virtual bool hasRigidBody(){ return true; }
	virtual void update(){}

	void draw();
	void setPosition( const Vector3& );
	void setAngleY( float );
	const Vector3* position() const;
	void getView( Matrix34* matrix, Vector3* position ) const;
	void setDamage( int damage );
	int getHitPoint() const;
	int getEnergy() const;
	bool getLockOn() const;
private:
	Vector3 mPos;
	float mAngleY;
	
	int mBulletNumber;

	int mCameraCount;
	int mCount; //�ړ��J�n�㉽�t���[���o�����H
	Vector3 mVelocity; //���݂̕��ʑ��x
	float mAngleVelocityY; //�U��������x

	int mHitPoint; //�̗�

	//weapon �N���X�����ׂ���?
	int mEnergy; //��������̂ɕK�v�ȃG�l���M�[
	bool mLockOn; //���b�N�I�����Ă܂����H

	Vector3 mClickPos;//target
	btRigidBody *m_pRigidBody;
};
*/
}//end namespace GameObject

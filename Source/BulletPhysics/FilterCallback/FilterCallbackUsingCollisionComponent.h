#pragma once
#include<btBulletCollisionCommon.h>
#include<BulletCollision/BroadphaseCollision/btOverlappingPairCallback.h>
#include"BulletPhysics/Manager.h"
#include"MMD/PMDRigidBody.h"
//���[��
//btCollisionObject��UserPointer��RigidBodyComponent*��nullptr�Ƃ���
//
//�����Ƃ�mmd�I�u�W�F�N�g�̏ꍇ�͂Ԃ���
//mmd�I�u�W�F�N�g�͑��̃I�u�W�F�N�g�ɂ͂Ԃ���Ȃ�
//mmd�I�u�W�F�N�g���ǂ�����group�Ŕ��ʂ���

//���d�p�������ꍇ��void*����̃C���^�[�t�F�C�X�N���X�ւ̃L���X�g�͂��܂��������o�O�𐶂݂₷��
//�o�O�����ꍇ�����ꍇ�����邪vtable�����Ă�
//�p�����Ȃǂ��l������Γ�������������
//�Ȃ̂Ńo�O���������߂�void*����̃L���X�g�͌��̃N���X�ɖ߂������Ƃ����̂�������

namespace BulletPhysics {
	namespace FilterCallback {

		class FilterCallbackUsingCollisionComponent : public btOverlapFilterCallback
		{
		public:
			// return true when pairs need collision
			virtual bool	needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
			{
				bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
				collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask) != 0;


				//MMDObject�ƃQ�[���I�u�W�F�N�g�͂Ԃ���Ȃ�
				//MMD�ł��t�B���^�����O���g���Ă���̂�
				//unsigned short 15�O���[�v�����ł͑���Ȃ��B
				//�Ȃ̂̂�mmd�Ŏg�p���Ă��镪�͊g��������
				//��������6�O���[�v����BulletPhysics�{�̂Ŏg���Ă���
				//��`���Ă��邾����StaticFilter�ȊO�͎g���Ă��Ȃ����ۂ�����
				///optional filtering to cull potential collisions
				//enum CollisionFilterGroups
				//{
				//        DefaultFilter = 1,
				//        StaticFilter = 2,
				//        KinematicFilter = 4,
				//        DebrisFilter = 8,
				//		SensorTrigger = 16,
				//		CharacterFilter = 32,
				//        AllFilter = -1 //all bits sets: DefaultFilter | StaticFilter | KinematicFilter | DebrisFilter | SensorTrigger
				//};

				//StaticFileter�g�p�ӏ�
				/*btDiscreteDynamicWorld.cpp��-------------------------------
				void	btDiscreteDynamicsWorld::addRigidBody(btRigidBody* body)
				{
				if (!body->isStaticOrKinematicObject() && !(body->getFlags() &BT_DISABLE_WORLD_GRAVITY))
				{
				body->setGravity(m_gravity);
				}

				if (body->getCollisionShape())
				{
				if (!body->isStaticObject())
				{
				m_nonStaticRigidBodies.push_back(body);
				} else
				{
				body->setActivationState(ISLAND_SLEEPING);
				}

				bool isDynamic = !(body->isStaticObject() || body->isKinematicObject());
				short collisionFilterGroup = isDynamic? short(btBroadphaseProxy::DefaultFilter) : short(btBroadphaseProxy::StaticFilter);
				short collisionFilterMask = isDynamic? 	short(btBroadphaseProxy::AllFilter) : 	short(btBroadphaseProxy::AllFilter ^ btBroadphaseProxy::StaticFilter);

				addCollisionObject(body,collisionFilterGroup,collisionFilterMask);
				}
				}
				*/

				btCollisionObject *co0 = static_cast<btCollisionObject*>(proxy0->m_clientObject);
				btCollisionObject *co1 = static_cast<btCollisionObject*>(proxy1->m_clientObject);
				Component::RigidBodyComponent *rbc0 = static_cast<Component::RigidBodyComponent*>(co0->getUserPointer());
				Component::RigidBodyComponent *rbc1 = static_cast<Component::RigidBodyComponent*>(co1->getUserPointer());

				collides = collides;



				//����if���̏����̏���������肭�ǂ��悤��
				//xor�g���΂��ꂢ�ɏ������������킩��ɂ�����

				//���҂Ƃ�MMDObject�̏ꍇ
				if (
					collides &&
					proxy0->m_collisionFilterGroup == BulletPhysics::MMDObjectFilter &&
					proxy1->m_collisionFilterGroup == BulletPhysics::MMDObjectFilter)
				{
					
					assert(rbc0 && rbc1);
					if (rbc0 && rbc1) {
						return rbc0->canCollide(co1) && rbc1->canCollide(co0);
					}
				}

				//MMDObject�ƃQ�[���I�u�W�F�N�g�͂Ԃ���Ȃ��̂�
				//�܂�A�ǂ��炩��MMD�I�u�W�F�N�g�Ȃ�Ԃ���Ȃ�
				collides = collides && proxy0->m_collisionFilterGroup != BulletPhysics::MMDObjectFilter;
				collides = collides && proxy1->m_collisionFilterGroup != BulletPhysics::MMDObjectFilter;


				if (collides) {
					if (rbc0 && rbc1) {
						return rbc0->canCollide(co1) && rbc1->canCollide(co0);
					}
					//rbc��nullptr�̏ꍇ�K���Ԃ���
					else {
						return true;
					}
				}
				return false;
			}
		};
	}
}

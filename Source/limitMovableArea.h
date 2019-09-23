#pragma once

#include<btBulletCollisionCommon.h>
#include<cmath>


//y�������̉~����btCollisionObject�𐧌�����B
//�͈͊O�ɂ����ꍇ���K���ɖ������ړ�������
//���������ꍇmmd ��rigidbody�͂ǂ��Ȃ�
void limitMovableArea(
	btCollisionObject* obj,
	const btVector3 &center,
	float radius) 
{
	assert(false && "�r���ŕK�v�Ȃ��Ȃ����̂Ńf�o�b�O���ĂȂ�");
	//�Q�Ƃɂ��Ă�C��t����
	btVector3 &rPos = obj->getWorldTransform().getOrigin();

	const float &px = rPos.x();
	const float &pz = rPos.z();

	const float &cx = center.x();
	const float &cz = center.z();

	const float &diffx = px - cx;
	const float &diffz = pz - cz;
	float distance2 = std::powf(diffx, 2) + std::powf(diffz, 2);


	float distance = std::powf(distance2, 0.5f);

	//�񐔂ɂȂ�ꍇ�����肻��

	if (distance == 0.f && distance <= 0.0001f) {
		return;
	}
	
	//������ւ������
	if (distance > radius) {
		rPos.setX(cx + diffx*radius / distance);
		rPos.setZ(cz + diffz*radius / distance);
		//�����߂荞�񂾂�ǂ�����
		//��Ƃ��ǂ�����́H

	}
}
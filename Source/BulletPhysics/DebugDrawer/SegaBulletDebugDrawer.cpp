#include "SegaBulletDebugDrawer.h"
#include <GameLib/Graphics/Manager.h>
#include <GameLib/Graphics/VertexBuffer.h>
#include <GameLib/Graphics/Vertex.h>
#include <GameLib/Graphics/Texture.h>
#include <GameLib/Math/Matrix34.h>

#include <d3dx9.h>
namespace BulletPhysics {
using GameLib::Math::Matrix34; 
using namespace GameLib::Graphics;
CSegaBulletDebugDrawer::CSegaBulletDebugDrawer() {
	vertexBuffer_ = VertexBuffer::create( 2 ); //8���_�̒��_�o�b�t�@
}


CSegaBulletDebugDrawer::~CSegaBulletDebugDrawer() {
}
void CSegaBulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
	//���x�ƂȂ��o�Ă���̂�m�ꕶ���ł��邱�Ƃ������ė~����
	GameLib::Graphics::Manager m = GameLib::Graphics::Manager::instance();

	Vertex* v = vertexBuffer_.lock(); //�������݃|�C���^�𓾂�
	//�ʒu������
	v[ 0 ].mPosition.set( from.x(), from.y(), from.z() );
	v[ 1 ].mPosition.set( to.x(), to.y(), to.z() );
	v[ 0 ].mNormal.set( 0.f, 1.f, 0.f );
	v[ 1 ].mNormal.set( 0.f, 1.f, 0.f );
	//���_�J���[������
	v[ 0 ].mColor = D3DCOLOR_XRGB( BYTE(255*lineColor.x()), BYTE(255*lineColor.y()), BYTE(255*lineColor.z()) );
	v[ 1 ].mColor = v[0].mColor;
	//�e�N�X�`�����W������(�K��)
	//v[ 0 ].mUv.set( 0.f, 0.f );
	//v[ 1 ].mUv.set( 0.f, 1.f );
	//�����I����������
	vertexBuffer_.unlock( &v );

	//�g���f�[�^���w�肷��
	m.setTexture(0);
	m.enableVertexBlend(false);
	Matrix34 w;
	w.setIdentity();
	m.setWorldMatrix(w); 
	m.setVertexBuffer( vertexBuffer_ );
	
	m.draw( 0, 1, PRIMITIVE_LINE );
}

void CSegaBulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	// draw a line to represent the normal.  This only lasts one frame, and is hard to see.
	//   it might help to linger this drawn object onscreen for a while to make it more noticeable
	
	btVector3 const startPoint = PointOnB;
	btVector3 const endPoint = PointOnB + normalOnB * distance;
	
	drawLine( startPoint, endPoint, color );
}

void CSegaBulletDebugDrawer::reportErrorWarning(const char* warningString)
{
	OutputDebugStringA( warningString );
}

void CSegaBulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	// not implemented
}

void CSegaBulletDebugDrawer::setDebugMode(int debugMode)
{
	// not implemented, this class just draws everything all the time
}

int CSegaBulletDebugDrawer::getDebugMode() const
{
	return -1; // draw everything
}
}
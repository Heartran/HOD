//------------------------------------------------------------------------
// File : LSPSM.cpp
// Desc : Light Space Perspective Shadow Maps
// Date : May 30, 2010
// Author : Pocol
//------------------------------------------------------------------------

//
// Includes
//
#include"LSPSM.h"
#include<assert.h>
#include"helper/MyOutputDebugString.h"
#include<GameLib/DebugScreen.h>

// Defines
//
#define NEW_FORMULA 0

namespace {
	float GetCrossingAngle( D3DXVECTOR3 v1, D3DXVECTOR3 v2 ){
		assert( D3DXVec3LengthSq(&v1)!=0.f );
		assert( D3DXVec3LengthSq(&v2)!=0.f );

		D3DXVec3Normalize( &v1, &v1 );
		D3DXVec3Normalize( &v2, &v2 );

		float dot = D3DXVec3Dot( &v1, &v2 );
		return acosf(dot);
		
	}
}


//------------------------------------------------------------------------
// Name : TransformCoord()
// Desc : �s���3�����x�N�g����ϊ�����
//------------------------------------------------------------------------
D3DXVECTOR3 TransformCoord(const D3DXVECTOR3 &point, const D3DXMATRIX &matrix)
{
	float x = matrix._11 * point.x + matrix._21 * point.y + matrix._31 * point.z + matrix._41;
	float y = matrix._12 * point.x + matrix._22 * point.y + matrix._32 * point.z + matrix._42;
	float z = matrix._13 * point.x + matrix._23 * point.y + matrix._33 * point.z + matrix._43;
	float w = matrix._14 * point.x + matrix._24 * point.y + matrix._34 * point.z + matrix._44;

	x /= w;
	y /= w;
	z /= w;
	return D3DXVECTOR3( x, y, z );
}

//////////////////////////////////////////////////////////////////////////
// PointList Class
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Name : PointList()
// Desc : �R���X�g���N�^
//------------------------------------------------------------------------
PointList::PointList()
{
	//���X�g���N���A
	m_list.clear();
}

//------------------------------------------------------------------------
// Name : GetSize()
// Desc : �T�C�Y��Ԃ�
//------------------------------------------------------------------------
int PointList::GetSize()
{
	return (int)m_list.size();
}

//------------------------------------------------------------------------
// Name : Add()
// Desc : ���W��ǉ�����
//------------------------------------------------------------------------
void PointList::Add(const D3DXVECTOR3& value)
{
	m_list.push_back( value );
}

//------------------------------------------------------------------------
// Name : Clear()
// Desc : ���X�g���N���A����
//------------------------------------------------------------------------
void PointList::Clear()
{
	m_list.clear();
}

//------------------------------------------------------------------------
// Name : Transform()
// Desc : �w�肵���s��Ń��X�g��ϊ�����
//------------------------------------------------------------------------
void PointList::Transform(const D3DXMATRIX &matrix)
{
	int size = (int)m_list.size();
	for( int i=0; i<size; i++ )
	{
		D3DXVECTOR3 point = m_list[i];
		m_list[i] = TransformCoord( point, matrix );
	}
}

//------------------------------------------------------------------------
// Name : ComputeBoundingBox()
// Desc : AABB���Z�o����
//------------------------------------------------------------------------
void PointList::ComputeBoundingBox(D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	int size = (int)m_list.size();
	if( size > 0 )
	{
		*min = *max = m_list[0];
		for( int i=1; i<size; i++ )
		{
			D3DXVec3Minimize( min, min, &m_list[i] );
			D3DXVec3Maximize( max, max, &m_list[i] );
		}
	}
}

//------------------------------------------------------------------------
// operator =
//------------------------------------------------------------------------
PointList& PointList::operator = (const PointList &value)
{
	m_list.clear();
	int size = (int)value.m_list.size();
	for( int i=0; i<size; i++ )
	{
		m_list.push_back( value.m_list[i] );
	}
	return (*this);
}

//------------------------------------------------------------------------
// operator []
//------------------------------------------------------------------------
D3DXVECTOR3& PointList::operator[](int index)
{
	return m_list[index];
}


//////////////////////////////////////////////////////////////////////////
// LSPSMCalculator Class
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// Name : LSPSMCalculator()
// Desc : �R���X�g���N�^
//------------------------------------------------------------------------
LSPSMCalculator::LSPSMCalculator()
{
	mNearClip = 0.1f;
	mUseLSPSM = false;
	mExtendLengthFromFrustum=200.f;
	mPointList.Clear();
}

//------------------------------------------------------------------------
// Name : ComputeUpVector()
// Desc : ���_�E���C�g�x�N�g������A�b�v�x�N�g�����v�Z����
//-------------------------------------------------------------------------
D3DXVECTOR3 LSPSMCalculator::ComputeUpVector(const D3DXVECTOR3 &viewDirection, const D3DXVECTOR3 &lightDirection)
{
	D3DXVECTOR3 left;
	D3DXVECTOR3 up;

	D3DXVec3Cross( &left, &lightDirection, &viewDirection );
	D3DXVec3Cross( &up, &left, &lightDirection );
	D3DXVec3Normalize( &up, &up );
	return up;
}

//------------------------------------------------------------------------
// Name : ComputeMatrix_USM()
// Desc : �ʏ�̃V���h�E�}�b�v�s����v�Z����
//------------------------------------------------------------------------
void LSPSMCalculator::ComputeMatrix_USM()
{
	D3DXVECTOR3 max, min;

	//���C�g�̃r���[�s������߂�
	D3DXMatrixLookAtLH( &mLightView, &mEyePosition, &(mEyePosition + mLightDirection), &mViewDirection );

	//���C�g��Ԃ֕ϊ�
	mPointList.Transform( mLightView );

	//AABB���Z�o
	mPointList.ComputeBoundingBox( &min, &max );

	//�͈͂�K���ɂ���
	mLightProjection = GetUnitCubeClipMatrix( min, max );
}

//------------------------------------------------------------------------
// Name : ComputeMatrix_LSPSM()
// Desc : ���C�g��ԓ����V���h�E�}�b�v�s����v�Z
//------------------------------------------------------------------------
void LSPSMCalculator::ComputeMatrix_LSPSM()
{
	//�R�s�y�������������ǂ��Ȃ������
	D3DXVECTOR3 max, min;

	//�����x�N�g���ƃ��C�g�x�N�g���̂Ȃ��p�x�����߂�
	//float angle = GetCrossingAngle(mViewDirection, mLightDirection);
	//assert( D3DXVec3LengthSq(&mViewDirection)==1.f );
	//assert( D3DXVec3LengthSq(&mLightDirection)!=1.f );

	float angle = acosf(abs(D3DXVec3Dot( &mViewDirection, &mLightDirection )));

	//TRACE1( "angle = %f�x\n", angle*180.f/D3DX_PI );

	//�J�����ƌ��̕��������s�ɋ߂��ꍇ(11)
	//if ( angle <= D3DX_PI/8.f )
	//{
	//	//���C�g�ɂ��c�݂��Ȃ��̂Œʏ�̃V���h�E�}�b�v��K�p
	//	ComputeMatrix_USM();
	//	return;
	//}

	//���X�g���R�s�[���Ă���
	PointList listClone = mPointList;

	float sinGamma = sinf(angle);
	assert(sinGamma!=0.f);
	// �A�b�v�x�N�g�����Z�o
	D3DXVECTOR3 up = ComputeUpVector( mViewDirection, mLightDirection );
	//D3DXVECTOR3 up = mViewDirection;

	//���C�g�̃r���[�s������߂�
	D3DXMatrixLookAtLH( &mLightView, &mEyePosition, &(mEyePosition + mLightDirection), &up );

	//���C�g�̃r���[�s��Ń��X�g��ϊ����AAABB���Z�o
	mPointList.Transform( mLightView );
	mPointList.ComputeBoundingBox( &min, &max );

	//�V��������������߂�
	const float factor = 1.0f / sinGamma;
	const float z_n = factor * mNearClip;
	const float d = abs( max.y - min.y );
#if NEW_FORMULA 
	// New Formula written in ShaderX4
	const float z0 = - z_n;
	const float z1 = - ( z_n + d * sinGamma );
	assert( z0!=0.f );
	assert( 0.f <= z1/z0 );
	const float n = d / ( sqrtf( z1 / z0 ) - 1.0f );
#else
 	// Old Formula written in papers
	const float z_f = z_n + d * sinGamma;
	const float n = ( z_n + sqrtf( z_f * z_n ) ) * factor;
#endif
	const float f = n + d;
	D3DXVECTOR3 pos = mEyePosition - up * ( n - mNearClip );

	//�V���h�E�}�b�v�����p���C�g�̃r���[�s��
	D3DXMatrixLookAtLH( &mLightView, &pos, &(pos + mLightDirection), &up );

	//Y�����ւ̎ˉe�s����擾
	D3DXMATRIX proj = GetPerspective( n, f );

	//�����ϊ���̋�Ԃ֕ϊ�����
	mLightProjection = mLightView * proj;
	listClone.Transform( mLightProjection );

	//AABB���Z�o
	listClone.ComputeBoundingBox( &min, &max );

	//GameLib::DebugScreen ds;
	//ds.setPosition( 20, 0);
	//ds.precision( 5 );
	//ds << "camera_light_angle: " << (angle*180.f/D3DX_PI)  << GameLib::endl;
	//ds << "up_mLightDirection_dot: " << D3DXVec3Dot( &mLightDirection, &up ) << GameLib::endl;
	//ds << "n: " << n <<GameLib::endl;
	//ds << "f: " << f << GameLib::endl; 
	//ds << "f-n: " << f-n << GameLib::endl;
	//ds << "min : " << min.x << ", " << min.y << ", " << min.z << GameLib::endl;
	//ds << "max : " << max.x << ", " << max.y << ", " << max.z << GameLib::endl;


	//�͈͂�K���ɂ���
	D3DXMATRIX clip = GetUnitCubeClipMatrix( min, max );

	//�V���h�E�}�b�v�����p���C�g�̎ˉe�s������߂�
	mLightProjection = proj * clip;
	//mLightProjection = clip * proj;
}

//------------------------------------------------------------------------
// Name : UpdateShadowMatrix()
// Desc : ���C�g�̍s����X�V����
//------------------------------------------------------------------------
void LSPSMCalculator::UpdateShadowMatrix()
{
	D3DXMATRIX viewProj;
	viewProj =  mEyeView * mEyeProjection;

	//�r���[�{�����[�������߂�
	ComputeLightVolumePoints( viewProj );

	if( mUseLSPSM ){
		// Light Space Perspective Shadow Map
		ComputeMatrix_LSPSM();
	} else{
		ComputeMatrix_USM();
	}

	//������W�n�ɕϊ�
	//D3DXMATRIX scale = CreateScale( 1.0f, 1.0f, -1.0f );
	//Multiply( mLightProjection, scale, mLightProjection );

	//�V���h�E�}�b�v�Ɏg���ŏI�I�ȍs������߂�
	mLightViewProjection = mLightView * mLightProjection;
}

//-----------------------------------------------------------------------
// Name : ComputeLightVolumePoints()
// Desc : �ʑ̂����߂�
//-----------------------------------------------------------------------
void LSPSMCalculator::ComputeLightVolumePoints(const D3DXMATRIX &viewProj)
{
	//�J�����̎���������߂�
	ComputeViewFrustum( viewProj );

	//�J�����̎�����ɉe�𓊂���I�u�W�F�N�g�����邩���肷��
	// Check 1: ������ƌ������Ă��邩�H
	/*** ��������`�����镽�ʂ�AABB�Ƃ̌���������s�� ***/
	///�ʓ|�Ȃ̂Ŗ�����///

	// Check 2: �I�u�W�F�N�g�̊e���_���烉�C�g�x�N�g��������
	// ���C���΂��������邩�H
	/*** �e���_���烉�C�g�x�N�g�������Ƀ��C���΂��A��������`�����镽�ʂƂ̌���������s�� ***/
	///�ʓ|�Ȃ̂Ŗ�����///
	
	//�ʑ̂𐳊m�ɏo���̂��߂�ǂ������̂�
	//�ʓ|�Ȃ̂Ō������Ɏ������L�΂�
	for( int i=0; i<8; ++i ){
		D3DXVECTOR3 p = mPointList[i];
		p += -mExtendLengthFromFrustum*mLightDirection;
		
		mPointList.Add(p);
	}
}

//-----------------------------------------------------------------------
// Name : ComputeViewFrustum()
// Desc : �J�����̎���������߂�
//�A�C�f�A
//������̒��ɂ��邩�����߂�Ȃ�Ίe�핽�ʕ��������߂��ق����悭�Ȃ���
//�܂��債�ĕς��Ȃ�����
//-----------------------------------------------------------------------
void LSPSMCalculator::ComputeViewFrustum( const D3DXMATRIX &viewProj )
{
	mPointList.Clear();

	//�@�����̂��쐬
	D3DXVECTOR3 v[8];
	v[0] = D3DXVECTOR3( -1.0f, +1.0f, -1.0f );
	v[1] = D3DXVECTOR3( -1.0f, -1.0f, -1.0f );
	v[2] = D3DXVECTOR3( +1.0f, -1.0f, -1.0f );
	v[3] = D3DXVECTOR3( +1.0f, +1.0f, -1.0f );
	v[4] = D3DXVECTOR3( -1.0f, +1.0f, +1.0f );
	v[5] = D3DXVECTOR3( -1.0f, -1.0f, +1.0f );
	v[6] = D3DXVECTOR3( +1.0f, -1.0f, +1.0f );
	v[7] = D3DXVECTOR3( +1.0f, +1.0f, +1.0f );
	for( int i=0; i<8; i++ )
	{
		mPointList.Add( v[i] );
	}

	//�@�r���[�s�񁨎ˉe�s��̋t�ϊ����s���s������߂�
	D3DXMATRIX invViewProj;
	D3DXMatrixInverse( &invViewProj, NULL, &viewProj );

	//�@�����̂ɋt�ϊ�����s��������A����������߂�
	mPointList.Transform( invViewProj );
}

//-----------------------------------------------------------------------
// Name : GetUnitCubeClipMatrix()
// Desc : �͈͂�K���ɂ���s����擾����
//-----------------------------------------------------------------------
D3DXMATRIX LSPSMCalculator::GetUnitCubeClipMatrix(const D3DXVECTOR3 &min, const D3DXVECTOR3 &max)
{
	D3DXMATRIX result;

	result._11 = 2.0f / ( max.x - min.x );
	result._21 = 0.0f;
	result._31 = 0.0f;
	result._41 = -( max.x + min.x ) / ( max.x - min.x );

	result._12 = 0.0f;
	result._22 = 2.0f / ( max.y - min.y );
	result._32 = 0.0f;
	result._42 = -( max.y + min.y ) / ( max.y - min.y );

	result._13 = 0.0f;
	result._23 = 0.0f;
	result._33 = 1.0f / ( max.z - min.z );
	result._43 = - min.z / ( max.z - min.z );

	result._14 = 0.0f;
	result._24 = 0.0f;
	result._34 = 0.0f;
	result._44 = 1.0f;

	return result;
}

//-----------------------------------------------------------------------
// Name : GetPerspective()
// Desc : Y�����ւ̎ˉe�s����擾����
//-----------------------------------------------------------------------
D3DXMATRIX LSPSMCalculator::GetPerspective( float nearDist, float farDist )
{
	D3DXMATRIX result;
	D3DXMatrixIdentity(&result);
	result._22 = farDist /( farDist - nearDist );
	result._24 = 1.0f;
	result._42 = -farDist * nearDist /( farDist - nearDist );
	result._44 = 0.0f;
	return result;
}

//-----------------------------------------------------------------------
// Name : SetEyePos()
// Desc : �J�����̈ʒu��ݒ�
//-----------------------------------------------------------------------
void LSPSMCalculator::SetEyePos( const D3DXVECTOR3 &value )
{
	mEyePosition = value;
}

//-----------------------------------------------------------------------
// Name : SetViewDir()
// Desc : �J�����̎����x�N�g����ݒ�
//-----------------------------------------------------------------------
void LSPSMCalculator::SetViewDir( const D3DXVECTOR3 &value )
{
	assert( D3DXVec3LengthSq(&value)!=0.f );
	D3DXVec3Normalize( &mViewDirection, &value );
	//mViewDirection = value;
}

//-----------------------------------------------------------------------
// Name : SetLightDir()
// Desc : ���C�g�x�N�g����ݒ�
//-----------------------------------------------------------------------
void LSPSMCalculator::SetLightDir( const D3DXVECTOR3 &value )
{
	
	assert( D3DXVec3LengthSq(&value)!=0.f );
	D3DXVec3Normalize( &mLightDirection, &value );
	//mLightDirection = value;
}

//-----------------------------------------------------------------------
// Name : SetEyeView()
// Desc : �J�����̃r���[�s���ݒ�
//-----------------------------------------------------------------------
void LSPSMCalculator::SetEyeView( const D3DXMATRIX &value )
{
	mEyeView = value; 
}

//-----------------------------------------------------------------------
// Name : SetEyeProjection()
// Desc : �J�����̎ˉe�s���ݒ�
//-----------------------------------------------------------------------
void LSPSMCalculator::SetEyeProjection( const D3DXMATRIX &value )
{
	mEyeProjection = value;
}

//-----------------------------------------------------------------------
// Name : SetNearClip()
// Desc : �j�A���ʂ܂ł̋�����ݒ肷��
//-----------------------------------------------------------------------
void LSPSMCalculator::SetNearClip( float value )
{
	mNearClip = value;
}

//-----------------------------------------------------------------------
// Name : SetUseLSPSM()
// Desc : �t���O�̐ݒ�
//-----------------------------------------------------------------------
void LSPSMCalculator::SetUseLSPSM( bool value )
{
	mUseLSPSM = value;
}

//-----------------------------------------------------------------------
// Name : GetLightView()
// Desc : ���C�g�̃r���[�s����擾����
//-----------------------------------------------------------------------
const D3DXMATRIX& LSPSMCalculator::GetLightView()const
{
	return mLightView; 
}

//-----------------------------------------------------------------------
// Name : GetLightProjection()
// Desc : ���C�g�̎ˉe�s����擾����
//-----------------------------------------------------------------------
const D3DXMATRIX& LSPSMCalculator::GetLightProjection()const
{
	return mLightProjection;
}

//-----------------------------------------------------------------------
// Name : GetLightViewProjection()
// Desc : ���C�g�̃r���[�ˉe�s����擾����
//-----------------------------------------------------------------------
const D3DXMATRIX& LSPSMCalculator::GetLightViewProjection()const
{
	return mLightViewProjection;
}


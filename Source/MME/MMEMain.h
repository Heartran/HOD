#pragma once
#include"Object/IObject.h"
#include<GameLib/Graphics/Texture.h>
#include<d3dx9.h>

#include<GameLib/Math/Vector3.h>

namespace MME{

	void regist( Object::IObjectSharedPtr obj, const char *fxFileName, const char *path=0 );
	void unregist( Object::IObjectSharedPtr obj );

	void registPostEffect(  const char *fxFileName, const char *path=0 );

	void preExe();
	void postExe();

	void drawAll();

	///�ݒ肵���e�N�X�`���A���_�o�b�t�@�A�C���f�N�X�o�b�t�@�ŕ`��
	void drawIndexed(
		int offset, 
		int primitiveNumber );
	///�ݒ肵���e�N�X�`���A���_�o�b�t�@�ŕ`��
	void draw( 
		int offset, 
		int primitiveNumber );

	//void setVertexBuffer( VertexBuffer );
	//void setIndexBuffer( IndexBuffer );
	//void setAnimatedVertexBoneBuffer( AnimatedVertexBoneBuffer );
	void setTexture( GameLib::Graphics::Texture );

	//�r���[�ϊ��s��Z�b�g
	void setViewMatrix( const D3DXMATRIX& m );
	///�����ϊ��s��Z�b�g
	void setProjectionMatrix( const D3DXMATRIX& m );
	///���[���h�ϊ��s��Z�b�g
	void setWorldMatrix( const D3DXMATRIX& m );

	using GameLib::Math::Vector3;
	///���C�g�ʒu
	void setLightPosition( int index, const Vector3& position );
	///���C�g����
	void setLightDirection( int index, const Vector3& dir );
	///���C�g���x(100�Ȃ�100m�̋�����1�̖��邳�ɂȂ�B���xI�Ƃ��āA���邳�́uI/�����v�Ō��܂�)
	//���C�g�����ɑ΂��Đ��K�����ď����邾���̗\��
	//void setLightIntensity( int index, float intensity );
	///���C�g�F
	void setLightColor( int index, const Vector3& );
	///���_�ʒu
	void setEyePosition( const Vector3& position );
	void setEyeDirection( const Vector3& dir );
	///�f�B�t���[�Y�J���[
	void setDiffuseColor( const Vector3& );
	///�X�y�L�����F
	void setSpecularColor( const Vector3& );
	///�����F
	void setEmissionColor( const Vector3& );
	///�A���r�G���g�F
	void setAmbientColor( const Vector3& );
	///�����x�ݒ�
	void setTransparency( float );
	///�X�y�L�����̉s��(�傫���قǉs���B�����̈Ӗ��͕ʂ̖{��ǂ���)
	void setSpecularSharpness( float );
	
	void sendParamsToGameLibGraphicsManager();
}
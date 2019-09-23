#pragma once
#include<d3dx9.h>
#include"formats.h"
#include<memory>
#include<string>
#include<vector>


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif//SAFE_RELEASE


namespace MME {
class BaseEffect;
class RenderTarget;
typedef std::shared_ptr<RenderTarget> RenderTargetSharedPtr;

class RenderTarget {
public:
	static RenderTargetSharedPtr create(  ID3DXEffect *effect, D3DXHANDLE hTex );
	static void clearCache();
	static void shrinkToFitCache();
	~RenderTarget();

	bool isError()const;

	void send(ID3DXEffect *effect);

	//IDirect3DSurface9* getSurfLevel( UINT level );


	//���O���ړI��Ǝ��ł��Ԃ��Ă��邪�킩��₷�����炱�̖��O�ɂ���
	//�����Ƃ��Ă�
	//	setToDevice
	//	setSelf
	void setRenderTarget( int renderTargetIndex, BaseEffect* effect );

	const std::string & getName()const;

	//�f�o�C�X���X�g���̂��߂̊J������
	void backup();

	//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
	void recover();

private:
	RenderTarget( ID3DXEffect *effect, D3DXHANDLE hTex );

	RenderTarget( const RenderTarget &src, ID3DXEffect *effect, D3DXHANDLE hTex );

	std::string mName;//script�Ŏg�p
	//	���R�}���hS
	//�Erendercolortarget=(�e�N�X�`���� or ��)
	//�Erendercolortarget0=(�e�N�X�`���� or ��)
	//�Erendercolortarget1=(�e�N�X�`���� or ��)
	//�Erendercolortarget2=(�e�N�X�`���� or ��)
	//�Erendercolortarget3=(�e�N�X�`���� or ��)
	//�@�@�����_�����O�^�[�Q�b�g��ݒ肷��B

	D3DXHANDLE mHandle;
	std::shared_ptr<IDirect3DTexture9> mTexture;
	std::shared_ptr<IDirect3DSurface9> mSurface;
	D3DFORMAT mFormat;
	unsigned int mHeight;
	unsigned int mWidth;
	int mMipLevels;
	bool mIsShared;

	static std::vector< std::weak_ptr<RenderTarget> > sCache;

};

}// end namespace MME
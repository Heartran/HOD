#pragma once
#include<d3dx9.h>
#include"formats.h"
#include<string>
#include<memory>
#include<vector>

namespace MME {
class RenderDepthStencil;
typedef std::shared_ptr<RenderDepthStencil> RenderDepthStencilSharedPtr;
class RenderDepthStencil {
public:
	static RenderDepthStencilSharedPtr create(  ID3DXEffect *effect, D3DXHANDLE hTex );
	static void clearCache();
	static void shrinkToFitCache();
	~RenderDepthStencil();
	bool isError()const;
	
	//IDirect3DSurface9* getZBuffer();

	void setDepthStencilSurface();

	const std::string & getName()const;

	void send(ID3DXEffect *effect);

	//�f�o�C�X���X�g���̂��߂̊J������
	void backup();
	//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
	void recover();

private:
	RenderDepthStencil( ID3DXEffect *effect, D3DXHANDLE hTex);
	RenderDepthStencil( const RenderDepthStencil &src, ID3DXEffect *effect, D3DXHANDLE hTex);
	
	std::string mName;//script�Ŏg�p
	//RenderDepthStencilTarget=(�e�N�X�`���� or ��)
	D3DXHANDLE mHandle;
	std::shared_ptr<IDirect3DTexture9> mTexture;
	std::shared_ptr<IDirect3DSurface9> mZBuffer;
	D3DFORMAT mFormat;
	unsigned int mHeight;
	unsigned int mWidth;
	//int mMipLevels;

	static std::vector< std::weak_ptr<RenderDepthStencil> > sCache;

};

}// end namespace MME
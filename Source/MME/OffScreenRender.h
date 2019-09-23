#pragma once
#include<d3dx9.h>
#include"formats.h"
#include<string>
#include<vector>
#include<GameLib/Math/Vector4.h>
#include"StandardEffect.h"

namespace MME {
class BaseEffect;
class OffScreenRender;
typedef std::shared_ptr<OffScreenRender> OffScreenRenderSharedPtr;


class OffScreenRender {
public:
	static OffScreenRenderSharedPtr create( ID3DXEffect *effect, D3DXHANDLE hTex, const std::string& currentPath, LPD3DXEFFECTPOOL pool );
	static void clearCache();
	static void shrinkToFitCache();

	~OffScreenRender();

	bool isError()const;

	void send(ID3DXEffect *effect);


	void begin();
	void end();
	void draw();

	LPDIRECT3DTEXTURE9 getDxTexture();


	const std::string & getName()const;

	//void removeRemovableObjects();
	
	void add( const Object::IObjectSharedPtr &obj );
	
	void clearAllObjects();
	
	//���݂��Ȃ��ꍇNULL��Ԃ�
	StandardEffectSharedPtr findStandardEffect( const std::string &name );

	//�I�u�W�F�N�g�̃��f��������͂Ƃ��āA
	//mDefalutEffects�̒����琳�K�\���Ń}�b�`����G�t�F�N�g��T���B
	//�Ȃ������ꍇfalse��Ԃ�
	bool findProperEffectName( std::string *found, const Object::IObjectSharedPtr &obj );

	//�f�o�C�X���X�g���̂��߂̊J������
	void backup();
	//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
	void recover();

	std::string findStandardEffect( const Object::IObjectSharedPtr &obj ); 

	unsigned int getNumberOfStandardEffects()const;
	StandardEffectSharedPtr getStandardEffectAt( unsigned int i);
private:
	OffScreenRender( ID3DXEffect *effect, D3DXHANDLE hTex, const std::string& currentPath, LPD3DXEFFECTPOOL pool );
	OffScreenRender( const OffScreenRender &src, ID3DXEffect *effect, D3DXHANDLE hTex, const std::string& currentPath, LPD3DXEFFECTPOOL pool );
	
	std::string mName;

	D3DXHANDLE mHandle;
	std::shared_ptr<IDirect3DTexture9> mTexture;
	std::shared_ptr<IDirect3DSurface9> mSurface;

	std::shared_ptr<IDirect3DSurface9> mZBuffer;

	
	LPDIRECT3DSURFACE9      mTempBackbuffer;	// �ޔ�p�o�b�N�o�b�t�@�[
	LPDIRECT3DSURFACE9		mTempBackZ;			// �ޔ�p�o�b�N�o�b�t�@�[�p�̐[�x�o�b�t�@�[
	D3DVIEWPORT9			mTempViewport;			// �ޔ�p�r���[�|�[�g


	// �r���[�|�[�g���̕ێ�
	D3DVIEWPORT9 viewport1;

	GameLib::Math::Vector4	mClearColor;
	float					mClearDepth;
	int						mIsAntiAilias;
	std::string				mDescription;

	//�Estring DefaultEffect
	//�@�I�t�X�N���[�������_�����O�Ŏg�p����G�t�F�N�g�t�@�C���̊��蓖�ĕ��@���w�肷��B
	//�@�P�̊��蓖�ẮA�ȉ��̏����ŋL�q����B
	//�@
	//�@�@�@"(�I�u�W�F�N�g�t�@�C����)=(�G�t�F�N�g�t�@�C����);"
	//�@�@
	//�@�@�I�u�W�F�N�g���Ɏg�p����G�t�F�N�g�t�@�C����؂�ւ���ɂ́A���̊��蓖�Ă𕡐���L�q����B
	//�@�@������L�q�����ꍇ�A�L�q�������ŃI�u�W�F�N�g�t�@�C��������r����A�ŏ��Ƀ}�b�`�������̂��g�p�����B
	//�@�@
	//�@�@�@��F string DefaultEffect = "self=hide; Mirror*.x=hide; *=MirrorObject.fx;";
	//�@�@
	//�@�@�I�u�W�F�N�g�t�@�C�����ɂ�"*"��"?"�ɂ�郏�C���h�J�[�h���w��ł���B
	//�@�@����ȃI�u�W�F�N�g�t�@�C�����Ƃ���"self"���w��ł��A����́A
	//�@�@����OFFSCREENRENDERTARGET�����G�t�F�N�g�����蓖�Ă�ꂽ�I�u�W�F�N�g���g��\���B
	//�@�@
	//�@�@�G�t�F�N�g�t�@�C�����ɑ��΃p�X�Ńt�@�C�������w�肵���ꍇ�A
	//�@�@�Q�ƌ��̃G�t�F�N�g�t�@�C�����i�[����Ă���t�H���_����ƂȂ�B
	//�@�@
	//�@�@�܂��A����ȃG�t�F�N�g�t�@�C�����Ƃ���"none"��"hide"���w��ł��A
	//�@�@����́A�u�G�t�F�N�g�Ȃ��v�Ɓu��\���v��\���B
	//�@�@�܂��A"main_default"���w�肷��ƁA���C���X�N���[���ł̃f�t�H���g����Ɠ��l�ȁA
	//�@�@�I�u�W�F�N�g�t�@�C���p�X�Ɋ�Â�fx�t�@�C����emd�t�@�C���̎����������s����B
	//�@�@
	struct DefalutEffect;
	std::vector<DefalutEffect*> mDefalutEffects;
	
	std::vector<StandardEffectSharedPtr> mEffects;

	D3DFORMAT mFormat;
	unsigned int mHeight;
	unsigned int mWidth;
	int mMipLevels;

	static std::vector< std::weak_ptr<OffScreenRender> > sCache;

};

}// end namespace MME
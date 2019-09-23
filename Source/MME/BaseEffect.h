#pragma once
#include<d3dx9.h>
#include"helper/MyOutputDebugString.h"
#include<vector>
#include<list>
#include"Enums.h"
#include<GameLib/Graphics/Texture.h>
#include<GameLib/Math/Vector4.h>
#include<memory>
#include<string>
#include<boost/unordered/unordered_map_fwd.hpp>
#include<boost/xpressive/xpressive_fwd.hpp>
#include"Object/IObject.h"
#include<GameLib/FileIO/InFile.h>


namespace MME {
	class Texture;
	typedef std::shared_ptr<Texture> TextureSharedPtr;
	class RenderTarget; 
	typedef std::shared_ptr<RenderTarget> RenderTargetSharedPtr;
	class RenderDepthStencil;
	typedef std::shared_ptr<RenderDepthStencil> RenderDepthStencilSharedPtr;

	class Technique;
	class Manager;
	class BaseTechnique;
	class StandardTechnique;
	class OffScreenRender;
	typedef std::shared_ptr<OffScreenRender> OffScreenRenderSharedPtr;
	class ControlObjectSelf;
	class BaseEffect;
typedef std::shared_ptr<BaseEffect> BaseEffectSharedPtr;
class BaseEffect {
public:
	enum ScriptOrder {
		SO_STANDARD=0,
		SO_POST_PROCESS=1,

	};

	BaseEffect( const std::string &fxFileName, GameLib::FileIO::InFile file, LPD3DXEFFECTPOOL pool );
	virtual ~BaseEffect();
	
	//name.empty()�̏ꍇNULL��Ԃ�
	RenderTargetSharedPtr findRenderTarget( const std::string &name );
	//name.empty()�̏ꍇNULL��Ԃ�
	RenderDepthStencilSharedPtr findRenderDepthStencil( const std::string &name );
	//name.empty()�̏ꍇNULL��Ԃ�
	OffScreenRenderSharedPtr findOffScreen(  const std::string &name );

	//RenderTarget��NULL���ƃf�t�H���g�̃o�b�N�o�b�t�@�ɖ߂�
	void setRenderTarget( int renderTargetIndex, RenderTarget *texture );
	//RenderTarget��NULL���ƃf�t�H���g��ZBuffe�ɖ߂�
	void setRenderDepthStencil( RenderDepthStencil *rds );

	void clearRenderTarget( float r, float g, float b, float a );
	void clearRenderDepthStencil( float z );

	bool isError()const{
		return mIsError;
	}
	const std::string& getName()const{
		return mName;
	}

	void sendMatrices(
		const D3DXMATRIX &w,
		const D3DXMATRIX &v,
		const D3DXMATRIX &p );

	void sendLightMatrices(
		const D3DXMATRIX &w,
		const D3DXMATRIX &v,
		const D3DXMATRIX &p );


	void setTexture( GameLib::Graphics::Texture tex );
	///���C�g�ʒu
	void setLightPosition( const GameLib::Math::Vector4& position );
	///���C�g����
	void setLightDirection( const GameLib::Math::Vector4& dir );
	///���C�g���x(100�Ȃ�100m�̋�����1�̖��邳�ɂȂ�B���xI�Ƃ��āA���邳�́uI/�����v�Ō��܂�)
	//���C�g�����ɑ΂��Đ��K�����ď����邾���̗\��
	//void setLightIntensity( int index, float intensity );
	///���C�g�F
	//void setLightColor( const GameLib::Math::Vector4& );
	void setLightDiffuse( const GameLib::Math::Vector4& );
	void setLightAmbient( const GameLib::Math::Vector4& );
	void setLightSpecular( const GameLib::Math::Vector4& );
	///���_�ʒu
	void setEyePosition( const GameLib::Math::Vector4& position );
	void setEyeDirection( const GameLib::Math::Vector4& dir );
	///�f�B�t���[�Y�J���[
	void setDiffuseColor( const GameLib::Math::Vector4& );
	///�X�y�L�����F
	void setSpecularColor( const GameLib::Math::Vector4& );
	///�����F
	void setEmissionColor( const GameLib::Math::Vector4& );
	///�A���r�G���g�F
	void setAmbientColor( const GameLib::Math::Vector4& );
	///�����x�ݒ�
	//vector4��4�ԖڂɂȂ��Ă���
	//void setTransparency( float );
	///�X�y�L�����̉s��
	void setSpecularSharpness( float );
	//toonColor
	void setToonColor( const GameLib::Math::Vector4& );
	void setToonTexture( GameLib::Graphics::Texture toonTexture );

	void sendWorldMatrixArray( const D3DXMATRIX *WorldMatrices, int size );
	
	
	//"TIME"�́A0�t���[���ڂ���̍Đ����Ԃ�\���B
	//�Ⴆ�΁A0�t���[���ڂł́A0.0�i�b�j�ƂȂ�A45�t���[���ڂł�1.5�i�b�j�ƂȂ�B
	//���ꏭ�Ȃ��t���[�����Ȃ�ǂ����Ǒ����Ȃ��float�덷�Œl���ς��Ȃ��Ȃ�
	//�o�O�ɂȂ鋰�ꂪ�A
	//���������Q�[���J�n����̌o�ߎ��ԂƂ��I�[�o�[�t���[������O��ł�邵��
	void setTime( float time );

	void setElapsedTime( float elapsedTime );

	void setEdgeColor( const GameLib::Math::Vector4& c );

	void setZNear( float znear );
	void setZFar( float zFar );

	//void enableVertexBlend( bool enable );

	//void setParams( Manager *mngr );

	void commitChanges();

	//�����_���[�^�[�Q�b�g�₳�܂��܂ȃe�N�X�`�����G�t�F�N�g�ɃZ�b�g
	void setTextures();

	void setViewportPixelSize( float x, float y );

	void setSphereTexture( GameLib::Graphics::Texture toonTexture );
	void setSphereTextureAdditive( bool isAdditive );

	
	bool canDraw( UseTexture useTexture, UseSphereMap useSphereMap, UseToon useToon, unsigned int materialIndex ) const;
	MMDPass getCurrentPass()const{
		return mPass;
	}

	void getCurrentPass(
		MMDPass *pass,
		UseTexture *useTexture,
		UseSphereMap *useSphereMap,
		UseToon *useToon,
		UseSelfShadow *useSelfShadow )const
	{
		*pass = mPass;
		*useTexture = mUseTexture;
		*useSphereMap = mUseSphereMap;
		*useToon = mUseToon;
		*useSelfShadow = mUseSelfShadow;
	}

	void drawOffScreen();

	//MMDPass Object_SS �� UseSelfShadow�@���������邽�߂Ɏg�p
	bool isThereObject_SSInShader()const{
		return mIsThereObject_SSInShader;
	}
	//�f�o�C�X���X�g���̂��߂̊J������
	void backup();
	//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
	void recover();
	
	
	void releaseCurrentBackBuffers();

	bool enable()const{
		return mIsEnable;
	}
	//predraw��postdraw�Ƃ��`�撆�ɌĂԂƃ����_�����O�^�[�Q�b�g��
	//�o�O��̂ŁA���̊Ԃ͐�΂ɌĂяo���Ȃ�����
	bool setEnable(bool e){
		return mIsEnable=e;
	}

	//void setFloat( const std::string &name, float value );
	//void setFloatArray( const std::string &name, const float *pValue, unsigned int count );
	//void setInt( const std::string &name, int value );
	//void setIntArray( const std::string &name, const int *pValue, unsigned int count );

	D3DXHANDLE getHandle(const std::string &name )const;

	void setFloat( D3DXHANDLE h, float value );
	void setFloatArray( D3DXHANDLE h, const float *pValue, unsigned int count );
	void setInt( D3DXHANDLE h, int value );
	void setIntArray( D3DXHANDLE h, const int *pValue, unsigned int count );


	//void add( const Object::IObjectSharedPtr& );
	
	//void removeObjectsAsPossible();
	//void removeObjectsThatCanRemove();
	//void removeRemovableObjects();

	void drawGeometry();

	//void matchByModelFileName( std::vector<Object::IObjectSharedPtr> *out, const boost::xpressive::sregex & filename  )const;

	//void clearAllObjects();


	//vector�ɂ��闘�_�@�������g��Ȃ��A���т��ǉ����ɂȂ�
	//list�ɂ��闘�_�@�폜�}���������A���т��ǉ����ɂȂ�
	//set�ɂ��闘�_�A�폜�}��find��O(log n) 2�d�ǉ��������Ȃ� ���т����������ɂȂ�
	//unordered_set �폜�}��find������(���������������)�A���Ԃ΂�΂�
	typedef std::vector<Object::IObjectWeakPtr> DrawObjects;
	typedef std::shared_ptr< DrawObjects > DrawObjectsSharedPtr;
	void setObjectsPtr(const DrawObjectsSharedPtr &);
	
	unsigned int getNumberOfOffScreens()const;
	OffScreenRenderSharedPtr getOffScreenAt( unsigned int i);
protected:
	//disallow
	BaseEffect(const BaseEffect&);
	//disallow
	void operator=(const BaseEffect&);
	
	//�X�N���v�g�̋󔒂��������邽�߂ɕK�v
	//�󔒂ɓ�����RT��ۑ�����
	//RenderColorTarget=(�e�N�X�`���� or ��)
	//�ERenderColorTarget0=(�e�N�X�`���� or ��)
	//�ERenderColorTarget1=(�e�N�X�`���� or ��)
	//�ERenderColorTarget2=(�e�N�X�`���� or ��)
	//�ERenderColorTarget3=(�e�N�X�`���� or ��)
	//�ERenderDepthStencilTarget=(�e�N�X�`���� or ��)
	void saveCurrentBackBuffers();

	void setUseFlag()const;

	//void clearAllObjectsInOffScreen();

	const std::string mName;
	ID3DXEffect *mEffect;
	
	enum {
		kMatrixHandlesSize=24
	};
	//enum {
	//	kFloat4HandlesSize=13
	//}; 
	D3DXHANDLE mMatrixHandles[kMatrixHandlesSize];
	D3DXHANDLE mLightMatrixHandles[kMatrixHandlesSize];
	
	D3DXHANDLE mWorldMatixArrayHandle;
	//D3DXHANDLE mFloat4Handles[kFloat4HandlesSize];

	//float4 Handles
	//geometory
	D3DXHANDLE mDiffuse;      
	D3DXHANDLE mAmbient;      
	D3DXHANDLE mSpecular;      
	D3DXHANDLE mPosition;//camera
	D3DXHANDLE mDirection;//camera
	D3DXHANDLE mEdgeColor;
	D3DXHANDLE mEmissive;     
	D3DXHANDLE mToonColor;    
	//light
	D3DXHANDLE mLightDiffuse;      
	D3DXHANDLE mLightAmbient;      
	D3DXHANDLE mLightSpecular;      
	D3DXHANDLE mLightPosition;
	D3DXHANDLE mLightDirection;

	//float Handle
	D3DXHANDLE mSpecularPowerHandle;
	D3DXHANDLE mTimeHandle;
	D3DXHANDLE mElapsedTimeHandle;
	D3DXHANDLE mStandardGlobalHandle;
	
	//float2
	D3DXHANDLE mViewPortPixelSize;
	
	//etc
	//bool	 spadd;    // �X�t�B�A�}�b�v���Z�����t���O
	D3DXHANDLE mSpAddHandle;
	//bool     parthf;   // �p�[�X�y�N�e�B�u�t���O
	D3DXHANDLE mParthfHandle;
	//bool     transp;   // �������t���O
	D3DXHANDLE mTranspHandle;
	
	D3DXHANDLE mUseTextureHandle;
	D3DXHANDLE mUseSphereMapHandle;
	D3DXHANDLE mUseToonHandle;

	D3DXHANDLE mMaterialTextureHandle;
	D3DXHANDLE mSphereMapTextureHandle;
	D3DXHANDLE mToonTextureHandle;

	D3DXHANDLE mZNearHandle;
	D3DXHANDLE mZFarHandle;

	
	std::vector<TextureSharedPtr> mTextures;
	std::vector<RenderTargetSharedPtr> mRenderTargets;
	std::vector<RenderDepthStencilSharedPtr> mRenderDepthStencils;
	std::vector<OffScreenRenderSharedPtr> mOffScreenRenders;

	//key: �ϐ����Avalue:�@�ϐ��ւ̃n���h��
	//boost::unordered_map< std::string, D3DXHANDLE > *mVariables;

	ControlObjectSelf* mControlObjectSelf;

	enum{
		kNumBackBuffers=4,
	};
	LPDIRECT3DSURFACE9 mDefaultBackBuffer[kNumBackBuffers];
	LPDIRECT3DSURFACE9 mDefaultZBuffer;

	bool mIsError;

	ScriptOrder mScriptOrder;

	StandardTechnique *mCurrentTech;

	//���݂̕`����
	MMDPass mPass;
	UseTexture mUseTexture;
	UseSphereMap mUseSphereMap;
	UseToon mUseToon;
	UseSelfShadow mUseSelfShadow;

	//shader����object_ss���g���Ă��邩?
	//�g���Ă����ꍇ�AMMDass��object��useSelfShadow�̎w�肪�Ȃ����̂�useSelfShadow=false�ɂ���
	bool mIsThereObject_SSInShader;
	static GameLib::Graphics::Texture msWhiteTexture; 

	bool mIsEnable;

	
	DrawObjectsSharedPtr mObjects;

};

}//end namespace MME
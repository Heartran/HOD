#pragma once
#include<memory>
#include<d3dx9.h>
#include<map>
#include"Enums.h"
#include"StandardEffect.h"
#include"PostEffect.h"
#include<GameLib/Math/Vector3.h>
#include<GameLib/Math/Vector4.h>
#include<GameLib/Graphics/Texture.h>
#include<GameLib/Graphics/VertexBuffer.h>
#include<GameLib/Graphics/IndexBuffer.h>
#include<GameLib/Graphics/AnimatedVertexBoneBuffer.h>
#include<GameLib/FileIO/InFile.h>
#include<boost/xpressive/xpressive_fwd.hpp>
#include<cassert>

namespace MME {

class BaseEffect;

class Manager {
public:
	//�Q�[���I�u�W�F�N�g����fx�t�@�C�����w�肵���ق����֗����낤�Ǝv�������A
	//(���������~�N���f���ł��Ⴄ�G�t�F�N�g�Ƃ��K�p�ł��邩��)
	//�ގ����Ɏw��ł���悤�ɂ����ق����֗��ȏ�ʂ������˂Ƃ���������
	//��ŃG�t�F�N�g�t�@�C�����Ƃɂ܂Ƃ߂ĕ`�悷��̂ŁAGameObject��n���͕̂K�{�B
	//���񂾏ꍇ(�������J��)���ǂ����邩(weak_ptr���H)
	//�`�揇�����O��(priority�����ׂ����H �Ƃ肠�����͖��O���H�v���ĕ`�揇���R���g���[������)
	StandardEffectSharedPtr registStandardEffect( const char *fxFileName, GameLib::FileIO::InFile file );

	//void addToStandardEfffect(  const Object::IObjectSharedPtr &obj, const char *fxFileName );

	//�폜����ꍇ�́AdrawAll�̈�ԏ��߂�obj->canRemove()�Ŋm���߂č폜����B
	//remove���������Ȃ������̂͂ǂ�standardEffect��offscreen��obj������̂��킩��Ȃ��̂ŁA
	//�S�����ׂ�K�v������B����͔�������Ɗ���������
	// void remove( Object::IObjectSharedPtr &obj );	
	// void removet( Object::IObjectSharedPtr &obj, const char *fxFileName );
	
	//void registStandardEffect( const char *fxFileName );
	//void unregistStandardEffect(  const char *fxFileName );

	//�|�X�g�G�t�F�N�g�}�b�v�ɓo�^
	//�Ȃ���ΐV���ɍ��
	//�G���[���N���Ă���0��Ԃ�
	//�`�揇�����O��(priority�����ׂ����H �Ƃ肠�����͖��O���H�v���ĕ`�揇���R���g���[������)
	PostEffectSharedPtr registPostEffect( const char *fxFileName, GameLib::FileIO::InFile file );
	//void unregistPostEffect(  const char *fxFileName );

	//void addToPostEffectEfffect(  const Object::IObjectSharedPtr &obj, const char *fxFileName );

	bool isReady();

	void drawAll();

	void drawIndexed(
		int offset, 
		int primitiveNumber );
	
	void draw( 
		int offset, 
		int primitiveNumber );

	void setVertexBuffer( GameLib::Graphics::VertexBuffer );
	void setIndexBuffer( GameLib::Graphics::IndexBuffer );
	void setAnimatedVertexBoneBuffer( GameLib::Graphics::AnimatedVertexBoneBuffer );
	
	void setTexture( GameLib::Graphics::Texture );
	void setToonTexture(  GameLib::Graphics::Texture );
	void setSphereTexture(  GameLib::Graphics::Texture );
	void setSphereTextureAdditive( bool isAdditive );

	//�r���[�ϊ��s��Z�b�g
	void setViewMatrix( const D3DXMATRIX& view );
	///�����ϊ��s��Z�b�g
	void setProjectionMatrix( const D3DXMATRIX& proj );
	///���[���h�ϊ��s��Z�b�g
	void setWorldMatrix( const D3DXMATRIX& world );

	//�r���[�ϊ��s��Q�b�g
	const D3DXMATRIX & Manager::getViewMatrix()const;
	//�����ϊ��s��Q�b�g
	const D3DXMATRIX & Manager::getProjectionMatrix()const;
	//���[���h�ϊ��s��Q�b�g
	const D3DXMATRIX & Manager::getWorldMatrix()const;


	//���C�g�r���[�ϊ��s��Z�b�g
	void setLightViewMatrix( int index, const D3DXMATRIX& lightView );
	///���C�g�����ϊ��s��Z�b�g
	void setLightProjectionMatrix( int index, const D3DXMATRIX& lightProjection );
	///���C�g���[���h�ϊ��s��Z�b�g
	void setLightWorldMatrix( int index, const D3DXMATRIX& lightWorld );

	///���C�g�ʒu
	void setLightPosition( int index, const GameLib::Math::Vector3& position );
	///���C�g����
	void setLightDirection( int index, const GameLib::Math::Vector3& dir );
	///���C�g���x(100�Ȃ�100m�̋�����1�̖��邳�ɂȂ�B���xI�Ƃ��āA���邳�́uI/�����v�Ō��܂�)
	//���C�g�����ɑ΂��Đ��K�����ď����邾���̗\��
	//void setLightIntensity( int index, float intensity );
	
	///���C�g�F
	//�Ƃ肠�����S��(diffuse, ambient, specular)�����l�ɂ���
	void setLightColor( int index, const GameLib::Math::Vector4& );
	void setLightDiffuse( int index, const GameLib::Math::Vector4 &d );
	void setLightAmbient( int index, const GameLib::Math::Vector4 &a );
	void setLightSpecular( int index, const GameLib::Math::Vector4 &s );

	///���_�ʒu
	void setEyePosition( const GameLib::Math::Vector3& position );
	void setEyeDirection( const GameLib::Math::Vector3& dir );
	///�f�B�t���[�Y�J���[
	void setDiffuseColor( const GameLib::Math::Vector4& );
	///�X�y�L�����F
	void setSpecularColor( const GameLib::Math::Vector4& );
	///�����F
	void setEmissionColor( const GameLib::Math::Vector4& );
	///�A���r�G���g�F
	void setAmbientColor( const GameLib::Math::Vector4& );
	///�֊s�F
	void setEdgeColor( const GameLib::Math::Vector4& );

	void setZNear( float );
	void setZFar( float );

	///�����x�ݒ�(gameLib�̌݊����̂��߂ɁA�����������Ȃ��ȁB)
	//diffuse, specular, ambient�̃A���t�@�l�ɃZ�b�g����
	//void setTransparency( float );

	void setToonColor( const GameLib::Math::Vector4& );


	///�X�y�L�����̉s��(�傫���قǉs��
	void setSpecularSharpness( float );
	void sendWorldMatrixArray( const D3DXMATRIX *WorldMatrices, int size );
	void enableVertexBlend( bool enable );

	GameLib::Math::Vector3 getLightDirection( int index ) const;
	
	
	void sendParamsToGameLibGraphicsManager();

	bool canDraw(
		UseTexture useTexture,
		UseSphereMap useSphereMap,
		UseToon useToon,
		unsigned int materialIndex )const;
	
	MMDPass getCurrentPass()const;

	void getCurrentPass(
		MMDPass *pass,
		UseTexture *useTexture,
		UseSphereMap *useSphereMap,
		UseToon *useToon,
		UseSelfShadow *useSelfShadow )const;


	//void matchByModelFileName( std::vector<Object::IObjectSharedPtr> *out, const boost::xpressive::sregex & filename  )const;
	
	void setCurrentEffect( BaseEffect *effect );

	void sendParams( BaseEffect* mme  );

	//offscreen���͒T���Ȃ�
	//������Ȃ��ꍇ��nullptr��Ԃ�
	StandardEffectSharedPtr findStandardEffect( const std::string &fullFileName );
	//offscreen���͒T���Ȃ�
	//������Ȃ��ꍇ��nullptr��Ԃ�
	PostEffectSharedPtr findPostEffect( const std::string &fullFileName );

	//offscreen�Ȃ����T��
	//������Ȃ��ꍇ��nullptr��Ԃ�
	StandardEffectSharedPtr findStandardEffect( const std::string &fullFileName, const std::string &offScreenName, const std::string &effectFileNameInOffScreen );

	//void clearAllObjects();

	//�f�o�C�X���X�g���̂��߂̊J������
	void backup();
	//�f�o�C�X���X�g���̂��߂̃��\�[�X�Ċm��
	void recover();

	void beginShadow();
	void endShadow();

	//debug�p
	void drawShadowMapBuffer()const;
private:
	void loadDefalutEffect();

	//�E�Ó��łȂ��e�N�j�b�N�͏��O�����B
	//�@http://msdn.microsoft.com/ja-jp/library/bb206324(v=VS.85).aspx
	//�EMMDPass="object","object_ss"�ȊO�̃e�N�j�b�N�ł́AUseTexture,UseSphereMap,UseToon�͐������@�\���Ȃ��B
	//�|�X�g�G�t�F�N�g�̑O����
	//MME�̃X�N���v�g��ScriptExternal=Color�̑O�̏���������
	void preExe();
	//�|�X�g�G�t�F�N�g�̌㏈��
	//MME�̃X�N���v�g��ScriptExternal=Color�̌�̏���������
	void postExe();

	//�e�e�N�X�`��������register0�ɃZ�b�g����
	//@param
	//enableFilter �e���ڂ������H
	void drawZPlot( bool enableFilter=true );

	void sendParamsIfIsChanged();

	

	//debug�p
	void drawOffScreen( const char* effectFileName, const char *offscreenName )const;

	
	typedef std::map< const std::string, StandardEffectSharedPtr > StandardMap;
	StandardMap mStandards;
	typedef std::map< const std::string, PostEffectSharedPtr > PostMap;
	PostMap mPosts;

	BaseEffect *mCurrentEffect;
	LPD3DXEFFECTPOOL mPool;


	//���ԃf�[�^�Ƃ��č�������ǁA����Ȃ��񂶂�Ȃ����H
	//�ł��AGameLib�ɐ؂�ւ���Ƃ��~�����C������B�G�t�F�N�g(GPU)���������Ă���΂������
	struct Material{
		Material()
			:mDiffuse(0.f, 0.f, 0.f, 0.f ),
			mAmbient(0.f, 0.f, 0.f, 0.f ),
			mSpecular(0.f, 0.f, 0.f, 0.f ),
			mToonColor(0.f, 0.f, 0.f, 0.f ),
			mIsChanged(true)
		{}
		~Material(){}
		void setDiffuse( const GameLib::Math::Vector4 &d ){
			mDiffuse = d;
			mIsChanged = true;
		}
		void setAmbient( const GameLib::Math::Vector4 &a ){
			mAmbient = a;
			mIsChanged = true;
		}
		void setSpecular( const GameLib::Math::Vector4 &s ){
			mSpecular = s;
			mIsChanged = true;
		}

		void setEmission( const GameLib::Math::Vector4 &e ){
			mEmission = e;
			mIsChanged = true;
		}

		void setSpecularSharpness( float sharpness ){
			mSharpness = sharpness;
			mIsChanged = true;
		}

		void setToonColor( const GameLib::Math::Vector4& toon ){
			mToonColor = toon;
			mIsChanged = true;
		}

		void setTransparency( float t ){
			mDiffuse.w=t;
			mAmbient.w=t;
			mSpecular.w=t;
			mEmission.w=t;
			mIsChanged = true;
		}

		GameLib::Math::Vector4 mDiffuse;
		GameLib::Math::Vector4 mAmbient;
		GameLib::Math::Vector4 mSpecular;
		GameLib::Math::Vector4 mEmission;
		GameLib::Math::Vector4 mToonColor;

		float mSharpness;
		bool mIsChanged;
	};

	enum{
		LIGHT_COUNT=4,
	};
	struct LightMaterials {
		LightMaterials()		
			:mIsChanged(true)
		{
			for(int i=0; i<LIGHT_COUNT; ++i ){
				mDiffuse[i].set(1.f, 1.f, 1.f, 1.f );
				mAmbient[i].set(1.f,1.f, 1.f, 1.f );
				mSpecular[i].set(1.f,1.f, 1.f, 1.f );
			}
		}
		void setDiffuse( int index, const GameLib::Math::Vector4 &d ){
			assert( index<LIGHT_COUNT );
			mDiffuse[index] = d;
			mIsChanged = true;
		}
		void setAmbient( int index, const GameLib::Math::Vector4 &a ){
			assert( index<LIGHT_COUNT );
			mAmbient[index] = a;
			mIsChanged = true;
		}
		void setSpecular( int index, const GameLib::Math::Vector4 &s ){
			assert( index<LIGHT_COUNT );
			mSpecular[index] = s;
			mIsChanged = true;
		}

		GameLib::Math::Vector4 mDiffuse[LIGHT_COUNT];
		GameLib::Math::Vector4 mAmbient[LIGHT_COUNT];
		GameLib::Math::Vector4 mSpecular[LIGHT_COUNT];
		bool mIsChanged;

	};

	struct LightCameras{
		LightCameras()
		 :mIsChanged(true)
		{}
		~LightCameras(){}

		void setPosition( int index, const GameLib::Math::Vector4& position ){
			mPosition[index] = position;
			mIsChanged = true;
		}
		void setDirection( int index, const GameLib::Math::Vector4& dir ){
			mDirection[index] = dir;
			mIsChanged = true;
		}

		//�r���[�ϊ��s��Z�b�g
		void setViewMatrix( int index, const D3DXMATRIX& view ){
			mView[index] = view;
			mIsChanged = true;
		}
		///�����ϊ��s��Z�b�g
		void setProjectionMatrix( int index, const D3DXMATRIX& proj ){
			mProjection[index] = proj;
			mIsChanged = true;
		}
		///���[���h�ϊ��s��Z�b�g
		void setWorldMatrix( int index, const D3DXMATRIX& w ){
			mWorld[index] = w;
			mIsChanged = true;
		}

		GameLib::Math::Vector4 mPosition[LIGHT_COUNT];
		GameLib::Math::Vector4 mDirection[LIGHT_COUNT];

		D3DXMATRIX mWorld[LIGHT_COUNT];
		D3DXMATRIX mView[LIGHT_COUNT];
		D3DXMATRIX mProjection[LIGHT_COUNT];

		bool mIsChanged;
	};

	struct Camera {
		Camera()
			:mPosition(0.f),
			mDirection(0.f),

			mIsChanged(true)
		{}

		~Camera(){}

		void setPosition( const GameLib::Math::Vector4& position ){
			mPosition= position;
			mIsChanged = true;
		}
		void setDirection( const GameLib::Math::Vector4& dir ){
			mDirection = dir;
			mIsChanged = true;
		}

		//�r���[�ϊ��s��Z�b�g
		void setViewMatrix( const D3DXMATRIX& view ){
			mView = view;
			mIsChanged = true;
		}
		///�����ϊ��s��Z�b�g
		void setProjectionMatrix( const D3DXMATRIX& proj ){
			mProjection = proj;
			mIsChanged = true;
		}
		///���[���h�ϊ��s��Z�b�g
		void setWorldMatrix( const D3DXMATRIX& w ){
			mWorld = w;
			mIsChanged = true;
		}

		void setZNear(float zn){
			mZNear=zn;
			mIsChanged = true;
		}
		void setZFar(float zf){
			mZFar=zf;
			mIsChanged = true;
		}

		GameLib::Math::Vector4 mPosition;
		GameLib::Math::Vector4 mDirection;

		D3DXMATRIX mWorld;
		D3DXMATRIX mView;
		D3DXMATRIX mProjection;
		float mZNear;
		float mZFar;
		bool mIsChanged;
	};

	LightCameras mLightCameras;
	LightMaterials mLightMaterials;
	Camera mCamera;
	Material mMaterial;
	GameLib::Math::Vector4 mEdgeColor;
	
	int mVertexNumber;

	bool mVertexBlendEnable;

	// �o�b�N�o�b�t�@�[�̑ޔ�p
	LPDIRECT3DSURFACE9      mTempBackbuffer;	// �o�b�N�o�b�t�@�[
    LPDIRECT3DSURFACE9		mTempBackZ;		// �o�b�N�o�b�t�@�[�p�̐[�x�o�b�t�@�[
	D3DVIEWPORT9			mTempViewport;

	// �V���h�E�}�b�v�Ƃ��Ďg�p���郌���_�����O�^�[�Q�b�g�i�I���W�i���T�C�Y�j
	LPDIRECT3DTEXTURE9      mShadowTexture;		// �e�N�X�`��
	LPDIRECT3DSURFACE9      mShadowTexSurface;	// �e�N�X�`������擾����T�[�t�F�[�X
    LPDIRECT3DSURFACE9		mShadowTexZ;		// �e�N�X�`���p�̐[�x�o�b�t�@�[
	GameLib::Graphics::Texture mShadowDummyTexture;//GameLib::Graphics::Manager�ɂ�����_�~�[�̃e�N�X�`��

	//�V���h�E�Ƀt�B���^�������邽�߂Ɏg�p
	PostEffectSharedPtr mGaussianEffect;
	GameLib::FileIO::InFile mGaussianFxFile;

	//�A���`�G�C���A�X
	PostEffectSharedPtr mFxaaEffect;
	GameLib::FileIO::InFile mFxaaFxFile;

	GameLib::FileIO::InFile mDefalutFxFile;
	GameLib::FileIO::InFile mDefalutPostFxFile;
	GameLib::FileIO::InFile mPointLightFxFile;

//�ȍ~�̓V���O���g���̎���
public:
	static Manager* instance();
	void destroy();
private:
	Manager();
	~Manager();

	//disallow assign
	Manager( const Manager & );
	//disallow copy
	void operator=( const Manager & );
	struct Deleter
	{
		void operator()( Manager const* const p ) const
		{
			delete p;
		}
	};
	typedef std::unique_ptr< Manager, Deleter > UptrManager;
	static std::vector<UptrManager> mThese;
};
}//end namespace MME


#ifndef __IMAGE_H__
#define __IMAGE_H__


namespace GameLib {
	namespace Math {
		class Vector2;
	}
	namespace Graphics {
		class Texture;
	}
	namespace Scene {
		class PrimitiveRenderer;
	}
}
using GameLib::Math::Vector2;

//��ʂ����ς��Ƀe�N�X�`������N���X
class Image{
public:
	Image( const char* filename );
	~Image();
	
	//int width() const;
	//int height() const;

	//void setRender( GameLib::Scene::PrimitiveRenderer render ){


	///Z=0���ʂɒ����`��`���B���W�n�̓X�N���[�����W�B	
	void draw(
		const Vector2& pos0,
		const Vector2& pos1,
		const Vector2& uv0,
		const Vector2& uv1,
		unsigned color = 0xffffffff,
		float depth = 0.f );

	//��ʂ����ς��ɉ摜�𒣂�B�����̓A���t�@�u�����h
	void draw( unsigned color = 0xffffffff );
	
	bool isReady();
private:
	//disallow copy
	Image( const Image& );	
	//disallow assin
	void operator=( const Image& );
	GameLib::Graphics::Texture			*mTexture;
	GameLib::Scene::PrimitiveRenderer	*mRender;
};

#endif

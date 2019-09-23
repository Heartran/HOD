#pragma once
#include<GameLib/Graphics/Texture.h>
#include<string>

#include<boost/flyweight/flyweight_fwd.hpp>
#include <boost/flyweight/key_value_fwd.hpp>


//������Ǝg���ɂ���
//���R
//	FlyWeightTexture t = createWhiteTexture();���o���Ȃ�����


//���L����ێ����Ă邪�A�Ϗ�����@�\�͂��Ă��Ȃ�
//�����܂ō쐬���̕⏕�N���X�ł����Ȃ�
//�����A�e�N�X�`�����Ϗ����������
//  FlyWeightTexture flyweightTex( "texname" );
//	GameLib::Graphics::Texture t = (const GameLib::Graphics::Texture& )flyweightTex;
//flyweightTex.reset( newfilename );�@���@delete����Ă����L���͈Ϗ�����Ă���
//���̏ꍇ�́A�V����texname ��������ꍇ�A�V���ȃe�N�X�`���͐V��������Aflyweight�̋@�\�͗L���Ȃ�
//�ȉ���2�̓t�@�C�����͓���Ȏ����Ƃ��Ԃ�̂����Ȃ̂ŋ֎~
//  FlyWeightTexture flyweightTex( "/white" );
//  FlyWeightTexture flyweightTex( "/black" );
class FlyWeightTexture{
public:
	//��̃e�N�X�`��(GameLib::Texture��mImpl=0��texture)�����
	//�f�t�H���g�͔��ɂ���Ƃ����Ă����������AMME��useTexture�ɂ��킹�₷���̂Ńf�t�H���g�͋�̃e�N�X�`���ɂ���
	//reset("")�ł���̃e�N�X�`���𐶐��ł���
	FlyWeightTexture();
	FlyWeightTexture( const std::string &filename );
	~FlyWeightTexture();
	operator const GameLib::Graphics::Texture&()const;
	
	void reset( const std::string &filename );
	void resetWhiteTexture();
	void resetBlackTexture();

	bool isReady();
	bool isError()const;

	bool isBlackTexture()const;
	bool isWhiteTexture()const;

private:
	//disallow
	FlyWeightTexture( const FlyWeightTexture& );
	//disallow
	void operator=( const FlyWeightTexture& );
	struct Impl_t;
	typedef boost::flyweights::flyweight<
		boost::flyweights::key_value<std::string, Impl_t >
	> Impl;
	Impl *mImpl;
};


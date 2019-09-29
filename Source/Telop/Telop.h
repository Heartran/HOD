#pragma once
#include<GameLib/Scene/Font.h>
#include<GameLib/FileIO/OutFile.h>
#include<string>

//�`��Ȃ��ŕ\�����镶���񂾂��~�����ꍇ���Ȃ���?
//�Ƃ��������̃N���X�ɕ\��������@�\�͕������ق����悩�����̂ł�
//void draw( float elapsed, std::string& out );


namespace Telop{
	void destroy();
	void insert( const char *text, float start, float duration );
	//���ݕ\�����Ă鎚��������
	void eraseNow();
	//�[������X�^�[�g���Ă�̂�O��
	//�r�����炾�ƕ\������Ȃ���������
	//��@1�b����10�b�ԕ\����2�b����͂��߂��ꍇ�A����͕\������Ȃ�
	void draw( float elapsed );
	void setTime(float time );
	void setFont( GameLib::Scene::Font );
	void setDefalutDuration( float duration);
	void setDefalutFontColor( unsigned int);
	void setPos( unsigned int x, unsigned int y );
	
	/*
	@0, 120 :0xffff0000
	�Ԏ���
	@120, 120 :0xffffffff
	1�ɂ���
	@240, 120 :0xffffffff
	2 �� a
	@   340, 120 
	�����Ȃ������甒
	*/
	bool parse( const char* data, unsigned int size );
	
	/*
	@0, 120 :0xffff0000
	�Ԏ���
	@120, 120 :0xffffffff
	1�ɂ���
	@240, 120 :0xffffffff
	2 �� a
	@   340, 120 
	�����Ȃ������甒
	*/
	bool parse(const std::string &text);

	//�e���b�v��S���Đ��������H
	//���[�h�Ɗ��Ⴂ���邩������Ȃ��̂�
	//isPlayEnd, isPlayOver�Ƃ������ق����킩��₷�����H
	//�e���b�v�������Ȃ��ꍇtrue��Ԃ�
	bool isFinished();
	GameLib::FileIO::OutFile write(const char*filename );

}
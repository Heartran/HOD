#pragma once
#include<d3dx9.h>
#include<string>
#include"Object/IObject.h"
#include<vector>

namespace MME{

class ControlObjectSelf{
public:
	//@param name : CONTROLOBJECT��name�A�m�e�[�V�����̒l
	//����self�̂�
	ControlObjectSelf( ID3DXEffect *effect , const std::string &name );
	~ControlObjectSelf();

	void send( ID3DXEffect *effect, Object::IObjectSharedPtr object );
private:
	std::string mName;
	
	D3DXHANDLE mhCanDraw;
	D3DXHANDLE mhScale;
	D3DXHANDLE mhOffset;
	D3DXHANDLE mhTransform;

	//�{�[���� : PMD���f���̎w��{�[���̍��W�܂��̓��[���h�ϊ��s����擾����B
	//�@�@�@�@ �@�^��float3,float4,float4x4�̂��������ꂩ�B
	//���Ȃ��
	//std::vector<D3DXHANDLE> mBones;
	//�Ƃ肠�����P�ʍs��ł������Ă������H

	//�\��@ : PMD���f���̎w�肵���\��̒l���擾����B�^��float�B
	//���Ȃ��B�����͈͂�MMD��CModel�܂ŋy�ԁB
	//�߂�ǂ�����
	
	//�Ƃ肠�����A�w�肵���t�@�C�����̃I�u�W�F�N�g�����݂��Ȃ��ꍇ�A�ȉ��̒l���ݒ肳���B�̒l�����Ă�����
	std::vector<D3DXHANDLE> mFloats;//0.0
	std::vector<D3DXHANDLE> mFloat3s;//(0,0,0)
	std::vector<D3DXHANDLE> mFloat4s;//(0,0,0,1)
	std::vector<D3DXHANDLE> mMatricies;



	D3DXHANDLE mhX;		//"X"      : �A�N�Z�T���̈ʒuX(�A�N�Z�T���p�l����X)�B�^��float�B
	D3DXHANDLE mhY;		//"Y"      : �A�N�Z�T���̈ʒuY(�A�N�Z�T���p�l����Y)�B�^��float�B
	D3DXHANDLE mhZ;		//"Z"      : �A�N�Z�T���̈ʒuZ(�A�N�Z�T���p�l����Z)�B�^��float�B
	D3DXHANDLE mhXYZ;	//"XYZ"    : �A�N�Z�T���̈ʒu(�A�N�Z�T���p�l����X,Y,Z)�B�^��float3�B
	D3DXHANDLE mhRx;	//"Rx"     : �A�N�Z�T���̉�]X(�A�N�Z�T���p�l����Rx)�B�^��float�B(���P�j
	D3DXHANDLE mhRy;	//"Ry"     : �A�N�Z�T���̉�]Y(�A�N�Z�T���p�l����Ry)�B�^��float�B
	D3DXHANDLE mhRz;	//"Rz"     : �A�N�Z�T���̉�]Z(�A�N�Z�T���p�l����Rz)�B�^��float�B
	D3DXHANDLE mhRxyz;	//"Rxyz"   : �A�N�Z�T���̉�](�A�N�Z�T���p�l����Rx,Ry,Rz)�B�^��float3�B
	D3DXHANDLE mhSi;	//"Si"     : �A�N�Z�T���̃T�C�Y(�A�N�Z�T���p�l����Si)�B�^��float�B�i���Q�j
	D3DXHANDLE mhTr;	//"Tr"     : �A�N�Z�T���̓����x(�A�N�Z�T���p�l����Si)�B�^��float�B

	//�E�w�肵���t�@�C�����̃I�u�W�F�N�g�����݂��Ȃ��ꍇ�A�ȉ��̒l���ݒ肳���B
	//X�t�@�C���̏ꍇ�F
	//�@�X�P�[�����O�l�@�F10
	//�@�I�t�Z�b�g�l�@�@�F(0,0,0,1)
	//�@���[���h�ϊ��s��F�X�P�[�����O�s��ixyz�e10�{�j
	//�@����item�l�@�@�@�F0
	//PMD�t�@�C���̏ꍇ�F
	//�@�X�P�[�����O�l�@�F1
	//�@�I�t�Z�b�g�l�@�@�F(0,0,0,1)
	//�@���[���h�ϊ��s��F�P�ʍs��
	//�@�{�[���̍��W�@�@�F(0,0,0,1)
	//�@�{�[���̕ϊ��s��F�P�ʍs��
	//�@�\��l�@�@�@�@�@�F0
};

}
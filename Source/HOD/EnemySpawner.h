#pragma once
#include<memory>
#include<string>
#include<vector>


class btTransform;
namespace GameObject {
	class Enemy;
	typedef std::shared_ptr<Enemy> EnemySharedPtr;
}

namespace XmlLoader {
	namespace GameObject {
		class Enemy;
		typedef std::shared_ptr<Enemy> EnemySharedPtr;
	}
}

namespace HOD {

class EnemySpawner {
public:
	EnemySpawner();
	~EnemySpawner();

	void update(float elapsedTime);

	//�\�񂵂��Ƃ���Enemy����邩
	//�\������s��Enemy���쐬�����Ƃ���Enemy�����ׂ���������
	//�����͐�������Enemy�̐����Ǘ����ǂ����邩�H�ɂ�������ė���
	//
	//��1
	//�G�����Ǘ��N���X�ɕ��荞��
	//��2
	//�\�񂵂��Ƃ���Enemy�����A�Ԃ�l�ɃX�}�[�g�|�C���^�[�ŕԂ�
	//
	//��2�ōs�����Ǝv��
	//�\�񂵂����_��Enemy����肽��
	//�Q�[���J�n���_�Ń��[�h���������Ă����������A�i�������ĂȂ��Ă̓X���[�Y�ɃQ�[�����i�s���Ȃ����A���[�h���̕\���Ƃ��l���ĂȂ���)
	//2�̂ق���1�t���[���Ԃł�CPU���ׂ�������������Ȃ����A(�\��͂��Ԃ�1�t���[���ŗ\�񂵍쐬����̂Łj
	//�����������ׂł͂Ȃ��Ǝv���邵�A���ׂ��������Ƃ��Ă��Q�[�����������Ȃ̂ŋ��e�ł��邾�낤
	//�̂��ɓG�����N���X�ɓ˂����ނ��ق��̕��@�l���邩�͍D���ɂ���΂�����

	//�G�̏o����\�񂷂�
	//�����œG�|�C���^��Ԃ���addToWorld���Ȃ�����
	//���̗\�񎞊ԑO��addToWorld�����ꍇ�\�񎞊Ԃ�assert������
	//�ʒu�ω��Ƃ��͂����Ȃ�
	//enemyName��������Ȃ����������ۂ̃X�}�[�g�|�C���^�[��Ԃ�
	GameObject::EnemySharedPtr reserve(const std::string &enemyName, float spawnTime, const btTransform& trans);

	//�����t�@�C�������d�����ēo�^�ł���̂ŋC��t���邱��
	void loadEnemy(const std::string &filename);

	bool isReady();

	//�Ⴄ�Ƃ����load������XmlLoader::GameObject::EnemySharedPtr��o�^������
	//enemy->isReady()��true�ɂȂ��Ă��邱�Ɛ���
	void registerEnemy(XmlLoader::GameObject::EnemySharedPtr enemy);

	void setTime(float t) {
		mTime = t;
	}
private:
	//���łɃ��\�[�X�����[�h���Ă���̂Ȃ烍�[�h���Ȃ��Ƃ����d�l�����̃N���X�ɓ���悤����������
	//����Ȃ����Ƃɂ����B
	//�X�N���v�g���烊�\�[�X���X�V�������Ƃ��Ɍ����肪�����΂Ȃ������Ƃ����
	//���̎d�l�͎��R�ł͂Ȃ��Ɗ������̂����R�B�������ɂ͂����Ă��܂��̂��f�t�H���g�ɂ͂������Ȃ�

	std::vector<XmlLoader::GameObject::EnemySharedPtr> mTempateEnemies;

	//���݃��[�h����enemy�����Ă����Ƃ���
	//���[�h���I�������mTemplateEnemies�Ɉړ�������
	//
	//map��vector�Ŗ�������vector�ōs�����Ƃɂ���
	//�����t�@�C����2�d���[�h���Ă��܂����Ƃ��ł��Ă��܂����ʂɖ��͂Ȃ����낤�Ɣ��f
	//�ǂ������[�h���I�������o�Ă�����
	std::vector<XmlLoader::GameObject::EnemySharedPtr> mLoadingTempateEnemies;
	
	
	struct Request;
	std::vector<Request> mRequests;
	float mTime;	
};


}


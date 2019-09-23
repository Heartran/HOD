// DixMeshTools.cpp

// v1.00 -> v1.01
// �E convertXFileFormat�֐���Format���K�p����Ă��Ȃ������o�O���C��


#include "convertXFileFormat.h"

using namespace Dix;

///////////////////////////////////
// CNormalAllocHierarchy
//////////////////
CNormalAllocHierarchy::CNormalAllocHierarchy(void)
{
}


// �f�X�g���N�^
CNormalAllocHierarchy::~CNormalAllocHierarchy(void)
{
	// �o�^���ꂽ�I�u�W�F�N�g��S�č폜����
	std::list<void*>::iterator it;
	for(it = m_DelList.begin(); it!=m_DelList.end(); it++)
		delete[] (*it);

	std::list<IUnknown*>::iterator comit;
	for(comit = m_ReleaseList.begin(); comit!=m_ReleaseList.end();comit++){
		if(*comit){
			ULONG u = (*comit)->Release();
			int a = 100;
		}
	}
}


// �t���[���\���̂𐶐�
D3DXFRAME* CNormalAllocHierarchy::CreateNewFrame()
{
	D3DXFRAME* tmp = new D3DXFRAME;
	ZeroMemory( tmp, sizeof(D3DXFRAME) );
	AddDelList( tmp );
	return tmp;
}


// ���b�V���R���e�i�\���̂𐶐�
D3DXMESHCONTAINER *CNormalAllocHierarchy::CreateNewMeshContainer()
{
	D3DXMESHCONTAINER* tmp = new D3DXMESHCONTAINER;
	ZeroMemory( tmp, sizeof(D3DXMESHCONTAINER) );
	AddDelList( tmp );
	return tmp;
}


// �������X�g�ɓo�^
void CNormalAllocHierarchy::AddDelList( void* ptr )
{
	m_DelList.push_back( ptr );
}

// �����[�X���X�g�ɓo�^
void CNormalAllocHierarchy::AddReleaseList( IUnknown *comptr )
{
	comptr->AddRef();
	m_ReleaseList.push_back( comptr );
}


// ������R�s�[�֐�
LPSTR CNormalAllocHierarchy::CopyStr(LPCSTR name)
{
	if(!name) return NULL;   // NULL�͕��������J�E���g�ł��Ȃ�
	LPSTR Str = new char[strlen(name)+1];
	strcpy_s(Str, strlen(name)+1, name);
	AddDelList( Str );
	return Str;
}


// ���b�V���f�[�^�o�^
void CNormalAllocHierarchy::RegistMeshData(CONST D3DXMESHDATA *pSrc, D3DXMESHDATA *pDest)
{
	pDest->Type = pSrc->Type;   // ���b�V���^�C�v
	pDest->pMesh = pSrc->pMesh;   // ���b�V���iunion�Ȃ̂łǂ�ł��ꏏ�j
	AddReleaseList( pDest->pMesh );  // Release���X�g�֓o�^
}


// �}�e���A���o�^
void CNormalAllocHierarchy::RegistMaterial(CONST D3DXMATERIAL *pSrc, DWORD num, D3DXMATERIAL **ppDest)
{
	// �}�e���A���z��̐���
	*ppDest = new D3DXMATERIAL[ num ];
	AddDelList( *ppDest );

	DWORD i;
	for(i=0; i<num; i++)
	{
		(*ppDest)[i].MatD3D = pSrc[i].MatD3D;   // �}�e���A���o�^
		(*ppDest)[i].pTextureFilename = CopyStr( pSrc[i].pTextureFilename );  // �e�N�X�`�����o�^
	}
}


// �G�t�F�N�g�o�^
void CNormalAllocHierarchy::RegistEffect(CONST D3DXEFFECTINSTANCE *pSrc, D3DXEFFECTINSTANCE **ppDest)
{
	*ppDest = new D3DXEFFECTINSTANCE;
	AddDelList( *ppDest );
	(*ppDest)->pEffectFilename = CopyStr(pSrc->pEffectFilename);     // �G�t�F�N�g��
	(*ppDest)->NumDefaults = pSrc->NumDefaults;                      // �G�t�F�N�g�f�t�H���g��
	(*ppDest)->pDefaults = new D3DXEFFECTDEFAULT[pSrc->NumDefaults];  // �G�t�F�N�g�f�t�H���g�z�񐶐�
	AddDelList( (*ppDest)->pDefaults );

	// �G�t�F�N�g�f�t�H���g�̓o�^
	D3DXEFFECTDEFAULT *pEDSrc  = pSrc->pDefaults;   // �R�s�[��
	D3DXEFFECTDEFAULT *pEDDest = (*ppDest)->pDefaults; // �R�s�[��
	unsigned int i=0;
	for(i=0; i<pSrc->NumDefaults; i++)
	{
	   pEDDest[i].pParamName = CopyStr(pEDSrc[i].pParamName);        // �p�����[�^�� 
	   DWORD NumBytes = pEDDest[i].NumBytes = pEDSrc[i].NumBytes;  // �p�����[�^�T�C�Y
	   pEDDest[i].Type = pEDSrc[i].Type;                           // �p�����[�^�^�C�v
	   if(pEDSrc[i].Type <= D3DXEDT_DWORD){
		  pEDDest[i].pValue = new DWORD[ NumBytes ];               // �p�����[�^�z�񐶐�
		  AddDelList( pEDDest[i].pValue );
		  memcpy( pEDDest[i].pValue, pEDSrc[i].pValue, NumBytes);     // �p�����[�^���R�s�[
	   }
	}
}


// �אڃ|���S���o�^
void CNormalAllocHierarchy::RegistAdjacency(CONST DWORD *Src, DWORD polynum, DWORD **Dest)
{
	*Dest = new DWORD[ polynum * 3 ];   // �z�񐶐�
	memcpy( *Dest, Src, polynum * 3 * sizeof(DWORD));  // �R�s�[
	AddDelList( *Dest );
}


// �X�L���o�^
void CNormalAllocHierarchy::RegistSkin( CONST LPD3DXSKININFO Src, LPD3DXSKININFO *Dest)
{
	if(!Src){
		*Dest = NULL;
		return;   // �X�L����������Ή������Ȃ�
	}
	*Dest = Src;               // �X�L�����R�s�[
	(*Dest)->AddRef();         // �Q�ƃJ�E���^����
	AddReleaseList( *Dest );   // �����[�X���X�g�ɓo�^
}


// �t���[�������֐�
HRESULT CNormalAllocHierarchy::CreateFrame(THIS_ 
   LPCSTR Name, 
   LPD3DXFRAME *ppNewFrame)
{
	// �t���[����V��������
	D3DXFRAME *pFrame = CreateNewFrame();
	pFrame->Name = CopyStr( Name );
	*ppNewFrame = pFrame;

	return D3D_OK;
}


// ���b�V���R���e�i�����֐�
HRESULT CNormalAllocHierarchy::CreateMeshContainer(THIS_ 
   LPCSTR Name, 
   CONST D3DXMESHDATA *pMeshData, 
   CONST D3DXMATERIAL *pMaterials, 
   CONST D3DXEFFECTINSTANCE *pEffectInstances, 
   DWORD NumMaterials, 
   CONST DWORD *pAdjacency, 
   LPD3DXSKININFO pSkinInfo, 
   LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	// ���b�V���R���e�i�I�u�W�F�N�g�̐���
	D3DXMESHCONTAINER *p = CreateNewMeshContainer();

	// ���O�o�^
	p->Name = CopyStr( Name );

	// ���b�V���f�[�^�o�^
	RegistMeshData( pMeshData, &p->MeshData );

	// �}�e���A���o�^
	p->NumMaterials = NumMaterials;
	RegistMaterial( pMaterials, NumMaterials, &p->pMaterials);

	// �G�t�F�N�g�o�^
	if(pEffectInstances){
		RegistEffect( pEffectInstances, &p->pEffects );
	}
	// �אڃ|���S���o�^
	RegistAdjacency( pAdjacency, pMeshData->pMesh->GetNumFaces(), &p->pAdjacency );

	// �X�L���o�^
	RegistSkin( pSkinInfo, &p->pSkinInfo );

	*ppNewMeshContainer = p;

	return D3D_OK;
}


// �t���[�����폜
HRESULT CNormalAllocHierarchy::DestroyFrame(THIS_ 
   LPD3DXFRAME pFrameToFree)
{
	// ���̃N���X�ł̓f�X�g���N�^�ō폜�����̂�
	// ���̊֐��͎g��Ȃ�
	return D3D_OK;
}


// �R���e�i���폜
HRESULT CNormalAllocHierarchy::DestroyMeshContainer(THIS_
   LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	// ���̃N���X�ł̓f�X�g���N�^�ō폜�����̂�
	// ���̊֐��͎g��Ȃ�
	return D3D_OK;
}


/////////////////////////////////////////////
// convertXFileFormat
//   X�t�@�C���t�H�[�}�b�g�R���o�[�g�c�[��
//---------------------------------------
// X�t�@�C�����o�C�i���̃e�L�X�g�ϊ�����
/////////////////////////////////////////////
bool Dix::convertXFileFormat( IDirect3DDevice9 *pDev, TCHAR* inXFilename, TCHAR* outXFilename, int Format)
{
	CNormalAllocHierarchy NAH;
	D3DXFRAME *pFrame = NULL;
	ID3DXAnimationController *pAnim = NULL;

	// X�t�@�C�������b�V����
	if( FAILED( D3DXLoadMeshHierarchyFromX( inXFilename, D3DXMESH_MANAGED, pDev, &NAH, NULL, &pFrame, &pAnim) ) )
		return false;

	// �ǂݍ��񂾃��b�V�����w��̃t�H�[�}�b�g�ŕۑ�
	D3DXSaveMeshHierarchyToFile( outXFilename, Format, pFrame, pAnim, NULL );

	return true;
}
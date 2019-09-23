#ifndef DIX_MESH_TOOLS
#define DIX_MESH_TOOLS

#include <tchar.h>
#include <d3dx9.h>
#include <list>

namespace Dix
{
class CNormalAllocHierarchy : public ID3DXAllocateHierarchy
{
protected:
	std::list<void*> m_DelList;   // �������X�g
	std::list<IUnknown*> m_ReleaseList;   // �����[�X���X�g

public:
	CNormalAllocHierarchy(void);
	virtual ~CNormalAllocHierarchy(void);

	// �t���[���𐶐�����
	STDMETHOD(CreateFrame)(THIS_ 
		LPCSTR Name,
		LPD3DXFRAME *ppNewFrame
	);

	// �R���e�i�𐶐�����
	STDMETHOD(CreateMeshContainer)(THIS_ 
		LPCSTR Name, 
		CONST D3DXMESHDATA *pMeshData, 
		CONST D3DXMATERIAL *pMaterials, 
		CONST D3DXEFFECTINSTANCE *pEffectInstances, 
		DWORD NumMaterials, 
		CONST DWORD *pAdjacency, 
		LPD3DXSKININFO pSkinInfo, 
		LPD3DXMESHCONTAINER *ppNewMeshContainer
	);

	// �t���[�����폜����
	STDMETHOD(DestroyFrame)(THIS_
		LPD3DXFRAME pFrameToFree
	);

	// �R���e�i���폜����
	STDMETHOD(DestroyMeshContainer)(THIS_
		LPD3DXMESHCONTAINER pMeshContainerToFree
	);


protected:
	// �������X�g�ɓo�^����
	virtual void AddDelList( void* ptr );

	// ��������R�s�[����
	virtual LPSTR CopyStr(LPCSTR name);

	// �t���[���\���̂𐶐�����
	virtual D3DXFRAME* CreateNewFrame();

	// ���b�V���R���e�i�\���̂𐶐�����
	virtual D3DXMESHCONTAINER* CreateNewMeshContainer();

	// ���b�V���f�[�^��o�^
	virtual void RegistMeshData(CONST D3DXMESHDATA *pSrc, D3DXMESHDATA *pDest);

	// �����[�X���X�g�ɓo�^
	virtual void AddReleaseList( IUnknown *comptr);

	// �}�e���A���o�^
	virtual void RegistMaterial(CONST D3DXMATERIAL *pSrc, DWORD num, D3DXMATERIAL **pDest);

	// �G�t�F�N�g�o�^
	virtual void RegistEffect(CONST D3DXEFFECTINSTANCE *pSrc, D3DXEFFECTINSTANCE **ppDest);

	// �אڃ|���S���o�^
	virtual void RegistAdjacency(CONST DWORD *Src, DWORD polynum, DWORD **Dest);

	// �X�L���o�^
	virtual void RegistSkin( CONST LPD3DXSKININFO Src, LPD3DXSKININFO *Dest);
};

// X�t�@�C���t�H�[�}�b�g�R���o�[�g�c�[��
extern bool convertXFileFormat( IDirect3DDevice9 *pDev, TCHAR* inXFilename, TCHAR* outXFilename, int Format);
};

#endif
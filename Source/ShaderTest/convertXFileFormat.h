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
	std::list<void*> m_DelList;   // 消去リスト
	std::list<IUnknown*> m_ReleaseList;   // リリースリスト

public:
	CNormalAllocHierarchy(void);
	virtual ~CNormalAllocHierarchy(void);

	// フレームを生成する
	STDMETHOD(CreateFrame)(THIS_ 
		LPCSTR Name,
		LPD3DXFRAME *ppNewFrame
	);

	// コンテナを生成する
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

	// フレームを削除する
	STDMETHOD(DestroyFrame)(THIS_
		LPD3DXFRAME pFrameToFree
	);

	// コンテナを削除する
	STDMETHOD(DestroyMeshContainer)(THIS_
		LPD3DXMESHCONTAINER pMeshContainerToFree
	);


protected:
	// 消去リストに登録する
	virtual void AddDelList( void* ptr );

	// 文字列をコピーする
	virtual LPSTR CopyStr(LPCSTR name);

	// フレーム構造体を生成する
	virtual D3DXFRAME* CreateNewFrame();

	// メッシュコンテナ構造体を生成する
	virtual D3DXMESHCONTAINER* CreateNewMeshContainer();

	// メッシュデータを登録
	virtual void RegistMeshData(CONST D3DXMESHDATA *pSrc, D3DXMESHDATA *pDest);

	// リリースリストに登録
	virtual void AddReleaseList( IUnknown *comptr);

	// マテリアル登録
	virtual void RegistMaterial(CONST D3DXMATERIAL *pSrc, DWORD num, D3DXMATERIAL **pDest);

	// エフェクト登録
	virtual void RegistEffect(CONST D3DXEFFECTINSTANCE *pSrc, D3DXEFFECTINSTANCE **ppDest);

	// 隣接ポリゴン登録
	virtual void RegistAdjacency(CONST DWORD *Src, DWORD polynum, DWORD **Dest);

	// スキン登録
	virtual void RegistSkin( CONST LPD3DXSKININFO Src, LPD3DXSKININFO *Dest);
};

// Xファイルフォーマットコンバートツール
extern bool convertXFileFormat( IDirect3DDevice9 *pDev, TCHAR* inXFilename, TCHAR* outXFilename, int Format);
};

#endif
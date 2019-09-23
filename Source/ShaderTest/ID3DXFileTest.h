#pragma once
#pragma comment(lib, "D3dxof.lib")

#include<d3dx9.h>
#include<dxfile.h>
#include<GameLib/Graphics/IndexBuffer.h>
#include<GameLib/Graphics/VertexBuffer.h>
#include<GameLib/Graphics/Texture.h>
#include<GameLib/Scene/Batch.h>
#include<rmxftmpl.h>
#include<rmxfguid.h>
#include"helper/MyOutputDebugString.h"
#include<helper/FlyWeightTexture.h>
#include<assert.h>

#include<vector>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif//SAFE_RELEASE

//Accessory::XfileLoader
//�K�v�Ȃ����͖̂���
//Accessory::Model����邩�B
namespace Acc{
struct Material {
	~Material() {}
	D3DMATERIAL9 mMaterial;
	FlyWeightTexture mTexture;
	//unsigned long mNumEdges;
};

struct Subset {
	unsigned long materialIndex_;
	unsigned long faceIndex_;//�J�n�ʔԍ�
	unsigned long faceCount_;
};
	

class Model{
	unsigned long mNumMaterials;
	Material *mMaterials;
	unsigned long mNumSubsets;
	Subset *mSubsets;
	GameLib::Graphics::VertexBuffer mVb;
	GameLib::Graphics::IndexBuffer mIb;
};

}//end namespace Acc

class XFilePrinter {
public:
	XFilePrinter(){}
	~XFilePrinter(){}

	HRESULT LoadMeshHierarchy()
	{
		TRACE("start LoadMeshHierarchy()\n");
		HRESULT hr = S_OK;
		//const char* filename = "ShaderTest/XFILE/tiny/tiny.x";
		//const char* filename = "ShaderTest/XFILE/negi.x";
		const char* filename = "ShaderTest/XFILE/sphere.x";
		LPDIRECTXFILE           dxFileApi    = NULL;	// DirectXFile �I�u�W�F�N�g
		LPDIRECTXFILEENUMOBJECT dxFileEnum   = NULL;	// DirectXFile �񋓃I�u�W�F�N�g
		LPDIRECTXFILEDATA       dxFileData = NULL;	// DirectXFile �f�[�^ �I�u�W�F�N�g
		DWORD dwOptions = 0;
		int cchFileName;

		cchFileName = strlen(filename);

		// ------------------------------------------------------------------------
		// XFile �̎��ۂ̓ǂݍ���
		if ( FAILED( hr = DirectXFileCreate(&dxFileApi) ) ){
			goto e_Exit;
		}

		//�I���W�i���̃J�X�^���e���v���[�g�����ׂ����H
		// �J�X�^�� �e���v���[�g��o�^����
		hr = dxFileApi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,  D3DRM_XTEMPLATE_BYTES);
		assert( SUCCEEDED(hr) );

		// �񋓃I�u�W�F�N�g�𐶐�����
		// �񋓃I�u�W�F�N�g�́A�O���[�o�� ���j�[�N���ʎq (GUID) �܂��͖��O�Ńf�[�^ �I�u�W�F�N�g���擾����̂Ɏg��
		if ( FAILED( hr =
			dxFileApi->CreateEnumObject(
				(LPVOID)filename,		// DXFILELOAD_FROMFILE �̂Ƃ��̓t�@�C����
				DXFILELOAD_FROMFILE,	// �u�t�@�C���v����f�[�^��ǂݎ��
				&dxFileEnum				// �쐬���ꂽ�񋓃I�u�W�F�N�g
			)
			) )
		{
			goto e_Exit;
		}

		// ------------------------------------------------------------------------
		// �f�[�^��ǂݍ���ŁA�t���[���ɂ��񂪂�o�^���Ă���
		while (SUCCEEDED(dxFileEnum->GetNextDataObject(&dxFileData))) {
			TRACE("start Sibling ");
			
			LoadFrames( dxFileData );
			SAFE_RELEASE(dxFileData);

			if( FAILED(hr) ){
				goto e_Exit;
			}
		}
e_Exit:
		// �����g��Ȃ����̂��J��
		SAFE_RELEASE(dxFileData);
		SAFE_RELEASE(dxFileEnum);
		SAFE_RELEASE(dxFileApi);

		return hr;
	}

	HRESULT LoadFrames( LPDIRECTXFILEDATA dxFileData )
	{
		TRACE("LoadFrames()\n");

		HRESULT hr = S_OK;
		LPDIRECTXFILEDATA fileChild = NULL;
		LPDIRECTXFILEOBJECT fileObjChild = NULL;
		const GUID *type;

		// �^�C�v�𒲂ׂāA��ނɉ��������[�h���s��
		if ( FAILED(hr = dxFileData->GetType(&type)) ){
			goto e_Exit;
		}

		if(TID_D3DRMMesh==*type){
			// ��D3D RM �̍�����̓`������i�H�j���b�V���t�@�C��
			//TRACE("TID_D3DRMMesh\n");
			hr = LoadMesh( dxFileData );
			if (FAILED(hr))goto e_Exit;
		}else if(TID_D3DRMFrameTransformMatrix==*type){
			// ���p���s��
			//D3DXMATRIX *pmatNew;
			//hr = dxFileData->GetData(NULL, &cbSize, (PVOID*)&pmatNew);
			TRACE("TID_D3DRMFrameTransformMatrix is ignored.\n");
			//if (FAILED(hr))	goto e_Exit;
		}else if(TID_D3DRMAnimationSet==*type){
			// ���A�j���[�V�����̏W��
			TRACE("TID_D3DRMAnimationSet is ignored.\n");
		} else if(TID_D3DRMAnimation==*type){
			// ���A�j���[�V����
			TRACE("TID_D3DRMAnimation is ignored.\n");
		}else if(TID_D3DRMFrame==*type){
			// ������ɐV�K�t���[��
							
			// �q���̃f�[�^�����񂪂�ǂݍ���
			while ( SUCCEEDED(dxFileData->GetNextObject(&fileObjChild)) ) {
				hr = fileObjChild->QueryInterface( IID_IDirectXFileData, (LPVOID *)&fileChild );
				if (SUCCEEDED(hr)) {// �q���� FileData ��v�����āAFileData ����������ċN�I�ɓǂݍ���
					TRACE("start Child ");
					hr = LoadFrames(fileChild );
									
					if (FAILED(hr)){
						goto e_Exit;
					}

					SAFE_RELEASE(fileChild);
				}
				SAFE_RELEASE(fileObjChild);
			}
		} else {
			assert( false && "unknown type found.");
		}
e_Exit:
		SAFE_RELEASE(fileChild);
		SAFE_RELEASE(fileObjChild);
		return hr;
	}


	HRESULT LoadMesh(LPDIRECTXFILEDATA dxFileData )
	{
		HRESULT hr = S_OK;
		DWORD cchName;
	
		// ���O�𒲂ׂ�
		if (FAILED(hr = dxFileData->GetName(NULL, &cchName))){
			goto e_Exit;
		}

		if (cchName > 0) {
			//if (NULL == (pmcMesh->szName=new char[cchName])) {
			//	hr = E_OUTOFMEMORY;	goto e_Exit;
			//}
			//if (FAILED(hr=dxFileData->GetName(pmcMesh->szName, &cchName)))	goto e_Exit;
		}

		
		DWORD nameLen;
		if ( FAILED(hr = dxFileData->GetName( 0, &nameLen )) ){
			goto e_Exit;
		}
		if(nameLen>0){
			char *name = new char[nameLen];
			if ( FAILED(hr = dxFileData->GetName( name, &nameLen )) ){
				delete [] name;
				goto e_Exit;
			}
			TRACE1("%s\n", name );
			delete [] name;
		}

		//vertexBuffer
		//indexBuffer
		{
			DWORD size;
			DWORD *pnVertices; 
			hr=dxFileData->GetData( "nVertices", &size, (PVOID*)&pnVertices );
			assert( SUCCEEDED(hr) );
			assert( size == sizeof(DWORD) );
			TRACE1( "nVertices=%d\n", *pnVertices );
			if( pnVertices>0 ){
				float *pVertices;
				hr=dxFileData->GetData( "vertices", &size, (PVOID*)&pVertices );
				assert( SUCCEEDED(hr) );
				assert( size == sizeof(float) * 3 * (*pnVertices) );
			}

			//index
			//3�Ƃ͌���Ȃ��炵��(�O�p�`)
			//����ȃf�[�^�\��
			//4;0,3,1,2;,
			//4;4,0,2,6;,
			//4;6,2,1,5;,
			//4;5,1,3,7;,
			//4;7,3,0,4;,
			//4;4,6,5,7;;
			DWORD *pnFaces;
			hr=dxFileData->GetData( "nFaces", &size, (PVOID*)&pnFaces );
			assert( SUCCEEDED(hr) );
			assert( size == sizeof(DWORD) );
		
			if( *pnFaces>0 ){
				DWORD *pFaces;
				hr=dxFileData->GetData( "faces", &size, (PVOID*)&pFaces );
				assert( SUCCEEDED(hr) );
				assert( size == sizeof(DWORD) * 4 * (*pnFaces) );
				//for( unsigned int i=0; i<*pnFaces; i++)
				//{   
				//	assert( *pFaces == 3 );
				//	pFaces+= 4;
				//}
			}
		}
		

		LPDIRECTXFILEDATA fileChild = NULL;
		LPDIRECTXFILEOBJECT fileObjChild = NULL;
		
		// �q���̃f�[�^�����񂪂�ǂݍ���
		while ( SUCCEEDED(dxFileData->GetNextObject(&fileObjChild)) ) {
			hr = fileObjChild->QueryInterface( IID_IDirectXFileData, (LPVOID *)&fileChild );
			if (SUCCEEDED(hr)) {// �q���� FileData ��v�����āAFileData ����������ċN�I�ɓǂݍ���
				
				//hr = fileChild->GetType();
				const GUID *type;
		
				hr = fileChild->GetType(&type);
				
				// template SkinWeights \
				// { \
				//     < 6F0D123B-BAD2-4167-A0D0-80224F25FABB > \
				//     STRING transformNodeName; \
				//     DWORD nWeights; \
				//     array DWORD vertexIndices[nWeights]; \
				//     array float weights[nWeights]; \
				//     Matrix4x4 matrixOffset; \
				// } 
				static const GUID TID_D3DRMSkinWeights ={
					0x6F0D123B, 0xBAD2, 0x4167, { 0xA0, 0xD0, 0x80, 0x22, 0x4f, 0x25, 0xfa, 0xbb } };


				// template VertexDuplicationIndices \
				// { \
				//     < B8D65549-D7C9-4995-89CF-53A9A8B031E3 > \
				//     DWORD nIndices; \
				//     DWORD nOriginalVertices; \
				//     array DWORD indices[nIndices]; \
				// }
				static const GUID TID_D3DRMVertexDuplicationIndices = {
					0xb8d65549, 0xd7c9, 0x4995, { 0x89, 0xcf, 0x53, 0xa9, 0xa8, 0xb0, 0x31, 0xe3 } };

				//template XSkinMeshHeader 
				//{ 
				//    < 3CF169CE-FF7C-44ab-93C0-F78F62D172E2 >  
				//    WORD nMaxSkinWeightsPerVertex; 
				//    WORD nMaxSkinWeightsPerFace; 
				//    WORD nBones; 
				//}
				static const GUID TID_D3DRMXSkinMeshHeader = {
					0x3CF169CE, 0xFF7C, 0x44ab, { 0x93, 0xc0, 0xf7, 0x8f, 0x62, 0xd1, 0x72, 0xe2 } };


				if( *type==TID_D3DRMMeshNormals ){
					loadMeshNormals( fileChild );
				} else if( *type==TID_D3DRMMeshTextureCoords ){
				} else if( *type==TID_D3DRMMeshMaterialList ){
					LoadMaterialList( fileChild );
				
				} else if( *type==TID_D3DRMXSkinMeshHeader ){
				} else if( *type==TID_D3DRMSkinWeights ){
				} else if( *type==TID_D3DRMVertexDuplicationIndices ){
				} else if( *type==TID_D3DRMMeshFaceWraps ){
				} else if( *type==TID_D3DRMMeshVertexColors ){
				} else {
					assert( false && "unknown type" );
				}
				
			}
			SAFE_RELEASE(fileObjChild);
		}
		
	e_Exit:
		return hr;
	}

	void LoadMaterialList(
		LPDIRECTXFILEDATA dxfMaterialList//Xfile Material List
	){
		HRESULT hr = S_OK;

		DWORD size;
		DWORD *nMaterials;
		DWORD *nFaceIndexes;
		DWORD *faceIndexes;//�`�悷��O�p�`�̐�

		hr=dxfMaterialList->GetData( "nMaterials", &size, (void**)&nMaterials );
		assert( SUCCEEDED(hr) );
		assert( size==sizeof(DWORD) );
		hr=dxfMaterialList->GetData( "nFaceIndexes", &size, (void**)&nFaceIndexes );
		assert( SUCCEEDED(hr) );
		assert( size==sizeof(DWORD) );
		hr=dxfMaterialList->GetData( "FaceIndexes", &size, (void**)&faceIndexes );
		assert( SUCCEEDED(hr) );
		assert( size == sizeof(DWORD) * (*nFaceIndexes) );




		
		LPDIRECTXFILEDATA dxfMaterial = NULL;
		LPDIRECTXFILEOBJECT childMaterial = NULL;
		//Material�����邩�𒲂ׂ�
		while ( SUCCEEDED( dxfMaterialList->GetNextObject(&childMaterial) ) ) {
			hr = childMaterial->QueryInterface( IID_IDirectXFileData, (LPVOID *)&dxfMaterial );
			if (SUCCEEDED(hr)) {
				const GUID *gctype;
				hr = dxfMaterial->GetType(&gctype);
				assert( SUCCEEDED(hr) );
				if(*gctype==TID_D3DRMMaterial){			
					DWORD size;
					//template Material {\
					//	<3d82ab4d-62da-11cf-ab39-0020af71e433>\
					//	ColorRGBA faceColor;\
					//	FLOAT power;\
					//	ColorRGB specularColor;\
					//	ColorRGB emissiveColor;\
					//	[...]\
					//}

					//float4�o�C�g���E�����炢��ˁ[�Ǝv��
					//pragma pack�����ق����������H
					struct RGBA{
						float r;
						float g;
						float b;
						float a;
					};
					struct RGB{
						float r;
						float g;
						float b;
					};
					RGBA *rgba;
					RGB *rgb;
					float *power;
					hr=dxfMaterial->GetData( "faceColor", &size, (LPVOID*)&rgba );
					assert( SUCCEEDED(hr) );
					assert( size==sizeof(RGBA) );
					hr=dxfMaterial->GetData( "power", &size, (LPVOID*)&power );
					assert( SUCCEEDED(hr) );
					assert( size==sizeof(float) );
					hr=dxfMaterial->GetData( "specularColor", &size, (LPVOID*)&rgb );
					assert( SUCCEEDED(hr) );
					assert( size==sizeof(RGB) );
					hr=dxfMaterial->GetData( "emissiveColor", &size, (LPVOID*)&rgb );
					assert( SUCCEEDED(hr) );
					assert( size==sizeof(RGB) );
					//TextureFileName�����邩�H
					LPDIRECTXFILEDATA dxfTextureFileName=NULL;
					LPDIRECTXFILEOBJECT dxfoTextureFileName=NULL;
					hr = dxfMaterial->GetNextObject(&dxfoTextureFileName);
					if( SUCCEEDED(hr) ){
						hr=dxfoTextureFileName->QueryInterface( IID_IDirectXFileData, (LPVOID*)&dxfTextureFileName );
						assert( SUCCEEDED( hr ) );

						const char** textureFileName=NULL;
						hr=dxfTextureFileName->GetData( "filename", &size, (void**)&textureFileName );
						assert( SUCCEEDED( hr ) );

						TRACE1("%s\n", *textureFileName );

						SAFE_RELEASE(dxfTextureFileName);
						SAFE_RELEASE(dxfoTextureFileName);
					} else{
						hr=S_OK;
					}

				} else{
					assert( false && "unknown type" );
				}

				SAFE_RELEASE(dxfMaterial)
			}
			SAFE_RELEASE(childMaterial);
		}
	}

	void loadMeshNormals(
		LPDIRECTXFILEDATA dxfMeshNormals//Xfile Mesh Normals
	){
		//template MeshNormals \
        //{ \
        //    < F6F23F43-7686-11cf-8F52-0040333594A3 > \
        //    DWORD nNormals; \
        //    array Vector normals[nNormals]; \
        //    DWORD nFaceNormals; \
        //    array MeshFace meshFaces[nFaceNormals]; \
        //}
		DWORD size;
		DWORD *nNormals;
		float *normals;
		DWORD *nFaceNormals;
		//template MeshFace \
        //{ \
        //    < 3D82AB5F-62DA-11cf-AB39-0020AF71E433 > \
        //    DWORD nFaceVertexIndices; \
        //    array DWORD faceVertexIndices[nFaceVertexIndices]; \
        //}
		DWORD *meshFaces;

		HRESULT hr=S_OK;

		hr=dxfMeshNormals->GetData( "nNormals", &size, (void**)&nNormals );
		assert( SUCCEEDED( hr ) );
		assert( size == sizeof( DWORD ) );
		TRACE1( "nNormals=%d\n", *nNormals );
		
		hr=dxfMeshNormals->GetData( "normals", &size, (void**)&normals );
		assert( SUCCEEDED( hr ) );
		assert( size == sizeof( float )*(*nNormals)*3 );
		
		hr=dxfMeshNormals->GetData( "nFaceNormals", &size, (void**)&nFaceNormals );
		assert( SUCCEEDED( hr ) );
		assert( size == sizeof( DWORD ) );
		TRACE1( "nFaceNormals=%d\n", *nFaceNormals );

		hr=dxfMeshNormals->GetData( "meshFaces", &size, (void**)&meshFaces );
		assert( SUCCEEDED( hr ) );
		assert( size == sizeof( DWORD )*4*(*nFaceNormals) );

		hr=dxfMeshNormals->GetData( "faceNormals", &size, (void**)&meshFaces );
		assert( SUCCEEDED( hr ) );
		assert( size == sizeof( DWORD )*4*(*nFaceNormals) );



		

	}




};
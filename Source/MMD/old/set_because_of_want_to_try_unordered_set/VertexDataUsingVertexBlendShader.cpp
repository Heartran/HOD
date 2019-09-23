#include"VertexDataUsingVertexBlendShader.h"
#include"Pmd.h"
#include"Model.h"
#include"Face.h"
#include"Bone.h"
#include <GameLib/Graphics/Manager.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <GameLib/Graphics/Vertex.h>
#include <string>
#include <GameLib/GameLib.h>
#include <GameLib/Graphics/Enum.h>
#include <GameLib/Math/Vector4.h>
#include <GameLib/Graphics/IndexBuffer.h>
#include <GameLib/Graphics/VertexBuffer.h>
#include <GameLib/Graphics/AnimatedVertexBoneBuffer.h>
#include <GameLib/Graphics/AnimatedVertexBone.h>
#include <GameLib/Graphics/Vertex.h>
#include <GameLib/Graphics/Texture.h>
#include "helper/MyOutputDebugString.h"
#include <tchar.h>
#include <numeric>

#include"SegaTypeHelper.h"
#include<GameLib/Framework.h>

#include"MME/Manager.h"

using namespace GameLib::Graphics;
using namespace GameLib::Math;
//
//ID3DXEffect*					VertexDataUsingVertexBlendShader::sEffect					=NULL;
//IDirect3DVertexShader9*			VertexDataUsingVertexBlendShader::sVertexShader				=NULL;
//IDirect3DPixelShader9*			VertexDataUsingVertexBlendShader::sPixelShader				=NULL;
//IDirect3DVertexDeclaration9*	VertexDataUsingVertexBlendShader::sVertexDeclaration		=NULL;
//LPD3DXCONSTANTTABLE             g_pConstantTable											= NULL;

namespace {

typedef VertexDataUsingVertexBlendShader::TypeBoneIndex TypeBoneIndex;
//(1,2,3,4), �Ƃ����̂��������Ƃ�����
//(1,3) ��������true��Ԃ�
//(1,2,3,4,5) �������� false��Ԃ�
//(6)�@�͂Ȃ��̂Ł@false��Ԃ�
bool do_a_have_b( 
	const std::set<TypeBoneIndex> &a,
	const std::set<TypeBoneIndex> &b );

//{ (1,2,3,4), (1,5), (6,7,9) } �Ƃ����̂��������Ƃ�����
//(1,3)��������0��Ԃ�
//(5,)�������� 1��Ԃ�
//((4,6,7)�@�͂Ȃ��̂Ł@-1��Ԃ�
int whichGroup(
	const std::set< std::set<TypeBoneIndex> > &patterns,
	const std::set<TypeBoneIndex> &pattern );

//�܂܂�Ă���d�������v�f����菜��
//{ (1,2,3,4),(1,3) } �Ƃ����̂��������Ƃ����� (1,3) �́@(1,2,3,4)�@�Ɋ܂܂�Ă���̂ŏ���
void removeContainedDuplicateElement( std::set< std::set<TypeBoneIndex> > *elements );

void assign( std::vector<TypeBoneIndex> *dst, const std::set<TypeBoneIndex> &source ){
	dst->reserve( source.size() );
	for( std::set<TypeBoneIndex>::iterator it=source.begin(), end=source.end();
		it!=end;
		++it )
	{
		dst->push_back( *it );
	}
	dst->shrink_to_fit();//�O�̂���
}

}// end no namespace

struct VertexDataUsingVertexBlendShader::Vertex {
	D3DXVECTOR4 mPosition;
	D3DXVECTOR3 mNormal;
	unsigned mColor; //���_�F
	D3DXVECTOR2 mUv;
};

struct VertexDataUsingVertexBlendShader::VertexBone {
	float mWeights[MODEL_BLEND_COUNT];
	TypeBoneIndex mIndices[MODEL_BLEND_COUNT];//�e������{�[���̃C���f�b�N�X
	void operator=( const VertexBone &vbBone ){
		for( int i=0; i<MODEL_BLEND_COUNT; ++i ){
			mWeights[i] = vbBone.mWeights[i];
			mIndices[i] = vbBone.mIndices[i];
		}
	}
};



VertexDataUsingVertexBlendShader::VertexDataUsingVertexBlendShader() 
	:vertexBuffer_(0),
	vbBone_(0),
	numVertices_(0),
	indexBuffer_(0),
	usingBoneIndicesMap_(0),
	numGroups_(0),
	numIndeicesOfGroupInSubset_(0),
	numSubsets_(0),
	usingBoneIndexPatternOfGroupInSubset_(0),
	vbOfGroupInSubset_(0),
	ibOfGroupInSubset_(0),
	vbBoneOfGroupInSubset_(0)
{
	vertexBuffer_ = NEW GameLib::Graphics::VertexBuffer();
	indexBuffer_  = NEW GameLib::Graphics::IndexBuffer();
	vbBone_ = NEW GameLib::Graphics::AnimatedVertexBoneBuffer();
}

VertexDataUsingVertexBlendShader::~VertexDataUsingVertexBlendShader() {
	
	//��n��
	for ( unsigned int i=0; i<numSubsets_; ++i) 
	{
		
		SAFE_DELETE_ARRAY( vbBoneOfGroupInSubset_[i] );
		SAFE_DELETE_ARRAY( ibOfGroupInSubset_[i] );
		SAFE_DELETE_ARRAY( vbOfGroupInSubset_[i] );
		
		SAFE_DELETE_ARRAY( numIndeicesOfGroupInSubset_[i] );
		SAFE_DELETE_ARRAY( usingBoneIndexPatternOfGroupInSubset_[i] );
	}

	SAFE_DELETE_ARRAY( vbBoneOfGroupInSubset_ );
	SAFE_DELETE_ARRAY( ibOfGroupInSubset_ );
	SAFE_DELETE_ARRAY( vbOfGroupInSubset_ );

	SAFE_DELETE_ARRAY( numIndeicesOfGroupInSubset_ ); 
	SAFE_DELETE_ARRAY( numGroups_ );
	
	SAFE_DELETE_ARRAY( usingBoneIndexPatternOfGroupInSubset_ );

	SAFE_DELETE(vertexBuffer_);
	SAFE_DELETE(vbBone_);
	SAFE_DELETE(indexBuffer_);
	SAFE_DELETE_ARRAY( usingBoneIndicesMap_ );
}

const char* VertexDataUsingVertexBlendShader::load(const char *pData) {
	//Manager m = Manager::instance();
	//IDirect3DDevice9 *pDevice=m.dxDevice();
	//-----------------------------------------------------
	// ���_���擾
	numVertices_ = *( reinterpret_cast<const unsigned long *>(pData) );
	pData += sizeof( unsigned long );

	// ���_�z����R�s�[
	*vertexBuffer_ = GameLib::Graphics::VertexBuffer::create( numVertices_, true );

	*vbBone_ = GameLib::Graphics::AnimatedVertexBoneBuffer::create( numVertices_ );
		
	GameLib::Graphics::Vertex *v;
	GameLib::Graphics::AnimatedVertexBone* vBone;
	v = vertexBuffer_->lock();
	vBone = vbBone_->lock();
				
	originalVertexBufferBone_.reserve(numVertices_);
	for (unsigned long i=0; i<numVertices_; ++i) {
		const PMD_VERTEX *pVertex = reinterpret_cast<const PMD_VERTEX *>(pData);
	
		v[i].mPosition.x=pVertex->x;
		v[i].mPosition.y=pVertex->y;
		v[i].mPosition.z=pVertex->z;
		v[i].mPosition.w=1.f;

		//TRACE3( "( %f, %f, %f )\n", v[i].mPosition.x, v[i].mPosition.y, v[i].mPosition.z );

		v[i].mNormal.x=pVertex->nx;
		v[i].mNormal.y=pVertex->ny;
		v[i].mNormal.z=pVertex->nz;
		//�}�e���A���Œ�������̂Ŕ��ŕ��C�̂͂�
		v[i].mColor= 0xffffffff;
				

		for (unsigned long j=0; j<MODEL_BLEND_COUNT; j++) {
			vBone[i].mIndices[j]=(std::numeric_limits<TypeBoneIndex>::max)();
			vBone[i].mWeights[j]=0.f;
		}
		vBone[i].mWeights[0]=static_cast<float>(pVertex->weight)/100.f;
		vBone[i].mWeights[1]=1.f-vBone[i].mWeights[0];
		
		vBone[i].mIndices[0]=static_cast<TypeBoneIndex>(pVertex->bone[0]);
		vBone[i].mIndices[1]=static_cast<TypeBoneIndex>(pVertex->bone[1]);
		assert(pVertex->bone[0]<(std::numeric_limits<TypeBoneIndex>::max)() && "buffer over flow. char���ᖳ���B");
		assert(pVertex->bone[1]<(std::numeric_limits<TypeBoneIndex>::max)() && "buffer over flow. char���ᖳ���B");
		//TRACE2( "mWeights[2]: %f, %f\n", vBone[i].mWeights[0], vBone[i].mWeights[1] ); 
		//GameLib::Graphics::AnimatedVertexBone *pvbBone;
		//pvbBone=&vBone[i];
		//assert( vBone[i].mWeights[0]!=0.f );



		originalVertexBufferBone_.push_back( BoneIndex( vBone[i].mIndices ) );

		v[i].mUv.x=pVertex->u;
		v[i].mUv.y=pVertex->v;
					
		pData += sizeof( PMD_VERTEX );
	}
	vbBone_->unlock(&vBone);
	vertexBuffer_->unlock(&v);

	// ���_�C���f�b�N�X���擾
	numIndices_ = *( reinterpret_cast<const unsigned long *>(pData) );
	pData += sizeof( unsigned long );
	
	*indexBuffer_ = GameLib::Graphics::IndexBuffer::create( numIndices_ );

	unsigned short *ib;
	ib = indexBuffer_->lock();
	for( unsigned int i=0; i<numIndices_; ++i) {
		ib[i]=*( reinterpret_cast<const unsigned short *>(pData) );
		pData += sizeof(unsigned short);
	}
	indexBuffer_->unlock(&ib);
	
	return pData;
}

void VertexDataUsingVertexBlendShader::draw(
	const CBone *bones, const unsigned short &boneSize,
	const CModel::Subset *subsets, const unsigned long &subsetSize,
	const CModel::Material *materials, const unsigned long &materialSize )
{
	//HRESULT hr;
	Manager m = Manager::instance();


	//IDirect3DDevice9 *pDevice=m.dxDevice();
	m.enableVertexBlend( true );
	//int isBool=1;
	//hr = pDevice->SetVertexShaderConstantB( 11, &isBool, 1 );
	//assert( SUCCEEDED( hr ) );

	//�o�b�t�@���Z�b�g
	m.setVertexBuffer( *vertexBuffer_ );
	m.setIndexBuffer( *indexBuffer_ );
	////�{�[���̃o�[�e�b�N�X�o�b�t�@�Z�b�g
	m.setAnimatedVertexBoneBuffer( *vbBone_ );

	unsigned long numFinishedIndices = 0;//�����I�����index�̐�

	// �T�u�Z�b�g�̕`��
	for (unsigned int subsetIdx=0; subsetIdx<subsetSize; ++subsetIdx) {
		
		const CModel::Subset* subset=&subsets[subsetIdx];
		
		// �F�A�e�N�X�`��
		unsigned long mi=subset->materialIndex_;
		const CModel::Material *pMaterial = &materials[mi];
		
		m.setDiffuseColor( pMaterial->diffuse_ );
		m.setTransparency( pMaterial->diffuse_.w );
		m.setAmbientColor( pMaterial->ambient_ );
		m.setSpecularColor( pMaterial->specular_ );
		m.setSpecularSharpness( pMaterial->power_ );
		m.setEmissionColor( pMaterial->emissive_ );
		m.setTexture( pMaterial->texture_ );

		for( unsigned int groupIdx=0; groupIdx<numGroups_[subsetIdx]; ++groupIdx ){

			const int &numDrawingIndices = numIndeicesOfGroupInSubset_[subsetIdx][groupIdx];
			assert(numDrawingIndices%3==0);
			//assert( numDrawingIndices==ibOfGroupInSubset_[subsetIdx][groupIdx].indexNumber() );
			sendMatrixArrayToShader( bones, subsetIdx, groupIdx, numFinishedIndices );

			m.drawIndexed( numFinishedIndices, numDrawingIndices/3 );

			numFinishedIndices += numDrawingIndices;

		}
	}

	//�X�g���[���������Ƃ�
	//pDevice->SetStreamSource(1, 0, 0, 0);
}

void VertexDataUsingVertexBlendShader::drawByMME(
	const CBone *bones, const unsigned short &boneSize,
	const CModel::Subset *subsets, const unsigned long &subsetSize,
	const CModel::Material *materials, const unsigned long &materialSize )
{

	//HRESULT hr;
	//Manager ggm = Manager::instance();//Gamelib Graphics Manager
	MME::Manager *mmem = MME::Manager::instance();// Miku Miku Effect Manager

	D3DXMATRIX matI;
	D3DXMatrixIdentity( &matI );
	MME::Manager::instance()->setWorldMatrix( matI );
	MME::Manager::instance()->setLightWorldMatrix( 0, matI );

	//IDirect3DDevice9 *pDevice=m.dxDevice();
	mmem->enableVertexBlend( true );
	//int isBool=1;
	//hr = pDevice->SetVertexShaderConstantB( 11, &isBool, 1 );
	//assert( SUCCEEDED( hr ) );

	//�o�b�t�@���Z�b�g
	mmem->setVertexBuffer( *vertexBuffer_ );
	mmem->setIndexBuffer( *indexBuffer_ );
	////�{�[���̃o�[�e�b�N�X�o�b�t�@�Z�b�g
	mmem->setAnimatedVertexBoneBuffer( *vbBone_ );

	unsigned long numFinishedIndices = 0;//�����I�����index�̐�

	// �T�u�Z�b�g�̕`��
	for (unsigned int subsetIdx=0; subsetIdx<subsetSize; ++subsetIdx) {
		
		const CModel::Subset* subset=&subsets[subsetIdx];
		
		unsigned long mi=subset->materialIndex_;
		const CModel::Material *pMaterial = &materials[mi];
		using namespace MME;

		UseTexture useTexture = 
			static_cast<const GameLib::Graphics::Texture &>(pMaterial->texture_) ? ENABLE_TEXTURE : DISABLE_TEXTURE;
		//bool useToon = static_cast<bool>( (void*)pMaterial->toonTexture_ );
		UseToon useToon = pMaterial->toonIndex_ != 0xff ? ENABLE_TOON : DISABLE_TOON;
		UseSphereMap useSphereMap =
			static_cast<const GameLib::Graphics::Texture &>(pMaterial->sphereTexture_) ? ENABLE_SPHEREMAP : DISABLE_SPHEREMAP;
		
		//��v���Ă���ꍇ�̂ݕ`�悾���A
		//zplot���̂݁Atexture��}�e���A����S�Ė������ď�������
		assert( mi < (std::numeric_limits< unsigned int >::max)() );
		if( mmem->canDraw( useTexture, useSphereMap, useToon, mi ) )
		{}
		else {
			for( unsigned int groupIdx=0; groupIdx<numGroups_[subsetIdx]; ++groupIdx ){

				const int &numDrawingIndices = numIndeicesOfGroupInSubset_[subsetIdx][groupIdx];
				assert(numDrawingIndices%3==0);

				//zplot���̂݁Atexture��}�e���A����S�Ė������ď�������
				if( mmem->getCurrentPass() == MME::MP_ZPLOT ){
					sendMatrixArrayToMMEShader( bones, subsetIdx, groupIdx, numFinishedIndices );
					mmem->drawIndexed( numFinishedIndices, numDrawingIndices/3 );
				}
				//assert( numDrawingIndices==ibOfGroupInSubset_[subsetIdx][groupIdx].indexNumber() );

				numFinishedIndices += numDrawingIndices;
			}
			continue;
		}

		mmem->setDiffuseColor( pMaterial->diffuse_ );
		//m.setTransparency( pMaterial->diffuse_.a );
		mmem->setAmbientColor( pMaterial->ambient_ );
		mmem->setSpecularColor( pMaterial->specular_ );
		mmem->setSpecularSharpness( pMaterial->power_ );
		mmem->setEmissionColor( pMaterial->emissive_ );
		mmem->setTexture( pMaterial->texture_ );
		
		if( useSphereMap ){
			mmem->setSphereTextureAdditive( pMaterial->isSphereTextureAdditive_!=0 );
			mmem->setSphereTexture( pMaterial->sphereTexture_ );
		}

		if( useToon ){
			mmem->setToonTexture( pMaterial->toonTexture_ );
			mmem->setToonColor( pMaterial->toonColor_ );
		}

		for( unsigned int groupIdx=0; groupIdx<numGroups_[subsetIdx]; ++groupIdx ){

			const int &numDrawingIndices = numIndeicesOfGroupInSubset_[subsetIdx][groupIdx];
			assert(numDrawingIndices%3==0);
			//assert( numDrawingIndices==ibOfGroupInSubset_[subsetIdx][groupIdx].indexNumber() );
			sendMatrixArrayToMMEShader( bones, subsetIdx, groupIdx, numFinishedIndices );

			mmem->drawIndexed( numFinishedIndices, numDrawingIndices/3 );

			numFinishedIndices += numDrawingIndices;
		}
	}

	//�X�g���[���������Ƃ�
	//pDevice->SetStreamSource(1, 0, 0, 0);
}

void VertexDataUsingVertexBlendShader::updateFaceVertex(const CFace *faces, const short& size ) {
	if( !faces ){
		return;
	}
	if(size==0){
		return;
	}
	GameLib::Graphics::Vertex *vb;
	vb = vertexBuffer_->lock();			
	const CFace *pBase = faces;

	//base�ŏ�����
	CFaceVertex *pBaseFaceVertexArray = pBase->vertices_[0];
	for(unsigned int i=0, imax=pBase->vertices_.size(); i<imax; ++i) {
		const D3DXVECTOR3 &baseVec3 = pBaseFaceVertexArray[i].vec3_;
		for(unsigned int k=0, kmax=pBaseFaceVertexArray[i].numReplacedIndices_; k<kmax; ++k) {
			const unsigned long & index=pBaseFaceVertexArray[i].replacedIndices_[k];
			vb[index].mPosition.set( baseVec3.x, baseVec3.y, baseVec3.z );
		}
	}

	const CFace *faceArray = faces;
	//i=0�̓x�[�X�B�����甲����
	for( unsigned int i=1, imax=size; i<imax; ++i ) {
		const CFace *pFace = &faceArray[i];
		if(pFace->weight_==0) continue;
		for( unsigned int j=0, jmax=pFace->vertices_.size(); j<jmax; ++j) {
			const CFaceVertex * pFaceVertex=pFace->vertices_[j];
			//���`��� V1 + s(V2-V1)
			GameLib::Vector3 vOut( vb[pFaceVertex->replacedIndices_[0]].mPosition );
			GameLib::Vector3 v = toVector3(pFaceVertex->vec3_);

			v -= vOut;
			v *=pFace->weight_;
			vOut += v;

			for(unsigned int k=0, kmax=pFaceVertex->numReplacedIndices_; k<kmax; ++k) {
				const unsigned long & index=pFaceVertex->replacedIndices_[k];
				//vb[index].mPosition.set( vOut.x, vOut.y, vOut.z, 1.0f );
				vb[index].mPosition = vOut;
				break;
			}
		}
	}
	vertexBuffer_->unlock(&vb);
}

void VertexDataUsingVertexBlendShader::init(
		const CModel::Subset *subsets,	const unsigned long &subsetSize,
		CFace* faces, const unsigned long &faceSize )
{
	//IndexBuffer�̕��ёւ�

	GameLib::Graphics::IndexBuffer tempIndexBuffer;
	tempIndexBuffer = GameLib::Graphics::IndexBuffer::create(numIndices_);
	
	GameLib::Graphics::AnimatedVertexBone *vbBone;
	vbBone = vbBone_->lock();

	unsigned short* ib;
	ib = indexBuffer_->lock();
	
	unsigned short* tib;//tempIndexBuffer
	tib = tempIndexBuffer.lock();

	memcpy( tib, ib, sizeof(unsigned short)*numIndices_ );
//#ifdef _DEBUG	
//	for(unsigned int i=0; i<numIndices_; ++i ){
//		assert( tib[i]==ib[i] );
//	}
//#endif//DEBUG
	
	numSubsets_ = subsetSize;
	//subset i�ɂ�����O���[�v�̐�
	numGroups_ = NEW unsigned int[numSubsets_];
	//subset i�ɂ�����O���[�v���̃C���f�b�N�X�̐�
	numIndeicesOfGroupInSubset_ = NEW unsigned int*[numSubsets_];

	//subset idx �ɂ�����@�O�p�`�P�ʂł̎g���Ă���{�[���C���f�b�N�X�̃p�^�[��
	std::set<TypeBoneIndex>** usingBoneIndexPatternOfGroupInSubset = NEW std::set<TypeBoneIndex>*[numSubsets_];
	
	for (unsigned int i=0; i<numSubsets_; ++i) {
		const CModel::Subset* subset=&(subsets[i]);
		std::set< std::set<TypeBoneIndex> > patternsInSubset;
		for( unsigned int j=0; j<subset->faceCount_*3; j+=3){
			assert(j+subset->faceIndex_*3 + 2 < numIndices_ );
			std::set<TypeBoneIndex> pattern;

			for( unsigned int triangleIdx=0; triangleIdx<3; ++triangleIdx ){
				const unsigned short &index = ib[j+subset->faceIndex_*3+triangleIdx];
					
				//GameLib::Graphics::AnimatedVertexBone *vBone = &vbBone[index];	
				//BoneIndex bi = originalVertexBufferBone_[index];
				//assert(vBone->index[0]==originalVertexBufferBone_[index][0]);
				
				for( unsigned int blendCount=0; blendCount<MODEL_BLEND_COUNT; ++blendCount ){
					const TypeBoneIndex &boneIndex = originalVertexBufferBone_[index][blendCount];
					if( boneIndex==(std::numeric_limits<TypeBoneIndex >::max)() ){
					} else {
						pattern.insert( boneIndex );
					}
				}
			}

			patternsInSubset.insert( pattern );
		}

		//TRACE1( "material%d�ł̎O�p�`�P�ʂ̎g�p�{�[��\n", i );	
		//BOOST_FOREACH( const std::set<TypeBoneIndex> & pattern, patternsInSubset )
		//{
		//	TRACE1( "  ( ", 0 );	
		//	BOOST_FOREACH( TypeBoneIndex bi, pattern ){
		//		TRACE1( "%d, ", bi );	
		//	}
		//	TRACE1( ")\n", 0 );	

		//}

		//�d������菜��
		removeContainedDuplicateElement( &patternsInSubset );
		//�����o�[�ϐ��ɃR�s�[
		usingBoneIndexPatternOfGroupInSubset[i] = NEW std::set<TypeBoneIndex>[ patternsInSubset.size() ];
		{
			unsigned int groupIdx=0;
			for(
				std::set< std::set<TypeBoneIndex> >::iterator it=patternsInSubset.begin(),
				end = patternsInSubset.end();
				it!=end;
				++it, ++groupIdx )
			{
				assert(groupIdx<patternsInSubset.size());
				const std::set<TypeBoneIndex> & pattern = *it; 
				assert( pattern.size() <= MODEL_BLEND_COUNT*3 );//1�̒��_�����Ă�ő�̃{�[����*3(�O�p�`)
				usingBoneIndexPatternOfGroupInSubset[i][groupIdx] = pattern;
			}
		}

		//{
		//	unsigned int groupIdx=0;
		//	TRACE1( "material%d�ł̎O�p�`�P�ʂ̎g�p�{�[��\n", i );	
		//	BOOST_FOREACH( const std::set<TypeBoneIndex> & pattern, patternsInSubset )
		//	{
		//		TRACE1( "group%d  ( ", groupIdx );	
		//		BOOST_FOREACH( TypeBoneIndex bi, pattern ){
		//			TRACE1( "%d, ", bi );	
		//		}
		//		TRACE1( ")\n", 0 );	
		//		++groupIdx;
		//	}
		//}

		//TRACE1( "material%d�ł̃O���[�v�����J�n\n", i );	
		//TRACE1( "indexBuffer��index : groupId \n", i );	
		//group����
		std::multimap< int, unsigned long > group_indexMap;//key groupId, value indexBuffer��index
		for( unsigned int j=0; j<subset->faceCount_*3; j+=3){
			assert(j+subset->faceIndex_*3 + 2 < numIndices_ );
			std::set<TypeBoneIndex> pattern;

			
			for( unsigned int triangleIdx=0; triangleIdx<3; ++triangleIdx ){
				//���בւ��Œl���ς��̂�tempIndexBuffer���g��
				const unsigned short &index = tib[j+subset->faceIndex_*3+triangleIdx];
				for( unsigned int blendCount=0; blendCount<MODEL_BLEND_COUNT; ++blendCount ){
					const TypeBoneIndex &boneIndex = originalVertexBufferBone_[index][blendCount];
					if( boneIndex==(std::numeric_limits<TypeBoneIndex >::max)() ){
					} else {
						pattern.insert( boneIndex );
					}
				}
			}

			//���̎O�p�`�͂ǂ̃O���[�v(�{�[���C���f�b�N�X�̃p�^�[��)�ɓ��邩?
			int group = whichGroup( patternsInSubset, pattern );
			assert( group!=-1 );//�A�T�[�g����悤�Ȃ�o�O����
			//TRACE2( "  index %d: %d\n", j+subset->faceIndex_*3, group );	

			group_indexMap.insert( std::pair<int, unsigned long>(group, j+subset->faceIndex_*3) );
		}

		//���בւ���̒l
		//TRACE1( "���בւ���", 1 );	
		//TRACE1( "indexBuffer��index : groupId \n", 1 );	
		std::pair<int, unsigned long> pair;
		//BOOST_FOREACH( pair, group_indexMap ){
		//	TRACE2( "  index %d: %d\n", pair.first, pair.second );	
		//}

		//subset i�ɂ�����O���[�v�̐�
		numGroups_[i] = group_indexMap.rbegin()->first+1;
		//subset i�ɂ�����O���[�v���̃C���f�b�N�X�̐�
		numIndeicesOfGroupInSubset_[i] = NEW unsigned int[ numGroups_[i] ];
		int total = 0;
		for( unsigned int groupidx=0; groupidx<numGroups_[i]; ++groupidx ){
			numIndeicesOfGroupInSubset_[i][groupidx] = group_indexMap.count(groupidx)*3; 
			total += numIndeicesOfGroupInSubset_[i][groupidx];
		}
		assert( group_indexMap.size() == subset->faceCount_ );
		assert( total == subset->faceCount_*3 );

		//indexBuffer����ёւ�
		int j=0; 
		BOOST_FOREACH( pair, group_indexMap ){
			ib[j+subset->faceIndex_*3]		= tib[ pair.second ];
			ib[j+subset->faceIndex_*3 + 1]	= tib[ pair.second+1 ];
			ib[j+subset->faceIndex_*3 + 2]	= tib[ pair.second+2 ];
			j+=3;

		}
	}

//#ifdef _DEBUG	
//	long int total = 0;
//	for (unsigned int i=0; i<subsets.size(); ++i) {
//		for( int groupidx=0; groupidx<numGroups_[i]; ++groupidx ){
//			total += numIndeicesOfGroupInSubset_[i][groupidx];				
//		}
//	}
//	assert( numIndices_ == total );
//#endif//_DEBUG
	

	//�V�F�[�_�̃��W�X�^�ɓ���œK�Ȓl�ɕ����������邽�߁A�ǂ��ŕ����邩���肷��B
	//���݂͍ŏ��̒l�ŃO���[�v�������Ă���B
	//�ő�ł��A3���_*4(MODEL_BLEND_COUNT)=12
	//�T�u�Z�b�g�𒴂��Ă͓������Ȃ�
	//int **numIndeicesOfGroupInSubset_;
	//std::set<TypeBoneIndex>** newUsingBoneIndexPatternsOfGroupInSubset;
	//�����̂��ړI

	unsigned int* newNumsGroups;
	newNumsGroups = NEW unsigned int [subsetSize];

	unsigned int **newNumIndeicesOfGroupInSubset;
	newNumIndeicesOfGroupInSubset = NEW unsigned int*[subsetSize];

	std::set<TypeBoneIndex>** newUsingBoneIndexPatternsOfGroupInSubset;
	newUsingBoneIndexPatternsOfGroupInSubset = NEW std::set<TypeBoneIndex>* [subsetSize];



	//�V�����O���[�v�̕�������m��(vector�g���΂����񂾂��ǁA�����o�[�ϐ����ȒP�ɓ���ւ��邽�߂Ɍ^�����킹�Ă���)
	for(unsigned int subsetIdx=0; subsetIdx<subsetSize; ++subsetIdx) {
		unsigned int newNumGroups=0;
		std::set<TypeBoneIndex> newBoneIndexPattern;

		//group�P�ʂœ����������̂�groupIdx�͎����ŃC���N�������g
		for( unsigned int groupIdx=0; groupIdx<numGroups_[subsetIdx]; ){
			std::vector<TypeBoneIndex> tempBoneIndices;
			
			//�V�����ǉ�������̂Ƃ��̃T�C�Y��m�肽��
			for(
				std::set<TypeBoneIndex>::iterator it = usingBoneIndexPatternOfGroupInSubset[subsetIdx][groupIdx].begin(),
				end = usingBoneIndexPatternOfGroupInSubset[subsetIdx][groupIdx].end();
				it!=end;
				++it
			){ 
				//���ɂȂ��ꍇ
				if( newBoneIndexPattern.find( *it ) == newBoneIndexPattern.end() ){
					tempBoneIndices.push_back( *it );
				}
			}

			//�e�ʂ�����̂�newBoneIndexPattern�ɓ����			
			if( SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE > tempBoneIndices.size()+newBoneIndexPattern.size()  ) {
				for( int i=0, size=tempBoneIndices.size(); i<size; ++i ){
					newBoneIndexPattern.insert( tempBoneIndices[i] );
				}
				++groupIdx;
			}
			else {
				//�����A����Ȃ��̂ł����ň��؂�
				newBoneIndexPattern.clear();
				++newNumGroups;
				continue;
			}
		}
		//�Ō�̓z�͓����Ă��Ȃ��̂ő���
		++newNumGroups;
		newNumsGroups[subsetIdx] = newNumGroups;
		newNumIndeicesOfGroupInSubset[subsetIdx] = NEW unsigned int[newNumGroups];
		//������
		for( unsigned int i=0; i<newNumGroups; ++i){
			newNumIndeicesOfGroupInSubset[subsetIdx][i]=0;
		}
		newUsingBoneIndexPatternsOfGroupInSubset[subsetIdx] = NEW std::set<TypeBoneIndex>[newNumGroups];
	}
	//�V�F�[�_�̃��W�X�^�ɓ���œK�Ȓl�ɕ����������邽�߁A�V�����O�[���[�v�킯������
	for(unsigned int subsetIdx=0; subsetIdx<subsetSize; ++subsetIdx) {
		unsigned int newGroupIdx=0;
		std::set<TypeBoneIndex> newBoneIndexPattern;
		unsigned int groupIdx=0;
		//group�P�ʂœ����������̂�groupIdx�͎����ŃC���N�������g
		for( ; groupIdx<numGroups_[subsetIdx]; ){
			
			std::vector<TypeBoneIndex> tempBoneIndices;
			
			//�V�����ǉ�������̂Ƃ��̃T�C�Y��m�肽��
			for(
				std::set<TypeBoneIndex>::iterator it = usingBoneIndexPatternOfGroupInSubset[subsetIdx][groupIdx].begin(),
				end = usingBoneIndexPatternOfGroupInSubset[subsetIdx][groupIdx].end();
				it!=end;
				++it
			){ 
				//group�P�ʂŃT�C�Y�����߂����̂ŁA�V�����������̂̑傫����m�肽��
				//���ɂȂ��ꍇ
				if( newBoneIndexPattern.find( *it ) == newBoneIndexPattern.end() ){
					tempBoneIndices.push_back( *it );
				}
			}
			//�e�ʂ�����̂�newBoneIndexPattern�ɓ����			
			if( SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE > tempBoneIndices.size()+newBoneIndexPattern.size()  ) {
				for( int i=0, size=tempBoneIndices.size(); i<size; ++i ){
					newBoneIndexPattern.insert( tempBoneIndices[i] );
				}
				const int &newNumIdx = newNumIndeicesOfGroupInSubset[subsetIdx][newGroupIdx];
				const int &oldNumIdx = numIndeicesOfGroupInSubset_[subsetIdx][groupIdx];

				newNumIndeicesOfGroupInSubset[subsetIdx][newGroupIdx] += numIndeicesOfGroupInSubset_[subsetIdx][groupIdx];
				++groupIdx;
			}
			else {
				//����Ȃ��̂ł����ŕۑ�
				newUsingBoneIndexPatternsOfGroupInSubset[subsetIdx][newGroupIdx] = newBoneIndexPattern;
				newBoneIndexPattern.clear();
				++newGroupIdx;
				continue;
			}
		}
		//�Ō�̓z�͓����Ă��Ȃ��̂ŕۑ�
		newUsingBoneIndexPatternsOfGroupInSubset[subsetIdx][newGroupIdx] = newBoneIndexPattern;
		newBoneIndexPattern.clear();
		++newGroupIdx;
		assert(newGroupIdx==newNumsGroups[subsetIdx]);
	}

//#ifdef _DEBUG	
//	total = 0;
//	for (unsigned int i=0; i<subsets.size(); ++i) {
//		for( int groupidx=0; groupidx<newNumsGroups[i]; ++groupidx ){
//			total += newNumIndeicesOfGroupInSubset[i][groupidx];				
//		}
//	}
//	assert( numIndices_ == total );
//#endif//_DEBUG

	//{
	//	TRACE1( "�V���������������\n", 0 );	

	//	for(unsigned int subsetIdx=0; subsetIdx<subsetSize; ++subsetIdx) {
	//		TRACE1( "material%d�ł̎O�p�`�P�ʂ̎g�p�{�[��\n", subsetIdx );	
	//		for( unsigned int groupIdx=0; groupIdx<newNumsGroups[subsetIdx]; ++groupIdx ){
	//			TRACE1( "group%d  ( ", groupIdx );	
	//			BOOST_FOREACH( TypeBoneIndex bi, newUsingBoneIndexPatternsOfGroupInSubset[subsetIdx][groupIdx] )
	//			{
	//				TRACE1( "%d, ", bi );	
	//			}
	//			TRACE1( ")\n", 0 );	
	//		}
	//	}
	//}

	//�V�����ق��Ƀ����o�[�֐����X�V
	//�Â��ق����J��
	for ( unsigned int i=0; i<numSubsets_; ++i) {
		SAFE_DELETE_ARRAY( numIndeicesOfGroupInSubset_[i] );
		SAFE_DELETE_ARRAY( usingBoneIndexPatternOfGroupInSubset[i] );
	}

	SAFE_DELETE_ARRAY( numIndeicesOfGroupInSubset_ ); 
	SAFE_DELETE_ARRAY( numGroups_ );
	SAFE_DELETE_ARRAY( usingBoneIndexPatternOfGroupInSubset );

	//�V�����l�Ƀ|�C���^��t���ւ�
	numIndeicesOfGroupInSubset_ = newNumIndeicesOfGroupInSubset;
	numGroups_ = newNumsGroups;
	//usingBoneIndexPatternOfGroupInSubset_ = newUsingBoneIndexPatternsOfGroupInSubset;
	
	assert( usingBoneIndexPatternOfGroupInSubset_==0 );
	usingBoneIndexPatternOfGroupInSubset_ = NEW std::vector<TypeBoneIndex>*[subsetSize];
	for( unsigned int subsetIdx=0; subsetIdx<subsetSize; ++subsetIdx ){ 
		usingBoneIndexPatternOfGroupInSubset_[subsetIdx] = NEW std::vector<TypeBoneIndex>[ numGroups_[subsetIdx] ];
	}

	//set����vector�ɕϊ��@�����o�[�ϐ��ɓ����
	for ( unsigned int i=0; i<numSubsets_; ++i) {
		for ( unsigned int j=0; j<numGroups_[i]; ++j) {	
			assign( &usingBoneIndexPatternOfGroupInSubset_[i][j], newUsingBoneIndexPatternsOfGroupInSubset[i][j] );
		}
		SAFE_DELETE_ARRAY( newUsingBoneIndexPatternsOfGroupInSubset[i] );
	}
	SAFE_DELETE_ARRAY( newUsingBoneIndexPatternsOfGroupInSubset );

	//BoneIndex�̒u���A�p�[�c�̕���
	//�T�u�Z�b�g�O���[�v���Ƃ�indexBuffer��vertexBuffer�𕪊����āA�V���ɍ��

	//faceIndex��u�����邽�߂�Table
	//key: oldVertexBufferIndex, value: newVertexBufferIndex
	std::map< unsigned long, std::set<unsigned long> > replacedFaceVertexIndexTable;

	GameLib::Graphics::Vertex *vb;
	vb=vertexBuffer_->lock();
	vbOfGroupInSubset_ = NEW GameLib::Graphics::VertexBuffer* [subsetSize];
	ibOfGroupInSubset_ = NEW GameLib::Graphics::IndexBuffer* [subsetSize];
	vbBoneOfGroupInSubset_ = NEW  GameLib::Graphics::AnimatedVertexBoneBuffer* [subsetSize];
	
	typedef std::set<unsigned long> UniqueIndices;
	//indexBuffer�̒u���e�[�u��
	typedef std::map<unsigned long, unsigned long> ReplacementTable; // key oldIndexBufferIdx, value newIndexBufferIdx;

	unsigned long ibStartIndex=0;//indexbuffer
	unsigned long vbStartIndex=0;
	for (unsigned int subsetIdx=0; subsetIdx<subsetSize; ++subsetIdx) {
		
		const int &numGroup = numGroups_[subsetIdx];
		vbOfGroupInSubset_[subsetIdx] = NEW GameLib::Graphics::VertexBuffer [numGroup];
		ibOfGroupInSubset_[subsetIdx] = NEW GameLib::Graphics::IndexBuffer [numGroup];
		vbBoneOfGroupInSubset_[subsetIdx] = NEW GameLib::Graphics::AnimatedVertexBoneBuffer [numGroup];
		

		for( int groupIdx=0; groupIdx<numGroup; ++groupIdx ){

			const unsigned int numIndices = numIndeicesOfGroupInSubset_[subsetIdx][groupIdx]; 
			
			//create replacement table
			ReplacementTable rpcmtTbl;
			UniqueIndices uniqueIndices;
			//���בւ��Əd�����Ȃ�
			//VertexBuffer�̌��̕��я��͈ێ�������
			/*-------------------------------------------
				VertexBuffer
				0 : a �g��Ȃ�
				1 :	b �g��
				2 :	c �g��
				3 :	d �g��Ȃ�
				4 :	e �g��

				IndexBuffer
				0 : 4
				1 :	1
				2 :	2

				new VertexBuffer
				0 : b �g��
				1 :	c �g��
				2 :	e �g��

				new IndexBuffer
				0 : 2
				1 :	0
				2 :	1
			-------------------------------------------*/
			//�܂���������
			//vector�ł͂Ȃ�set�Ȃ͕̂��я��A�d�����Ȃ�����
			//���я���412���ƎO�p�`�̍œK�����Ȃ��Ȃ邩������Ȃ�����(vertexBuffer�̂���)
			/*
				uniqueIndices
				1
				2
				4
			*/			
			//���j�[�N�ȃC���f�b�N�X�̃e�[�u�������
			for( unsigned long ibIndex=ibStartIndex, ibEndIndex = ibStartIndex + numIndices;
				ibIndex<ibEndIndex;
				++ibIndex )
			{
				uniqueIndices.insert(ib[ibIndex]);
			}
			//�u���e�[�u�������
			{
				unsigned long newIndex = 0;
				for( UniqueIndices::iterator it=uniqueIndices.begin(), end=uniqueIndices.end();
					it!=end;
					++it )
				{
					//key���Ȃ������ꍇ����� PairI(terator)B(ool)
					ReplacementTable::_Pairib pib = rpcmtTbl.insert( std::make_pair( *it, newIndex ) );
					if( pib.second ){
						++newIndex;
					}
					else{
						//set�g���Ă���̂łȂ��͂�
						assert(	false && "�L�[���d�����Ă���" );
					}
				}
			}
			const unsigned int  numVertex = uniqueIndices.size(); 

			//�O���[�v�̃C���f�b�N�X�̐����V�����̂����
			ibOfGroupInSubset_[subsetIdx][groupIdx] = GameLib::Graphics::IndexBuffer::create(numIndices);

			//���j�[�N�ȃC���f�b�N�X�̐��@�C�R�[���@�V������钸�_��
			vbOfGroupInSubset_[subsetIdx][groupIdx] = GameLib::Graphics::VertexBuffer::create( numVertex, true );
			
			vbBoneOfGroupInSubset_[subsetIdx][groupIdx] = GameLib::Graphics::AnimatedVertexBoneBuffer::create( numVertex );
			

			unsigned short *newIb = ibOfGroupInSubset_[subsetIdx][groupIdx] .lock();
			GameLib::Graphics::Vertex *newVb = vbOfGroupInSubset_[subsetIdx][groupIdx].lock(); 			
			GameLib::Graphics::AnimatedVertexBone *newVbBone = 
				vbBoneOfGroupInSubset_[subsetIdx][groupIdx].lock();

			// create new VertexBuffer
			{
				unsigned long newIndex=0;
				for( 
					UniqueIndices::iterator it=uniqueIndices.begin(), end=uniqueIndices.end();
					it!=end;
					++it, ++newIndex )
				{
					const unsigned long &oldIndex = *it;
					//�g����������
					newVb[newIndex] = vb[oldIndex];
					newVbBone[newIndex] = vbBone[oldIndex];
					
					//face�̃��[�t�̂��߂ɂ����Œu���e�[�u���ɑ}��
					std::map< unsigned long, std::set<unsigned long> >::iterator rfvitIt = 
						replacedFaceVertexIndexTable.find(oldIndex);
					//�����Ă��Ȃ��ꍇ
					if( rfvitIt==replacedFaceVertexIndexTable.end() ){
						std::set<unsigned long> indices;
						indices.insert( newIndex+vbStartIndex );
						replacedFaceVertexIndexTable.insert( std::make_pair( oldIndex, indices) ); 
					}
					else{
						(*rfvitIt).second.insert( newIndex+vbStartIndex );//vbStartIndex�̓I�t�Z�b�g�B��łЂƂɂ���̂�
					}
					
					for( int iBone=0; iBone<MODEL_BLEND_COUNT; ++iBone ){
						const std::vector<TypeBoneIndex> &pattern = usingBoneIndexPatternOfGroupInSubset_[subsetIdx][groupIdx];
						unsigned int patternIdx=0;
						//�u��
						for(
							std::vector<TypeBoneIndex>::const_iterator itPattern=pattern.begin(), endPattern = pattern.end();  
							itPattern!=endPattern;
							++itPattern, ++patternIdx )
						{
							if( newVbBone[newIndex].mIndices[iBone] == (std::numeric_limits< TypeBoneIndex >::max)() ){
								continue;
							}

							if( newVbBone[newIndex].mIndices[iBone] == *itPattern )
							{
								newVbBone[newIndex].mIndices[iBone] = patternIdx;
								break;
							}
						}
						const GameLib::Graphics::AnimatedVertexBone &boneA=newVbBone[newIndex];
						const GameLib::Graphics::AnimatedVertexBone &boneB=vbBone[oldIndex];
						assert(
							patternIdx<pattern.size() ||
							newVbBone[newIndex].mIndices[iBone] == (std::numeric_limits< TypeBoneIndex >::max)() &&
							"�K���u������͂�");
					}
				}
				assert( newIndex==numVertex );
				//create new indexBuffer
				unsigned long offset = ibStartIndex;
				for( unsigned long i=0;
					i<numIndices;
					++i )
				{
					const unsigned short &oldIndex = ib[i+offset];
					//�u��
					const unsigned long &newIndex = rpcmtTbl[oldIndex];
					assert( newIndex <= (std::numeric_limits<unsigned short>::max)() );
					assert( newIndex <= oldIndex );
					newIb[i] = static_cast<unsigned short>(newIndex); 
				}
			}
			vbBoneOfGroupInSubset_[subsetIdx][groupIdx].unlock(&newVbBone);
			vbOfGroupInSubset_[subsetIdx][groupIdx].unlock(&newVb);
			ibOfGroupInSubset_[subsetIdx][groupIdx].unlock(&newIb);
			ibStartIndex += numIndices;
			vbStartIndex += vbOfGroupInSubset_[subsetIdx][groupIdx].vertexNumber();
			//TRACE3( "Subset %d, groupIdx %d, numBoneIndcies %d\n", subsetIdx, groupIdx, usingBoneIndexPatternOfGroupInSubset_[subsetIdx][groupIdx].size() );
		}	
	}

	vertexBuffer_->unlock(&vb);
	vbBone_->unlock(&vbBone);
	tempIndexBuffer.unlock(&tib);
	indexBuffer_->unlock(&ib);

	//�����o�[�֐�(�o�b�t�@�n)����ɓ�������
	//�Â��̊J��
	*vertexBuffer_=0;
	//indexBuffer_�͑傫���ς��Ȃ��̂ł��̂܂܎g���B
	//*indexBuffer_=0;
	*vbBone_ = 0;
	
	//�V�����o�b�t�@���쐬
	numVertices_ = 0;
	for (unsigned int i=0; i<numSubsets_; ++i) {
		for( unsigned int groupidx=0; groupidx<numGroups_[i]; ++groupidx ){
			numVertices_ += vbOfGroupInSubset_[i][groupidx].vertexNumber();				
		}
	}
	*vertexBuffer_ = GameLib::Graphics::VertexBuffer::create( numVertices_, true );
	*vbBone_ = GameLib::Graphics::AnimatedVertexBoneBuffer::create( numVertices_ );

	//indexBuffer_��V�����̂ɑ������
	ib = indexBuffer_->lock();
	unsigned long ibTotalIndex=0;
	unsigned short offset = 0;
	for (unsigned int subsetIdx=0; subsetIdx<numSubsets_; ++subsetIdx) {
		for( unsigned int groupIdx=0; groupIdx<numGroups_[subsetIdx]; ++groupIdx ){
			
			unsigned short * oldIb = ibOfGroupInSubset_[subsetIdx][groupIdx].lock();
			for( int i=0, iMax=ibOfGroupInSubset_[subsetIdx][groupIdx].indexNumber(); i<iMax; ++i, ++ibTotalIndex ){
				ib[ibTotalIndex] = oldIb[i]+offset;
			}
			ibOfGroupInSubset_[subsetIdx][groupIdx].unlock(&oldIb);
			offset += vbOfGroupInSubset_[subsetIdx][groupIdx].vertexNumber();
		}
	}
	indexBuffer_->unlock(&ib);
	
	//vertexBuffer_,vbBone_��V�����̂ɑ������
	vbBone = vbBone_->lock();
	vb = vertexBuffer_->lock();	
	unsigned long vbTotalIndex=0;
	for (unsigned int subsetIdx=0; subsetIdx<numSubsets_; ++subsetIdx) {
		for( unsigned int groupIdx=0; groupIdx<numGroups_[subsetIdx]; ++groupIdx ){
			GameLib::Graphics::Vertex *oldVb = vbOfGroupInSubset_[subsetIdx][groupIdx].lock();
			GameLib::Graphics::AnimatedVertexBone *oldVbBone =
				vbBoneOfGroupInSubset_[subsetIdx][groupIdx].lock();

			for( int i=0, iMax=vbOfGroupInSubset_[subsetIdx][groupIdx].vertexNumber(); i<iMax; ++i, ++vbTotalIndex ){
				vb[vbTotalIndex] = oldVb[i];
				vbBone[vbTotalIndex] = oldVbBone[i]; 
			}
			vbBoneOfGroupInSubset_[subsetIdx][groupIdx].unlock(&oldVbBone);
			vbOfGroupInSubset_[subsetIdx][groupIdx].unlock(&oldVb);
		}
	}
	vertexBuffer_->unlock(&vb);
	vbBone_->unlock(&vbBone);

	//face��������
	if(faceSize==0){
		return;
	}
	CFace *pFaceArray = &faces[0];
	for( int i=0, imax=faceSize; i<imax; ++i )
	{
		CFaceVertex *pFaceVertexArray = pFaceArray[i].vertices_[0];
		for( int j=0, jmax=pFaceArray[i].vertices_.size(); j<jmax; ++j )
		{
			std::map< unsigned long, std::set<unsigned long> >::iterator rfvitIt = 
				replacedFaceVertexIndexTable.find( pFaceVertexArray[j].index_ );
			pFaceVertexArray[j].initVertexBlendIndices( rfvitIt->second );
		}
	}
}



void VertexDataUsingVertexBlendShader::sendMatrixArrayToShader(
	const CBone *bones,
	unsigned int subsetIndex,
	unsigned int groupIndex,
	unsigned int startIndexBufferIdx )
{
	Matrix34 matArray[SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE];
	const CBone *boneArray = bones;
	
	unsigned int i=0;
	//for( 
	//	std::set<TypeBoneIndex>::iterator it=usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex].begin(),
	//	end = usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex].end();
	//	it!=end;
	//	++it, ++i )
	//{
	//	assert(i<SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE);

	//	if( *it!=(std::numeric_limits<TypeBoneIndex >::max)() ){
	//		matArray[i] = toMatrix34( boneArray[ *it ].getSkinningMatrix() );
	//	} else {
	//		assert(false);
	//		//matArray[i]=matZero;
	//	}
	//}
	unsigned int size = usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex].size();
	for( ; i<size; ++i ){
		const std::vector<TypeBoneIndex> &indexes = usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex];
		assert(i<SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE);

		if( indexes[i]!=(std::numeric_limits<TypeBoneIndex >::max)() ){
			matArray[i] = toMatrix34( boneArray[ indexes[i] ].getSkinningMatrix() );
		} else {
			assert(false);
			//matArray[i]=matZero;
		}
	}
	assert( i==size );

	Manager m = Manager::instance();
	
	m.sendWorldMatrixArray(matArray, i );
	
	//HRESULT hr;
	//IDirect3DDevice9 *pDevice=m.dxDevice();
	//hr = pDevice->SetVertexShaderConstantF( 20, &matArray->m00, 3*SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE ); 
	//assert( SUCCEEDED( hr ) );
	//TRACE4( "sended mIndicesTable : %d, %d, %d, %d, ", boneIndicesTable[0], boneIndicesTable[1], boneIndicesTable[2], boneIndicesTable[3] );
	//TRACE4( "%d, %d, %d, %d, ", boneIndicesTable[4], boneIndicesTable[5], boneIndicesTable[6], boneIndicesTable[7] );
	//TRACE4( "%d, %d, %d, %d\n", boneIndicesTable[8], boneIndicesTable[9], boneIndicesTable[10], boneIndicesTable[11] );
}

//MME�͍�����W
void VertexDataUsingVertexBlendShader::sendMatrixArrayToMMEShader(
	const CBone *bones,
	unsigned int subsetIndex,
	unsigned int groupIndex,
	unsigned int startIndexBufferIdx )
{
	D3DXMATRIX matArray[SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE];
	const CBone *boneArray = bones;
	
	unsigned int i=0;
	//for( 
	//	std::set<TypeBoneIndex>::iterator it=usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex].begin(),
	//	end = usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex].end();
	//	it!=end;
	//	++it, ++i )
	//{
	//	assert(i<SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE);

	//	if( *it!=(std::numeric_limits<TypeBoneIndex >::max)() ){
	//		matArray[i] = boneArray[ *it ].getSkinningMatrix();
	//	} else {
	//		assert(false);
	//		//matArray[i]=matZero;
	//	}
	//}
	unsigned int size = usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex].size();
	for( ; i<size; ++i ){
		const std::vector<TypeBoneIndex> &indexes = usingBoneIndexPatternOfGroupInSubset_[subsetIndex][groupIndex];
		assert(i<SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE);

		if( indexes[i]!=(std::numeric_limits<TypeBoneIndex >::max)() ){
			matArray[i] = boneArray[ indexes[i] ].getSkinningMatrix();
		} else {
			assert(false);
			//matArray[i]=matZero;
		}
	}
	assert( i==size );

	MME::Manager *mmem = MME::Manager::instance();//Miku Miku Effect Manager
	
	mmem->sendWorldMatrixArray(matArray, i );
	
	//HRESULT hr;
	//IDirect3DDevice9 *pDevice=m.dxDevice();
	//hr = pDevice->SetVertexShaderConstantF( 20, &matArray->m00, 3*SENDED_WORLD_MATRIX_ARRAY_MAX_SIZE ); 
	//assert( SUCCEEDED( hr ) );
	//TRACE4( "sended mIndicesTable : %d, %d, %d, %d, ", boneIndicesTable[0], boneIndicesTable[1], boneIndicesTable[2], boneIndicesTable[3] );
	//TRACE4( "%d, %d, %d, %d, ", boneIndicesTable[4], boneIndicesTable[5], boneIndicesTable[6], boneIndicesTable[7] );
	//TRACE4( "%d, %d, %d, %d\n", boneIndicesTable[8], boneIndicesTable[9], boneIndicesTable[10], boneIndicesTable[11] );
}

/*
void VertexDataUsingVertexBlendShader::initStaticObject(){
	

	if(!sVertexDeclaration)
	{
		HRESULT hr;
		Manager m = Manager::instance();
		IDirect3DDevice9 *pDevice=m.dxDevice();
	
		//�����X�g���[����p�����`��̂��ߐV���ɍ��
		D3DVERTEXELEMENT9 vElements[] = {
			{ 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 28, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 1,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
			{ 1, 16, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
			D3DDECL_END(),
		};
		assert( sizeof(TypeBoneIndex)==sizeof(unsigned char) );
		assert( MODEL_BLEND_COUNT == 4 );


		hr = pDevice->CreateVertexDeclaration( vElements, &sVertexDeclaration );
		//LPVOID string;

		//FormatMessage(
		//	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		//	NULL,
		//	hr,
		//	0,//MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // ����̌���
		//	(LPTSTR)&string,
		//	0,
		//	NULL);

		//// �G���[���b�Z�[�W��\������
		////MessageBox(NULL, (LPCTSTR)string, TEXT("Error"), MB_OK);
	
		////�o�b�t�@���������	
		//LocalFree(string);

		STRONG_ASSERT( SUCCEEDED( hr ) && "CreateVertexDeclaration : INVALID CALL" );
	
		hr = pDevice->SetVertexDeclaration( sVertexDeclaration );
		STRONG_ASSERT( SUCCEEDED( hr ) && "setVertexDeclaration : INVALID CALL" );
	}

	if( !sEffect ){
		while(true){
			HRESULT hr;
			Manager m = Manager::instance();
			IDirect3DDevice9 *pDevice=m.dxDevice();
	
			LPD3DXBUFFER pCompilationErrors;	
			//hr=D3DXCreateEffect( m.dxDevice(), gSimpleFx, sizeof(gSimpleFx), NULL, NULL, 0, NULL, &sEffect, NULL);
			hr=D3DXCreateEffectFromFile(
				pDevice,
				_T("shader/test.fx"),
				NULL,
				NULL,
				0,
				NULL,
				&sEffect,
				&pCompilationErrors
			);
			if(pCompilationErrors){
				const char* p= (const char*)pCompilationErrors->GetBufferPointer();
				OutputDebugStringA( p );
				//OutputDebugString( p );
				pCompilationErrors->Release();
				if( SUCCEEDED( hr ) ) {
					break;
				}
			}
			else{
				STRONG_ASSERT( SUCCEEDED( hr ) && "setVertexDeclaration : INVALID CALL" );
				break;
			}
		}
	}

	if(!sVertexShader){
		HRESULT hr;
		Manager m = Manager::instance();
		IDirect3DDevice9 *pDevice=m.dxDevice();
		// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
		// shader debugger. Debugging vertex shaders requires either REF or software vertex 
		// processing, and debugging pixel shaders requires REF.  The 
		// D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
		// shader debugger.  It enables source level debugging, prevents instruction 
		// reordering, prevents dead code elimination, and forces the compiler to compile 
		// against the next higher available software target, which ensures that the 
		// unoptimized shaders do not exceed the shader model limitations.  Setting these 
		// flags will cause slower rendering since the shaders will be unoptimized and 
		// forced into software.  See the DirectX documentation for more information about 
		// using the shader debugger.
		DWORD dwShaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

#ifdef DEBUG_VS
			dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
		#endif
#ifdef DEBUG_PS
			dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
#endif

		LPD3DXBUFFER pCode;
		LPD3DXBUFFER pCompilationErrors;

		// Assemble the vertex shader from the file
		hr = D3DXCompileShaderFromFile( 
			_T("shader/VertexLightingVertexBlendVs.fx"), NULL, NULL, "main",
			"vs_3_0", dwShaderFlags, &pCode,
			&pCompilationErrors, &g_pConstantTable );
		if(pCompilationErrors){
			const char* p= (const char*)pCompilationErrors->GetBufferPointer();
			OutputDebugStringA( p );
			//OutputDebugString( p );
			pCompilationErrors->Release();
		}
		assert( SUCCEEDED( hr ) );



		// Create the vertex shader
		hr = pDevice->CreateVertexShader( 
			( DWORD* )pCode->GetBufferPointer(),
			&sVertexShader );
		assert( SUCCEEDED( hr ) );
		pCode->Release();
	
	}

	if(!sPixelShader) {
		HRESULT hr;
		Manager m = Manager::instance();
		IDirect3DDevice9 *pDevice=m.dxDevice();
		// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
		// shader debugger. Debugging vertex shaders requires either REF or software vertex 
		// processing, and debugging pixel shaders requires REF.  The 
		// D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
		// shader debugger.  It enables source level debugging, prevents instruction 
		// reordering, prevents dead code elimination, and forces the compiler to compile 
		// against the next higher available software target, which ensures that the 
		// unoptimized shaders do not exceed the shader model limitations.  Setting these 
		// flags will cause slower rendering since the shaders will be unoptimized and 
		// forced into software.  See the DirectX documentation for more information about 
		// using the shader debugger.
		DWORD dwShaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

#ifdef DEBUG_VS
			dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
		#endif
#ifdef DEBUG_PS
			dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
#endif

		LPD3DXBUFFER pCode;
		LPD3DXBUFFER pCompilationErrors;

		// Assemble the vertex shader from the file
		hr = D3DXCompileShaderFromFile( 
			_T("shader/testPs.fx"), NULL, NULL, "PS_main",
			"ps_3_0", dwShaderFlags, &pCode,
			&pCompilationErrors, NULL );
		if(pCompilationErrors){
			const char* p= (const char*)pCompilationErrors->GetBufferPointer();
			OutputDebugStringA( p );
			//OutputDebugString( p );
			pCompilationErrors->Release();
		}

		assert( SUCCEEDED( hr ) );

		// Create the vertex shader
		hr = pDevice->CreatePixelShader( 
			( DWORD* )pCode->GetBufferPointer(),
			&sPixelShader );
		assert( SUCCEEDED( hr ) );
		pCode->Release();
	
	}

	//HRESULT hr;
	//Manager m = Manager::instance();
	//IDirect3DDevice9 *pDevice=m.dxDevice();
	//pDevice->SetVertexShader( sVertexShader );
	//pDevice->SetPixelShader( sPixelShader );


}

void VertexDataUsingVertexBlendShader::releaseStaticObject(){
	SAFE_RELEASE(sVertexDeclaration);
	SAFE_RELEASE(sEffect);
	SAFE_RELEASE(sVertexShader);
	SAFE_RELEASE(sPixelShader);
}
*/

namespace {

//(1,2,3,4), �Ƃ����̂��������Ƃ�����
//(1,3) ��������true��Ԃ�
//(1,2,3,4,5) �������� false��Ԃ�
//(6)�@�͂Ȃ��̂Ł@false��Ԃ�
bool do_a_have_b( 
	const std::set<TypeBoneIndex> &a,
	const std::set<TypeBoneIndex> &b )
{
	std::set<TypeBoneIndex>::const_iterator end = a.cend();
	BOOST_FOREACH( TypeBoneIndex i, b ){
		//a�̒���i���Ȃ��ꍇ
		if( end == a.find(i) ){
			return false;
		}
	}
	return true;
}

//{ (1,2,3,4), (1,5), (6,7,9) } �Ƃ����̂��������Ƃ�����
//(1,3)��������0��Ԃ�
//(5,)�������� 1��Ԃ�
//((4,6,7)�@�͂Ȃ��̂Ł@-1��Ԃ�
int whichGroup(
	const std::set< std::set<TypeBoneIndex> > &patterns,
	const std::set<TypeBoneIndex> &pattern )
{
	int num = patterns.size();
	int group=0;
	BOOST_FOREACH( const std::set<TypeBoneIndex> &elementPatterns, patterns ){
		if( do_a_have_b( elementPatterns, pattern ) ){
			return group;
		}
		++group;
	}
	return -1;
}

//�܂܂�Ă���d�������v�f����菜��
//{ (1,2,3,4),(1,3) } �Ƃ����̂��������Ƃ����� (1,3) �́@(1,2,3,4)�@�Ɋ܂܂�Ă���̂ŏ���
void removeContainedDuplicateElement( std::set< std::set<TypeBoneIndex> > *elements ) {
 
	typedef std::set< std::set<TypeBoneIndex> > Elements;
	Elements::iterator	it  = elements->begin();
		
	while(it!=elements->end()){
		
		for( Elements::iterator	jt  = elements->begin();
				jt!=elements->end();
			)
		{
			if( it==jt ){
				++jt;
				continue;
			}
			if( do_a_have_b( *it, *jt ) ){
				jt = elements->erase( jt );
			} else {
				++jt;
			}
		}
		++it;
	}
}

}// end no namespace
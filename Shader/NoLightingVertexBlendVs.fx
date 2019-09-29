float4 gTransform[ 4 ] : register( c0 );
float4 gDiffuseColor : register( c10 );
static const int MAX_MATRICES = 32;
bool gIsVertexBlend : register(c19);
float4 gWorldMatrixArray[MAX_MATRICES*3] : register( c20 );

struct ToPixel{
	float4 screenPosition : POSITION;
	float4 color : COLOR0; //���_�J���[
	float2 texCoord : TEXCOORD0; //�e�N�X�`�����W
};
struct VsInput{
	float4			pos             : POSITION;
	float4			blendWeights    : BLENDWEIGHT;
	int4				blendIndices    : BLENDINDICES;
	float3			normal          : NORMAL;
	float2			tex0            : TEXCOORD0;
	float4			color			: COLOR0;
};
ToPixel main( VsInput i ){
	ToPixel o;
	float4 worldPos;
	float4 world[3] = {
		{ 0.0, 0.0, 0.0, 0.0, },
		{ 0.0, 0.0, 0.0, 0.0, },
		{ 0.0, 0.0, 0.0, 0.0, } };
	//int imax = min(NumBones, 4);
	// calculate the pos/normal using the "normal" weights 
	//        and accumulate the weights to calculate the last weight
	for (int iBone = 0; iBone<2; iBone++){
		if( i.blendIndices[iBone]==255 ){
			break;
		}
		world[0] += gWorldMatrixArray[i.blendIndices[iBone]*3+0]  * i.blendWeights[iBone];
		world[1] += gWorldMatrixArray[i.blendIndices[iBone]*3+1]  * i.blendWeights[iBone];
		world[2] += gWorldMatrixArray[i.blendIndices[iBone]*3+2]  * i.blendWeights[iBone];
	}
	worldPos.x = dot( world[ 0 ], i.pos );
	worldPos.y = dot( world[ 1 ], i.pos );
	worldPos.z = dot( world[ 2 ], i.pos );
	worldPos.w = 1.0;
	
	o.screenPosition.x = dot( gTransform[ 0 ], worldPos );
	o.screenPosition.y = dot( gTransform[ 1 ], worldPos );
	o.screenPosition.z = dot( gTransform[ 2 ], worldPos );
	o.screenPosition.w = dot( gTransform[ 3 ], worldPos );
#ifdef LINEAR_ZBUFFER
	o.screenPosition.z *= o.screenPosition.w;
#endif
	o.color.xyzw = i.color.zyxw * gDiffuseColor / 255.0; //BGRA�ŗ���̂ŏ��ԕύX�B������UBYTE4�Ȃ̂�255�Ŋ����Đ��K��
	o.texCoord = i.tex0;
	return o;
}

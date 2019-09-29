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
ToPixel mainVS( VsInput i ){
	ToPixel o;
	o.screenPosition.x = dot( gTransform[ 0 ], i.pos );
	o.screenPosition.y = dot( gTransform[ 1 ], i.pos );
	o.screenPosition.z = dot( gTransform[ 2 ], i.pos );
	o.screenPosition.w = dot( gTransform[ 3 ], i.pos );
#ifdef LINEAR_ZBUFFER
	o.screenPosition.z *= o.screenPosition.w;
#endif
	o.color.xyzw = i.color.zyxw * gDiffuseColor / 255.0; //BGRA�ŗ���̂ŏ��ԕύX�B������UBYTE4�Ȃ̂�255�Ŋ����Đ��K��
	o.texCoord = i.tex0;
	return o;
}

sampler2D gSampler : register( s0 );
float4 mainPS( ToPixel input ) : COLOR {
	float4 c = input.color * tex2D( gSampler, input.texCoord );
	return c;
}

technique main
{
	pass p0
	{
		VertexShader = compile vs_2_0 mainVS();
		PixelShader = compile ps_2_0 mainPS();
	}
}


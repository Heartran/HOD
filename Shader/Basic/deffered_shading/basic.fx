////////////////////////////////////////////////////////////////////////////////////////////////
//
//  basic.fx ver1.2
//  �쐬: ���͉��P
//
////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�錾

// ���@�ϊ��s��
float4x4 WorldViewProjMatrix      : WORLDVIEWPROJECTION;
float4x4 WorldMatrix              : WORLD;
float4x4 LightWorldViewProjMatrix : WORLDVIEWPROJECTION < string Object = "Light"; >;

float3   LightDirection    : DIRECTION < string Object = "Light"; >;
float3   CameraPosition    : POSITION  < string Object = "Camera"; >;

// �}�e���A���F
float4   MaterialDiffuse   : DIFFUSE  < string Object = "Geometry"; >;
float3   MaterialAmbient   : AMBIENT  < string Object = "Geometry"; >;
float3   MaterialEmmisive  : EMISSIVE < string Object = "Geometry"; >;
float3   MaterialSpecular  : SPECULAR < string Object = "Geometry"; >;
float    SpecularPower     : SPECULARPOWER < string Object = "Geometry"; >;
float3   MaterialToon      : TOONCOLOR;
// ���C�g�F
float3   LightDiffuse      : DIFFUSE   < string Object = "Light"; >;
float3   LightAmbient      : AMBIENT   < string Object = "Light"; >;
float3   LightSpecular     : SPECULAR  < string Object = "Light"; >;
static float4 DiffuseColor  = MaterialDiffuse  * float4(LightDiffuse, 1.0f);
static float3 AmbientColor  = saturate(MaterialAmbient  * LightAmbient + MaterialEmmisive);
static float3 SpecularColor = MaterialSpecular * LightSpecular;

float4   EdgeColor         : EDGECOLOR;

static bool     parthf=true;   // �p�[�X�y�N�e�B�u�t���O
static bool     transp=false;   // �������t���O
#define SKII1    1500
#define SKII2    8000
#define Toon     3

// �I�u�W�F�N�g�̃e�N�X�`��
texture ObjectTexture: MATERIALTEXTURE;
sampler ObjTexSampler = sampler_state
{
	texture = <ObjectTexture>;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

// MMD�{����sampler���㏑�����Ȃ����߂̋L�q�ł��B�폜�s�B
// sampler MMDSamp0 : register(s0);
// sampler MMDSamp1 : register(s1);
// sampler MMDSamp2 : register(s2);

// �I�u�W�F�N�g�`��{{{

struct BufferShadow_OUTPUT {
	float4 Pos      : POSITION;     // �ˉe�ϊ����W
	float4 ZCalcTex : TEXCOORD0;    // Z�l
	float2 Tex      : TEXCOORD1;    // �e�N�X�`��
	float3 Normal   : TEXCOORD2;    // �@��
	float3 Eye      : TEXCOORD3;    // �J�����Ƃ̑��Έʒu
	float2 SpTex    : TEXCOORD4;	 // �X�t�B�A�}�b�v�e�N�X�`�����W
	float4 Color    : COLOR0;       // �f�B�t���[�Y�F
};


struct VS_OUTPUT
{
	float4 Vertex  : POSITION;    // �ˉe�ϊ����W
	float4 ScreenPos  : TEXCOORD0;    // �ˉe�ϊ����W
	float2 Tex        : TEXCOORD1;   // �e�N�X�`��
	float3 Normal     : TEXCOORD2;   // �@��
	float4 WorldPos        : TEXCOORD3;
	float4 Color      : COLOR0;      // �f�B�t���[�Y�F
};

// ���_�V�F�[�_
VS_OUTPUT drawGBufferVS( MMM_SKINNING_INPUT IN, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	MMM_SKINNING_OUTPUT SkinOut = MMM_SkinnedPositionNormal( IN.Pos, IN.Normal, IN.BlendWeight, IN.BlendIndices, IN.SdefC, IN.SdefR0, IN.SdefR1 );

	Out.WorldPos = mul( SkinOut.Position, WorldMatrix );
	// �J�������_�̃��[���h�r���[�ˉe�ϊ�
	Out.Vertex = mul( SkinOut.Position, WorldViewProjMatrix );
	Out.ScreenPos = Out.Vertex;
	// ���_�@��
	Out.Normal = normalize( mul( SkinOut.Normal, (float3x3)WorldMatrix ) );

	// �f�B�t���[�Y�F�{�A���r�G���g�F �v�Z
	Out.Color.rgb = saturate( max(0,dot( Out.Normal, -LightDirection )) * DiffuseColor.rgb + AmbientColor );
	Out.Color.a = DiffuseColor.a;

	// �e�N�X�`�����W
	Out.Tex = IN.Tex;

	return Out;
}
struct PS_INPUT{
	float4 ScreenPos  : TEXCOORD0;    // �ˉe�ϊ����W
	float2 Tex        : TEXCOORD1;   // �e�N�X�`��
	float3 Normal     : TEXCOORD2;   // �@��
	float3 WorldPos        : TEXCOORD3;
	float4 Color      : COLOR0;      // �f�B�t���[�Y�F
};

struct PS_OUTPUT
{
	float4 Color				: COLOR0;
	float4 Pos			: COLOR1;
	float4 Normal				: COLOR2;
};
// �s�N�Z���V�F�[�_
PS_OUTPUT drawGBufferPS( PS_INPUT IN , uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;

	Out.Color = IN.Color;
	if ( useTexture ) {  //������if���͔�����I
		// �e�N�X�`���K�p
		Out.Color *= tex2D( ObjTexSampler, IN.Tex );
	}
	Out.Normal = float4( IN.Normal, 1 );
	Out.Pos = float4( IN.WorldPos, IN.ScreenPos.z);
	// Out.Pos = float4( IN.WorldPos, IN.ScreenPos.z/IN.ScreenPos.w);

	return Out;
}

// // �s�N�Z���V�F�[�_
float4 drawPS( VS_OUTPUT IN, uniform bool useTexture ) : COLOR0
{

	float4 Color = IN.Color;
	if ( useTexture ) {  //������if���͔�����I
		// �e�N�X�`���K�p
		Color *= tex2D( ObjTexSampler, IN.Tex );
	}

	return Color;
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N�i�A�N�Z�T���p�j�e������e�Ȃ����I�b�P�i�Ǝ��g���j{{{
// �s�v�Ȃ��͍̂폜��
technique MainTec0 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(false, false, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, false, false);
	}
}

technique MainTec1 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(true, false, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, false, false);
	}
}

technique MainTec2 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(false, true, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, true, false);
	}
}

technique MainTec3 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferVS(true, true, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, true, false);
	}
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N�iPMD���f���p�j
technique MainTec4 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferVS(false, false, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, false, true);
	}
}

technique MainTec5 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferVS(true, false, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, false, true);
	}
}

technique MainTec6 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(false, true, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, true, true);
	}
}

technique MainTec7 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(true, true, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, true, true);
	}
}
//}}}


//}}}

// �֊s�`��{{{
// ���_�V�F�[�_
float4 ColorRender_VS(MMM_SKINNING_INPUT IN) : POSITION 
{
	MMM_SKINNING_OUTPUT SkinOut = MMM_SkinnedPositionNormal( IN.Pos, IN.Normal, IN.BlendWeight, IN.BlendIndices, IN.SdefC, IN.SdefR0, IN.SdefR1 );
	float4 Pos = mul( SkinOut.Position, WorldViewProjMatrix );


	// �@�������ɂ�����Ƃ����c��܂�
	float3 Normal = mul( SkinOut.Normal, (float3x3)WorldMatrix );
	float2 dir = normalize(Normal.xy);
	Pos.xy += dir/Pos.w;
	return Pos;
}

// �s�N�Z���V�F�[�_
float4 ColorRender_PS() : COLOR
{
	// �֊s�F�œh��Ԃ�
	return EdgeColor;
}

// �֊s�`��p�e�N�j�b�N
technique EdgeTec < string MMDPass = "edge"; > {
	pass DrawEdge {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		// ZFunc=Equal;
#endif
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		CULLMODE = CW;
		VertexShader = compile vs_2_0 ColorRender_VS();
		PixelShader  = compile ps_2_0 ColorRender_PS();
	}
}
//}}}

// �Z���t�V���h�E�pZ�l�v���b�g{{{

struct VS_ZValuePlot_OUTPUT {
	float4 Pos : POSITION;              // �ˉe�ϊ����W
	float4 ShadowMapTex : TEXCOORD0;    // Z�o�b�t�@�e�N�X�`��
};

// ���_�V�F�[�_
VS_ZValuePlot_OUTPUT ZValuePlot_VS( MMM_SKINNING_INPUT IN )
{
	VS_ZValuePlot_OUTPUT Out = (VS_ZValuePlot_OUTPUT)0;

	float4 Pos =  MMM_SkinnedPosition(IN.Pos, IN.BlendWeight, IN.BlendIndices, IN.SdefC, IN.SdefR0, IN.SdefR1);
	// ���C�g�̖ڐ��ɂ�郏�[���h�r���[�ˉe�ϊ�������
	Out.Pos = mul( Pos, LightWorldViewProjMatrix );

	// �e�N�X�`�����W�𒸓_�ɍ��킹��
	Out.ShadowMapTex = Out.Pos;

	return Out;
}

// �s�N�Z���V�F�[�_
//���U�V���h�E�}�b�v�p
float4 VSM_ZValuePlot_PS( float4 ShadowMapTex : TEXCOORD0 ) : COLOR
{
	float depth = ShadowMapTex.z/ShadowMapTex.w;
	return float4( depth, depth*depth, 0.f, 0.f );
}

float4 ZValuePlot_PS( float4 ShadowMapTex : TEXCOORD0 ) : COLOR
{
	float depth = ShadowMapTex.z/ShadowMapTex.w;
	
	return float4( depth, 0.f, 0.f, 0.f );
}


// Z�l�v���b�g�p�e�N�j�b�N
technique ZplotTec < 
string MMDPass = "zplot";
string Script = 
"Pass=ZValuePlot;";
> {
#ifdef ENABLE_VSM
	//���U�V���h�E�}�b�v�̏ꍇ
	pass ZValuePlot {
		AlphaTestEnable  = FALSE;
		AlphaBlendEnable = FALSE;
		VertexShader = compile vs_2_0 ZValuePlot_VS();
		PixelShader  = compile ps_2_0 VSM_ZValuePlot_PS();
	}
#else
	pass ZValuePlot {
		AlphaTestEnable  = FALSE;
		AlphaBlendEnable = FALSE;
		VertexShader = compile vs_2_0 ZValuePlot_VS();
		PixelShader  = compile ps_2_0 ZValuePlot_PS();
	}
#endif


}

//}}}
// vim : ft=fx

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  basic.fx ver1.2
//  �쐬: ���͉��P
//
////////////////////////////////////////////////////////////////////////////////////////////////
// #define ENABLE_VSM
// #define DISABLE_SHADOW

// �p�����[�^�錾

// ���@�ϊ��s��
float4x4 WorldViewProjMatrix      : WORLDVIEWPROJECTION;
float4x4 WorldMatrix              : WORLD;
float4x4 ViewMatrix               : VIEW;
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
float znear : ZNEAR;
float zfar : ZFAR;

static bool     parthf=true;   // �p�[�X�y�N�e�B�u�t���O
static bool     transp=false;   // �������t���O
bool	 spadd;    // �X�t�B�A�}�b�v���Z�����t���O
#define SKII1    1500
#define SKII2    8000
#define Toon     3

#define SPECULAR_BASE 100
static bool IsEmittion = (SPECULAR_BASE <= SpecularPower) && (dot(MaterialSpecular, MaterialSpecular) < 0.01);
// static float EmittionPower0 = IsEmittion ? ((SpecularPower - SPECULAR_BASE) / 7.0) : 1;
// static float EmittionPower1 = EmittionPower0 * (LightUp * 2 + 1.0) * pow(400, LightUpE) * (1.0 - LightOff);






// �I�u�W�F�N�g�̃e�N�X�`��
texture ObjectTexture: MATERIALTEXTURE;
sampler ObjTexSampler = sampler_state
{
	texture = <ObjectTexture>;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

// �X�t�B�A�}�b�v�̃e�N�X�`��
texture ObjectSphereMap: MATERIALSPHEREMAP;
sampler ObjSphareSampler = sampler_state {
	texture = <ObjectSphereMap>;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};



// MMD�{����sampler���㏑�����Ȃ����߂̋L�q�ł��B�폜�s�B
// sampler MMDSamp0 : register(s0);
// sampler MMDSamp1 : register(s1);
// sampler MMDSamp2 : register(s2);

// �I�u�W�F�N�g�`��{{{

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
	float4 Color		: COLOR0;
	float4 Pos			: COLOR1;
	float4 Normal		: COLOR2;
	float4 Emittion	: COLOR3;
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

// �I�u�W�F�N�g�`��p�e�N�j�b�N{{{
// �s�v�Ȃ��͍̂폜��
technique MainTec0 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = false; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(false, false, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, false, false);
	}
}

technique MainTec1 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = false; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(true, false, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, false, false);
	}
}

technique MainTec2 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = false; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(false, true, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, true, false);
	}
}

technique MainTec3 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = false; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferVS(true, true, false);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, true, false);
	}
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N�iPMD���f���p�j
technique MainTec4 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = true; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferVS(false, false, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, false, true);
	}
}

technique MainTec5 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = true; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferVS(true, false, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, false, true);
	}
}

technique MainTec6 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = true; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(false, true, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(false, true, true);
	}
}

technique MainTec7 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = true; bool UseSelfShadow = false; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferVS(true, true, true);
		PixelShader  = compile ps_3_0 drawGBufferPS(true, true, true);
	}
}
//}}}

//}}}

//�I�u�W�F�N�g�`��(�e��) {{{ 
	
struct BufferShadow_OUTPUT {
	float4 Pos      : POSITION;     // �ˉe�ϊ����W
	float4 ZCalcTex : TEXCOORD0;    // Z�l
	float2 Tex      : TEXCOORD1;    // �e�N�X�`��
	float3 Normal   : TEXCOORD2;    // �@��
	float3 Eye      : TEXCOORD3;    // �J�����Ƃ̑��Έʒu
	float2 SpTex    : TEXCOORD4;	 // �X�t�B�A�}�b�v�e�N�X�`�����W
	float4 WorldPos : TEXCOORD5;
	float4 Color    : COLOR0;       // �f�B�t���[�Y�F
	float4 Emittion : COLOR1;
};

// �V���h�E�o�b�t�@�̃T���v���B"register(s0)"�Ȃ̂�MMD��s0���g���Ă��邩��
sampler DefSampler : register(s0);


// ���_�V�F�[�_
BufferShadow_OUTPUT drawGBufferShadowVS( MMM_SKINNING_INPUT IN, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon)
{
	BufferShadow_OUTPUT Out = (BufferShadow_OUTPUT)0;


	MMM_SKINNING_OUTPUT SkinOut = MMM_SkinnedPositionNormal( IN.Pos, IN.Normal, IN.BlendWeight, IN.BlendIndices, IN.SdefC, IN.SdefR0, IN.SdefR1 );
	// �J�������_�̃��[���h�r���[�ˉe�ϊ�
	Out.Pos = mul( SkinOut.Position, WorldViewProjMatrix );
	Out.WorldPos = mul( SkinOut.Position, WorldMatrix );

	// �J�����Ƃ̑��Έʒu
	Out.Eye = CameraPosition - Out.WorldPos;
	// ���_�@��
	Out.Normal = normalize( mul( SkinOut.Normal, (float3x3)WorldMatrix ) );
	// ���C�g���_�ɂ�郏�[���h�r���[�ˉe�ϊ�
	Out.ZCalcTex = mul( SkinOut.Position, LightWorldViewProjMatrix );

	// �f�B�t���[�Y�F�{�A���r�G���g�F �v�Z
	Out.Color.rgb = AmbientColor;
	if ( !useToon ) {
		Out.Color.rgb += max(0,dot( Out.Normal, -LightDirection )) * DiffuseColor.rgb;
	}
	Out.Color.a = DiffuseColor.a;
	Out.Color = saturate( Out.Color );

	// �e�N�X�`�����W
	Out.Tex = IN.Tex;

	if ( useSphereMap ) {
		// �X�t�B�A�}�b�v�e�N�X�`�����W
		float2 NormalWV = mul( Out.Normal, (float3x3)ViewMatrix );
		Out.SpTex.x = NormalWV.x * 0.5f + 0.5f;
		Out.SpTex.y = NormalWV.y * -0.5f + 0.5f;
	}
	
	//�����F
	if( IsEmittion ) {
		Out.Emittion = MaterialDiffuse;
		Out.Emittion.rgb += MaterialEmmisive * 0.5;
		Out.Emittion.rgb *= 0.5;
	}

	return Out;
}

// �s�N�Z���V�F�[�_
PS_OUTPUT drawGBufferShadowPS(BufferShadow_OUTPUT IN, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;
	Out.Normal=float4( IN.Normal, 1.0 );
	Out.Pos = IN.WorldPos;
	Out.Emittion = IN.Emittion;
	// �X�y�L�����F�v�Z
	float3 HalfVector = normalize( normalize(IN.Eye) + -LightDirection );
	float3 Specular = pow( max(0,dot( HalfVector, normalize(IN.Normal) )), SpecularPower ) * SpecularColor;

	float4 Color = IN.Color;
	float4 ShadowColor = float4(AmbientColor, Color.a);  // �e�̐F
	if ( useTexture ) {
		// �e�N�X�`���K�p
		float4 TexColor = tex2D( ObjTexSampler, IN.Tex );
		Color *= TexColor;
		ShadowColor *= TexColor;
	}
	if ( useSphereMap ) {
		// �X�t�B�A�}�b�v�K�p
		float4 TexColor = tex2D(ObjSphareSampler,IN.SpTex);
		if(spadd) {
			Color.rgb += TexColor.rgb;
			ShadowColor.rgb += TexColor.rgb;
		} else {
			Color.rgb *= TexColor.rgb;
			ShadowColor.rgb *= TexColor.rgb;
		}
	}
	// �X�y�L�����K�p
	Color.rgb += Specular;
#if defined( DISABLE_SHADOW )
	Out.Color = Color;
	return Out;
#endif

	// �e�N�X�`�����W�ɕϊ�
	IN.ZCalcTex /= IN.ZCalcTex.w;
	float2 TransTexCoord;
	TransTexCoord.x = (1.0f + IN.ZCalcTex.x)*0.5f;
	TransTexCoord.y = (1.0f - IN.ZCalcTex.y)*0.5f;


#if defined( ENABLE_VSM )
	// Variance Shadow Map ���U�V���h�E�}�b�v
	if( any( saturate(TransTexCoord) != TransTexCoord ) ) {
		// �V���h�E�o�b�t�@�O
		Out.Color=Color;
		return Out;
	} else {
		float2 shadow =  tex2D( DefSampler, TransTexCoord );
		shadow += 0.0001;

		float e_x2 = shadow.y;
		float ex_2 = shadow.x * shadow.x;
		float variance = e_x2 - ex_2;
		float md = shadow.x - IN.ZCalcTex.z;
		float p = variance / ( variance + (md * md) );
		p = saturate( max( p, IN.ZCalcTex.z < shadow.x ) );
		// �g�D�[���K�p
		if ( useToon ) {
			p = min(saturate(dot(IN.Normal,-LightDirection)*Toon), p);
			ShadowColor.rgb *= MaterialToon;
		}

		Out.Color = lerp(ShadowColor, Color, p);
		return Out;
	}
// #else
// 	if( any( saturate(TransTexCoord) != TransTexCoord ) ) {
// 		// �V���h�E�o�b�t�@�O
// 		Out.Color = Color;
// 		return Out;
// 	} else {
// 		float comp;
// 		if(parthf) {
// 			// �Z���t�V���h�E mode2
// 			comp=1-saturate(max(IN.ZCalcTex.z-tex2D(DefSampler,TransTexCoord).r , 0.0f)*SKII2*TransTexCoord.y-0.3f);
// 		} else {
// 			// �Z���t�V���h�E mode1
// 			comp=1-saturate(max(IN.ZCalcTex.z-tex2D(DefSampler,TransTexCoord).r , 0.0f)*SKII1-0.3f);
// 		}
// 		if ( useToon ) {
// 			// �g�D�[���K�p
// 			comp = min(saturate(dot(IN.Normal,-LightDirection)*Toon),comp);
// 			ShadowColor.rgb *= MaterialToon;
// 		}
//
// 		float4 ans = lerp(ShadowColor, Color, comp);
// 		// if( transp ) ans.a = 0.5f;
// 		Out.Color = ans;
// 		return Out;
// 	}
#else
	if( any( saturate(TransTexCoord) != TransTexCoord ) ) {
		// �V���h�E�o�b�t�@�O
		Out.Color=Color;
		return Out;
	} else {
		float2 shadow =  tex2D( DefSampler, TransTexCoord );
		// shadow.x += 0.0001;
		shadow.x += 0.005;
		
		//slop scaled bias���ǂ�(����)
		// float sbias = 0.005*tan(acos( dot(IN.Normal, LightDirection) ) );
		// sbias = clamp(sbias, -0.005 , 0.005 );
		// shadow.x -= sbias;

		if( IN.ZCalcTex.z > shadow.x ){
			Out.Color = ShadowColor;
		} else {
			Out.Color = Color;
		}

		return Out;
	}

#endif
}

//�e�N�j�b�N{{{
technique MainTec8 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = false; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferShadowVS(false, false, false);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(false, false, false);
	}
}

technique MainTec9 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = false; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferShadowVS(true, false, false);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(true, false, false);
	}
}

technique MainTec10 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = false; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferShadowVS(false, true, false);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(false, true, false);
	}
}

technique MainTec11 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = false; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferShadowVS(true, true, false);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(true, true, false);
	}
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N�iPMD���f���p�j
technique MainTec12 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = true; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferShadowVS(false, false, true);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(false, false, true);
	}
}

technique MainTec13 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = true; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;

		VertexShader = compile vs_3_0 drawGBufferShadowVS(true, false, true);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(true, false, true);
	}
}

technique MainTec14 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = true; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferShadowVS(false, true, true);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(false, true, true);
	}
}

technique MainTec15 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = true; bool UseSelfShadow = true; > {
	pass DrawGBuffer{
		AlphaBlendEnable = FALSE;
		AlphaTestEnable  = FALSE;
		VertexShader = compile vs_3_0 drawGBufferShadowVS(true, true, true);
		PixelShader  = compile ps_3_0 drawGBufferShadowPS(true, true, true);
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
	
	float z=Pos.w;
	float zn=znear;
	float zf=zfar;
	
	
	//����1�h�b�g�͕ۏႷ��
	{
		float w=640;
		float h=480;
	
		float3 n = mul( SkinOut.Normal, (float3x3)WorldViewProjMatrix );
		n=sign(n);//���̒�����1�h�b�g�ɂ��邽��,x�̕������m�肽��

		//�G�b�W��1�h�b�g�Ȃ̂ł��̕����炷
		// Pos.x += n.x*( 2.f*z/w -2.f*zn/w );
		Pos.y += n.y*( 2.f/h*z -2.f*zn/h );//�����������₷�Ƃ�������
	}
	
	
	//24bit
	//float da=5.96e-08;
	//float�ŏ��l �[�x�o�b�t�@�̐��x��ς����ꍇ�����̒l��ς��鎖
	static const float da=1.175494351e-38F;
	// static const float da=0.0001;

	// <a href="http://marupeke296.com/DXG_No71_depthbuffer.html">����71 �[�x�o�b�t�@�̐��x���āH</a>
	//�ŒZ���������߂�
	float a =1.f/(1.f/z - da*(zf-zn)/(zf*zn)) - z;
	//��̖ʂŗ��\�̂���|���S�����`�悳��Ă��܂��̂�h������
	Pos.z += zf*a/(zf-zn);
	Pos.w += a;
	
	
	// (Pos.w-zn)/(zf-zn); 0-1�̊�
	
	// Pos.w -= (Pos.w-zn)/(zf-zn);
	


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

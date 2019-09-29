////////////////////////////////////////////////////////////////////////////////////////////////
//
//  full.fx ver1.4
//  �쐬: ���͉��P
//
////////////////////////////////////////////////////////////////////////////////////////////////
#define ENABLE_VSM
// #define ENABLE_SIMPLE_SM
// #define ENABLE_PCF

#define ZPREPASS_ENABLE

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
float4   EdgeColor         : EDGECOLOR;
// ���C�g�F
float3   LightDiffuse      : DIFFUSE   < string Object = "Light"; >;
float3   LightAmbient      : AMBIENT   < string Object = "Light"; >;
float3   LightSpecular     : SPECULAR  < string Object = "Light"; >;
static float4 DiffuseColor  = MaterialDiffuse  * float4(LightDiffuse, 1.0f);
static float3 AmbientColor  = saturate(MaterialAmbient  * LightAmbient + MaterialEmmisive);
static float3 SpecularColor = MaterialSpecular * LightSpecular;

static bool parthf=false;   // �p�[�X�y�N�e�B�u�t���O
static bool transp=false;   // �������t���O
bool spadd=false;;    // �X�t�B�A�}�b�v���Z�����t���O
#define SKII1    1500
#define SKII2    8000
#define Toon     3

#define SHADOW_EPSILON 0.005

// �X�N���[���T�C�Y
float2 ViewportSize : VIEWPORTPIXELSIZE;

static float2 ViewportOffset = (float2(0.5,0.5)/ViewportSize);
static float2 ShadowMapSize = ViewportSize*3.f;


// �I�u�W�F�N�g�̃e�N�X�`��
texture ObjectTexture: MATERIALTEXTURE;
sampler ObjTexSampler = sampler_state {
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

////////////////////////////////////////////////////////////////////////////////////////////////
// �֊s�`��

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


///////////////////////////////////////////////////////////////////////////////////////////////
// �e�i��Z���t�V���h�E�j�`��

// ���_�V�F�[�_
float4 Shadow_VS(float4 Pos : POSITION) : POSITION
{
    // �J�������_�̃��[���h�r���[�ˉe�ϊ�
    return mul( Pos, WorldViewProjMatrix );
}

// �s�N�Z���V�F�[�_
float4 Shadow_PS() : COLOR
{
    // �A���r�G���g�F�œh��Ԃ�
    return float4(AmbientColor.rgb, 0.65f);
}

// �e�`��p�e�N�j�b�N
technique ShadowTec < string MMDPass = "shadow"; > {
    pass DrawShadow {
        VertexShader = compile vs_2_0 Shadow_VS();
        PixelShader  = compile ps_2_0 Shadow_PS();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
// �I�u�W�F�N�g�`��i�Z���t�V���h�EOFF�j

struct VS_OUTPUT {
    float4 Pos        : POSITION;    // �ˉe�ϊ����W
    float2 Tex        : TEXCOORD1;   // �e�N�X�`��
    float3 Normal     : TEXCOORD2;   // �@��
    float3 Eye        : TEXCOORD3;   // �J�����Ƃ̑��Έʒu
    float2 SpTex      : TEXCOORD4;	 // �X�t�B�A�}�b�v�e�N�X�`�����W
    float4 Color      : COLOR0;      // �f�B�t���[�Y�F
};

// ���_�V�F�[�_
VS_OUTPUT Basic_VS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Tex : TEXCOORD0, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    // �J�������_�̃��[���h�r���[�ˉe�ϊ�
    Out.Pos = mul( Pos, WorldViewProjMatrix );
    
    // �J�����Ƃ̑��Έʒu
    Out.Eye = CameraPosition - mul( Pos, WorldMatrix );
    // ���_�@��
    Out.Normal = normalize( mul( Normal, (float3x3)WorldMatrix ) );
    
    // �f�B�t���[�Y�F�{�A���r�G���g�F �v�Z
    Out.Color.rgb = AmbientColor;
    if ( !useToon ) {
        Out.Color.rgb += max(0,dot( Out.Normal, -LightDirection )) * DiffuseColor.rgb;
    }
    Out.Color.a = DiffuseColor.a;
    Out.Color = saturate( Out.Color );
    
    // �e�N�X�`�����W
    Out.Tex = Tex;
    
    if ( useSphereMap ) {
        // �X�t�B�A�}�b�v�e�N�X�`�����W
        float2 NormalWV = mul( Out.Normal, (float3x3)ViewMatrix );
        Out.SpTex.x = NormalWV.x * 0.5f + 0.5f;
        Out.SpTex.y = NormalWV.y * -0.5f + 0.5f;
    }
    
    return Out;
}

// �s�N�Z���V�F�[�_
float4 Basic_PS(VS_OUTPUT IN, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon) : COLOR0
{
    // �X�y�L�����F�v�Z
    float3 HalfVector = normalize( normalize(IN.Eye) + -LightDirection );
    float3 Specular = pow( max(0,dot( HalfVector, normalize(IN.Normal) )), SpecularPower ) * SpecularColor;
    
    float4 Color = IN.Color;
    if ( useTexture ) {
        // �e�N�X�`���K�p
        Color *= tex2D( ObjTexSampler, IN.Tex );
    }
    if ( useSphereMap ) {
        // �X�t�B�A�}�b�v�K�p
        if(spadd) Color.rgb += tex2D(ObjSphareSampler,IN.SpTex).rgb;
        else      Color.rgb *= tex2D(ObjSphareSampler,IN.SpTex).rgb;
    }
    
    if ( useToon ) {
        // �g�D�[���K�p
        float LightNormal = dot( IN.Normal, -LightDirection );
        Color.rgb *= lerp(MaterialToon, float3(1,1,1), saturate(LightNormal * 16 + 0.5));
    }
    
    // �X�y�L�����K�p
    Color.rgb += Specular;
    
    return Color;
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N�i�A�N�Z�T���p�j
// �s�v�Ȃ��͍̂폜��
technique MainTec0 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = false; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(false, false, false);
        PixelShader  = compile ps_2_0 Basic_PS(false, false, false);
    }
}

technique MainTec1 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = false; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(true, false, false);
        PixelShader  = compile ps_2_0 Basic_PS(true, false, false);
    }
}

technique MainTec2 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = false; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(false, true, false);
        PixelShader  = compile ps_2_0 Basic_PS(false, true, false);
    }
}

technique MainTec3 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = false; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(true, true, false);
        PixelShader  = compile ps_2_0 Basic_PS(true, true, false);
    }
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N�iPMD���f���p�j
technique MainTec4 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = true; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(false, false, true);
        PixelShader  = compile ps_2_0 Basic_PS(false, false, true);
    }
}

technique MainTec5 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = true; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(true, false, true);
        PixelShader  = compile ps_2_0 Basic_PS(true, false, true);
    }
}

technique MainTec6 < string MMDPass = "object"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = true; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(false, true, true);
        PixelShader  = compile ps_2_0 Basic_PS(false, true, true);
    }
}

technique MainTec7 < string MMDPass = "object"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = true; > {
    pass DrawObject {
        VertexShader = compile vs_2_0 Basic_VS(true, true, true);
        PixelShader  = compile ps_2_0 Basic_PS(true, true, true);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
// �Z���t�V���h�E�pZ�l�v���b�g

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

	// float4 unpacked_depth = float4( 0.f, 65536.f, 0.f, 65536.f );
	// unpacked_depth.g *= modf( depth*65536.f, unpacked_depth.r);
	// unpacked_depth.a *= modf( depth*depth*65536.0f, unpacked_depth.b);

	// return unpacked_depth / 65536.f;  // �W����
	
}

//�o�b�t�@�̐��x���グ��̂Ɏg�p
float4 ZValuePlot_PS( float4 ShadowMapTex : TEXCOORD0 ) : COLOR
{
	float depth = ShadowMapTex.z/ShadowMapTex.w;
	
	return float4( depth, 0.f, 0.f, 0.f );
	// float4 unpacked_depth = float4(0, 0, 256.0f, 256.0f);
	// unpacked_depth.g = modf( depth*256.0f, unpacked_depth.r);
	// unpacked_depth.b *= modf( unpacked_depth.g*256.0f, unpacked_depth.g );
	// // A�������v�Z���Ă��܂��񂪁A
	// //����͖`���ɏq�ׂ��悤��float�^�̉�������23bit�ł��邽�߁A
	// //24bit�ȍ~�̐��x�����߂�Ӗ����Ȃ����߂ł��B
	// return unpacked_depth / 256.0f;  // �W����
}

float Convert_Color_To_Z( float4 color )
{
   return color.r + (color.g + color.b/256.0f) / 256.0f;
}

float2 Convert_Color_To_Z2( float4 color )
{
   return float2(color.r + color.g/65536.0f,  color.b+color.a/65536.0f );
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
        AlphaBlendEnable = FALSE;
        VertexShader = compile vs_2_0 ZValuePlot_VS();
        PixelShader  = compile ps_2_0 VSM_ZValuePlot_PS();
    }
#else
    pass ZValuePlot {
        AlphaBlendEnable = FALSE;
        VertexShader = compile vs_2_0 ZValuePlot_VS();
        PixelShader  = compile ps_2_0 ZValuePlot_PS();
    }
#endif

		
}


///////////////////////////////////////////////////////////////////////////////////////////////
// �I�u�W�F�N�g�`��i�Z���t�V���h�EON�j

// �V���h�E�o�b�t�@�̃T���v���B"register(s0)"�Ȃ̂�MMD��s0���g���Ă��邩��
sampler DefSampler : register(s0);

struct BufferShadow_OUTPUT {
    float4 Pos      : POSITION;     // �ˉe�ϊ����W
    float4 ZCalcTex : TEXCOORD0;    // Z�l
    float2 Tex      : TEXCOORD1;    // �e�N�X�`��
    float3 Normal   : TEXCOORD2;    // �@��
    float3 Eye      : TEXCOORD3;    // �J�����Ƃ̑��Έʒu
    float2 SpTex    : TEXCOORD4;	 // �X�t�B�A�}�b�v�e�N�X�`�����W
    float4 Color    : COLOR0;       // �f�B�t���[�Y�F
};

// ���_�V�F�[�_
BufferShadow_OUTPUT BufferShadow_VS( MMM_SKINNING_INPUT IN, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon)
{
    BufferShadow_OUTPUT Out = (BufferShadow_OUTPUT)0;

		MMM_SKINNING_OUTPUT SkinOut = MMM_SkinnedPositionNormal( IN.Pos, IN.Normal, IN.BlendWeight, IN.BlendIndices, IN.SdefC, IN.SdefR0, IN.SdefR1 );
    // �J�������_�̃��[���h�r���[�ˉe�ϊ�
    Out.Pos = mul( SkinOut.Position, WorldViewProjMatrix );
    
    // �J�����Ƃ̑��Έʒu
    Out.Eye = CameraPosition - mul( SkinOut.Position, WorldMatrix );
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
    
    return Out;
}
struct PS_INPUT {
    float4 Pos      : VPOS;     // �ˉe�ϊ����W
    float4 ZCalcTex : TEXCOORD0;    // Z�l
    float2 Tex      : TEXCOORD1;    // �e�N�X�`��
    float3 Normal   : TEXCOORD2;    // �@��
    float3 Eye      : TEXCOORD3;    // �J�����Ƃ̑��Έʒu
    float2 SpTex    : TEXCOORD4;	 // �X�t�B�A�}�b�v�e�N�X�`�����W
    float4 Color    : COLOR0;       // �f�B�t���[�Y�F
};


struct PS_OUTPUT
{
	float4 Normal				: COLOR0;
	float4 Color				: COLOR1;
	// float4 Pos			: COLOR2;
};
// �s�N�Z���V�F�[�_
float4 BufferShadow_PS(PS_INPUT IN, uniform bool useTexture, uniform bool useSphereMap, uniform bool useToon) : COLOR0
{
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
		
    // �e�N�X�`�����W�ɕϊ�
    IN.ZCalcTex /= IN.ZCalcTex.w;
    float2 TransTexCoord;
		
    TransTexCoord.x = (1.0f + IN.ZCalcTex.x)*0.5f;
    TransTexCoord.y = (1.0f - IN.ZCalcTex.y)*0.5f;
		
#if defined( ENABLE_VSM )
		// Variance Shadow Map ���U�V���h�E�}�b�v
		if( any( saturate(TransTexCoord) != TransTexCoord ) ) {
		    // �V���h�E�o�b�t�@�O
		    return Color;
		} else {
			// float2 shadow =  Convert_Color_To_Z2( tex2D( DefSampler, TransTexCoord ) );
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
			
			float4 ans = lerp(ShadowColor, Color, p);
			// if( transp ) ans.a = 0.5f;
			return ans;
		}
#elif defined( ENABLE_PCF )
		if( any( saturate(TransTexCoord) != TransTexCoord ) ) {
			// �V���h�E�o�b�t�@�O
			return Color;
		} else {
			// Pixel is in lit area. Find out if it's
			// in shadow using 2x2 percentage closest filtering

			// transform to texel space
			float2 texelpos = ShadowMapSize * TransTexCoord;

			// Determine the lerp amounts           
			float2 lerps = frac( texelpos );

			//read in bilerp stamp, doing the shadow checks
			float sourcevals[4];
			sourcevals[0] = (tex2D( DefSampler, TransTexCoord ) +  SHADOW_EPSILON < IN.ZCalcTex.z)? 0.0f: 1.0f;  
			sourcevals[1] = (tex2D( DefSampler, TransTexCoord + float2(1.0/ShadowMapSize.x , 0) ) +  SHADOW_EPSILON < IN.ZCalcTex.z)? 0.0f: 1.0f;  
			sourcevals[2] = (tex2D( DefSampler, TransTexCoord + float2(0, 1.0/ShadowMapSize.y) ) + SHADOW_EPSILON < IN.ZCalcTex.z)? 0.0f: 1.0f;  
			sourcevals[3] = (tex2D( DefSampler, TransTexCoord + float2(1.0/ShadowMapSize.x, 1.0/ShadowMapSize.y) ) + SHADOW_EPSILON < IN.ZCalcTex.z)? 0.0f: 1.0f;  

			// lerp between the shadow values to calculate our light amount
			float LightAmount = lerp( lerp( sourcevals[0], sourcevals[1], lerps.x ),
					lerp( sourcevals[2], sourcevals[3], lerps.x ),
					lerps.y );
			// Light it
			return lerp(ShadowColor, Color, LightAmount);
		}

#elif defined( ENABLE_SIMPLE_SM )
		// �V���v���ȃV���h�E�}�b�v
		if( any( saturate(TransTexCoord) != TransTexCoord ) ) {
			// �V���h�E�o�b�t�@�O
			return Color;
		} else {
			// float SM_Z = Convert_Color_To_Z( tex2D( DefSampler, TransTexCoord ) );
			float SM_Z = tex2D( DefSampler, TransTexCoord ).x;

			// �Z�o�_���V���h�E�}�b�v��Z�l�����傫����Ήe�Ɣ��f
			if( IN.ZCalcTex.z > SM_Z + 0.005){
				return ShadowColor;
			}
			return Color;
		}
#else
		// MMD�̃V���h�E�o�b�t�@
		if( any( saturate(TransTexCoord) != TransTexCoord ) ) {
			// �V���h�E�o�b�t�@�O
			return Color;
		} else {
			float comp;
			if(parthf) {
				// �Z���t�V���h�E mode2
				// comp=1-saturate(max(IN.ZCalcTex.z-Convert_Color_To_Z( tex2D(DefSampler,TransTexCoord) ) , 0.0f)*SKII2*TransTexCoord.y-0.3f);
				comp=1-saturate(max(IN.ZCalcTex.z-tex2D(DefSampler,TransTexCoord).r, 0.0f)*SKII2*TransTexCoord.y-0.3f);
			} else {
				// �Z���t�V���h�E mode1
				// comp=1-saturate(max(IN.ZCalcTex.z-Convert_Color_To_Z( tex2D(DefSampler,TransTexCoord) ) , 0.0f)*SKII1-0.3f);
				comp=1-saturate(max(IN.ZCalcTex.z-tex2D(DefSampler,TransTexCoord).r, 0.0f)*SKII1-0.3f);
			}
			if ( useToon ) {
				// �g�D�[���K�p
				comp = min(saturate(dot(IN.Normal,-LightDirection)*Toon),comp);
				ShadowColor.rgb *= MaterialToon;
			}

			float4 ans = lerp(ShadowColor, Color, comp);
			if( transp ) ans.a = 0.5f;
			return ans;
		}
#endif
}


// �I�u�W�F�N�g�`��p�e�N�j�b�N�i�A�N�Z�T���p�j
technique MainTecBS0  < string MMDPass = "object_ss"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = false; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(false, false, false);
		PixelShader  = compile ps_3_0 BufferShadow_PS(false, false, false);
	}
}

technique MainTecBS1  < string MMDPass = "object_ss"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = false; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(true, false, false);
		PixelShader  = compile ps_3_0 BufferShadow_PS(true, false, false);
	}
}

technique MainTecBS2  < string MMDPass = "object_ss"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = false; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(false, true, false);
		PixelShader  = compile ps_3_0 BufferShadow_PS(false, true, false);
	}
}

technique MainTecBS3  < string MMDPass = "object_ss"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = false; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(true, true, false);
		PixelShader  = compile ps_3_0 BufferShadow_PS(true, true, false);
	}
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N�iPMD���f���p�j
technique MainTecBS4  < string MMDPass = "object_ss"; bool UseTexture = false; bool UseSphereMap = false; bool UseToon = true; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(false, false, true);
		PixelShader  = compile ps_3_0 BufferShadow_PS(false, false, true);
	}
}

technique MainTecBS5  < string MMDPass = "object_ss"; bool UseTexture = true; bool UseSphereMap = false; bool UseToon = true; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(true, false, true);
		PixelShader  = compile ps_3_0 BufferShadow_PS(true, false, true);
	}
}

technique MainTecBS6  < string MMDPass = "object_ss"; bool UseTexture = false; bool UseSphereMap = true; bool UseToon = true; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(false, true, true);
		PixelShader  = compile ps_3_0 BufferShadow_PS(false, true, true);
	}
}

technique MainTecBS7  < string MMDPass = "object_ss"; bool UseTexture = true; bool UseSphereMap = true; bool UseToon = true; > {
	pass DrawObject {
#ifdef ZPREPASS_ENABLE
		ZWriteEnable=FALSE;
		ZFunc=Equal;
#endif
		VertexShader = compile vs_3_0 BufferShadow_VS(true, true, true);
		PixelShader  = compile ps_3_0 BufferShadow_PS(true, true, true);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////

//ZprePass {{{

#ifdef ZPREPASS_ENABLE
// ���_�V�F�[�_
float4 drawZPrePassVS( MMM_SKINNING_INPUT IN ) : POSITION0
{
	float4 pos = MMM_SkinnedPosition( IN.Pos, IN.BlendWeight, IN.BlendIndices, IN.SdefC, IN.SdefR0, IN.SdefR1 );

	// �J�������_�̃��[���h�r���[�ˉe�ϊ�
	return mul( pos, WorldViewProjMatrix );
}

// �s�N�Z���V�F�[�_
float4 drawZPrePassPS() : COLOR0
{
	return float4(0,0,0,0);
}

technique ZPrePassTech  < string MMDPass = "zprepass"; > {
	pass ZPrePass {
		COLORWRITEENABLE=0;
		VertexShader = compile vs_3_0 drawZPrePassVS();
		PixelShader  = compile ps_3_0 drawZPrePassPS();
	}
}
#endif


//}}}
// vim: ft=fx

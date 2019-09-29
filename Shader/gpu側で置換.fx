static const int MAX_MATRICES = 12;
float4x4    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;
int			mIndicesTable[12];
float4x4    mViewProj : VIEWPROJECTION;
int NumUsedBones = 2;

struct VS_INPUT
{
    float4			Pos             : POSITION;
    float4			BlendWeights    : BLENDWEIGHT;
    int4			BlendIndices    : BLENDINDICES;
    float3			Normal          : NORMAL;
    float3			Tex0            : TEXCOORD0;
	float4			Color			: COLOR0;
};


float4 gTransform[ 4 ] : register( c0 );
float4 gWorldMatrix[ 3 ] : register( c4 );
float4 gInvTransposedWorldMatrix[ 3 ] : register( c7 );
float4 gDiffuseColor : register( c10 );

struct ToPixel{
	float4 screenPosition : POSITION;
	float4 color : COLOR0; //���_�J���[
	float2 texCoord : TEXCOORD0; //�e�N�X�`�����W
	float3 position : TEXCOORD1; //���[�J�����W�ʒu
	float3 normal : TEXCOORD2; //���[�J�����W�@��
};


//�s�N�Z���ɏ��������邾����Vertex�V�F�[�_
ToPixel VS_main( VS_INPUT i){
	ToPixel o;
	float3      Pos = 0.0f;
    float3      Normal = 0.0f;    

    // cast the vectors to arrays for use in the for loop below
   // float BlendWeightsArray[4] = (float[4])i.BlendWeights;
    //int   IndexArray[4]        = (int[4])IndexVector;
    
	float4x4 world= {
						0.0, 0.0, 0.0, 0.0,
						0.0, 0.0, 0.0, 0.0,
						0.0, 0.0, 0.0, 0.0,
						0.0, 0.0, 0.0, 0.0  };
	//int imax = min(NumBones, 4);
    // calculate the pos/normal using the "normal" weights 
    //        and accumulate the weights to calculate the last weight
    for (int iBone = 0; iBone < 2; iBone++)
    {
		if( i.BlendIndices[iBone]==255 ){
			break;
		}
		//cpu�Ŏ������ׂ����H
		for( int j = 0; j < 12; j++){
			if( i.BlendIndices[iBone]==mIndicesTable[j] ){
				world += mWorldMatrixArray[j]  * i.BlendWeights[iBone];
				break;
			}
		}
	}

	Pos = mul( i.Pos , world );
    Normal = mul( float4(i.Normal.xyz, 0.0), world );	
	
	// normalize normals
	Normal = normalize(Normal);

	o.screenPosition = mul( float4(Pos.xyz, 1.f) , mViewProj );
	//o.screenPosition.x = dot( gTransform[0], float4(Pos.xyz, 1.f) );
	//o.screenPosition.y = dot( gTransform[1], float4(Pos.xyz, 1.f) );
	//o.screenPosition.z = dot( gTransform[2], float4(Pos.xyz, 1.f) );
	//o.screenPosition.w = dot( gTransform[3], float4(Pos.xyz, 1.f) );
	float3 wp=Pos.xyz; //���[���h�ʒu
	//float3 wn; //���[���h�@��
	//wn.x = dot( gInvTransposedWorldMatrix[ 0 ].xyz, Normal );
	//wn.y = dot( gInvTransposedWorldMatrix[ 1 ].xyz, Normal );
	//wn.z = dot( gInvTransposedWorldMatrix[ 2 ].xyz, Normal );
#ifdef LINEAR_ZBUFFER
	o.screenPosition.z *= o.screenPosition.w;
#endif
	o.color = i.Color.zyxw * gDiffuseColor / 255.0;
	o.texCoord = i.Tex0;
	o.position = wp;
	//o.normal = wn;
	o.normal = Normal;
	return o;
}

float3 gAmbient : register( c0 );
float4 gLightPosX : register( c1 );
float4 gLightPosY : register( c2 );
float4 gLightPosZ : register( c3 );
float3 gEyePosition : register( c4 );
float4 gSpecularColor : register( c5 );
float3 gLightColor0 : register( c6 );
float3 gLightColor1 : register( c7 );
float3 gLightColor2 : register( c8 );
float3 gLightColor3 : register( c9 );
float3 gEmissionColor : register( c10 );

sampler2D gSampler : register( s0 );

struct FromVertex{
	float4 color : COLOR0; //���_�J���[
	float2 texCoord : TEXCOORD0;
	float3 position : TEXCOORD1;
	float3 normal : TEXCOORD2;
};

float4 PS_main( FromVertex input ) : COLOR {
/*
	//���˃x�N�^�̐���(�����������x�N�^�ɑ΂���)
	float3 ev = gEyePosition - input.position;
	float3 nv = normalize( input.normal );
	float3 rv = 2.0 * nv * dot( nv, ev ) - ev;
	//���C�g�x�N�^�̐���
	float4 lvX, lvY, lvZ;
	lvX = gLightPosX - input.position.xxxx;
	lvY = gLightPosY - input.position.yyyy;
	lvZ = gLightPosZ - input.position.zzzz;
	//���C�g�x�N�^�Ɣ��˃x�N�^�̓���
	float4 s;
	s = lvX * rv.xxxx;
	s += lvY * rv.yyyy;
	s += lvZ * rv.zzzz;
	//�w���֐������O�� |L||R|�ŏ����K�v����B
	float4 lvL2 = lvX * lvX; //���C�g�x�N�^��撷��
	lvL2 += lvY * lvY;
	lvL2 += lvZ * lvZ;
	lvL2 += 0.0001; //0����h�~
	float4 rcpLvL = rsqrt( lvL2 ); //�����̋t��
	float4 rvL2 = dot( rv, rv );
	s *= rsqrt( lvL2 * rvL2 );
	s = max( 0, s );
	s = pow( s, gSpecularColor.w );
	//����Ƀ��C�g�����ŏ���
	s *= rcpLvL;
	//�f�B�t���[�Y�����v�Z����B
	float4 d = lvX * nv.xxxx;
	d += lvY * nv.yyyy;
	d += lvZ * nv.zzzz; 
	d *= rcpLvL * rcpLvL; //�����|L|^2�ŏ�����B
	d = max( 0, d );

	float3 dc = gAmbient;
	dc += gLightColor0 * d.x;
	dc += gLightColor1 * d.y;
	dc += gLightColor2 * d.z;
	dc += gLightColor3 * d.w;
	float3 sc = 0;
	sc += gLightColor0 * s.x;
	sc += gLightColor1 * s.y;
	sc += gLightColor2 * s.z;
	sc += gLightColor3 * s.w;

	float4 c = input.color * tex2D( gSampler, input.texCoord );
	c.xyz *= dc.xyz;
	c.xyz += sc * gSpecularColor.xyz;
	c.xyz += gEmissionColor.xyz;
	c.w = c.w;
	return c;
*/
	float4 c = input.color * tex2D( gSampler, input.texCoord );
	//c.xyz *= dc.xyz;
	//c.xyz += sc * gSpecularColor.xyz;
	//c.xyz += gEmissionColor.xyz;
	//c.w = c.w;
	return c;
}

//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique t0
{


    pass p0
    {
        VertexShader = compile vs_3_0 VS_main();
        PixelShader = compile ps_3_0 PS_main();
    }
}


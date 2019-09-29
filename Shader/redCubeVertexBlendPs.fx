//�s�N�Z���ɏ��������邾���̃V�F�[�_
float4 gProjectionView[ 4 ] : register( c0 );
float4 gWorldMatrix[ 3 ] : register( c4 );
float4 gInvTransposedWorldMatrix[ 3 ] : register( c7 );
float4 gDiffuseColor : register( c10 );
// float2 gOffsetPixel : register( c19 );
static const int MAX_MATRICES = 32;
float4 gWorldMatrixArray[MAX_MATRICES*3] : register( c19 );

struct ToPixel{
	float4 screenPosition : POSITION;
	float4 color : COLOR0; //���_�J���[
	float2 texCoord : TEXCOORD0; //�e�N�X�`�����W
	float3 position : TEXCOORD1; //���[�J�����W�ʒu
	float3 normal : TEXCOORD2; //���[�J�����W�@��
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
	float4 world[3] = {
		{ 0.0, 0.0, 0.0, 0.0, },
		{ 0.0, 0.0, 0.0, 0.0, },
		{ 0.0, 0.0, 0.0, 0.0, } };
	for (int iBone = 0; iBone<2; iBone++){
		world[0] += gWorldMatrixArray[i.blendIndices[iBone]*3+0]  * i.blendWeights[iBone];
		world[1] += gWorldMatrixArray[i.blendIndices[iBone]*3+1]  * i.blendWeights[iBone];
		world[2] += gWorldMatrixArray[i.blendIndices[iBone]*3+2]  * i.blendWeights[iBone];
	}
	
	// float4 localPos;
	// localPos.x = dot( gWorldMatrix[ 0 ], i.pos );
	// localPos.y = dot( gWorldMatrix[ 1 ], i.pos );
	// localPos.z = dot( gWorldMatrix[ 2 ], i.pos );
	// localPos.w = 1.0;
	// 
	// float4 wp; //���[���h�ʒu
	// wp.x = dot( world[ 0 ], localPos );
	// wp.y = dot( world[ 1 ], localPos );
	// wp.z = dot( world[ 2 ], localPos );
	// wp.w = 1.0;
	// 
	// o.position = wp.xyz;

	float4 localPos;
	localPos.x = dot( world[ 0 ], i.pos );
	localPos.y = dot( world[ 1 ], i.pos );
	localPos.z = dot( world[ 2 ], i.pos );
	localPos.w = 1.0;
	
	float4 wp; //���[���h�ʒu
	wp.x = dot( gWorldMatrix[ 0 ], localPos );
	wp.y = dot( gWorldMatrix[ 1 ], localPos );
	wp.z = dot( gWorldMatrix[ 2 ], localPos );
	wp.w = 1.0;
	
	o.position = wp.xyz;

	o.screenPosition.x = dot( gProjectionView[ 0 ], wp );
	o.screenPosition.y = dot( gProjectionView[ 1 ], wp );
	o.screenPosition.z = dot( gProjectionView[ 2 ], wp );
	o.screenPosition.w = dot( gProjectionView[ 3 ], wp );
	
	// o.screenPosition.x += gOffsetPixel.x; 
	// o.screenPosition.y += gOffsetPixel.y; 
	
#ifdef LINEAR_ZBUFFER
	o.screenPosition.z *= o.screenPosition.w;
#endif
	
	// float3 n;
	// n.x = dot( gInvTransposedWorldMatrix[ 0 ].xyz, i.normal );
	// n.y = dot( gInvTransposedWorldMatrix[ 1 ].xyz, i.normal );
	// n.z = dot( gInvTransposedWorldMatrix[ 2 ].xyz, i.normal );

	// //�{���͋t�]�u�s��Ŗ@�������߂�ׂ������A��]�s��͒��s�s��Ȃ̂ŋ��߂Ȃ��B
	// //�X�P�[�����O���Ȃ����炱�ꂪ�g����B
	// o.normal.x = dot( world[ 0 ].xyz, n );
	// o.normal.y = dot( world[ 1 ].xyz, n );
	// o.normal.z = dot( world[ 2 ].xyz, n );
	
	float3 n;
	n.x = dot( world[ 0 ].xyz, i.normal );
	n.y = dot( world[ 1 ].xyz, i.normal );
	n.z = dot( world[ 2 ].xyz, i.normal );

	//�{���͋t�]�u�s��Ŗ@�������߂�ׂ������A��]�s��͒��s�s��Ȃ̂ŋ��߂Ȃ��B
	//�X�P�[�����O���Ȃ����炱�ꂪ�g����B
	o.normal.x = dot( gInvTransposedWorldMatrix[ 0 ].xyz, n );
	o.normal.y = dot( gInvTransposedWorldMatrix[ 1 ].xyz, n );
	o.normal.z = dot( gInvTransposedWorldMatrix[ 2 ].xyz, n );
	

	
	
	o.normal = normalize( o.normal );
	
	o.color = i.color.zyxw * gDiffuseColor / 255.0;
	o.texCoord = i.tex0;
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

float4 mainPS( FromVertex input ) : COLOR {
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
}


technique main
{
	pass p0
	{
		VertexShader = compile vs_2_0 mainVS();
		PixelShader = compile ps_2_0 mainPS();
	}
}


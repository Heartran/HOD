////////////////////////////////////////////////////////////////////////////////////////////////

// �w�i�̃N���A�l
float4 ClearColor = {1,1,1,0};
float ClearDepth  = 1.0;

////////////////////////////////////////////////////////////////////////////////////////////////

// �|�X�g�G�t�F�N�g�ł͕K���ȉ��̐ݒ������B
float Script : STANDARDSGLOBAL <
string ScriptOutput = "color";
string ScriptClass = "scene";
string ScriptOrder = "postprocess";
> = 0.8;


// �I���W�i���̕`�挋�ʂ��L�^���邽�߂̃����_�[�^�[�Q�b�g
texture ScnMap : RENDERCOLORTARGET <
float2 ViewPortRatio = {1.0,1.0};
>;
sampler ScnSamp = sampler_state {
	texture = <ScnMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;
};

// �[�x�o�b�t�@
texture DepthBuffer : RENDERDEPTHSTENCILTARGET <
float2 ViewPortRatio = {1.0,1.0};
>;


// �X�N���[���T�C�Y
float2 ViewportSize : VIEWPORTPIXELSIZE;

// ���s�N�Z��
static float2 ViewportOffset = (float2(0.5,0.5)/ViewportSize);


struct VS_OUTPUT {
	float4 Pos			: POSITION;
	float2 Tex			: TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// �V�F�[�_

VS_OUTPUT VS_DrawBuffer( float4 Pos : POSITION, float4 Tex : TEXCOORD0 ){
	VS_OUTPUT Out; 

	Out.Pos = Pos;
	Out.Tex = Tex + ViewportOffset;

	return Out;
}

float4 PS_DrawBuffer(float2 Tex: TEXCOORD0) : COLOR
{   
	float4 Color = tex2D( ScnSamp, Tex );

	// ��������
	float Gray = (Color.r + Color.g + Color.b ) / 3;  //���̍s��ǉ�
	Color.rgb = float3(Gray, Gray, Gray);             //����


	return Color;
}

////////////////////////////////////////////////////////////////////////////////////////////////

technique PostEffect <
string Script = 
"RenderColorTarget0=ScnMap;"
"RenderDepthStencilTarget=DepthBuffer;"
"ClearSetColor=ClearColor;"
"ClearSetDepth=ClearDepth;"
"Clear=Color;"
"Clear=Depth;"
"ScriptExternal=Color;"
"RenderColorTarget0=;"
"RenderDepthStencilTarget=;"
"Pass=DrawBuffer;"
;
> {
	pass DrawBuffer < string Script= "Draw=Buffer;"; > {
		AlphaBlendEnable = FALSE;
		VertexShader = compile vs_2_0 VS_DrawBuffer();
		PixelShader  = compile ps_2_0 PS_DrawBuffer();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////

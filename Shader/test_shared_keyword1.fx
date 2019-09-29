float Script : STANDARDSGLOBAL <
    string ScriptOutput = "color";
    string ScriptClass = "sceneorobject";
    string ScriptOrder = "postprocess";
> = 0.8;
texture Dummy;

shared texture EmittionTex;

shared sampler EmittionSamp = sampler_state
{
	texture = < EmittionTex >;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;
	AddressU  = CLAMP;
	AddressV = CLAMP;
};

float value;
shared float shared_value;
struct VS_Out
{
	float4 vertex : POSITION;
	float4 screen_pos : TEXCOORD0;
	float4 lightpos_mvp : TEXCOORD1;
};


VS_Out drawSquadVS( float4 pos : POSITION ) {
	VS_Out o = (VS_Out)0;
	o.vertex=pos;
	o.screen_pos = pos;
	return o;
}

float4 drawPS( float4 screen_pos : TEXCOORD0 ) : COLOR0
{
	float2 tex = (screen_pos.xy / screen_pos.w + 1.0) * 0.5;
	tex.y = 1.0-tex.y;
	float4 color = tex2D( EmittionSamp, tex )*shared_value*value;
	return color;
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N
technique MainTec  <
string MMDPass = "object";
string Script = 
	"ScriptExternal=Color;"
	//�o�b�t�@�N���A
	"Clear=Color;"
	"Clear=Depth;"
	"Pass=passTest;"
	;
> {
	
	pass passTest< string Script= "Draw=Buffer;"; > {
		ZEnable = FALSE;
		ZWriteEnable = FALSE;
		AlphaBlendEnable = FALSE;
		VertexShader = compile vs_3_0 drawSquadVS();
		PixelShader  = compile ps_3_0 drawPS();
	}
}

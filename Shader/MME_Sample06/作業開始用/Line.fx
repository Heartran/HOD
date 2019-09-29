// CodeZine MME�A�ڃT���v��
// ���C���G�t�F�N�g
// ����F�r�[���}��P

//�^�[�Q�b�g�I�u�W�F�N�g������W���擾
float3 TargetPos2 : CONTROLOBJECT < string name = "Target.x"; >;

//������Si�l���擾
float Scale : CONTROLOBJECT < string name = "Line.x"; string item = "Si"; >;
//������Tr�l���擾
float Alpha : CONTROLOBJECT < string name = "Line.x"; string item = "Tr"; >;


//�g�p����e�N�X�`��
texture Line_Tex
<
   string ResourceName = "Tex.png";
>;
//�e�N�X�`���T���v��
sampler LineTexSampler = sampler_state
{
   //�g�p����e�N�X�`��
   Texture = (Line_Tex);
   //�e�N�X�`���͈�0.0�`1.0���I�[�o�[�����ۂ̏���
   //WRAP:���[�v
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   //�e�N�X�`���t�B���^�[
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = NONE;
};
#define RND_SIZE 128.0
//�����_���e�N�X�`��
texture2D RndTex <
    string ResourceName = "RndTex.bmp";
>;
//�����_���e�N�X�`���T���v��
sampler RndSampler = sampler_state {
    texture = <RndTex>;
};
// ���@�ϊ��s��
float4x4 ViewProjMatrix      : VIEWPROJECTION;
float4x4 WorldMatrix      : WORLD;

float3   CameraPosition    : POSITION  < string Object = "Camera"; >;

//�G���~�[�g�⊮�֐�
float3 HermiteLerp(float3 s,float3 e,float3 svec,float3 evec,float t)
{
	return (((t-1)*(t-1))*(2*t+1)*s) + ((t*t)*(3-2*t)*e) +((1-(t*t))*t*svec) + ((t-1)*(t*t)*evec);
}

//����
float time : TIME;

struct VS_OUTPUT
{
    float4 Pos        : POSITION;    // �ˉe�ϊ����W
    float2 Tex        : TEXCOORD1;   // �e�N�X�`��
};

// ���_�V�F�[�_
VS_OUTPUT Line_VS(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
   
    //�^�[�Q�b�g�p���W
    float3 TargetPos = float3(0,0,100);
    //�o�͗p���W
    float4 LastPos = 1;
    //---���݂̍��W�擾---//
    //���[���h�s���4�s�ڂ��烏�[���h���W���擾
    float3 ZeroPos = WorldMatrix[3].xyz;
   
    //���_�ƖڕW�_�̊Ԃ���`��Ԃō��W���擾
    //��ԗp�W���̓��f����Z���W
    float3 NowPos = lerp(ZeroPos,TargetPos,Pos.z);
   
    //�o�͗p���W�ɕۑ�
    LastPos.xyz = NowPos;
   
    //---�r���{�[�h�v�Z---//
	//�������珑��������
	LastPos.xy = Pos.xy;
	//�����܂�
   
   
    // �J�������_�r���[�ˉe�ϊ�
    Out.Pos = mul( LastPos, ViewProjMatrix );
   
    // �e�N�X�`�����W
    Out.Tex = Tex;
   
    return Out;
}

// �s�N�Z���V�F�[�_
float4 Line_PS( VS_OUTPUT IN ) : COLOR0
{
	//�o�͗p�J���[
	float4 Color;
	
	Color = 1;
	
    return Color;
}

// �I�u�W�F�N�g�`��p�e�N�j�b�N
technique MainTec < string MMDPass = "object"; > {
    pass DrawObject
    {
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		CULLMODE = NONE;
		ALPHABLENDENABLE = TRUE;
		SRCBLEND=SRCALPHA;
		DESTBLEND=ONE;
        VertexShader = compile vs_3_0 Line_VS();
        PixelShader  = compile ps_3_0 Line_PS();
    }
}
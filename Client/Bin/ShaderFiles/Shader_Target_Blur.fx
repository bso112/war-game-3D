


texture		g_BlurTexture;
texture		g_BlendTexture;

float		g_fWinCX;
float		g_fWinCY;

float		offsets[9] = { -4,-3,-2,-1,0,1,2,3,4 };
float		weight[9][9] =
{

	0.012345,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012345
	,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346
	,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346
	,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346
	,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346
	,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346
	,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346
	,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346
	,0.012345,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012346,0.012345


};
sampler BlurSampler = sampler_state
{
	texture = g_BlurTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
sampler BlendSampler = sampler_state
{
	texture = g_BlendTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};




struct PS_OUT
{
	vector vColor : COLOR0;
	vector vColor_Src : COLOR1;
	vector vColor_Src2 : COLOR2;
	vector vColor_Src3 : COLOR3;//
};


PS_OUT PS_BLUR(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1)
{
	PS_OUT	Out = (PS_OUT)0;

	float ratioX = 1 / g_fWinCX;
	float ratioY = 1 / g_fWinCY;

	float2 texel;

	for (int i = 0; i < 9; ++i)
	{

		/*
		�ȼ����̴����� �� �� �ִ°��� UV��ǥ ���̴�.
		���� ���� �ʿ��� ���� ��Ȯ�� �ȼ��� ��ġ��. �׷��� weight�� �ε����� ���밡��.
		���� UV�κ��� 11x11 ���� �ȼ��� weight�� ������ �� �ջ��� ���� ���� �ȼ��� ������ ���Ѵ�.
		texel.x = In.vTexUV.x + offsets[i] * ratioX; �� ���� UV�κ��� 11x11 ���� �ȼ��� �ش��ϴ� UV��ǥ�� ��� ���� ���̴�.
		��, �ȼ��� ��ǥ�� UV�� ��ȯ�� ���̴�.
		ratioX�� ȭ���� �ʺ� 1�� ������ ���ȼ��� �����ϴ� ������ �ǹ�.
		��, UV�� ������ �� �ȼ��� �ʺ� �ǹ�.
		offsets�� ���� �ȼ����� ������ ������ �ǹ�.
		���� �ؼ���ǥ�� �ȼ��� ��ǥ�� UV�� �������� UV ��ǥ��.
		*/
		texel.x = _vTexUV.x + offsets[i] * ratioX;

		for (int j = 0; j < 9; ++j)
		{
			texel.y = _vTexUV.y + offsets[j] * ratioY;
			Out.vColor += tex2D(BlurSampler, texel) * weight[i][j];
		}
	}

	vector	vBlendColor = tex2D(BlendSampler, _vTexUV);
	Out.vColor.rgb = vBlendColor.rgb * vBlendColor.a + Out.vColor.rgb * (1.f - vBlendColor.a);
	Out.vColor.a = 0.f;

	Out.vColor_Src = Out.vColor;
	Out.vColor_Src2 = Out.vColor;
	Out.vColor_Src.a = Out.vColor_Src2.a = 1.f;

	Out.vColor_Src3 = Out.vColor;
	Out.vColor_Src3.a = 0.f;
	return Out;
}

PS_OUT PS_RADIAL_BLUR(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1)
{
	PS_OUT	Out = (PS_OUT)0;

	//float step_size = 180.f / 9.f;
	//float radius = 60.f;

	//for (float angle = 0.0; angle <= 180.f; angle += step_size)
	//{
	//	int index = max((angle / step_size) -1 , 0);
	//	float2 vUv = float2(cos(angle)*radius, sin(angle)*radius);
	//	Out.vColor += weight[0][index] * tex2D(BlendSampler, vUv);
	//	//weight *= 0.9; // or some other falloff
	//}

	Out.vColor /= 9.f;


	return Out;


}




technique DefaultTechnique
{

	pass Blur
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		ZEnable = false;
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLUR();
	}

	pass RadialBlur
	{
		ZEnable = false;
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_RADIAL_BLUR();
	}


}
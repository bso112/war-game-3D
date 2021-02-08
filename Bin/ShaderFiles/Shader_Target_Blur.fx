


texture g_DiffuseTexture;
//±âº» 1
float	g_Alpha;
float	g_fMaxHp;
float	g_fCurrHp;
vector	g_vColor = (vector)0.f;



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
sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
};


struct PS_OUT
{
	vector vColor : COLOR0;
};

PS_OUT PS_Default(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = tex2D(DiffuseSampler, _vTexUV);

	return Out;

}


PS_OUT PS_BLUR(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1)
{
	PS_OUT	Out = (PS_OUT)0;

	float ratioX = 1 / g_fWinCX;
	float ratioY = 1 / g_fWinCY;

	float2 texel;

	for (int i = 0; i < 9; ++i)
	{
		texel.x = _vTexUV.x + offsets[i] * ratioX;

		for (int j = 0; j < 9; ++j)
		{
			texel.y = _vTexUV.y + offsets[j] * ratioY;
			Out.vColor += tex2D(DiffuseSampler, texel) * weight[i][j];
		}
	}


	return Out;
}




technique DefaultTechnique
{

	pass Default
	{
		ZEnable = false;
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_Default();
	}
	pass Blur
	{
		ZEnable = false;
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLUR();
	}


}
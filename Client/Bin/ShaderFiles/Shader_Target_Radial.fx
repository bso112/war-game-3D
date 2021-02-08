
float		g_fInput = 1.f;

texture		g_BlurTexture;
texture		g_RadialTexture;

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
sampler RadialSampler = sampler_state
{
	texture = g_RadialTexture;
	//minfilter = linear;
	//magfilter = linear;
	//mipfilter = linear;
};
////////////////////////////////////////////////////////////////



struct PS_OUT
{
	vector vColor : COLOR0;
};


PS_OUT PS_BLUR(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1)
{
	PS_OUT	Out = (PS_OUT)0;

	float ratioX = 1 / g_fWinCX;
	float ratioY = 1 / g_fWinCY;
	vector vRadialColor = vector(0.f, 0.f, 0.f, 0.f);
	vector vwNormal = (0.f, 0.f, 0.f, 0.f);

	float fIntensity = 100.f * g_fInput;
	/////////////////////////////////////////////////////////////////////////////////////////
	float2 texel;
	vwNormal = tex2D(RadialSampler, _vTexUV);
	float fLen = length(float3(vwNormal.x - 0.5f, vwNormal.y - 0.5f, 0.f));
	fLen = pow(fLen * 4.f, 5);

	//texel.x = _vTexUV.x - ((vwNormal.x - 0.5f) * 2.f) * fLen * ratioX * fIntensity;
	//texel.y = _vTexUV.y + ((vwNormal.y - 0.5f) * 2.f) * fLen * ratioY * fIntensity;

	
	//vRadialColor = tex2D(BlurSampler, texel);
	//////////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 9; ++i)
	{
		//float2 texel;
		//vwNormal = tex2D(RadialSampler, _vTexUV);
	texel.x = _vTexUV.x - ((vwNormal.x - 0.5f) * 2.f) * fLen * ratioX * fIntensity * i / 8.f;
	texel.y = _vTexUV.y + ((vwNormal.y - 0.5f) * 2.f) * fLen * ratioY * fIntensity * i / 8.f;

		vRadialColor += tex2D(BlurSampler, texel)/100.f;
	}
	vRadialColor  = vRadialColor / 9.f * 100.f;
	//////////////////////////////////////////////////////////////////////////////////////////

	vector vOriginalColor = tex2D(BlurSampler, _vTexUV);
	vwNormal.a = length(vwNormal) * vwNormal.a;/////////////////
	Out.vColor = vRadialColor * vwNormal.a + vOriginalColor * (1.f - vwNormal.a);
	//Out.vColor = vRadialColor;
	//Out.vColor.a = 1.f;

	//Out.vColor = vector(vwNormal.x,vwNormal.y, 0.f, 1.f) * vwNormal.a;

	//Out.vColor = vOriginalColor;
	//Out.vColor = vector(1.f,1.f,0.f,1.f);
	return Out;

	/*for (int i = 0; i < 9; ++i)
	{
		texel.x = _vTexUV.x + offsets[i] * ratioX;

		for (int j = 0; j < 9; ++j)
		{
			texel.y = _vTexUV.y + offsets[j] * ratioY;
			Out.vColor += tex2D(BlurSampler, texel) * weight[i][j];
		}
	}

	vector	vBlendColor = tex2D(BlendSampler, _vTexUV);
	Out.vColor.rgb = vBlendColor.rgb * vBlendColor.a + Out.vColor.rgb * (1.f - vBlendColor.a);

	return Out;*/
}




technique DefaultTechnique
{

	pass Blur
	{
		ZEnable = false;
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLUR();
	}

	//pass RadialBlur
	//{
	//	ZEnable = false;
	//	VertexShader = NULL;
	//	PixelShader = compile ps_3_0 PS_RADIAL_BLUR();
	//}


}
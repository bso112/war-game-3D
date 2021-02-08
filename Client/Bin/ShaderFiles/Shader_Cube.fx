
float		g_fmDist;
float		g_fMDist;
vector		g_BlurCenter;

matrix		g_matWorld, g_matView, g_matProj;

texture		g_DiffuseTexture;

float		g_Alpha;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

VS_OUT/*정점*/ VS_MAIN(VS_IN In/*정점*/)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	vector		vWorldPos = mul(float4(In.vPosition, 1.0f), g_matWorld);
	Out.vWorldPos = vWorldPos;

	return Out;
}

// w나누기.(원근투영)

// 뷰포트변환.

// 래스터라이즈.

struct PS_IN
{
	float4		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : COLOR0;
	float4		vBlurColor : COLOR1;
	float4		vBlendColor : COLOR2;
};

PS_OUT PS_MAIN(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = texCUBE(DiffuseSampler, In.vTexUV);


	Out.vBlurColor = Out.vColor;
	Out.vBlendColor = Out.vColor;
	Out.vBlurColor.a = 1.f;
	Out.vBlendColor.a = 0.f;

	//float fDistance = length(In.vWorldPos - g_BlurCenter);
	//fDistance -= g_fmDist;

	//if (fDistance < 0.f)
	//	fDistance = 0.f;
	//float fAlpha = g_fMDist - fDistance;
	//if (fAlpha < 0.f)
	//	fAlpha = 0.f;
	//Out.vBlendColor.a = pow(fAlpha / g_fMDist, 2.f);


	return Out;
}

PS_OUT PS_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = texCUBE(DiffuseSampler, In.vTexUV);
	Out.vColor.rgb = 1.f;
	Out.vColor.a = g_Alpha;

	Out.vBlendColor = Out.vColor;
	//Out.vBlurColor = Out.vColor;

	return Out;
}

technique Default_Technique
{
	// 내가 표현하고자하는 기법들의ㅡ 집합.(명암, 스펙큘러, 그림자, 림라이트, 모션블러)
	pass Default_Rendering
	{
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass SkyBox
	{
		CullMode = cw;
		ZWriteEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass Alpha
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_ALPHA();
	}
	pass CollideBox
	{
		FillMode = WireFrame;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}
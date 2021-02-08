
matrix		g_matWorld, g_matView, g_matProj;

texture	g_DiffuseTexture;

float		g_Percent;

vector g_Color;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT/*정점*/ VS_MAIN(VS_IN In/*정점*/)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

// w나누기.(원근투영)

// 뷰포트변환.

// 래스터라이즈.

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : COLOR0;
};



PS_OUT PS_MAIN(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_COLOR(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	float alpha = Out.vColor.a;
	Out.vColor = g_Color;
	Out.vColor.a = alpha;

	return Out;
}

PS_OUT PS_HP(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float4 TexColor = tex2D(DiffuseSampler, In.vTexUV);
	if (TexColor.r * TexColor.b * TexColor.a != 0)
	{
		if (In.vTexUV.x < g_Percent)
		{
			TexColor = float4(1.f, 0.f, 0.f, 1.f);
		}
		else
		{
			TexColor = (float4)0;
		}
	}
	Out.vColor = TexColor;
	return Out;
}


technique Default_Technique
{
	pass DEFAULT
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	// 내가 표현하고자하는 기법들의ㅡ 집합.(명암, 스펙큘러, 그림자, 림라이트, 모션블러)
	pass Color_Rendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_COLOR();
	}
	pass UI_Rendering
	{
		ZEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Color_HP
	{
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HP();
	}
}
float		g_fmDist;
float		g_fMDist;
vector		g_BlurCenter;

matrix		g_matWorld, g_matView, g_matProj;

texture		g_DiffuseTexture;

float		g_Alpha = 1;
float		g_CoolDown;
float3		g_CenterPos;

float		g_Percent;

bool		g_isBlue;
bool		g_isYellow;

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
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : COLOR0;
	float4		vBlurColor : COLOR1;
	float4		vBlendColor : COLOR2;
};

struct PS_PS
{
	float4	vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vBlurColor = Out.vColor;
	Out.vBlendColor = Out.vColor;

	float fDistance = length(In.vWorldPos - g_BlurCenter);
	fDistance -= g_fmDist;

	if (fDistance < 0.f)
		fDistance = 0.f;
	float fAlpha = g_fMDist - fDistance;
	if (fAlpha < 0.f)
		fAlpha = 0.f;
	//Out.vBlendColor.a = min(pow(fAlpha / g_fMDist, 2.f), Out.vColor.a);
	//Out.vBlurColor.a = min((1.f - pow(fAlpha / g_fMDist, 2.f)), Out.vColor.a);
	Out.vBlendColor.a = min(pow(fAlpha / g_fMDist, 2.f), Out.vColor.a);
	Out.vBlurColor.a = Out.vColor.a;

	return Out;
}

PS_OUT PS_COLOR(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	float alpha = Out.vColor.a;
	Out.vColor = g_Color;
	Out.vColor.a = alpha;
	Out.vBlendColor = Out.vColor;

	//Out.vBlurColor = Out.vColor;
	//Out.vBlendColor = Out.vColor;

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

PS_OUT PS_UI(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vBlendColor = Out.vColor;

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
	Out.vBlendColor = Out.vColor;
	return Out;
}

// FlagColor
PS_OUT PS_FLAG(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float4 vColor = tex2D(DiffuseSampler, In.vTexUV);

	float fBigger = vColor.r > vColor.b ? vColor.r : vColor.b;
	float fSmaller = vColor.r < vColor.b ? vColor.r : vColor.b;

	Out.vColor = true == g_isBlue ? float4(fSmaller, vColor.g, fBigger, vColor.a) : float4(fBigger, vColor.g, fSmaller, vColor.a);

	Out.vBlurColor = Out.vColor;
	Out.vBlendColor = Out.vColor;

	float fDistance = length(In.vWorldPos - g_BlurCenter);
	fDistance -= g_fmDist;

	if (fDistance < 0.f)
		fDistance = 0.f;
	float fAlpha = g_fMDist - fDistance;
	if (fAlpha < 0.f)
		fAlpha = 0.f;
	Out.vBlendColor.a = pow(fAlpha / g_fMDist, 2.f) * Out.vColor.a;

	return Out;
}

PS_OUT PS_ALPHA(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
	Out.vBlendColor = Out.vColor;
	if (Out.vBlendColor.a * 1 != 0)
		Out.vBlendColor.a = g_Alpha;

	return Out;
}

PS_OUT PS_INTRO(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
	Out.vBlendColor = Out.vColor;
	return Out;
}

PS_OUT PS_MENU(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float4 vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor = true == g_isYellow ? vColor : float4(1.f, 1.f, 1.f, vColor.a);

	Out.vBlendColor = Out.vColor;

	return Out;
}

PS_OUT PS_COOLDOWN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	float2 vUp = float2(0.f, -1.f);
	float2 vDir = float2(In.vTexUV.x - 0.5f, In.vTexUV.y - 0.5f);
	vDir = normalize(vDir);

	float theta = dot(vUp, vDir);
	float rad = acos(theta);
	float Result = 0;

	if (vUp.x > vDir.x)
		Result = rad;
	else
		Result = (2 * 3.141592) - rad;

	float Percent = g_CoolDown * 2 * 3.141592;
	 
	if (Result > Percent)
		Out.vColor.rgb = 0;

	Out.vBlendColor = Out.vColor;
	return Out;
}

technique Default_Technique
{
	pass DEFAULT	// 0
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	// 내가 표현하고자하는 기법들의ㅡ 집합.(명암, 스펙큘러, 그림자, 림라이트, 모션블러)
	pass Color_Rendering	// 1
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_COLOR();
	}

	pass UI_Rendering	// 2
	{
		ZEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_UI();
	}

	pass Color_HP	// 3
	{
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HP();
	}

	pass Alpha_NoneCull // 4
	{
		cullmode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;


		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_ALPHA();
	}

	pass DEFAULT_alphaTest // 5
	{
		AlphaBlendEnable = false;
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Weapon_AlphaTest // 6
	{
		CullMode = none;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Billboard_AlphaTest // 7
	{
		AlphaTestEnable = true;
		AlphaRef = 0.99;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Flag_AlphaBlend // 8
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_FLAG();
	}

	pass Intro // 9
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_INTRO();
	}

	pass UI_Rendering	// 10
	{
		ZEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MENU();
	}

	pass DEFAULT_alphaTest // 11
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_UI();
	}

	pass COOL_DOWN		// 12
	{
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_COOLDOWN();
	}
}
vector		g_Center;

matrix		g_matWorld, g_matView, g_matProj;

float		g_fTimeRation;

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
	float4		vNormal : TEXCOORD2;
};

VS_OUT/*정점*/ VS_MAIN(VS_IN In/*정점*/)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	vector		vWorldPos = mul(float4(In.vPosition, 1.0f), g_matWorld);

	vector vwNormal = vWorldPos - g_Center;

	vector		vecTemp;
	vecTemp		= mul(vwNormal, g_matView);
	//vecTemp = mul(vecTemp, g_matProj);
	vecTemp = normalize(vecTemp);
	vecTemp.a = 1.f;
	vecTemp.z = 0.f;
	vecTemp.x = vecTemp.x * 0.5f + 0.5f;
	vecTemp.y = vecTemp.y * 0.5f + 0.5f;
	Out.vNormal = vecTemp;

	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = vWorldPos;
	//Out.vNormal = vwNormal;
	//Out.vNormal = float4(In.vPosition, 1.0f);
	//Out.vNormal.z = 0.f;
	return Out;
}

// w나누기.(원근투영)

// 뷰포트변환.

// 래스터라이즈.

struct PS_IN
{
	//float4		vPosition : POSITION;
	//float3		vTexUV : TEXCOORD0;
	//float4		vWorldPos : TEXCOORD1;
	float4		vNormal : TEXCOORD2;
};

struct PS_OUT
{
	float4		vColor_Blank : COLOR0;
	float4		vColor : COLOR1;
	float4		vColor_1:COLOR2;
	float4		vTarget : COLOR3;
};

PS_OUT PS_MAIN(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;
	//Out.vTarget = vector(1.f,1.f,1.f,1.f);
	Out.vColor_Blank = vector(0.f, 0.f, 0.f,0.f);
	Out.vColor = vector(0.f, 0.f, 0.f, 0.f);
	Out.vColor_1 = vector(0.f, 0.f, 0.f, 0.f);
	//Out.vColor_Blank.a = 0.f;
	//Out.vColor.a = 0.f;
	//Out.vColor_1.a = 0.f;
	Out.vTarget = In.vNormal;
	//Out.vTarget.a = 1.f;
	//Out.vTarget = vector(0.f,1.f,0.f,1.f);

	//Out.vColor.a = 1.f;

	//Out.vColor = texCUBE(DiffuseSampler, In.vTexUV);
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



technique Default_Technique
{
	// 내가 표현하고자하는 기법들의ㅡ 집합.(명암, 스펙큘러, 그림자, 림라이트, 모션블러)
	pass Default_Rendering
	{
		//AlphaTestEnable = true;
		//AlphaRef = 0;
		//AlphaFunc = Greater;
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		zenable = false;

		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Default_Rendering2
	{
		//AlphaTestEnable = true;
		//AlphaRef = 0;
		//AlphaFunc = Greater;
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		zenable = true;

		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	//pass WireFrame
	//{
	//	fillmode = wireframe;
	//	VertexShader = compile vs_3_0 VS_MAIN();
	//	PixelShader = compile ps_3_0 PS_COLOR();
	//}
	//pass Default_Red
	//{
	//	AlphaBlendEnable = true;
	//	SrcBlend = SrcAlpha;
	//	DestBlend = InvSrcAlpha;
	//	ZEnable = true;
	//	ZWriteEnable = true;

	//	Cullmode = none;
	//	VertexShader = compile vs_3_0 VS_MAIN();
	//	PixelShader = compile ps_3_0 PS_RED();
	//}

}
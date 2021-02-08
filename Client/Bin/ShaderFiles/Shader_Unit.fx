float		g_fmDist;
float		g_fMDist;
vector		g_BlurCenter;

matrix		g_matWorld, g_matView, g_matProj;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);


vector		g_vCamPosition;

texture		g_DiffuseTexture;



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
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float4		vShade : COLOR0;
	float4		vSpecular : COLOR1;
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

	vector		vWorldNormal = mul(float4(In.vNormal, 0.0f), g_matWorld);
	vector		vWorldPos = mul(float4(In.vPosition, 1.0f), g_matWorld);

	Out.vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)), 0.f);

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 20.f);

	Out.vTexUV = In.vTexUV/**0.05*/;

	Out.vWorldPos = vWorldPos;

	return Out;
}

// w나누기.(원근투영)

// 뷰포트변환.

// 래스터라이즈.

struct PS_IN
{
	float4		vPosition : POSITION;
	float4		vShade : COLOR0;
	float4		vSpecular : COLOR1;
	float4		vWorldPos : TEXCOORD1;
	float2		vTexUV : TEXCOORD0;
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

	vector	vMtrlDiffuse = tex2D(DiffuseSampler, In.vTexUV /** 30.0f*/);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.vSpecular;

	Out.vColor = vMtrlDiffuse;
	//Out.vBlurColor = vMtrlDiffuse;
	//Out.vBlendColor = vMtrlDiffuse;
	//Out.vBlendColor.a *= 0.7f;

	Out.vBlurColor = Out.vColor;
	Out.vBlendColor = Out.vColor;

	float fDistance = length(In.vWorldPos - g_BlurCenter);
	fDistance -= g_fmDist;

	if (fDistance < 0.f)
		fDistance = 0.f;
	float fAlpha = g_fMDist - fDistance;
	if (fAlpha < 0.f)
		fAlpha = 0.f;
	Out.vBlendColor.a = pow(fAlpha / g_fMDist, 2.f);

	return Out;
}

PS_OUT PS_SKILL(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vMtrlDiffuse = tex2D(DiffuseSampler, In.vTexUV /** 30.0f*/);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.vSpecular;

	Out.vColor = vMtrlDiffuse;
	Out.vBlurColor = vMtrlDiffuse;
	Out.vBlendColor = vMtrlDiffuse;
	//Out.vBlendColor.a *= 0.7f;
	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass Default_Rendering
	{
		fillmode = wireframe;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass Default_Rendering
	{
		cullmode = none;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass CullNone_Rendering
	{
		cullmode = none;

		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SKILL();
	}
}
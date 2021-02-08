
matrix		g_matWorld, g_matView, g_matProj;

float		g_fmDist;
float		g_fMDist;
vector		g_BlurCenter;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector		g_vCamPosition;

bool		g_bBrushOn;
float		g_fBrushRange;
float3		g_vBrushPos;

texture		g_DiffuseTexture;
texture		g_DiffuseTexture_1;
texture		g_BrushTexture;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

sampler DiffuseSampler_1 = sampler_state
{
	texture = g_DiffuseTexture_1;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

sampler BrushSampler = sampler_state
{
	texture = g_BrushTexture;
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
	float3		vNormal : NORMAL;
	float3		vLength : TEXCOORD2;
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
	Out.vWorldPos = vWorldPos;

	Out.vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)), 0.f);

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 60.f);

	Out.vLength = vLook;

	Out.vTexUV = In.vTexUV;
	Out.vNormal = In.vNormal;

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
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float3		vNormal : NORMAL;
	float3		vLength : TEXCOORD2;
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

	float fIntensity = 1.5f;
	float fHeightCompression = 800.f;
	float fMaximum_Acceleration = 0.95f;

	float fFloorTexRatio = 20.f;
	float fCliffTexRatio = 6.f;

	vector	vMtrlDiffuse = tex2D(DiffuseSampler, In.vTexUV * fFloorTexRatio);
	if (abs(In.vNormal.y) < fMaximum_Acceleration)
	{
		vector vTempDiffuse_x;
		vector vTempDiffuse_z;
		vector vTempDiffuse;
		if (abs(In.vNormal.x) < fMaximum_Acceleration)
		{
			vTempDiffuse_x = tex2D(DiffuseSampler_1, float2(In.vTexUV.x, In.vWorldPos.y / fHeightCompression) * fCliffTexRatio);
			vTempDiffuse_z = tex2D(DiffuseSampler_1, float2(In.vTexUV.y, In.vWorldPos.y / fHeightCompression) * fCliffTexRatio);
			vTempDiffuse = vTempDiffuse_x * (abs(In.vNormal.z) / (abs(In.vNormal.x) + abs(In.vNormal.z))) + vTempDiffuse_z*(1.f - (abs(In.vNormal.z) / (abs(In.vNormal.x) + abs(In.vNormal.z))));
		}
		vMtrlDiffuse *= pow(abs(In.vNormal.y) / fMaximum_Acceleration, fIntensity);
		vMtrlDiffuse += vTempDiffuse * (1 - pow(abs(In.vNormal.y) / fMaximum_Acceleration, fIntensity));
	}

	if (g_bBrushOn)
	{
		if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange &&
			g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
		{
			float2	vUV;

			vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
			vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

			vector vBrush = tex2D(BrushSampler, vUV);
			vMtrlDiffuse.rgb = vBrush.rgb * vBrush.a + vMtrlDiffuse.rgb * (1.f - vBrush.a);
		}
	}

	float fLen = length(In.vLength);

	fLen -= 300.f;
	fLen /= 20.f;

	if (fLen < 0.f)
		fLen = 0.f;
	if (fLen > 1.f)
		fLen = 1.f;

	Out.vColor = (g_vLightDiffuse * (vMtrlDiffuse)) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient));

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


	//Out.vBlendColor.a = 1;
	//Out.vBlendColor.x = pow(fAlpha / g_fMDist, 2.f);
	//Out.vBlendColor.y = pow(fAlpha / g_fMDist, 2.f);
	//Out.vBlendColor.z = pow(fAlpha / g_fMDist, 2.f);

	return Out;
}



PS_OUT PS_MAIN_DEBUG(PS_IN In/*픽셀*/)
{
	PS_OUT	Out = (PS_OUT)0;

	float fIntensity = 1.5f;
	float fHeightCompression = 800.f;
	float fMaximum_Acceleration = 0.95f;

	float fFloorTexRatio = 20.f;
	float fCliffTexRatio = 6.f;

	vector	vMtrlDiffuse = tex2D(DiffuseSampler, In.vTexUV * fFloorTexRatio);
	if (abs(In.vNormal.y) < fMaximum_Acceleration)
	{
		vector vTempDiffuse_x;
		vector vTempDiffuse_z;
		vector vTempDiffuse;
		if (abs(In.vNormal.x) < fMaximum_Acceleration)
		{
			vTempDiffuse_x = tex2D(DiffuseSampler_1, float2(In.vTexUV.x, In.vWorldPos.y / fHeightCompression) * fCliffTexRatio);
			vTempDiffuse_z = tex2D(DiffuseSampler_1, float2(In.vTexUV.y, In.vWorldPos.y / fHeightCompression) * fCliffTexRatio);
			vTempDiffuse = vTempDiffuse_x * (abs(In.vNormal.z) / (abs(In.vNormal.x) + abs(In.vNormal.z))) + vTempDiffuse_z*(1.f - (abs(In.vNormal.z) / (abs(In.vNormal.x) + abs(In.vNormal.z))));
		}
		vMtrlDiffuse *= pow(abs(In.vNormal.y) / fMaximum_Acceleration, fIntensity);
		vMtrlDiffuse += vTempDiffuse * (1 - pow(abs(In.vNormal.y) / fMaximum_Acceleration, fIntensity));
	}

	if (g_bBrushOn)
	{
		if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange &&
			g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
		{
			float2	vUV;

			vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
			vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

			vector vBrush = tex2D(BrushSampler, vUV);
			vMtrlDiffuse.rgb = vBrush.rgb * vBrush.a + vMtrlDiffuse.rgb * (1.f - vBrush.a);
		}
	}

	float fLen = length(In.vLength);

	fLen -= 300.f;
	fLen /= 20.f;

	if (fLen < 0.f)
		fLen = 0.f;
	if (fLen > 1.f)
		fLen = 1.f;

	Out.vColor = (g_vLightDiffuse * (vMtrlDiffuse)) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient));

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

	if (int(In.vTexUV.x * (16 * 129 - 15)) % 129 == 0 ||
		int(In.vTexUV.y * (16 * 129 - 15)) % 129 == 0)
	{
		Out.vBlendColor = vector(0.f, 1.f, 1.f, 1.f);
		Out.vBlurColor = vector(0.f, 1.f, 1.f, 0.f);
	}

	//Out.vBlendColor.a = 1;
	//Out.vBlendColor.x = pow(fAlpha / g_fMDist, 2.f);
	//Out.vBlendColor.y = pow(fAlpha / g_fMDist, 2.f);
	//Out.vBlendColor.z = pow(fAlpha / g_fMDist, 2.f);

	return Out;
}



technique Default_Technique
{
	// 내가 표현하고자하는 기법들의ㅡ 집합.(명암, 스펙큘러, 그림자, 림라이트, 모션블러)
	pass Default_Rendering
	{

		alphablendenable = false;
		alphatestenable = true;
		zwriteenable = true;
		zenable = true;
		fillmode = solid;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Wireframe_Rendering
	{
		alphablendenable = false;
		alphatestenable = true;
		zwriteenable = true;
		zenable = true;
		fillmode = solid;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_DEBUG();
	}

	//// 알파블렌딩 + 왜곡
	//pass AlphaBlending
	//{
	//	VertexShader = ;
	//	PixelShader = ;
	//}
}
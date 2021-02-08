


texture g_BaseTexture;
//기본 1
float	g_Alpha;
float	g_fMaxHp;
float	g_fCurrHp;
vector	g_vColor = (vector)0.f;


sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
};


struct PS_OUT
{
	vector vColor : COLOR0;

};

PS_OUT PS_Default(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1)
{
	PS_OUT Out = (PS_OUT)0.f;
	Out.vColor = tex2D(BaseSampler, _vTexUV);



	return Out;

}

vector PS_Transparent(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1) : COLOR0
{
	vector vColor = (vector)0.f;

vColor = tex2D(BaseSampler, _vTexUV);

//투명하지 않은 부분만 반투명화
vColor.a *= 0.5f;

return vColor;

}


vector PS_BLINK(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1) : COLOR0
{
	vector vColor = (vector)0.f;
                
vColor = tex2D(BaseSampler, _vTexUV);

vColor.rgb = 1.f;

return vColor;
}

vector PS_ALPHA(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1) : COLOR0
{
	vector vColor = (vector)0.f;

vColor = tex2D(BaseSampler, _vTexUV);

vColor.a = g_Alpha;

return vColor;
}

vector PS_HPBAR(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1) : COLOR0
{
	vector vColor = (vector)0.f;

vColor = tex2D(BaseSampler, _vTexUV);

float ratio = g_fCurrHp / g_fMaxHp;

//UV좌표는 곧 이미지의 비율이니까.
if (ratio < _vTexUV.x)
	vColor.a = 0;

return vColor;
}



vector PS_COLOR(float4 _vPosition : POSITION, float2 _vTexUV : TEXCOORD0, float4 _vWinPos : TEXCOORD1) : COLOR0
{
	return g_vColor;
}




technique DefaultTechnique
{

	pass Default
	{
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_Default();
	}
	pass Transparent
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_Transparent();
	}
	pass BLINK
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLINK();
	}
	pass Alpha
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_ALPHA();
	}
	pass HpBar
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_HPBAR();
	}
	pass COLOR
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_COLOR();
	}

}
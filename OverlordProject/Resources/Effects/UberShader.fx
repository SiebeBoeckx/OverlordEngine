/*
******************
* DAE Ubershader *
******************
**This Shader Contains:
- Diffuse (Texture & Color)
	- Regular Diffuse
- Specular
	- Specular Level (Texture & Value)
	- Shininess (Value)
	- Models
		- Blinn
		- Phong
- Ambient (Color)
- EnvironmentMapping (CubeMap)
	- Reflection + Fresnel Falloff
	- Refraction
- Normal (Texture)
- Opacity (Texture & Value)
-Techniques
	- WithAlphaBlending
	- WithoutAlphaBlending
*/

//GLOBAL MATRICES
//***************
// The World View Projection Matrix
float4x4 gMatrixWVP : WORLDVIEWPROJECTION;
// The ViewInverse Matrix - the third row contains the camera position!
float4x4 gMatrixViewInverse : VIEWINVERSE;
// The World Matrix
float4x4 gMatrixWorld : WORLD;

//STATES
//******
RasterizerState gRS_FrontCulling
{
    CullMode = FRONT;
};

BlendState gBS_EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

//SAMPLER STATES
//**************
SamplerState gTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};

//LIGHT
//*****
float3 gLightDirection : DIRECTION
<
	string UIName = "Light Direction";
	string Object = "TargetLight";
> = float3(0.551f, -0.534f, 0.641f);

//DIFFUSE
//*******
bool gUseTextureDiffuse
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Bool";
> = true;

float4 gColorDiffuse
<
	string UIName = "Diffuse Color";
	string UIWidget = "Color";
> = float4(1, 0.1f, 0.14f, 1);

Texture2D gTextureDiffuse
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Texture";
	string ResourceName = "CobbleStone_DiffuseMap.dds";
>;

//SPECULAR
//********
float4 gColorSpecular
<
	string UIName = "Specular Color";
	string UIWidget = "Color";
> = float4(0, 0.796, 0.252, 1);

Texture2D gTextureSpecularIntensity
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Texture";
	string ResourceName = "Specular_Level.tga";
>;

bool gUseTextureSpecularIntensity
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Bool";
> = false;

int gShininess
<
	string UIName = "Shininess";
	string UIWidget = "Slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.1f;
> = 40;

//AMBIENT
//*******
float4 gColorAmbient
<
	string UIName = "Ambient Color";
	string UIWidget = "Color";
> = float4(0, 0, 0, 1);

float gAmbientIntensity
<
	string UIName = "Ambient Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
> = 0.0f;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel
<
	string UIName = "Flip Green Channel";
	string UIWidget = "Bool";
> = true;

bool gUseTextureNormal
<
	string UIName = "Normal Mapping";
	string UIWidget = "Bool";
> = false;

Texture2D gTextureNormal
<
	string UIName = "Normal Texture";
	string UIWidget = "Texture";
	string ResourceName = "CobbleStone_NormalMap.dds";
>;

//ENVIRONMENT MAPPING
//*******************
TextureCube gCubeEnvironment
<
	string UIName = "Environment Cube";
	string ResourceType = "Cube";
	string ResourceName = "Sunol_Cubemap.dds";
>;

bool gUseEnvironmentMapping
<
	string UIName = "Environment Mapping";
	string UIWidget = "Bool";
> = false;

float gReflectionStrength
<
	string UIName = "Reflection Strength";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.1;
> = 0.8f;

float gRefractionStrength
<
	string UIName = "Refraction Strength";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
	float UIStep = 0.1;
> = 0.5f;

float gRefractionIndex
<
	string UIName = "Refraction Index";
> = 0.9f;

//FRESNEL FALLOFF
//***************
bool gUseFresnelFalloff
<
	string UIName = "Fresnel FallOff";
	string UIWidget = "Bool";
> = true;


float4 gColorFresnel
<
	string UIName = "Fresnel Color";
	string UIWidget = "Color";
> = float4(1, 1, 1, 1);

float gFresnelPower
<
	string UIName = "Fresnel Power";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 100;
	float UIStep = 0.1;
> = 2.0f;

float gFresnelMultiplier
<
	string UIName = "Fresnel Multiplier";
	string UIWidget = "slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.1;
> = 1.5f;

float gFresnelHardness
<
	string UIName = "Fresnel Hardness";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 100;
	float UIStep = 0.1;
> = 2.0f;

//OPACITY
//***************
float gOpacityIntensity
<
	string UIName = "Opacity Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
> = 1.0f;

bool gTextureOpacityIntensity
<
	string UIName = "Opacity Map";
	string UIWidget = "Bool";
> = false;

Texture2D gTextureOpacity
<
	string UIName = "Opacity Map";
	string UIWidget = "Texture";
	string ResourceName = "Specular_Level.tga";
>;


//SPECULAR MODELS
//***************
bool gUseSpecularBlinn
<
	string UIName = "Specular Blinn";
	string UIWidget = "Bool";
> = false;

bool gUseSpecularPhong
<
	string UIName = "Specular Phong";
	string UIWidget = "Bool";
> = true;

//VS IN & OUT
//***********
struct VS_Input
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD0;
};

struct VS_Output
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : COLOR0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD0;
};

float3 CalculateSpecularBlinn(float3 viewDirection, float3 normal)
{
    const float3 halfVector = normalize(viewDirection + gLightDirection);
    const float specularStrength = saturate(dot(halfVector, -normal));
    const float specularValue = pow(specularStrength, gShininess);
	
    return gColorSpecular * specularValue;
}

float3 CalculateSpecularPhong(float3 viewDirection, float3 normal)
{	
    const float specularStrength = saturate(dot(reflect(-gLightDirection, normal), viewDirection));
    const float specularValue = pow(specularStrength, gShininess);
	
    return gColorSpecular * specularValue;
}

float3 CalculateSpecular(float3 viewDirection, float3 normal, float2 texCoord)
{
    float3 specColor = float3(0, 0, 0);
	
    if (gUseSpecularBlinn)
        specColor += saturate(CalculateSpecularBlinn(viewDirection, normal));
		
    if (gUseSpecularPhong)
        specColor += saturate(CalculateSpecularPhong(viewDirection, normal));
		
    if (!gUseTextureSpecularIntensity)
        return specColor;
		
    return specColor * gTextureSpecularIntensity.Sample(gTextureSampler, texCoord).r;
}

float3 CalculateNormal(float3 tangent, float3 normal, float2 texCoord)
{
    if (!gUseTextureNormal)
        return normal;
	
    float3 binormal = normalize(cross(tangent, normal));
    if (gFlipGreenChannel)
        binormal = -binormal;
    float3x3 localAxis = float3x3(tangent, binormal, normal);
	
    float3 sampledNormal = gTextureNormal.Sample(gTextureSampler, texCoord);
    sampledNormal = sampledNormal * 2.f - float3(1.f, 1.f, 1.f); // move from 0_1 range into -1_1 range
	
    return normalize(mul(sampledNormal, localAxis));
}

float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
    if (!gUseTextureDiffuse)
        return gColorDiffuse;

    float3 diffuse = gTextureDiffuse.Sample(gTextureSampler, texCoord);
    diffuse.r *= gColorDiffuse.r;
    diffuse.g *= gColorDiffuse.g;
    diffuse.b *= gColorDiffuse.b;
		
	// basic shading
    normal = normalize(normal);
    float diffuseStrength = dot(normal, -gLightDirection);
    diffuseStrength = saturate(diffuseStrength);
	
	// Ambient
    float4 ambientColor = float4(gColorAmbient.xyz * gAmbientIntensity, 1.f);
	
    return (diffuse * diffuseStrength) + ambientColor;
}

float3 CalculateFresnelFalloff(float3 normal, float3 viewDirection, float3 environmentColor)
{
    if (!gUseFresnelFalloff)
        return environmentColor;
	
	// fresnel
    float fresnel = 1 - clamp(abs(dot(normal, viewDirection)), 0.0, 1.0);
    fresnel = pow(fresnel, gFresnelPower) * gFresnelMultiplier;
	
    float fresnelMask = 1 - clamp(dot(float3(0, -1, 0), normal), 0.0, 1.0);
    fresnelMask = pow(fresnelMask, gFresnelHardness);
	
    if (gUseEnvironmentMapping)
        return fresnel * fresnelMask * environmentColor;
	
    return fresnel * fresnelMask * gColorFresnel;
}

float3 CalculateEnvironment(float3 viewDirection, float3 normal)
{
    if (!gUseEnvironmentMapping)
        return float3(0, 0, 0);
	
    const float3 reflected = normalize(reflect(viewDirection, normal));
    return gCubeEnvironment.Sample(gTextureSampler, reflected);
}

float CalculateOpacity(float2 texCoord)
{
    return 1 - gTextureOpacity.Sample(gTextureSampler, texCoord).r * gOpacityIntensity * gTextureOpacityIntensity;
}

// The main vertex shader
VS_Output MainVS(VS_Input input)
{
	
    VS_Output output = (VS_Output) 0;
	
    output.Position = mul(float4(input.Position, 1.0), gMatrixWVP);
    output.WorldPosition = mul(float4(input.Position, 1.0), gMatrixWorld);
    output.Normal = mul(input.Normal, (float3x3) gMatrixWorld);
    output.Tangent = mul(input.Tangent, (float3x3) gMatrixWorld);
    output.TexCoord = input.TexCoord;
	
    return output;
}

// The main pixel shader
float4 MainPS(VS_Output input) : SV_TARGET
{
	// NORMALIZE
    input.Normal = normalize(input.Normal);
    input.Tangent = normalize(input.Tangent);
	
    float3 viewDirection = normalize(input.WorldPosition.xyz - gMatrixViewInverse[3].xyz);
	
	//NORMAL
    float3 newNormal = CalculateNormal(input.Tangent, input.Normal, input.TexCoord);
		
	//SPECULAR
    float3 specColor = CalculateSpecular(viewDirection, newNormal, input.TexCoord);
		
	//DIFFUSE
    float3 diffColor = CalculateDiffuse(newNormal, input.TexCoord);
		
	//AMBIENT
    float3 ambientColor = gColorAmbient * gAmbientIntensity;
		
	//ENVIRONMENT MAPPING
    float3 environmentColor = CalculateEnvironment(viewDirection, newNormal);
	
	//FRESNEL FALLOFF
    environmentColor = CalculateFresnelFalloff(newNormal, viewDirection, environmentColor);
		
	//FINAL COLOR CALCULATION
    float3 finalColor = diffColor + specColor + environmentColor + ambientColor;
	
	//OPACITY
    float opacity = CalculateOpacity(input.TexCoord);
	
    return float4(finalColor, opacity);
}

// Default Technique
technique10 WithAlphaBlending
{
    pass p0
    {
        SetRasterizerState(gRS_FrontCulling);
        SetBlendState(gBS_EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}

// Default Technique
technique10 WithoutAlphaBlending
{
    pass p0
    {
        SetRasterizerState(gRS_FrontCulling);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}

#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	
	//TODO_W8

	//Update Shadow Variables
	//1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	//	LightWVP = model_world * light_viewprojection
	//	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	XMFLOAT4X4 lightWVP;
	const auto shadowMapRenderer = ShadowMapRenderer::Get();
	const auto world = XMLoadFloat4x4(&pModel->GetTransform()->GetWorld());
	const auto lightVP = XMLoadFloat4x4(&shadowMapRenderer->GetLightVP());
	XMStoreFloat4x4(&lightWVP, XMMatrixMultiply(world, lightVP));

	//Update The Shader Variables
	SetVariable_Matrix(L"gWorldViewProj_Light", lightWVP);

	//2. Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", shadowMapRenderer->GetShadowMap());

	//3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	auto direction = sceneContext.pLights->GetDirectionalLight().direction;
	SetVariable_Vector(L"gLightDirection", direction);
}

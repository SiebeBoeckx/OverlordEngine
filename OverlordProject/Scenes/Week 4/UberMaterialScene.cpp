#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Materials/UberMaterial.h"

void UberMaterialScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	m_pMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	m_pMaterial->SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");
	m_pMaterial->SetSpecularTexture(L"Textures/Skulls_Heightmap.tga");
	m_pMaterial->SetNormalTexture(L"Textures/Skulls_Normalmap.tga");
	m_pMaterial->SetCubeMap(L"Textures/Sunol_Cubemap.dds");

	m_pSphere = new GameObject();
	auto* model = m_pSphere->AddComponent(new ModelComponent(L"Meshes/Sphere.ovm"));
	model->SetMaterial(m_pMaterial);
	AddChild(m_pSphere);

	m_SceneContext.pCamera->GetTransform()->Translate(0, 3, -5);
}

void UberMaterialScene::Update()
{
	const float yRotation = 45 * m_SceneContext.pGameTime->GetTotal();
	m_pSphere->GetTransform()->Rotate(0, yRotation, 0);
}

void UberMaterialScene::OnSceneActivated()
{
	if (DebugRenderer::IsEnabled())
		DebugRenderer::ToggleDebugRenderer();
}

void UberMaterialScene::OnSceneDeactivated()
{
	DebugRenderer::ToggleDebugRenderer();
}

void UberMaterialScene::OnGUI()
{
	m_pMaterial->DrawImGui();
}
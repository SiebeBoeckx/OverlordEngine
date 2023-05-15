#include "stdafx.h"
#include "SpikyScene.h"
#include "Materials/SpikyMaterial.h"

void SpikyScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	m_pMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	m_pSphere = new GameObject();
	auto* model = m_pSphere->AddComponent(new ModelComponent(L"Meshes/OctaSphere.ovm"));
	model->SetMaterial(m_pMaterial);
	AddChild(m_pSphere);

	m_SceneContext.pCamera->GetTransform()->Translate(0, 3, -5);
}

void SpikyScene::Update()
{
	const float yRotation = 45 * m_SceneContext.pGameTime->GetTotal();
	m_pSphere->GetTransform()->Rotate(0, yRotation, 0);
}

void SpikyScene::OnGUI()
{
	m_pMaterial->DrawImGui();
}


void SpikyScene::OnSceneActivated()
{
	if (DebugRenderer::IsEnabled())
		DebugRenderer::ToggleDebugRenderer();
}

void SpikyScene::OnSceneDeactivated()
{
	DebugRenderer::ToggleDebugRenderer();
}
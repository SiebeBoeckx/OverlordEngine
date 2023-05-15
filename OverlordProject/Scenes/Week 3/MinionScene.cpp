//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "MinionScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/TorusPrefab.h"


MinionScene::MinionScene() :
	GameScene(L"MinionScene"){}

void MinionScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	//m_SceneContext.settings.drawGrid = true;
	m_SceneContext.settings.enableOnGUI = true;

	//Logger::LogInfo(L"Welcome, humble Minion!");

	m_pSmallTorus = new TorusPrefab(6.f, 50, 1.4f, 5, XMFLOAT4{ Colors::Red });
	AddChild(m_pSmallTorus);

	m_pMediumTorus = new TorusPrefab(9.f, 50, 1.4f, 5, XMFLOAT4{ Colors::Blue });
	m_pSmallTorus->AddChild(m_pMediumTorus);

	m_pBigTorus = new TorusPrefab(12.f, 50, 1.4f, 5, XMFLOAT4{ Colors::Orange });
	m_pMediumTorus->AddChild(m_pBigTorus);
}

void MinionScene::Update()
{
	//Optional
	const float baseXRotation = 90 * m_SceneContext.pGameTime->GetTotal();
	const float baseYRotation = 70 * m_SceneContext.pGameTime->GetTotal();
	m_pSmallTorus->GetTransform()->Rotate(baseXRotation, baseYRotation, baseYRotation);
	m_pMediumTorus->GetTransform()->Rotate(0, baseXRotation * 0.23f, 0);
	m_pBigTorus->GetTransform()->Rotate(baseYRotation * 0.17f, 0, 0);
}

void MinionScene::Draw()
{
	//Optional
}

void MinionScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}

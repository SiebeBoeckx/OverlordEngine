#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/TorusPrefab.h"


PongScene::PongScene() :
	GameScene(L"PongScene") {}

void PongScene::Initialize()
{
	m_SceneContext.settings.clearColor = XMFLOAT4(0, 0, 0, 1);

	auto& physx = PxGetPhysics();
	const auto pBouncyMaterial = physx.createMaterial(.0f, .0f, 1.05f);
	const auto pSlipperyMaterial = physx.createMaterial(.0f, .0f, 0.f);
	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pSlipperyMaterial);



	//Ball
	m_pBall = new SpherePrefab(0.5f, 10, XMFLOAT4{Colors::Red});
	AddChild(m_pBall);
	m_pBall->GetTransform()->Translate(0.f, 0.f, 0.f);

	m_pSphereRigid = m_pBall->AddComponent(new RigidBodyComponent());
	m_pSphereRigid->SetConstraint(RigidBodyConstraint::TransY, true);
	m_pSphereRigid->SetDensity(0.f);
	m_pSphereRigid->AddCollider(PxSphereGeometry(0.25f), *pBouncyMaterial);

	RepositionBall();



	//Players
	const PxVec3 playerDim = PxVec3(1.f, 1.f, 4.f);
	const PxBoxGeometry playerGeom(playerDim / 2);

	//Player 1
	m_pCubeLeft = new CubePrefab(playerDim.x, playerDim.y, playerDim.z, XMFLOAT4{Colors::Gray});
	AddChild(m_pCubeLeft);
	m_pCubeLeft->GetTransform()->Translate(-14.f, 0.f, 0.f);

	RigidBodyComponent* pPlayer1Rigid = m_pCubeLeft->AddComponent(new RigidBodyComponent());
	pPlayer1Rigid->SetKinematic(true);
	pPlayer1Rigid->AddCollider(playerGeom, *pBouncyMaterial);

	//Player 2
	m_pCubeRight = new CubePrefab(playerDim.x, playerDim.y, playerDim.z, XMFLOAT4{ Colors::Gray });
	AddChild(m_pCubeRight);
	m_pCubeRight->GetTransform()->Translate(14.f, 0.f, 0.f);

	RigidBodyComponent* pPlayer2Rigid = m_pCubeRight->AddComponent(new RigidBodyComponent());
	pPlayer2Rigid->SetKinematic(true);
	pPlayer2Rigid->AddCollider(playerGeom, *pBouncyMaterial);



	// Level
	{
		// wall sizes
		const PxVec3 wallDim = PxVec3(50.f, 2.f, 2.f);
		const PxBoxGeometry wallGeom(wallDim / 2);

		// top wall
		GameObject* pWall = new CubePrefab(wallDim.x, wallDim.y, wallDim.z, { 1.f, 1.f, 1.f, 0.f });
		pWall->GetTransform()->Translate(0.f, 0.5f, 12.f);
		auto pWallRigid = pWall->AddComponent(new RigidBodyComponent(true));
		pWallRigid->AddCollider(wallGeom, *pBouncyMaterial);
		AddChild(pWall);

		// bottom wall
		pWall = new CubePrefab(wallDim.x, wallDim.y, wallDim.z, { 1.f, 1.f, 1.f, 0.f });
		pWall->GetTransform()->Translate(0.f, 0.5f, -12.f);
		pWallRigid = pWall->AddComponent(new RigidBodyComponent(true));
		pWallRigid->AddCollider(wallGeom, *pBouncyMaterial);
		AddChild(pWall);
	}



	//Camera
	FixedCamera* pCamera = new FixedCamera();
	pCamera->GetTransform()->Translate(0.f, 26.f, 0.f);
	pCamera->GetTransform()->Rotate(90.f, 0.f, 0.f);
	AddChild(pCamera);
	SetActiveCamera(pCamera->GetComponent<CameraComponent>());



	// input
	m_SceneContext.pInput->AddInputAction(InputAction(0, InputState::down, 'W'));
	m_SceneContext.pInput->AddInputAction(InputAction(1, InputState::down, 'S'));
	m_SceneContext.pInput->AddInputAction(InputAction(2, InputState::down, VK_UP));
	m_SceneContext.pInput->AddInputAction(InputAction(3, InputState::down, VK_DOWN));

	m_SceneContext.pInput->AddInputAction(InputAction(4, InputState::pressed, VK_SPACE));
}

void PongScene::Update()
{
	TransformComponent* pTransform = m_pCubeLeft->GetTransform();
	if (m_SceneContext.pInput->IsActionTriggered(0))
	{
		MovePlayer(pTransform, m_MoveSpeed);
	}
	if (m_SceneContext.pInput->IsActionTriggered(1))
	{
		MovePlayer(pTransform, -m_MoveSpeed);
	}

	pTransform = m_pCubeRight->GetTransform();
	if (m_SceneContext.pInput->IsActionTriggered(2))
	{
		MovePlayer(pTransform, m_MoveSpeed);
	}
	if (m_SceneContext.pInput->IsActionTriggered(3))
	{
		MovePlayer(pTransform, -m_MoveSpeed);
	}

	//reset if it passed a player
	if (m_pSphereRigid->GetTransform()->GetPosition().x > m_ResetDistance || m_pSphereRigid->GetTransform()->GetPosition().x < -m_ResetDistance)
	{
		RepositionBall();
	}

	//Start moving ball
	if (m_SceneContext.pInput->IsActionTriggered(4))
	{
		StartBall();
	}
}

void PongScene::OnGUI()
{
	ImGui::TextColored({ 0.f, 0.f, 1.f, 1.f }, "Input: W / S");
	ImGui::Spacing();
	ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Input: UP / DOWN");
}

void PongScene::OnSceneActivated()
{
	if (DebugRenderer::IsEnabled())
		DebugRenderer::ToggleDebugRenderer();
}

void PongScene::OnSceneDeactivated()
{
	DebugRenderer::ToggleDebugRenderer();
}

void PongScene::MovePlayer(TransformComponent* pTransform, float speed) const
{
	XMFLOAT3 position = pTransform->GetPosition();
	position.z += speed;
	position.z = PxClamp(position.z, -m_PlayerLimit, m_PlayerLimit);

	pTransform->Translate(position);
}

void PongScene::StartBall() const
{
	PxVec3 forceVector = PxVec3(float(((rand() % 2) * 2) - 1) * MathHelper::randF(5.f, 10.f),
									0.f,
									MathHelper::randF(-15.f, 15.f)).getNormalized();

	forceVector *= m_StartSpeed;
	
	m_pSphereRigid->AddForce(
		{
			forceVector.x,
			forceVector.y,
			forceVector.z
		},
		PxForceMode::eIMPULSE
	);
}

void PongScene::RepositionBall() const
{
	m_pSphereRigid->GetTransform()->Translate(0.f, 0.f, 0.f);
	m_pSphereRigid->ClearForce();
	m_pSphereRigid->ClearTorque();
}
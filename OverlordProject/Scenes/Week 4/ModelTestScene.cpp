#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"

ModelTestScene::ModelTestScene() :
	GameScene(L"ModelTestScene") {}

void ModelTestScene::Initialize()
{
	auto& physx = PxGetPhysics();
	const auto pBouncyMaterial = physx.createMaterial(.5f, .5f, 1.f);
	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	m_pChair = new GameObject();
	AddChild(m_pChair);

	//Create new material instance
	DiffuseMaterial* pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	//Get materialId
	//const UINT materialId = pMaterial->GetMaterialId();
	//Retrieve effect variable
	//auto pColorVariable = pMaterial->GetVariable(L"gColor");
	//Set variable
	pMaterial->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	const auto pModel = new ModelComponent(L"Meshes/Chair.ovm");
	pModel->SetMaterial(pMaterial);

	auto pRigidBody = new RigidBodyComponent();
	PxConvexMesh* pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");
	PxConvexMeshGeometry geometry = PxConvexMeshGeometry(pConvexMesh);
	pRigidBody->AddCollider(geometry, *pBouncyMaterial);

	m_pChair->AddComponent(pModel);
	m_pChair->AddComponent(pRigidBody);

	m_pChair->GetTransform()->Translate(0, 1, 0);
}
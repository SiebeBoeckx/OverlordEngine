#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/SpherePrefab.h"

ComponentTestScene::ComponentTestScene() :
	GameScene(L"ComponentTestScene") {}

void ComponentTestScene::Initialize()
{
	auto& physx = PxGetPhysics();
	const auto pBouncyMaterial = physx.createMaterial(.5f, .5f, 1.f);
	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//Red sphere (Group 0) & (ignore group 1 & 2)
	const auto pSphereRed = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Red });
	AddChild(pSphereRed);

	pSphereRed->GetTransform()->Translate(0.f, 30.f, 0.f);

	auto pRigidBody = pSphereRed->AddComponent(new RigidBodyComponent());
	pRigidBody->AddCollider(PxSphereGeometry(1), *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
	//pRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);

	//Green sphere
	const auto pSphereGreen = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Green });
	AddChild(pSphereGreen);

	pSphereGreen->GetTransform()->Translate(0.f, 20.f, 0.f);

	pRigidBody = pSphereGreen->AddComponent(new RigidBodyComponent());
	pRigidBody->AddCollider(PxSphereGeometry(1), *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group1);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);

	//Blue sphere
	const auto pSphereBlue = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Blue });
	AddChild(pSphereBlue);

	pSphereBlue->GetTransform()->Translate(0.f, 10.f, 0.f);

	pRigidBody = pSphereBlue->AddComponent(new RigidBodyComponent());
	pRigidBody->AddCollider(PxSphereGeometry(1), *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group2);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);
}
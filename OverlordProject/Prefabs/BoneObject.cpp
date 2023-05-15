#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* material, float length)
	:m_pMaterial(material)
	,m_Length(length)
{
}

void BoneObject::AddBone(BoneObject* bone)
{
	bone->GetTransform()->Translate(m_Length, 0, 0);
	this->AddChild(bone);
}

void BoneObject::CalculateBindPose()
{
	auto world = this->GetTransform()->GetWorld();

	XMMATRIX worldMatrix = XMLoadFloat4x4(&world);

	XMMATRIX invWorld = XMMatrixInverse(nullptr, worldMatrix);

	XMStoreFloat4x4(&m_BindPose, invWorld);

	for(auto& childBone : this->GetChildren<BoneObject>())
	{
		auto childWorld = childBone->GetTransform()->GetWorld();

		XMMATRIX childWorldMatrix = XMLoadFloat4x4(&childWorld);

		XMMATRIX childInvWorld = XMMatrixInverse(nullptr, childWorldMatrix);

		XMStoreFloat4x4(&childBone->m_BindPose, childInvWorld);
	}
}

void BoneObject::Initialize(const SceneContext&)
{
	GameObject* pEmpty = new GameObject();
	this->AddChild(pEmpty); //This, otherwise you add the gameObject to the scene

	ModelComponent* pModel = new ModelComponent(L"./Meshes/Bone.ovm");
	pEmpty->AddComponent(pModel);
	pModel->SetMaterial(m_pMaterial);
	pEmpty->GetTransform()->Rotate(0, -90, 0);
	pEmpty->GetTransform()->Scale(m_Length, m_Length, m_Length);
}

#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Prefabs/BoneObject.h"
#include "Materials/ColorMaterial.h"

void SoftwareSkinningScene_1::Initialize()
{
	ColorMaterial* pMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	GameObject* pRoot = new GameObject();
	AddChild(pRoot);

	m_pBone0 = new BoneObject(pMaterial, 15.f);
	pRoot->AddChild(m_pBone0);

	m_pBone1 = new BoneObject(pMaterial, 15.f);
	m_pBone0->AddBone(m_pBone1);

	m_SceneContext.settings.enableOnGUI = true;
}

void SoftwareSkinningScene_1::Update()
{
	if (m_AutoRotate)
	{
		float maxRotation = 45.f;

		m_BoneRotationZ += m_RotationSign * maxRotation * GetSceneContext().pGameTime->GetElapsed();

		if (abs(m_BoneRotationZ) >= maxRotation)
		{
			m_RotationSign *= -1;
		}

		m_pBone0->GetTransform()->Rotate(0, 0, m_BoneRotationZ);
		m_pBone1->GetTransform()->Rotate(0, 0, -m_BoneRotationZ * 2.f);
	}
	else
	{
		m_pBone0->GetTransform()->Rotate(m_AltBoneRotation0.x, m_AltBoneRotation0.y, m_AltBoneRotation0.z);
		m_pBone1->GetTransform()->Rotate(m_AltBoneRotation1.x, m_AltBoneRotation1.y, m_AltBoneRotation1.z);
	}

}

void SoftwareSkinningScene_1::OnGUI()
{
	ImGui::DragFloat3("Bone 0 - ROT", &m_AltBoneRotation0.x, 1.f, -360.f, 360.f);
	ImGui::DragFloat3("Bone 1 - ROT", &m_AltBoneRotation1.x, 1.f, -360.f, 360.f);
	ImGui::Checkbox("Auto Rotate", &m_AutoRotate);
}

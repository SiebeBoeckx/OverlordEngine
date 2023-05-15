#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//TODO_W6_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.duration);

		//2. 
		//IF m_Reversed is true
		if (m_Reversed)
		{
			//	Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if (m_TickCount < 0)
				m_TickCount += m_CurrentClip.duration;
		}
		//ELSE
		else
		{
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if (m_TickCount > m_CurrentClip.duration)
				m_TickCount -= m_CurrentClip.duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		for (auto& key : m_CurrentClip.keys)
		{
			//keyA > Closest Key with Tick before/smaller than m_TickCount
			if (key.tick < m_TickCount)
				keyA = key;

			//keyB > Closest Key with Tick after/bigger than m_TickCount
			else // if key.tick is bigger
			{
				keyB = key;
				break;
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float blendFactor = (keyB.tick - keyA.tick) / m_TickCount;
		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		for (int i = 0; i < keyA.boneTransforms.size(); ++i)
		{
			//	Retrieve the transform from keyA (transformA)
			auto transformA = keyA.boneTransforms[i];
			// 	Retrieve the transform from keyB (transformB)
			auto transformB = keyB.boneTransforms[i];
			//	Decompose both transforms
			XMVECTOR posA, rotA, scaleA;
			XMMatrixDecompose(&scaleA, &rotA, &posA, XMLoadFloat4x4(&transformA));
			XMVECTOR posB, rotB, scaleB;
			XMMatrixDecompose(&scaleB, &rotB, &posB, XMLoadFloat4x4(&transformB));
			//	Lerp between all the transformations (Position, Scale, Rotation)
			XMFLOAT4 origin = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			XMVECTOR orig = XMLoadFloat4(&origin);
			XMVECTOR lerpedPos = XMVectorLerp(posA, posB, blendFactor);
			XMVECTOR lerpedRot = XMQuaternionSlerp(rotA, rotB, blendFactor);
			XMVECTOR lerpedScale = XMVectorLerp(scaleA, scaleB, blendFactor);
			//	Compose a transformation matrix with the lerp-results
			XMFLOAT4X4 transformMat;
			XMStoreFloat4x4(&transformMat, XMMatrixTransformation(orig, lerpedRot, lerpedScale, orig, lerpedRot, lerpedPos));

			//	Add the resulting matrix to the m_Transforms vector
			m_Transforms.push_back(transformMat);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//TODO_W6_()
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (auto& animationClip : m_pMeshFilter->m_AnimationClips)
	{
		//If found,
		if (animationClip.name == clipName)
			//	Call SetAnimation(Animation Clip) with the found clip
			SetAnimation(animationClip);
		//Else
		else
		{
			//	Call Reset
			Reset();
			//	Log a warning with an appropriate message
			std::cout << "animation clip not found\n";
		}
	}
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//TODO_W6_()
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber < m_pMeshFilter->m_AnimationClips.size())
	{
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}

	//If not
	Reset();
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//TODO_W6_()
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;

	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//TODO_W6_()
	//If pause is true, set m_IsPlaying to false
	if (pause)
		m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;

	//If m_ClipSet is true
	if (m_ClipSet)
	{
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		const auto& boneTransforms = m_CurrentClip.keys[0].boneTransforms;
		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(boneTransforms.begin(), boneTransforms.end());
	}
	//Else
	else
	{
		//	Create an IdentityMatrix
		XMFLOAT4X4 idMatrix;
		XMStoreFloat4x4(&idMatrix, XMMatrixIdentity());
		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms.assign(m_Transforms.size(), idMatrix);
	}
}

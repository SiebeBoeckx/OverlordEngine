#include "stdafx.h"
#include "GeoCharacter.h"
#include "Prefabs/CubePrefab.h"

GeoCharacter::GeoCharacter(const CharacterDesc& characterDesc)
	:Character(characterDesc)
	,m_CharacterDesc{ characterDesc }
	,m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime)
	,m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void GeoCharacter::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	pCamera->GetTransform()->Translate(20.f, 0.f, 0.f);
	pCamera->GetTransform()->Rotate(0.f, -90.f, -90.f);

	//tempbox

	const auto xCube = AddChild(new CubePrefab(1,1,1));
	xCube->GetTransform()->Translate(1, 0, 0);

	const auto yCube = AddChild(new CubePrefab(1, 1, 1, static_cast<XMFLOAT4>(Colors::Green)));
	yCube->GetTransform()->Translate(0, 1, 0);

	const auto zCube = AddChild(new CubePrefab(1, 1, 1, static_cast<XMFLOAT4>(Colors::Blue)));
	zCube->GetTransform()->Translate(0, 0, 1);
}

void GeoCharacter::Update(const SceneContext& /*sceneContext*/)
{
	if (m_pCameraComponent->IsActive())
	{
		////Rotate gameObject with gravVector
		////Retrieve the TransformComponent
		//
		//m_GravVector.x = m_pControllerComponent->GetFootPosition().x - m_GravPoint.x;
		//m_GravVector.y = m_pControllerComponent->GetFootPosition().y - m_GravPoint.y;
		//m_GravVector.z = m_pControllerComponent->GetFootPosition().z - m_GravPoint.z;
		//
		//if (m_pControllerComponent->GetTransform()->GetUp().x != m_GravVector.x
		//	|| m_pControllerComponent->GetTransform()->GetUp().y != m_GravVector.y
		//	|| m_pControllerComponent->GetTransform()->GetUp().z != m_GravVector.z) //Since XMFLOAT3 != XMFLOAT3 doesn't work
		//{
		//
		//	XMVECTOR tempGravVec = XMLoadFloat3(&m_GravVector);
		//	tempGravVec = DirectX::XMVector3Normalize(tempGravVec);
		//	XMStoreFloat3(&m_GravVector, tempGravVec);
		//
		//	m_pControllerComponent->SetUpVector(m_GravVector);
		//
		//	//m_pCameraComponent->GetTransform()->Rotate(	-m_pControllerComponent->GetTransform()->GetRight().x,
		//	//											-m_pControllerComponent->GetTransform()->GetRight().y,
		//	//											-m_pControllerComponent->GetTransform()->GetRight().z);
		//	//
		//	std::cout	<< m_pCameraComponent->GetTransform()->GetForward().x << ','
		//				<< m_pCameraComponent->GetTransform()->GetForward().y << ','
		//				<< m_pCameraComponent->GetTransform()->GetForward().z << '\n';
		//
		//
		//}

		// Rotate the object to align with the gravity vector
		XMVECTOR gravVector = XMLoadFloat3(&m_GravVector);
		gravVector = XMVector3Normalize(gravVector);
		m_pControllerComponent->SetUpVector(m_GravVector);

		// Calculate an orthonormal basis from the gravity vector
		XMVECTOR up = gravVector;
		//XMVECTOR right = XMVector3Orthogonal(gravVector, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		//if (XMVector3Equal(right, XMVectorZero()))
		//{
		//	right = XMVector3Orthogonal(gravVector, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
		//}
		//right = XMVector3Normalize(right);
		//XMVECTOR forward = XMVector3Cross(up, right);

		//***************
		//HANDLE INPUT

		//No optionals, so not used
		constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero

		auto pInput = GetScene()->GetSceneContext().pInput;

		//## Input Gathering (move)
		XMFLOAT2 move{0.f,0.f}; //Uncomment
		bool isMoving{ false };

		auto leftThumbstick = pInput->GetThumbstickPosition();
		auto rightThumbstick = pInput->GetThumbstickPosition(false);
		//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)
		if (pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
		{
			move.y = 1;
			isMoving = true;
		}
		if (pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
		{
			move.y = -1;
			isMoving = true;
		}
		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if(abs(leftThumbstick.y) > epsilon)
		{
			move.y = leftThumbstick.y;
			isMoving = true;
		}

		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)
		if (pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		{
			move.x = 1;
			isMoving = true;
		}
		if (pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		{
			move.x = -1;
			isMoving = true;
		}
		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (abs(leftThumbstick.x) > epsilon)
		{
			move.x = leftThumbstick.x;
			isMoving = true;
		}

		//************************
		//GATHERING TRANSFORM INFO

		auto deltaT = GetScene()->GetSceneContext().pGameTime->GetElapsed();

		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent
		XMVECTOR forward = XMLoadFloat3(&m_pControllerComponent->GetTransform()->GetForward());
		XMVECTOR right = XMLoadFloat3(&m_pControllerComponent->GetTransform()->GetUp());
		right *= -1;

		//********
		//MOVEMENT

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		auto currentMoveAcc = m_MoveAcceleration * deltaT;
		//If the character is moving (= input is pressed)
		// Calculate the movement direction based on the input and the new basis
		XMVECTOR moveDirection = forward * move.y + right * move.x;
		moveDirection = XMVector3Normalize(moveDirection);
		if (isMoving)
		{
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			moveDirection += forward * move.y;
			moveDirection += right * move.x;

			XMStoreFloat3(&m_CurrentDirection, moveDirection);
			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed += currentMoveAcc;
			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
			if (m_MoveSpeed > m_CharacterDesc.maxMoveSpeed)
			{
				m_MoveSpeed = m_CharacterDesc.maxMoveSpeed;
			}
		}
		//Else (character is not moving, or stopped moving)
		else
		{
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed -= currentMoveAcc;
			//Make sure the current MoveSpeed doesn't get smaller than zero
			if (m_MoveSpeed < 0)
			{
				m_MoveSpeed = 0;
			}
		}

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		auto horizontalVelocity = XMLoadFloat3(&m_CurrentDirection) * m_MoveSpeed;
		XMFLOAT3 horVel{};
		XMStoreFloat3(&horVel, horizontalVelocity);
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		m_TotalVelocity.x = horVel.x;
		m_TotalVelocity.z = horVel.z;
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)

		//## Vertical Movement (Jump/Fall)
		//If the Controller Component is NOT grounded (= freefall)
		if (m_pControllerComponent->GetCollisionFlags() != PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			//XMVECTOR up = XMLoadFloat3(&m_pControllerComponent->GetTransform()->GetRight());
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			XMVECTOR tempTotalVel = XMLoadFloat3(&m_TotalVelocity);
			tempTotalVel -= up * m_FallAcceleration * deltaT;
			XMStoreFloat3(&m_TotalVelocity, tempTotalVel);
			//moveDirection += up * m_TotalVelocity.y;
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			if (m_TotalVelocity.y < -m_CharacterDesc.maxFallSpeed)
			{
				m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
			}
		}
		//Else If the jump action is triggered
		else if (pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
		}
		//Else (=Character is grounded, no input pressed)
		else
		{
			//m_TotalVelocity.y is zero
			m_TotalVelocity.y = -0.1f;
		}

		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		auto vel = XMLoadFloat3(&m_TotalVelocity);
		vel *= deltaT; //velocity (m/s) * elapsedTime(s) = displacement (m)
		XMFLOAT3 displacement{};
		XMStoreFloat3(&displacement, vel);
		//Calculate the displacement (m) for the current frame and move the ControllerComponent
		m_pControllerComponent->Move(displacement);

		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)
	}
}
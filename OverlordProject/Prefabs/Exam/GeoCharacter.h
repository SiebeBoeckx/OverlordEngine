#pragma once
#include "Prefabs/Character.h"

class GeoCharacter : public Character
{
public:
	GeoCharacter(const CharacterDesc& characterDesc);
	~GeoCharacter() override = default;

	GeoCharacter(const GeoCharacter& other) = delete;
	GeoCharacter(GeoCharacter&& other) noexcept = delete;
	GeoCharacter& operator=(const GeoCharacter& other) = delete;
	GeoCharacter& operator=(GeoCharacter&& other) noexcept = delete;

	void SetGravPoint(XMFLOAT3 point) { m_GravPoint = point; };

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};
	
	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)
	
	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)

	XMFLOAT3 m_GravPoint{};
	XMFLOAT3 m_GravVector{};
};
#pragma once

class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	GameObject* m_pBall{};
	RigidBodyComponent* m_pSphereRigid{};

	GameObject* m_pCubeLeft{};
	GameObject* m_pCubeRight{};

	float m_MoveSpeed{ 0.15f };
	float m_PlayerLimit{ 9.f };

	float m_ResetDistance{ 20.f };
	float m_StartSpeed{ 20.f };

	void MovePlayer(TransformComponent* pTransform, float speed) const;
	void RepositionBall() const;
	void StartBall() const;
};
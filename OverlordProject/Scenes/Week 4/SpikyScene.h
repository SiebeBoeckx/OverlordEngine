#pragma once

class SpikyMaterial;

class SpikyScene final : public GameScene
{
public:
	SpikyScene() : GameScene(L"SpikyScene") {}
	~SpikyScene() override = default;
	SpikyScene(const SpikyScene&) = delete;
	SpikyScene(SpikyScene&&) noexcept = delete;
	SpikyScene& operator=(const SpikyScene&) = delete;
	SpikyScene& operator=(SpikyScene&&) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
	void OnGUI() override;

private:
	SpikyMaterial* m_pMaterial = nullptr;
	GameObject* m_pSphere = nullptr;
};
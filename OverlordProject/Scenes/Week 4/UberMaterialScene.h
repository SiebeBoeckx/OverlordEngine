#pragma once
class UberMaterial;

class UberMaterialScene final : public GameScene
{
public:
	UberMaterialScene() : GameScene(L"UberMaterialScene") {}
	~UberMaterialScene() override = default;
	UberMaterialScene(const UberMaterialScene&) = delete;
	UberMaterialScene(UberMaterialScene&&) noexcept = delete;
	UberMaterialScene& operator=(const UberMaterialScene&) = delete;
	UberMaterialScene& operator=(UberMaterialScene&&) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
	void OnGUI() override;

private:
	UberMaterial* m_pMaterial = nullptr;
	GameObject* m_pSphere = nullptr;
};
#pragma once
class UberMaterial final : public Material<UberMaterial>
{
public:
	UberMaterial();
	~UberMaterial() = default;
	UberMaterial(const UberMaterial&) = delete;
	UberMaterial(UberMaterial&&) noexcept = delete;
	UberMaterial& operator=(const UberMaterial&) = delete;
	UberMaterial& operator=(UberMaterial&&) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetSpecularTexture(const std::wstring& assetFile);
	void SetNormalTexture(const std::wstring& assetFile);
	void SetCubeMap(const std::wstring& assetFile);
	void SetOpacityMap(const std::wstring& assetFile);

protected:
	virtual void InitializeEffectVariables() override;

private:
	TextureData* m_pDiffuseTexture = nullptr;
	TextureData* m_pSpecularTexture = nullptr;
	TextureData* m_pNormalTexture = nullptr;
	TextureData* m_pCubeMap = nullptr;
	TextureData* m_pOpacityMap = nullptr;
};
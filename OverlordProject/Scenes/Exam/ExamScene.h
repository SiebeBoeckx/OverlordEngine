#pragma once
class PlayerCharacter;

class ExamScene : public GameScene
{
public:
	ExamScene() :GameScene(L"ExamScene") {}
	~ExamScene() override = default;
	ExamScene(const ExamScene& other) = delete;
	ExamScene(ExamScene&& other) noexcept = delete;
	ExamScene& operator=(const ExamScene& other) = delete;
	ExamScene& operator=(ExamScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;

private:
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
	};

	PlayerCharacter* m_pCharacter{};
};


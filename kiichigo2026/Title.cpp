# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene(init)
{
}

void Title::update()
{
	bool triggered = false;

	if (m_modeRectL.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
		if (MouseL.down())
		{
			getData().GameMode = (getData().GameMode + 1) % getData().GameModeList.size();
			triggered = true;
		}
	}
	else if (m_modeRectR.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
		if (MouseL.down())
		{
			getData().GameMode = (getData().GameMode - 1 + getData().GameModeList.size()) % getData().GameModeList.size();
			triggered = true;
		}
	}

	if (not triggered and MouseL.down())
	{
		changeScene(U"Game", 1.0);

		getData().seExtend.playOneShot(0.5);
	}

	getData().m_effects.update();
}

void Title::draw() const
{
	FontAsset(U"Title")(GameInfo::GameTitle).drawAt(Scene::Center(), Palette::White);
	FontAsset(U"font")(U"クリックでスタート").drawAt(24, Vec2{ Scene::Width() / 2, Scene::Height() - 100 }, Palette::White);

	FontAsset(U"font")(getData().GameModeList[getData().GameMode]).drawAt(24, Vec2{Scene::Width() / 2, 100}, Palette::White);

	Triangle{ Scene::Width() / 2 - 100.0, 100, 30, 30_deg }.draw(Palette::White);
	Triangle{ Scene::Width() / 2 + 100.0, 100, 30, -30_deg }.draw(Palette::White);

	getData().m_effects.draw();
}

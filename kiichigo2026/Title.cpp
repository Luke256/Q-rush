# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene(init)
{
}

void Title::update()
{
	if (MouseL.down())
	{
		changeScene(U"Game", 1.0);

		AudioAsset(U"extend").playOneShot(0.5);
	}
}

void Title::draw() const
{
	FontAsset(U"Title")(GameInfo::GameTitle).drawAt(Scene::Center(), Palette::White);
	FontAsset(U"font")(U"クリックでスタート").drawAt(24, Vec2{ Scene::Width() / 2, 500 }, Palette::White);
}

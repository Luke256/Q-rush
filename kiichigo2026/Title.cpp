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

		getData().seExtend.playOneShot(0.5);
	}
}

void Title::draw() const
{
	FontAsset(U"Title")(GameInfo::GameTitle).drawAt(Scene::Center(), Palette::White);
	FontAsset(U"font")(U"クリックでスタート").drawAt(24, Vec2{ Scene::Width() / 2, Scene::Height() - 100 }, Palette::White);
}

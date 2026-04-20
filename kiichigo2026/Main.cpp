# include "Main.hpp"
# include "Title.hpp"
# include "Game.hpp"

void Main()
{
	Window::SetTitle(GameInfo::GameTitle);
	Scene::SetBackground(Palette::Lightcoral);

	LicenseManager::AddLicense({
		.title = U"BGMer",
		.copyright = U"(C) BGMer",
		.text = U"https://bgmer.net/terms",
	});

	LicenseManager::AddLicense({
		.title = U"OtoLogic",
		.copyright = U"(C) BGMer",
		.text = U"https://otologic.jp/free/license.html"
	});

	MyApp manager;
	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");

	manager.init(U"Title", 0s);

	FontAsset::Register(U"Title", FontMethod::MSDF, 60, Typeface::Bold);
	FontAsset::Register(U"font", FontMethod::MSDF, 60, Typeface::Bold);

#ifndef SIV3D_WEB
	AudioAsset::Register(U"bgm", Audio::Stream, Resource(U"data/237_BPM152.mp3"));
	AudioAsset::Register(U"slash", Resource(U"data/Motion-Swish07-1.mp3"));
	AudioAsset::Register(U"unslash", Resource(U"data/Motion-Swish07-4.mp3"));
	AudioAsset::Register(U"miss", Resource(U"data/Hit-Punch01-2.mp3"));
	AudioAsset::Register(U"ok", Resource(U"data/Inspiration03-1(High).mp3"));
	AudioAsset::Register(U"extend", Resource(U"data/Single_Accent13-1(Dry).mp3"));
#endif

	while (System::Update())
	{
		if (!manager.update()) break;
	}
}

# include "Main.hpp"
# include "Title.hpp"
# include "Game.hpp"

void Main()
{
	Window::SetTitle(GameInfo::GameTitle);
	Scene::SetBackground(Palette::Lightcoral);
	Window::SetStyle(WindowStyle::Sizable);

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

#ifdef SIV3D_WEB
	Array<AsyncTask<Wave>> tasks;
	tasks << s3d::Platform::Web::AudioDecoder::DecodeFromFile(U"data/237_BPM152.mp3");
	tasks << s3d::Platform::Web::AudioDecoder::DecodeFromFile(U"data/Motion-Swish07-1.mp3");
	tasks << s3d::Platform::Web::AudioDecoder::DecodeFromFile(U"data/Motion-Swish07-4.mp3");
	tasks << s3d::Platform::Web::AudioDecoder::DecodeFromFile(U"data/Hit-Punch01-2.mp3");
	tasks << s3d::Platform::Web::AudioDecoder::DecodeFromFile(U"data/Inspiration03-1(High).mp3");
	tasks << s3d::Platform::Web::AudioDecoder::DecodeFromFile(U"data/Single_Accent13-1(Dry).mp3");

	while (System::Update())
	{
		int32 readyCount = tasks.count_if([](const auto& task) { return task.isReady(); });

		FontAsset(U"Title")(U"Loading... {}%"_fmt(readyCount * 100 / tasks.size())).drawAt(32, Scene::Center(), Palette::White);

		if (readyCount == tasks.size())
		{
			manager.get()->bgm = Audio(tasks[0].get());
			manager.get()->seSlash = Audio(tasks[1].get());
			manager.get()->seUnslash = Audio(tasks[2].get());
			manager.get()->seMiss = Audio(tasks[3].get());
			manager.get()->seOk = Audio(tasks[4].get());
			manager.get()->seExtend = Audio(tasks[5].get());
			break;
		}

	}
#else
	manager.get()->bgm = Audio(Audio::Stream, Resource(U"data/237_BPM152.mp3"));
	manager.get()->seSlash = Audio(Resource(U"data/Motion-Swish07-1.mp3"));
	manager.get()->seUnslash = Audio(Resource(U"data/Motion-Swish07-4.mp3"));
	manager.get()->seMiss = Audio(Resource(U"data/Hit-Punch01-2.mp3"));
	manager.get()->seOk = Audio(Resource(U"data/Inspiration03-1(High).mp3"));
	manager.get()->seExtend = Audio(Resource(U"data/Single_Accent13-1(Dry).mp3"));
#endif

	while (System::Update())
	{
		if (!manager.update()) break;
	}
}

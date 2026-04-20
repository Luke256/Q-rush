# pragma once
# include <Siv3D.hpp>

//# define SIV3D_WEB

struct GameData
{
	int32 score;
	Audio bgm;
	Audio seSlash, seUnslash, seMiss, seOk, seExtend;
};

using MyApp = SceneManager<String, GameData>;

namespace GameInfo
{
	const String GameTitle = U"Q-rush";
}

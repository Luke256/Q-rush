# pragma once
# include "MyEffect.hpp"
# include <Siv3D.hpp>

//# define SIV3D_WEB

struct GameData
{
	const Array<String> GameModeList{ U"ノーマル", U"エキスパート" };
	int32 score = 0;
	Audio bgm;
	Audio seSlash, seUnslash, seMiss, seOk, seExtend;
	MyEffectManager m_effects;
	int32 GameMode = 0;
};

using MyApp = SceneManager<String, GameData>;

namespace GameInfo
{
	const String GameTitle = U"Q-rush";
}

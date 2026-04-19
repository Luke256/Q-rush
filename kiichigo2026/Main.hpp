# pragma once
# include <Siv3D.hpp>

struct GameData
{
	int32 score;
};

using MyApp = SceneManager<String, GameData>;

namespace GameInfo
{
	const String GameTitle = U"Q-rush";
}

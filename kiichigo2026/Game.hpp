# pragma once

# include "Main.hpp"

struct GameConfig;

struct LevelData
{
	Array<int32> queue, groups;
	int32 queTotal = 0;

	LevelData() = default;
	LevelData(int32 difficulty, const GameConfig& config);
};

struct QueItem
{
	Vec2 pos, targetPos;
	int32 distToNext; // 一緒の場合、次のアイテムまでの距離

	void update()
	{
		pos += (targetPos - pos) * 0.1;
	}

	void draw() const
	{
		Circle{ pos, 16 }.draw(Palette::Khaki);
		Line{ pos, pos + Vec2{distToNext, 0} }.draw(8, Palette::Khaki);
	}
};

struct SplitItem
{
	Vec2 pos, targetPos;
	double width = 0.0;
	bool active = false;

	RectF getRect() const
	{
		return RectF{ pos - Vec2{ width / 2, 80 }, Vec2{ width, 160 }};
	}

	void update()
	{
		pos += (targetPos - pos) * 0.1;
	}

	void draw() const
	{
		const Vec2 offset{ 0, 40 };
		const Triangle triangle{ pos + offset, 16 };
		const Line splitLine{ pos - offset, pos + offset };

		Color color;
		if (active)
		{
			color = Palette::Maroon;
		}
		else if (getRect().mouseOver())
		{
			color = Palette::Royalblue;
		}
		else
		{
			color = Palette::Lightblue;
		}
		splitLine.draw(LineStyle::SquareDot, 4, color);
		triangle.draw(color);
	}
};

struct GroupItem
{
	Vec2 pos;
	double width = 0.0;
	double alpha = 0.0;
	int32 amount = 0;

	void udate()
	{
		alpha += (1.0 - alpha) * 0.1;
	}

	void draw() const
	{
		RectF{
			Arg::center(pos),
			Vec2{ width, 80 }
		}.drawFrame(0, 4, ColorF{ 1.0, alpha });
		FontAsset(U"font")(U"{}"_fmt(amount))
			.drawAt(pos, HSV{ 60 + 300 * Math::Exp(-0.3 * amount), 0.3, 1, alpha });
	}
};

struct GameConfig
{
	Array<int32> GroupSizeList;
	Array<int32> LevelStep;
	Array<int32> MaxGroupsList;
	Array<int32> QueSizeList;
	Array<bool> ShuffleList;
};

class Game : public MyApp::Scene
{
	enum Phase
	{
		InGame,
		Result,
	};

	void nextLevel();
	void refreshLevel();
	bool isCleared() const;
	void gameOver();

	void inGameUpdate();
	void resultUpdate();

	void inGameDraw() const;
	void resultDraw() const;
public:
	Game(const InitData& init);
	void update() override;
	void draw() const override;
private:
	const double TimeLimit = 8.0;
	const HashTable<String, GameConfig> gameConfigs = {
		{
			U"ノーマル",
			GameConfig {
				.GroupSizeList	{ 2, 3, 3,	3,	4,	4,	5,	5,	5 },
				.LevelStep		{ 3, 6, 10, 15, 25, 35, 45, 55, 70, 85 },
				.MaxGroupsList	{ 2, 3, 5,	10, 20, 30, 40, 50, 60 },
				.QueSizeList	{ 5, 7, 7,	9,	9, 12, 12, 15,	17 },
				.ShuffleList	{ false, false, true, true, true, true, true },
			}
		},
		{
			U"エキスパート",
			GameConfig {
				.GroupSizeList	{ 2, 3, 3,	3,	4,	4,	5,	5,	5 },
				.LevelStep		{ 5, 10, 15, 25, 30, 40, 50, 60, 70, 80 },
				.MaxGroupsList	{ 3, 10, 10, 10, 10, 20, 30, 40, 50, 60 },
				.QueSizeList	{ 5, 7, 10, 13, 14, 15, 16, 17, 18 },
				.ShuffleList	{ true, true, true, true, true, true, true },
			}
		},
	};

	String m_gameMode;
	Array<int32> levelStep;
	LevelData m_levelData;
	int32 m_difficulty = 0;
	double m_timeLimit = 0.0;
	Array<QueItem> m_queItems;
	Array<SplitItem> m_splitItems;
	Array<GroupItem> m_groupItems;
	Phase GamePhase = InGame;

	double m_resultTimer = 0.0;
	double m_bgEffectInterval = 0.5;
	double m_bgEffectTimer = 0.0;
};

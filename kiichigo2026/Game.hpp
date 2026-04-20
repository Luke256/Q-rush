# pragma once

# include "Main.hpp"
# include "MyEffect.hpp"

struct LevelData
{
	Array<int32> queue, groups;
	int32 queTotal = 0;

	LevelData() = default;
	LevelData(int32 difficulty);
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
	bool active = false;

	RectF getRect() const
	{
		return RectF{ pos - Vec2{ 16, 50 }, Vec2{ 32, 100 }};
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
	const Array<int32> levelStep{ 5, 10, 15, 25, 35, 45, 60, 75, 90, 100 };
	const double TimeLimit = 8.0;

	LevelData m_levelData;
	int32 m_difficulty = 0;
	double m_timeLimit = 0.0;
	Array<QueItem> m_queItems;
	Array<SplitItem> m_splitItems;
	Array<GroupItem> m_groupItems;
	Phase GamePhase = InGame;
	MyEffectManager m_effects;

	double m_resultTimer = 0.0;
	double m_bgEffectInterval = 0.5;
	double m_bgEffectTimer = 0.0;
};

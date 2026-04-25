# include "Game.hpp"

Game::Game(const InitData& init)
  : IScene(init),
	m_gameMode(getData().GameModeList[getData().GameMode]),
	levelStep(gameConfigs.at(m_gameMode).LevelStep)
{
	refreshLevel();
	getData().score = 0;
	getData().bgm.setLoop(true);
	getData().bgm.setVolume(0.25);
	getData().bgm.play();
}

void Game::update()
{
	if (GamePhase == InGame)
	{
		inGameUpdate();
	}
	if (GamePhase == Result)
	{
		resultUpdate();
	}
	getData().m_effects.update();
}

void Game::draw() const
{
	if (GamePhase == InGame)
	{
		inGameDraw();
	}
	if (GamePhase == Result)
	{
		resultDraw();
	}
	getData().m_effects.draw();
}

void Game::nextLevel()
{
	getData().seOk.playOneShot(0.7);

	getData().score += 1;
	if (levelStep[Min(m_difficulty, (int32)levelStep.size() - 1)] == getData().score)
	{
		m_difficulty++;
		m_bgEffectInterval *= 0.8;
		getData().seExtend.playOneShot(0.7);
	}

	for (const auto& item : m_queItems)
	{
		getData().m_effects.add<SparkEffect>(item.pos);
	}

	refreshLevel();
}

void Game::refreshLevel()
{
	m_timeLimit = TimeLimit;

	m_levelData = LevelData(m_difficulty, gameConfigs.at(m_gameMode));

	int32 margin = Min(80, (Scene::Width() - 100) / (m_levelData.queTotal - 1));
	int32 idx = 0;

	m_queItems.clear();
	m_splitItems.clear();

	const int32 QueLineUpY = Scene::Height() / 2 - 100;
	const int32 GroupLineUpY = Scene::Height() / 2 + 100;

	for (auto x : m_levelData.queue)
	{
		for (auto i : step(x))
		{
			m_queItems << QueItem{
				Vec2{ Scene::Width() * 2, QueLineUpY },
				Vec2{ (Scene::Width() / 2) + (idx - (m_levelData.queTotal - 1.0) / 2.0) * margin, QueLineUpY},
				margin,
			};

			idx++;
		}

		m_queItems.back().distToNext = 0;

		m_splitItems << SplitItem{
			Vec2{ Scene::Width() * 2, QueLineUpY },
			Vec2{ (Scene::Width() / 2) + (idx - (m_levelData.queTotal - 1.0) / 2.0) * margin - margin / 2, QueLineUpY},
		};
	}
	m_splitItems.pop_back();

	m_groupItems.clear();
	double groupWidth = Min(120, (Scene::Width() - 100) / (int32)m_levelData.groups.size());
	for (auto [i, g] : Indexed(m_levelData.groups))
	{
		m_groupItems << GroupItem{
			Vec2{
				Scene::Width() / 2 - groupWidth * m_levelData.groups.size() / 2 + groupWidth * i + groupWidth / 2,
				GroupLineUpY,
			},
			groupWidth * 0.9,
			0.0,
			g
		};
	}
}

LevelData::LevelData(int32 difficulty, const GameConfig& config)
{
	const Array<int32> &queSizeList = config.QueSizeList;
	const Array<int32> &groupSizeList = config.GroupSizeList;
	const Array<int32> &maxGroupsList = config.MaxGroupsList;
	const Array<bool> &shuffleList = config.ShuffleList;

	int32 queSize = queSizeList[Min(difficulty, (int32)queSizeList.size() - 1)];
	int32 groupSize = groupSizeList[Min(difficulty, (int32)groupSizeList.size() - 1)];
	int32 maxGroups = maxGroupsList[Min(difficulty, (int32)maxGroupsList.size() - 1)];
	bool shuffle = shuffleList[Min(difficulty, (int32)shuffleList.size() - 1)]; // グループをシャッフルするかどうか

	// 総和が queSize になるように長さ 2 以上のランダムな整数列を生成して、queue とする
	// 数列の各項は、40%で1、30%で2、20%で3、10%で4とする
	DiscreteDistribution dist{ 40, 30, 20, 10 };

	while (true)
	{
		queue.clear();
		queTotal = 0;

		while (queTotal < queSize and queue.size() < maxGroups)
		{
			int32 val = static_cast<int32>(dist(GetDefaultRNG())) + 1;
			queue.push_back(val);
			queTotal += val;
		}

		if (queue.size() == 1)
		{
			continue;
		}
		break;
	}

	// queue を groupSize 個のグループに分割して、groups とする
	Array<int32> idxList(queue.size() - 1);
	for (int32 i = 0; i < idxList.size(); ++i)
	{
		idxList[i] = i + 1;
	}

	Array<int32> sep = Sample<Array<int32>>(groupSize - 1, idxList);
	groups = Array<int32>(1);
	for (auto i : step(queue.size()))
	{
		if (sep.size() > 0 and sep.front() == i)
		{
			groups.push_back(0);
			sep.pop_front();
		}
		groups.back() += queue[i];
	}

	if (shuffle)
	{
		Shuffle(groups);
	}
}

bool Game::isCleared() const
{
	Array<int32> groups(1);
	for (auto i : step(m_levelData.queue.size() - 1))
	{
		groups.back() += m_levelData.queue[i];
		if (m_splitItems[i].active)
		{
			groups.push_back(0);
		}
	}
	groups.back() += m_levelData.queue.back();

	return groups.sorted() == m_levelData.groups.sorted();
}

void Game::inGameUpdate()
{
	m_timeLimit -= Scene::DeltaTime();
	m_bgEffectTimer += Scene::DeltaTime();

	if (m_bgEffectTimer > m_bgEffectInterval)
	{
		m_bgEffectTimer = 0.0;
		getData().m_effects.add<BubbleEffect>();
	}

	for (auto& item : m_queItems)
	{
		item.update();
	}
	for (auto& item : m_groupItems)
	{
		item.udate();
	}

	bool triggered = false;

	for (auto& item : m_splitItems)
	{
		item.update();
		if (item.getRect().mouseOver())
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		if (item.getRect().leftClicked())
		{
			item.active = !item.active;
			triggered = true;

			if (item.active)
			{
				getData().m_effects.add<SlashEffect>(item.pos + Vec2{ 0, 60 }, 120);
				getData().seSlash.playOneShot(0.4, (item.pos.x - Scene::Width() / 2) / Scene::Width() * 2);
			}
			else
			{
				getData().seUnslash.playOneShot(0.4, (item.pos.x - Scene::Width() / 2) / Scene::Width() * 2);
			}
		}
	}

	if (triggered and isCleared())
	{
		nextLevel();
	}

	if (m_timeLimit <= 0.0)
	{
		gameOver();
	}
}

void Game::inGameDraw() const
{

	FontAsset(U"font")(U"ステージ {}"_fmt(getData().score)).draw(24, Vec2{ 20, 20 }, Palette::White);
	FontAsset(U"font")(U"難易度　 {}"_fmt(m_difficulty + 1)).draw(24, Vec2{ 20, 50 }, Palette::White);

	for (const auto& item : m_queItems)
	{
		item.draw();
	}
	for (const auto& item : m_groupItems)
	{
		item.draw();
	}

	for (const auto& item : m_splitItems)
	{
		item.draw();
	}

	const double TimerWidth = 250;
	const double timeT = Max(0.0, m_timeLimit / TimeLimit);
	const Color timerColor = (m_timeLimit < 2.0) ? Palette::Crimson : Palette::White;
	const double timerLength = EaseInSine(timeT) * TimerWidth;
	Line{ Vec2{ Scene::Width() / 2, 50 }, Arg::direction = Vec2::Left() * timerLength }.draw(8, timerColor);
	Line{ Vec2{ Scene::Width() / 2, 50 }, Arg::direction = Vec2::Right() * timerLength }.draw(8, timerColor);
}

void Game::gameOver()
{
	GamePhase = Result;
	m_resultTimer = 0.0;

	getData().bgm.fadeVolume(0.1, 1s);
	getData().seMiss.playOneShot(0.7);
}

void Game::resultUpdate()
{
	m_resultTimer += Scene::DeltaTime();

	if (m_resultTimer > 1.0 and MouseL.down())
	{
		getData().bgm.stop(1s);
		changeScene(U"Title");
	}
}

void Game::resultDraw() const
{
	// ゲームの最後の部分を描画
	{
		const Transformer2D t{ Mat3x2::Translate(Math::Exp(m_resultTimer * -10) * Math::Sin(m_resultTimer * 50.0) * 80, 0) };
		inGameDraw();
	}

	Rect{ 0, 0, Scene::Size() }.draw(ColorF{ 0.0, 0.6 });

	// 1秒後に、スコアと難易度を表示する
	FontAsset(U"Title")(U"Game Over").drawAt(Vec2{ Scene::Width() / 2, 100 }, Palette::White);

	if (m_resultTimer < 1.0)
	{
		return;
	}

	FontAsset(U"font")(U"スコア:").draw(Arg::rightCenter(Scene::Width() / 2, Scene::Height() / 2 - 50));
	FontAsset(U"font")(U"{}"_fmt(getData().score)).draw(Arg::leftCenter(Scene::Width() / 2 + 20, Scene::Height() / 2 - 50));
	FontAsset(U"font")(U"難易度:").draw(Arg::rightCenter(Scene::Width() / 2, Scene::Height() / 2 + 50));
	FontAsset(U"font")(U"{}"_fmt(m_difficulty + 1)).draw(Arg::leftCenter(Scene::Width() / 2 + 20, Scene::Height() / 2 + 50));

	FontAsset(U"font")(U"クリックしてタイトルへ").drawAt(24, Vec2{ Scene::Width() / 2, Scene::Height() - 100 }, Palette::White);
}

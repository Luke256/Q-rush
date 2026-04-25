# pragma once

# include "Main.hpp"

class Title : public MyApp::Scene
{
public:
	Title(const InitData& init);
	void update() override;
	void draw() const override;
private:
	const RectF m_modeRectL{ Arg::center(Scene::Width() / 2 - 100, 100), Vec2{ 200, 100 } };
	const RectF m_modeRectR{ Arg::center(Scene::Width() / 2 + 100, 100), Vec2{ 200, 100 } };
};

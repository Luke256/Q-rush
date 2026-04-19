# pragma once
# include <Siv3D.hpp>

class IMyEffect
{
public:
	~IMyEffect() = default;
	virtual bool update() = 0; // true なら継続、false なら終了
	virtual void draw() const = 0;
};

class MyEffectManager
{
public:
	void update() {
		for (int32 i = 0; i < m_effects.size();)
		{
			if (!m_effects[i]->update())
			{
				swap(m_effects[i], m_effects.back());
				m_effects.pop_back();
			}
			else {
				i++;
			}
		}
	}
	void draw() const {
		for (const auto& effect : m_effects)
		{
			effect->draw();
		}
	}

	template <class EffectType, class... Args>
	void add(Args&&... args)
	{
		m_effects.push_back(std::make_unique<EffectType>(std::forward<Args>(args)...));
	}
private:
	Array<std::unique_ptr<IMyEffect>> m_effects;
};


struct SparkParticle
{
	Vec2 pos, vel;
	double rot, rotVel, scale;
	Color color;
};

struct SparkEffect : IMyEffect
{
	double time = 0.0;
	Array<SparkParticle> particles;

	SparkEffect(Vec2 pos): particles(48)
	{
		for (auto& p : particles)
		{
			p.pos = pos + RandomVec2(2.0);
			p.vel = RandomVec2(1.0) * Random(400.0);
			p.rot = Random(3.14);
			p.rotVel = Random(-3.14, 3.14);
			p.scale = Random(0.5, 1.0);
			p.color = HSV{ Random(0, 90), 0.5, 1.0, 0.8 };
		}
	}

	bool update() override
	{
		time += Scene::DeltaTime();
		for (auto& p : particles)
		{
			p.vel.y += 400 * Scene::DeltaTime();
			p.pos += p.vel * Scene::DeltaTime();
			p.rot += p.rotVel * Scene::DeltaTime();
		}
		return time < 1.0;
	}

	void draw() const override
	{
		for (const auto& p : particles)
		{
			RectF{p.pos, (1 - time) * 24 * p.scale}
				.rotated(p.rot)
				.draw(p.color);
		}
	}
};

struct BubbleEffect : IMyEffect
{
	Vec2 pos;
	double size;
	double speed;

	BubbleEffect() {
		pos = Vec2{ Random(Scene::Width()), Scene::Height() + 20 };
		size = Random(10.0, 30.0);
		speed = Random(50.0, 200.0);
	}
	bool update() override
	{
		pos.y -= speed * Scene::DeltaTime();
		return pos.y > -20;
	}
	void draw() const override
	{
		Circle{pos,size}
			.draw(ColorF{1.0, 0.15})
			.drawFrame(1, ColorF{ 1.0, 0.3 });
	}
};

// 下から上に切ったようなエフェクト
struct SlashEffect : IMyEffect
{
	struct SparkParticle
	{
		Vec2 pos, vel;
	};

	const double Duration = 0.2;
	Vec2 pos;
	double time;
	double length;
	Array<SparkParticle> particles;

	SlashEffect(Vec2 pos, double length) :
		pos(pos),
		time(0.0),
		length(length)
	{

	}

	double getProgress() const
	{
		return EaseOutCirc(Min(1.0, time / Duration));
	}

	bool update() override
	{
		time += Scene::DeltaTime();

		Vec2 top = pos + Vec2{ 0, -length * getProgress()};

		if (time < Duration) {
			for (auto i : step(4))
			particles << SparkParticle{ top, RandomVec2(1.0) * Random(200.0) };
		}

		for (auto& p : particles)
		{
			p.vel.y += 200 * Scene::DeltaTime();
			p.pos += p.vel * Scene::DeltaTime();
		}

		return time < Duration * 2;
	}

	void draw() const override
	{
		double alpha = 1 - Max(0.0, time - Duration) / Duration;
		Vec2 top = pos + Vec2{ 0, -length * getProgress() };

		Line{ pos, top }.draw(4, ColorF{ 1.0, alpha });

		for (const auto& p : particles)
		{
			Circle{p.pos, 4}.draw(ColorF{ 1.0, alpha });
		}
	}
};

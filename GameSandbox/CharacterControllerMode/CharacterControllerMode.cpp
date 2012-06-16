// CharacterControllerMode.cpp
// Nolan Check
// Created 3/22/2012

#include "CharacterControllerMode.hpp"

#include <vector>

#include "MainMenuMode/MainMenuMode.hpp"
#include "World.hpp"

namespace CharacterControllerMode
{

CharacterControllerMode::CharacterControllerMode()
	: m_intendedVel(0.0f, 0.0f),
	m_actualVel(0.0f, 0.0f)
{ }

std::unique_ptr<CharacterControllerMode> CharacterControllerMode::create(Game* game)
{
	std::unique_ptr<CharacterControllerMode> p(new CharacterControllerMode);

	p->m_game = game;

	p->m_renderer.init(p->m_game->GetHWnd());
		
	p->m_playerCharacter = CharacterPtr(new Character);
	p->m_playerCharacter->pos = Vector2f(0.0f, 3.0f);
	p->m_playerCharacter->radius = 1.0f;
	p->m_characters.push_back(p->m_playerCharacter);
		
	p->m_npcCharacter = CharacterPtr(new Character);
	p->m_npcCharacter->pos = Vector2f(5.0f, 4.0f);
	p->m_npcCharacter->radius = 1.0f;
	p->m_characters.push_back(p->m_npcCharacter);

	p->m_b2World.reset(new b2World(b2Vec2(0.0f, 0.0f)));

	// Import World into b2World

	World::WallList walls = p->m_world.GetWalls();
	for (World::WallList::const_iterator it = walls.begin(); it != walls.end(); ++it)
	{
		b2BodyDef bd;
		b2Body* wall = p->m_b2World->CreateBody(&bd);

		b2EdgeShape shape;
		shape.Set(b2Vec2(it->start.x, it->start.y), b2Vec2(it->end.x, it->end.y));

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = 0.0f;
		wall->CreateFixture(&fd);
	}

	{
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.fixedRotation = true;
		bd.position = b2Vec2(p->m_playerCharacter->pos.x, p->m_playerCharacter->pos.y);
		p->m_b2Character = p->m_b2World->CreateBody(&bd);

		b2CircleShape shape;
		shape.m_radius = p->m_playerCharacter->radius;

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = 0.0f;
		p->m_b2Character->CreateFixture(&fd);
	}

	return p;
}

class MainMenuModeSwitcher : public GameModeSwitcher
{
public:
	std::unique_ptr<GameMode> createMode(Game* game) {
		return MainMenuMode::MainMenuMode::create(game);
	}
};

void CharacterControllerMode::Tick(const GameInput& input)
{
	if (input.esc)
	{
		m_game->SwitchMode(new MainMenuModeSwitcher);
	}
	else
	{
		static const float CHAR_SPEED = 8.0f; // world units per second

		Vector2f intendedVel = CHAR_SPEED * input.move;

		Vector2f actualVel = intendedVel;

		m_b2Character->SetLinearVelocity(b2Vec2(intendedVel.x, intendedVel.y));

		m_b2World->Step(1.0f / Game::TICKS_PER_SEC, 8, 3);

		b2Vec2 charPos = m_b2Character->GetPosition();
		m_playerCharacter->pos = Vector2f(charPos.x, charPos.y);
		m_intendedVel = intendedVel;
		b2Vec2 charVel = m_b2Character->GetLinearVelocity();
		m_actualVel = Vector2f(charVel.x, charVel.y);
	}
}

void CharacterControllerMode::Render()
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_renderer.GetD2DTarget();

	d2dTarget->BeginDraw();
		
	ComPtr<ID2D1Factory> factory;
	d2dTarget->GetFactory(factory.Receive());

	D2D1_SIZE_F targetSize = d2dTarget->GetSize();
	Rectf vp(0.0f, 0.0f, targetSize.width, targetSize.height);
	Matrix3x2f worldToViewport = m_camera.GetWorldToViewport(vp);

	ComPtr<ID2D1SolidColorBrush> blackBrush;
	d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), blackBrush.Receive());

	d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightGray));

	ComPtr<ID2D1PathGeometry> geom;
	factory->CreatePathGeometry(geom.Receive());
	ComPtr<ID2D1GeometrySink> sink;
	geom->Open(sink.Receive());

	// Render walls
	const World::WallList& walls = m_world.GetWalls();
	for (World::WallList::const_iterator it = walls.begin(); it != walls.end(); ++it)
	{
		sink->BeginFigure(it->start, D2D1_FIGURE_BEGIN_HOLLOW);
		sink->AddLine(it->end);
		sink->EndFigure(D2D1_FIGURE_END_OPEN);
	}
	
	sink->Close();

	ComPtr<ID2D1TransformedGeometry> transGeom;
	factory->CreateTransformedGeometry(geom, worldToViewport, transGeom.Receive());
	d2dTarget->DrawGeometry(transGeom, blackBrush);
		
	// Render characters
	d2dTarget->SetTransform(worldToViewport);

	for (CharacterList::const_iterator it = m_characters.begin();
		it != m_characters.end(); ++it)
	{
		d2dTarget->FillEllipse(
			D2D1::Ellipse((*it)->pos, (*it)->radius, (*it)->radius),
			blackBrush);
	}

	// Render intended velocity as a red line
	// XXX: Black brush is used.
	blackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
	d2dTarget->DrawLine(m_playerCharacter->pos,
		m_playerCharacter->pos + m_intendedVel,
		blackBrush, m_playerCharacter->radius/8.0f);
	// Render actualVelocity as a blue line
	blackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
	d2dTarget->DrawLine(m_playerCharacter->pos,
		m_playerCharacter->pos + m_actualVel,
		blackBrush, m_playerCharacter->radius/8.0f);

	d2dTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	d2dTarget->EndDraw();
	// TODO: Handle D2DERR_RECREATETARGET
}

}

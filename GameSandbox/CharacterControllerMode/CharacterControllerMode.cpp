// CharacterControllerMode.cpp
// Nolan Check
// Created 3/22/2012

#include "CharacterControllerMode.hpp"

#include <sstream>
#include <string>
#include <vector>

#include "MainMenuMode/MainMenuMode.hpp"
#include "World.hpp"

namespace CharacterControllerMode
{

CharacterControllerMode::CharacterControllerMode()
	: m_intendedVel(0.0f, 0.0f),
	m_actualVel(0.0f, 0.0f)
{ }

CharacterControllerMode::Ptr CharacterControllerMode::create(Game::Ptr game)
{
	Ptr p(new CharacterControllerMode);

	p->m_game = game;

	p->m_renderer = D2DRenderer::create(game->GetHWnd());

	p->m_state = STATE_WALKING;
		
	CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)p->m_dwriteFactory.Receive()));

	CHECK_HR(p->m_dwriteFactory->CreateTextFormat(L"Trebuchet MS", NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"en-US", p->m_textFormat.Receive()));

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
		bd.position = p->m_playerCharacter->pos;
		p->m_b2Character = p->m_b2World->CreateBody(&bd);

		b2CircleShape shape;
		shape.m_radius = p->m_playerCharacter->radius;

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = 0.0f;
		p->m_b2Character->CreateFixture(&fd);
	}

	{
		b2BodyDef bd;
		bd.position = p->m_npcCharacter->pos;
		p->m_b2Npc = p->m_b2World->CreateBody(&bd);

		b2CircleShape shape;
		shape.m_radius = p->m_npcCharacter->radius;

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = 0.0f;
		p->m_b2Npc->CreateFixture(&fd);
	}

	p->m_wasTouching = false;

	return p;
}

class MainMenuModeSwitcher : public GameModeSwitcher
{
public:
	GameMode::Ptr createMode(Game::Ptr game) {
		return MainMenuMode::MainMenuMode::create(game);
	}
};

void CharacterControllerMode::Tick(const GameInput& input)
{
	Game::Ptr game = m_game.lock();
	assert(game);

	if (input.esc)
	{
		game->SwitchMode(GameModeSwitcher::Ptr(new MainMenuModeSwitcher));
	}
	else
	{
		if (m_revealingText && input.enter)
		{
			m_revealingText->finish();
		}

		switch (m_state)
		{
		case STATE_WALKING:
			tickWalking(input);
			break;
		case STATE_TALKING:
			tickTalking(input);
			break;
		}

		if (m_revealingText) {
			m_revealingText->Tick();
		}
	}
}

void CharacterControllerMode::tickWalking(const GameInput& input)
{
	static const float CHAR_SPEED = 8.0f; // world units per second

	Vector2f intendedVel = CHAR_SPEED * input.move;

	Vector2f actualVel = intendedVel;

	m_b2Character->SetLinearVelocity(intendedVel);

	m_b2World->Step(1.0f / Game::TICKS_PER_SEC, 8, 3);

	bool switchToTalking = false;
	for (b2ContactEdge* ce = m_b2Npc->GetContactList(); ce != NULL; ce = ce->next)
	{
		b2Contact* c = ce->contact;
		if (c->IsTouching())
		{
			if (!m_wasTouching)
			{
				startRevealingText(L"You are now touching an NPC.");
				m_wasTouching = true;
			}
			if (input.enter)
			{
				switchToTalking = true;
			}
		}
		else
		{
			clearRevealingText();
			m_wasTouching = false;
		}
	}

	m_playerCharacter->pos = m_b2Character->GetPosition();
	m_intendedVel = intendedVel;
	m_actualVel = m_b2Character->GetLinearVelocity();

	if (switchToTalking)
	{
		enterTalking();
	}
}

const std::wstring CharacterControllerMode::TALK_SCRIPT[] =
{
	L"Say", L"Hello, I'm just a silly NPC.",
	L"Sleep", L"2000",
	L"Say", L"How do you do?",
	L"Sleep", L"2000",
	L"Say", L"Make a choice, yes or no.",
	L"YesNoChoice",
	L""
};

void CharacterControllerMode::interpretTalkScript()
{
	assert(m_talkState == TALKSTATE_RUNNING);

	bool done = false;
	while (!done)
	{
		const std::wstring& cmd = TALK_SCRIPT[m_talkScriptPtr];
		if (cmd == L"Say")
		{
			const std::wstring& dlg = TALK_SCRIPT[m_talkScriptPtr+1];
			startRevealingText(dlg);
			m_talkState = TALKSTATE_SAYING;
			m_talkScriptPtr += 2;
			done = true;
		}
		else if (cmd == L"Sleep")
		{
			const std::wstring& msStr = TALK_SCRIPT[m_talkScriptPtr+1];
			std::wistringstream iss(msStr);
			int ms;
			iss >> ms;
			m_talkState = TALKSTATE_SLEEPING;
			m_talkSleepCount = ms * Game::TICKS_PER_SEC / 1000;
			m_talkScriptPtr += 2;
			done = true;
		}
		else if (cmd == L"YesNoChoice")
		{
			m_buttons = Buttons::create(m_textFormat);
			m_buttons->addChoice(L"Yes");
			m_buttons->addChoice(L"No");
			m_talkState = TALKSTATE_WAITING;
			++m_talkScriptPtr;
			done = true;
		}
		else if (cmd.empty())
		{
			m_buttons.reset();
			clearRevealingText();
			m_state = STATE_WALKING;
			done = true;
		}
	}
}

void CharacterControllerMode::enterTalking()
{
	m_state = STATE_TALKING;
	m_talkState = TALKSTATE_RUNNING;
	m_talkScriptPtr = 0;
	interpretTalkScript();
}

void CharacterControllerMode::tickTalking(const GameInput& input)
{
	switch (m_talkState)
	{
	case TALKSTATE_RUNNING:
		interpretTalkScript();
		break;
	case TALKSTATE_SAYING:
		if (m_revealingText->isFinished())
		{
			m_talkState = TALKSTATE_RUNNING;
			interpretTalkScript();
		}
		break;
	case TALKSTATE_SLEEPING:
		--m_talkSleepCount;
		if (m_talkSleepCount <= 0)
		{
			m_talkState = TALKSTATE_RUNNING;
			interpretTalkScript();
		}
		break;
	case TALKSTATE_WAITING:
		if (input.enter)
		{
			m_talkState = TALKSTATE_RUNNING;
			interpretTalkScript();
		}
		else
		{
			m_buttons->tick(input);
		}
		break;
	}
}

void CharacterControllerMode::Render()
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_renderer->GetD2DTarget();

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

	if (m_revealingText) {
		m_revealingText->Render(d2dTarget);
	}

	if (m_buttons) {
		m_buttons->render(d2dTarget);
	}

	d2dTarget->EndDraw();
	// TODO: Handle D2DERR_RECREATETARGET
}

void CharacterControllerMode::startRevealingText(const std::wstring& str)
{
	Rectf layoutBox(0.0f, 0.0f, m_renderer->GetD2DTarget()->GetSize().width,
		m_renderer->GetD2DTarget()->GetSize().height);
	m_revealingText = RevealingText::create(m_dwriteFactory, m_textFormat,
		str, layoutBox);
}

void CharacterControllerMode::clearRevealingText()
{
	m_revealingText.reset();
}

}

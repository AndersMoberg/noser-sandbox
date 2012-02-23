// Game.cpp
// Nolan Check
// Created 1/21/2012

#include "Game.hpp"

#include "WindowsUtils.hpp"
#include "ButtonGroup.hpp"

static const float STEPS_PER_SEC = 3600.0f;

Game::Game()
	: m_intendedVel(0.0f, 0.0f),
	m_actualVel(0.0f, 0.0f),
	m_talking(false)
{ }

Game::~Game()
{
}

GamePtr Game::Create()
{
	GamePtr p(new Game);

	// The frequency cannot change while the system is running, so it's safe
	// to query it once
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	p->m_frequency = li.QuadPart;

	QueryPerformanceCounter(&li);
	p->m_prevTime = li.QuadPart;
	
	p->m_renderTarget = GameRenderTarget::Create();

	p->m_camera = Camera::Create();
	p->m_world = World::Create();

	p->m_playerCharacter = CharacterPtr(new Character);
	p->m_playerCharacter->pos = Vector2f(0.0f, 3.0f);
	p->m_playerCharacter->radius = 1.0f;
	p->m_characters.push_back(p->m_playerCharacter);

	p->m_npcCharacter = CharacterPtr(new Character);
	p->m_npcCharacter->pos = Vector2f(5.0f, 4.0f);
	p->m_npcCharacter->radius = 1.0f;
	p->m_characters.push_back(p->m_npcCharacter);

	p->m_revealingText = RevealingText::Create(L"Dialogue will go in here. This is some really long dialogue designed to test the Revealing Text object.",
		Rectf(0.0f, 0.0f, 640.0f, 480.0f));

	p->m_buttonGroup = ButtonGroup::Create(Vector2f(200.0f, 200.0f));
	p->m_buttonGroup->AddButton(L"Left");
	p->m_buttonGroup->AddButton(L"Right");

	return p;
}

void Game::SetD2DTarget(ID2D1RenderTarget* target)
{
	m_renderTarget->SetD2DTarget(target);
}

void Game::ReleaseD2DTarget()
{
	m_renderTarget->ReleaseD2DTarget();
}

// ws: wall start; we: wall end; p: character position; pRadius: character radius
static bool TestWallPosConstraint(const Vector2f& ws, const Vector2f& we,
	const Vector2f& p, float pRadius)
{
	// Test position constraint of circular character against line segment wall

	bool pastS = Vector2f::Dot(we - ws, p - ws) > 0.0f;
	bool pastE = Vector2f::Dot(ws - we, p - we) > 0.0f;
	if (pastS && pastE) {
		// character is not near either end
		return DistancePointLineSquared(p, ws, we) <= pRadius*pRadius;
	} else if (pastS) {
		// character is near we
		return (p - we).LengthSquared() <= pRadius*pRadius;
	} else if (pastE) {
		// character is near ws
		return (p - ws).LengthSquared() <= pRadius*pRadius;
	} else {
		return false;
	}
}

// ws: wall start; we: wall end; p: character position; v: character velocity
static bool TestWallVelConstraint(const Vector2f& ws, const Vector2f& we,
	const Vector2f& p, const Vector2f& v)
{
	// Test velocity constraint of circular character against line segment wall
	// (assuming character is already position-constrained)

	bool pastS = Vector2f::Dot(we - ws, p - ws) > 0.0f;
	bool pastE = Vector2f::Dot(ws - we, p - we) > 0.0f;
	if (pastS && pastE) {
		// character is not near either end
		Vector2f wallPerp = (we - ws).Perpendicular();
		Vector2f wallSToP = p - ws;
		// NOTE: I did all this math on paper at one point, but I should review
		// soon.
		return Vector2f::Dot(wallPerp, wallSToP) * Vector2f::Dot(wallPerp, v) <= 0.0f;
	} else if (pastS) {
		// character is near we
		return Vector2f::Dot(v, p - we) <= 0.0f;
	} else if (pastE) {
		// character is near ws
		return Vector2f::Dot(v, p - ws) <= 0.0f;
	} else {
		return false;
	}
}

// ws: wall start; we: wall end; p: character position; v: character velocity
static Vector2f CorrectVelAgainstWall(const Vector2f& ws, const Vector2f& we,
	const Vector2f& p, const Vector2f& v)
{
	// If character is constrained in position and velocity against a wall,
	// calculate a new velocity that satisfies the constraint
	// i.e. "push" the character against the wall but don't let it through.

	bool pastS = Vector2f::Dot(we - ws, p - ws) > 0.0f;
	bool pastE = Vector2f::Dot(ws - we, p - we) > 0.0f;
	if (pastS && pastE) {
		// character is not near either end
		return Vector2f::Projection(v, we - ws);
	} else if (pastS) {
		// character is near we
		return Vector2f::Projection(v, (p - we).Perpendicular());
	} else if (pastE) {
		// character is near ws
		return Vector2f::Projection(v, (p - ws).Perpendicular());
	} else {
		return v;
	}
}

void Game::Update(const Vector2f& move, bool spaceTrigger)
{
	// FIXME: Maybe time should be given as an argument to this function,
	// instead of querying it ourselves.
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);
	
	long long timeDiff = curTime.QuadPart - m_prevTime;

	if (m_talking)
	{
		if (spaceTrigger) {
			m_talking = false;
		} else {
			m_revealingText->Update(curTime.QuadPart);
			m_buttonGroup->Update(move);
		}
		m_prevTime = curTime.QuadPart;
	}
	else if (spaceTrigger && CanPlayerTalk())
	{
		m_talking = true;
		m_revealingText->Start(curTime.QuadPart, m_frequency, 128.0f);
		m_prevTime = curTime.QuadPart;
	}
	else
	{
		for (long long t = 0; t < timeDiff; t += (long long)(m_frequency / STEPS_PER_SEC))
		{
			Step(move);
			m_prevTime += (long long)(m_frequency / STEPS_PER_SEC);
		}
	}
}

Game::Collisions Game::CheckCharacterCollisions(
	const Character& character, const Vector2f& vel)
{
	Collisions result;

	const World::WallList& walls = m_world->GetWalls();
	for (World::WallList::const_iterator it = walls.begin(); it != walls.end(); ++it)
	{
		bool posConstrained = TestWallPosConstraint(it->start, it->end,
			character.pos, character.radius);
		if (posConstrained)
		{
			bool velConstrained = TestWallVelConstraint(it->start, it->end,
				character.pos, vel);
			if (velConstrained)
			{
				result.push_back(&*it);
			}
		}
	}

	return result;
}

void Game::Step(const Vector2f& move)
{
	static const float CHAR_SPEED = 8.0f; // world units per second

	Vector2f intendedVel = CHAR_SPEED * move;

	Vector2f actualVel = intendedVel;

	Collisions collisions = CheckCharacterCollisions(*m_playerCharacter, actualVel);
	if (collisions.size() == 1)
	{
		const Wall* wall = collisions.back();
		actualVel = CorrectVelAgainstWall(wall->start, wall->end,
			m_playerCharacter->pos, actualVel);
	}
	else if (collisions.size() >= 2)
	{
		const Wall* wall1 = collisions.front();
		const Wall* wall2 = collisions.back();
		// Find wall1 correction, test against wall2, then vice-versa
		Vector2f wall1Correct = CorrectVelAgainstWall(wall1->start, wall1->end,
			m_playerCharacter->pos, actualVel);
		Vector2f wall2Correct = CorrectVelAgainstWall(wall2->start, wall2->end,
			m_playerCharacter->pos, actualVel);
		if (!TestWallVelConstraint(wall2->start, wall2->end, m_playerCharacter->pos,
			wall1Correct))
		{
			actualVel = wall1Correct;
		}
		else if (!TestWallVelConstraint(wall1->start, wall1->end, m_playerCharacter->pos,
			wall2Correct))
		{
			actualVel = wall2Correct;
		}
		else if (wall1Correct == wall2Correct)
		{
			// FIXME: Comparison is sensitive to tiny floating-point errors
			actualVel = wall1Correct;
		}
		else
		{
			actualVel = Vector2f(0.0f, 0.0f);
		}
	}
	else if (collisions.size() > 2)
	{
		// TODO: Generalize the algorithm above for more than 2 constraints
		actualVel = Vector2f(0.0f, 0.0f);
	}

	m_playerCharacter->pos += actualVel / STEPS_PER_SEC;
	m_intendedVel = intendedVel;
	m_actualVel = actualVel;
}

void Game::Render()
{
	ID2D1RenderTarget* d2dTarget = m_renderTarget->GetD2DTarget();

	ID2D1Factory* factory;
	d2dTarget->GetFactory(&factory);

	D2D1_SIZE_F targetSize = d2dTarget->GetSize();
	Rectf vp(0.0f, 0.0f, targetSize.width, targetSize.height);
	Matrix3x2f worldToViewport = m_camera->GetWorldToViewport(vp);

	d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	ID2D1PathGeometry* geom;
	factory->CreatePathGeometry(&geom);
	ID2D1GeometrySink* sink;
	geom->Open(&sink);

	// Render walls
	const World::WallList& walls = m_world->GetWalls();
	for (World::WallList::const_iterator it = walls.begin(); it != walls.end(); ++it)
	{
		sink->BeginFigure(it->start, D2D1_FIGURE_BEGIN_HOLLOW);
		sink->AddLine(it->end);
		sink->EndFigure(D2D1_FIGURE_END_OPEN);
	}
	
	sink->Close();
	sink->Release();
	ID2D1TransformedGeometry* transGeom;
	factory->CreateTransformedGeometry(geom, worldToViewport, &transGeom);
	geom->Release();
	d2dTarget->DrawGeometry(transGeom, m_renderTarget->GetBlackBrush());
	transGeom->Release();

	// Render characters
	d2dTarget->SetTransform(worldToViewport);

	for (CharacterList::const_iterator it = m_characters.begin();
		it != m_characters.end(); ++it)
	{
		d2dTarget->FillEllipse(
			D2D1::Ellipse((*it)->pos, (*it)->radius, (*it)->radius),
			m_renderTarget->GetBlackBrush());
	}

	// Render intended velocity as a red line
	// XXX: Black brush is used.
	m_renderTarget->GetBlackBrush()->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
	d2dTarget->DrawLine(m_playerCharacter->pos,
		m_playerCharacter->pos + m_intendedVel,
		m_renderTarget->GetBlackBrush(), m_playerCharacter->radius/8.0f);
	// Render actualVelocity as a blue line
	m_renderTarget->GetBlackBrush()->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
	d2dTarget->DrawLine(m_playerCharacter->pos,
		m_playerCharacter->pos + m_actualVel,
		m_renderTarget->GetBlackBrush(), m_playerCharacter->radius/8.0f);
	d2dTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	m_renderTarget->GetBlackBrush()->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

	// Render text overlay
	if (CanPlayerTalk() && !m_talking)
	{
		m_renderTarget->DrawText(L"Press Space to talk",
			m_renderTarget->GetDialogTextFormat(),
			m_renderTarget->GetBlackBrush(), 0.5f,
			NULL,
			vp);
	}
	else if (m_talking)
	{
		m_revealingText->Render(m_renderTarget);
		m_buttonGroup->Render(m_renderTarget);
	}
}

void Game::RenderPrintf(ID2D1RenderTarget* pD2DTarget,
	IDWriteTextFormat* textFormat, const D2D1_RECT_F& layoutRect,
	ID2D1Brush* defaultForegroundBrush,
	LPCWSTR msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);

	int len = _vscwprintf(msg, args);

	WCHAR* buf = new WCHAR[len + 1];
	vswprintf_s(buf, len + 1, msg, args);

	va_end(args);

	pD2DTarget->DrawTextW(buf, len, textFormat, layoutRect, defaultForegroundBrush);

	delete[] buf;
}

bool Game::CanPlayerTalk()
{
	// Compare player character and npc player positions
	Vector2f npcToPlayer = m_playerCharacter->pos - m_npcCharacter->pos;
	float canTalkDistance = 2.5f;
	if (npcToPlayer.LengthSquared() <= canTalkDistance*canTalkDistance)
	{
		return true;
	}
	return false;
}

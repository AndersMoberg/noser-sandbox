// Game.cpp
// Nolan Check
// Created 1/21/2012

#include "Game.hpp"

#include "WindowsUtils.hpp"

static const float STEPS_PER_SEC = 3600.0f;

Game::Game()
	: m_pDWriteFactory(NULL),
	m_pDialogTextFormat(NULL),
	m_pBlackBrush(NULL),
	m_intendedVel(0.0f, 0.0f),
	m_actualVel(0.0f, 0.0f),
	m_talking(false)
{ }

Game::~Game()
{
	SafeRelease(m_pBlackBrush);
	SafeRelease(m_pDialogTextFormat);
	SafeRelease(m_pDWriteFactory);
}

GamePtr Game::Create()
{
	GamePtr p(new Game);

	CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)&p->m_pDWriteFactory));

	CHECK_HR(p->m_pDWriteFactory->CreateTextFormat(L"Calibri", NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-US", &p->m_pDialogTextFormat));

	// The frequency cannot change while the system is running, so it's safe
	// to query it once
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	p->m_frequency = li.QuadPart;

	QueryPerformanceCounter(&li);
	p->m_prevTime = li.QuadPart;
	
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

	return p;
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

	if (spaceTrigger && CanPlayerTalk())
	{
		m_talking = true;
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

void Game::Render(ID2D1RenderTarget* target)
{
	ID2D1Factory* factory;
	target->GetFactory(&factory);

	if (!m_pBlackBrush)
	{
		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);
	}

	D2D1_SIZE_F targetSize = target->GetSize();
	Rectf vp(0.0f, 0.0f, targetSize.width, targetSize.height);
	Matrix3x2f worldToViewport = m_camera->GetWorldToViewport(vp);

	target->Clear(D2D1::ColorF(D2D1::ColorF::White));

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
	target->DrawGeometry(transGeom, m_pBlackBrush);
	transGeom->Release();

	// Render characters
	target->SetTransform(worldToViewport);

	for (CharacterList::const_iterator it = m_characters.begin();
		it != m_characters.end(); ++it)
	{
		target->FillEllipse(
			D2D1::Ellipse((*it)->pos, (*it)->radius, (*it)->radius),
			m_pBlackBrush);
	}

	// Render intended velocity as a red line
	m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
	target->DrawLine(m_playerCharacter->pos,
		m_playerCharacter->pos + m_intendedVel, m_pBlackBrush, m_playerCharacter->radius/8.0f);
	// Render actualVelocity as a blue line
	m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
	target->DrawLine(m_playerCharacter->pos,
		m_playerCharacter->pos + m_actualVel, m_pBlackBrush, m_playerCharacter->radius/8.0f);
	target->SetTransform(D2D1::Matrix3x2F::Identity());

	m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

	// Render text overlay
	if (CanPlayerTalk() && !m_talking)
	{
		RenderPrintf(target, m_pDialogTextFormat, vp, m_pBlackBrush,
			L"Press Space to talk", 123);
	}
	else if (m_talking)
	{
		Rectf buttonRc(100.0f, 100.0f, 180.0f, 150.0f);
		RenderButton(target, buttonRc, L"Button");
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

void Game::RenderButton(ID2D1RenderTarget* target,
	const Rectf& rc, const std::wstring& label)
{
	target->DrawRectangle(rc, m_pBlackBrush);
	target->DrawTextW(label.c_str(), label.size(), m_pDialogTextFormat,
		rc, m_pBlackBrush);
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
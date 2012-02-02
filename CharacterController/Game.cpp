// Game.cpp
// Nolan Check
// Created 1/21/2012

#include "Game.hpp"

#include "WindowsUtils.hpp"

static const float STEPS_PER_SEC = 3600.0f;

Game::Game()
	: m_pDWriteFactory(NULL),
	m_pDialogTextFormat(NULL),
	m_intendedVel(0.0f, 0.0f),
	m_actualVel(0.0f, 0.0f)
{ }

Game::~Game()
{
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

	p->m_characterPos = Vector2f(0.0f, 3.0f);
	p->m_characterRadius = 1.0f;

	return p;
}

static bool TestWallPosConstraint(const Vector2f& ws, const Vector2f& we,
	const Vector2f& p, float pRadius)
{
	// Test position constraint of circular character against line segment wall

	bool pastS = Vector2f::Dot(we - ws, p - ws) > 0.0f;
	bool pastE = Vector2f::Dot(ws - we, p - we) > 0.0f;
	if (pastS && pastE) {
		return DistancePointLineSquared(p, ws, we) <= pRadius*pRadius;
	} else if (pastS) {
		return (p - we).LengthSquared() <= pRadius*pRadius;
	} else if (pastE) {
		return (p - ws).LengthSquared() <= pRadius*pRadius;
	} else {
		return false;
	}
}

static bool TestWallVelConstraint(const Vector2f& ws, const Vector2f& we,
	const Vector2f& p, const Vector2f& v)
{
	// Test velocity constraint of circular character against line segment wall
	// (assuming character is already position-constrained)

	bool pastS = Vector2f::Dot(we - ws, p - ws) > 0.0f;
	bool pastE = Vector2f::Dot(ws - we, p - we) > 0.0f;
	if (pastS && pastE) {
		Vector2f wallPerp = (we - ws).Perpendicular();
		Vector2f wallSToP = p - ws;
		// NOTE: I did all this math on paper at one point, but I should review
		// soon.
		return Vector2f::Dot(wallPerp, wallSToP) * Vector2f::Dot(wallPerp, v) <= 0.0f;
	} else if (pastS) {
		return Vector2f::Dot(v, p - we) <= 0.0f;
	} else if (pastE) {
		return Vector2f::Dot(v, p - ws) <= 0.0f;
	} else {
		return false;
	}
}

static Vector2f CorrectVelAgainstWall(const Vector2f& ws, const Vector2f& we,
	const Vector2f& p, const Vector2f& v)
{
	// If character is constrained in position and velocity against a wall,
	// calculate a new velocity that satisfies the constraint
	// i.e. "push" the character against the wall but don't let it through.

	bool pastS = Vector2f::Dot(we - ws, p - ws) > 0.0f;
	bool pastE = Vector2f::Dot(ws - we, p - we) > 0.0f;
	if (pastS && pastE) {
		return Vector2f::Projection(v, we - ws);
	} else if (pastS) {
		return Vector2f::Projection(v, (p - we).Perpendicular());
	} else if (pastE) {
		return Vector2f::Projection(v, (p - ws).Perpendicular());
	} else {
		return v;
	}
}

void Game::Update(const Vector2f& move)
{
	// FIXME: Maybe time should be given as an argument to this function,
	// instead of querying it ourselves.
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);
	
	long long timeDiff = curTime.QuadPart - m_prevTime;

	for (long long t = 0; t < timeDiff; t += (long long)(m_frequency / STEPS_PER_SEC))
	{
		Step(move);
		m_prevTime += (long long)(m_frequency / STEPS_PER_SEC);
	}
}

void Game::Step(const Vector2f& move)
{
	static const float CHAR_SPEED = 8.0f; // world units per second

	Vector2f intendedVel = CHAR_SPEED * move;

	Vector2f actualVel = intendedVel;

	bool constrained = false;
	const World::WallList& walls = m_world->GetWalls();
	for (World::WallList::const_iterator it = walls.begin(); it != walls.end(); ++it)
	{
		bool posConstrained = TestWallPosConstraint(it->start, it->end,
			m_characterPos, m_characterRadius);
		if (posConstrained)
		{
			bool velConstrained = TestWallVelConstraint(it->start, it->end,
				m_characterPos, actualVel);
			if (velConstrained)
			{
				// If more than one wall constrains the character, no movement
				// is possible.
				// FIXME: Collisions can be missed if a wall isn't tested
				// against the new actualVel!
				if (constrained)
				{
					actualVel = Vector2f(0.0f, 0.0f);
					break;
				}
				else
				{
					constrained = true;
					actualVel = CorrectVelAgainstWall(it->start, it->end,
						m_characterPos, actualVel);
				}
			}
		}
	}

	m_characterPos += actualVel / STEPS_PER_SEC;
	m_intendedVel = intendedVel;
	m_actualVel = actualVel;
}

void Game::Render(ID2D1RenderTarget* target)
{
	ID2D1Factory* factory;
	target->GetFactory(&factory);

	D2D1_SIZE_F targetSize = target->GetSize();
	Rectf vp(0.0f, 0.0f, targetSize.width, targetSize.height);
	Matrix3x2f worldToViewport = m_camera->GetWorldToViewport(vp);

	target->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Create brush for drawing stuff
	ID2D1SolidColorBrush* brush;
	target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);

	// Render walls
	const World::WallList& walls = m_world->GetWalls();
	for (World::WallList::const_iterator it = walls.begin(); it != walls.end(); ++it)
	{
		ID2D1PathGeometry* geom;
		factory->CreatePathGeometry(&geom);

		ID2D1GeometrySink* sink;
		geom->Open(&sink);

		sink->BeginFigure(it->start, D2D1_FIGURE_BEGIN_HOLLOW);
		sink->AddLine(it->end);
		sink->EndFigure(D2D1_FIGURE_END_OPEN);
		sink->Close();

		sink->Release();

		ID2D1TransformedGeometry* transGeom;
		factory->CreateTransformedGeometry(geom, worldToViewport, &transGeom);

		geom->Release();

		target->DrawGeometry(transGeom, brush);

		transGeom->Release();
	}

	// Render character
	target->SetTransform(worldToViewport);
	target->FillEllipse(
		D2D1::Ellipse(m_characterPos, m_characterRadius, m_characterRadius),
		brush);
	// Render intended velocity as a green line
	brush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
	target->DrawLine(m_characterPos, m_characterPos + m_intendedVel, brush, m_characterRadius/8.0f);
	// Render actualVelocity as a blue line
	brush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
	target->DrawLine(m_characterPos, m_characterPos + m_actualVel, brush, m_characterRadius/8.0f);
	target->SetTransform(D2D1::Matrix3x2F::Identity());

	// Render text overlay
	RenderPrintf(target, m_pDialogTextFormat, vp, brush,
		L"Hello %d!", 123);

	brush->Release();
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

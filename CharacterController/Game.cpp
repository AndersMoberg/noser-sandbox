// Game.cpp
// Nolan Check
// Created 1/21/2012

#include "Game.hpp"

Game::Game()
{ }

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
	
	p->m_camera = Camera::Create();
	p->m_world = World::Create();

	p->m_characterPos = Vector2f(0.0f, 0.0f);
	p->m_characterRadius = 1.0f;

	return p;
}

void Game::Update(const Vector2f& move)
{
	static const float CHAR_SPEED = 1.0f; // world units per second

	// FIXME: Maybe time should be given as an argument to this function,
	// instead of querying it ourselves.
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);

	long long timeDiff = curTime.QuadPart - m_prevTime;
	double timeDiffSecs = (double)timeDiff / m_frequency;

	// TODO: Control with time
	m_characterPos += (float)(CHAR_SPEED * timeDiffSecs) * move;

	m_prevTime = curTime.QuadPart;
}

void Game::Render(ID2D1RenderTarget* target)
{
	ID2D1Factory* factory;
	target->GetFactory(&factory);

	D2D1_SIZE_F targetSize = target->GetSize();
	Rectf vp(0.0f, 0.0f, targetSize.width, targetSize.height);
	Matrix3x2f worldToViewport = m_camera->GetWorldToViewport(vp);

	target->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

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
	target->SetTransform(D2D1::Matrix3x2F::Identity());

	brush->Release();
}

// Game.cpp
// Nolan Check
// Created 1/21/2012

#include "Game.hpp"

Game::Game()
{ }

GamePtr Game::Create()
{
	GamePtr p(new Game);
	
	p->m_camera = Camera::Create();
	if (!p->m_camera) {
		return NULL;
	}

	p->m_world = World::Create();
	if (!p->m_world) {
		return NULL;
	}

	p->m_characterPos = Vector2f(0.0f, 0.0f);

	return p;
}

void Game::Render(ID2D1RenderTarget* target)
{
	ID2D1Factory* factory;
	target->GetFactory(&factory);

	target->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

	// Create brush for drawing stuff
	ID2D1SolidColorBrush* brush;
	target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);

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

		D2D1_SIZE_F size = target->GetSize();
		Rectf vp(0.0f, 0.0f, size.width, size.height);

		ID2D1TransformedGeometry* transGeom;
		factory->CreateTransformedGeometry(geom,
			m_camera->GetWorldToViewport(vp), &transGeom);

		geom->Release();

		target->DrawGeometry(transGeom, brush);

		transGeom->Release();
	}

	brush->Release();
}

// CharacterControllerMode.cpp
// Nolan Check
// Created 3/22/2012

#include "CharacterControllerMode.hpp"

#include "GameRenderer.hpp"
#include "World.hpp"

class CharacterControllerModeImpl : public CharacterControllerMode
{

private:

	CharacterControllerModeImpl()
	{ }

	GameRendererPtr m_renderer;

	D2DLayerPtr m_d2dLayer;

	CameraPtr m_camera;
	WorldPtr m_world;

public:

	typedef std::shared_ptr<CharacterControllerModeImpl> Ptr;

	static CharacterControllerModePtr Create(GameRendererPtr renderer)
	{
		Ptr p(new CharacterControllerModeImpl);

		p->m_renderer = renderer;

		p->m_d2dLayer = D2DLayer::Create(p->m_renderer);

		p->m_camera = Camera::Create();
		p->m_world = World::Create();

		return p;
	}

	virtual void Tick(const Vector2f& move)
	{
	}

	virtual void Render()
	{
		unsigned int width = m_renderer->GetGLES2Manager()->GetWidth();
		unsigned int height = m_renderer->GetGLES2Manager()->GetHeight();

		glViewport(0, 0, width, height);

		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ComPtr<ID2D1RenderTarget> d2dTarget = m_d2dLayer->GetD2DTarget();

		d2dTarget->BeginDraw();
		
		ComPtr<ID2D1Factory> factory;
		d2dTarget->GetFactory(factory.Receive());

		D2D1_SIZE_F targetSize = d2dTarget->GetSize();
		Rectf vp(0.0f, 0.0f, targetSize.width, targetSize.height);
		Matrix3x2f worldToViewport = m_camera->GetWorldToViewport(vp);

		ComPtr<ID2D1SolidColorBrush> blackBrush;
		d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), blackBrush.Receive());

		// Clear to transparent black
		d2dTarget->Clear();

		ComPtr<ID2D1PathGeometry> geom;
		factory->CreatePathGeometry(geom.Receive());
		ComPtr<ID2D1GeometrySink> sink;
		geom->Open(sink.Receive());

		// Render walls
		const World::WallList& walls = m_world->GetWalls();
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

		// TODO: Render characters, port more of CharacterController over here.

		d2dTarget->EndDraw();

		// Render D2D layer to GL screen
		GLES2TexturePtr texture = m_d2dLayer->GetGLTexture();

		glBindTexture(GL_TEXTURE_2D, texture->Get());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Premultiplied alpha blending
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glEnable(GL_BLEND);

		m_renderer->GetGLES2Manager()->SetTexturedQuadMatrix(Matrix3x2f::IDENTITY);
		m_renderer->GetGLES2Manager()->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));
	}

};

CharacterControllerModePtr CharacterControllerMode::Create(
	GameRendererPtr renderer)
{
	return CharacterControllerModeImpl::Create(renderer);
}

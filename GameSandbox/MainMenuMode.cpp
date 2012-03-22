// MainMenuMode.cpp
// Nolan Check
// Created 3/21/2012

#include "MainMenuMode.hpp"

class MainMenuModeImpl : public MainMenuMode
{

private:

	D2DLayerPtr m_d2dLayer;

public:

	MainMenuModeImpl(GameRendererPtr renderer)
	{
		m_d2dLayer = D2DLayer::Create(renderer);
	}

	virtual void Tick(const Vector2f& move)
	{
	}

	virtual void Render()
	{
	}

};

MainMenuModePtr MainMenuMode::Create(GameRendererPtr renderer)
{
	return MainMenuModePtr(new MainMenuModeImpl(renderer));
}

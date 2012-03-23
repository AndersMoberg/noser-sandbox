// CharacterControllerMode.cpp
// Nolan Check
// Created 3/22/2012

#include "CharacterControllerMode.hpp"

#include "GameRenderer.hpp"

class CharacterControllerModeImpl : public CharacterControllerMode
{

private:

	CharacterControllerModeImpl()
	{ }

	GameRendererPtr m_renderer;

public:

	typedef std::shared_ptr<CharacterControllerModeImpl> Ptr;

	static CharacterControllerModePtr Create(GameRendererPtr renderer)
	{
		Ptr p(new CharacterControllerModeImpl);

		p->m_renderer = renderer;

		return p;
	}

	virtual void Tick(const Vector2f& move)
	{
	}

	virtual void Render()
	{
	}

};

CharacterControllerModePtr CharacterControllerMode::Create(
	GameRendererPtr renderer)
{
	return CharacterControllerModeImpl::Create(renderer);
}

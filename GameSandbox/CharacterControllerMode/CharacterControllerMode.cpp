// CharacterControllerMode.cpp
// Nolan Check
// Created 3/22/2012

#include "CharacterControllerMode.hpp"

#include "GameRenderer.hpp"
#include "World.hpp"

namespace CharacterControllerMode
{

CharacterControllerMode::CharacterControllerMode()
	: m_intendedVel(0.0f, 0.0f),
	m_actualVel(0.0f, 0.0f)
{ }

CharacterControllerMode* CharacterControllerMode::Create(GameRenderer* renderer)
{
	CharacterControllerMode* p(new CharacterControllerMode);

	p->m_renderer = renderer;

	p->m_d2dLayer.reset(D2DLayer::Create(p->m_renderer));
		
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
	
typedef std::list<const Wall*> Collisions;
Collisions CharacterControllerMode::CheckCharacterCollisions(
	const Character& character, const Vector2f& vel)
{
	Collisions result;

	const World::WallList& walls = m_world.GetWalls();
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

void CharacterControllerMode::Tick(const Vector2f& move)
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

	m_playerCharacter->pos += actualVel / (float)Game::TICKS_PER_SEC;
	m_intendedVel = intendedVel;
	m_actualVel = actualVel;
}

void CharacterControllerMode::Render()
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
	Matrix3x2f worldToViewport = m_camera.GetWorldToViewport(vp);

	ComPtr<ID2D1SolidColorBrush> blackBrush;
	d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), blackBrush.Receive());

	// Clear to transparent black
	d2dTarget->Clear();

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

	// Render D2D layer to GL screen
	GLES2Texture* texture = m_d2dLayer->GetGLTexture();

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

}

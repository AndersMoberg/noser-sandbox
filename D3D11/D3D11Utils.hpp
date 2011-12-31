// D3D11Utils.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11UTILS_HPP
#define _D3D11UTILS_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

template<class Interface>
inline void SafeRelease(Interface*& pi)
{
	if (pi)
	{
		pi->Release();
		pi = NULL;
	}
}

class VertexShader;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;

class VertexShader
{

private:

	VertexShader();

	ID3D11VertexShader* m_pVShader;

};

}

#endif

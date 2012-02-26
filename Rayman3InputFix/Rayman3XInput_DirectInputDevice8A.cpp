// Rayman3XInput_DirectInputDevice8A.cpp
// Nolan Check
// Created 2/25/2012

#include "Rayman3XInput_DirectInputDevice8A.hpp"

HRESULT Rayman3XInput_DirectInputDevice8A::Create(Rayman3XInput_DirectInputDevice8A** ppvOut)
{
	HRESULT hr;

	*ppvOut = NULL;

	Rayman3XInput_DirectInputDevice8A* result = new Rayman3XInput_DirectInputDevice8A;
	if (result)
	{
		hr = result->CreateInternal();
		if (FAILED(hr)) {
			result->Release();
		} else {
			*ppvOut = result;
		}
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

Rayman3XInput_DirectInputDevice8A::Rayman3XInput_DirectInputDevice8A()
	: m_refCount(1)
{ }

Rayman3XInput_DirectInputDevice8A::~Rayman3XInput_DirectInputDevice8A()
{ }

HRESULT Rayman3XInput_DirectInputDevice8A::CreateInternal()
{
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::QueryInterface(REFIID riid, void** ppvObject)
{
	IUnknown* pUnk = NULL;
	if (riid == IID_IUnknown) {
		pUnk = this;
	} else if (riid == IID_IDirectInputDevice8A) {
		pUnk = this;
	}

	*ppvObject = pUnk;
	if (pUnk)
	{
		pUnk->AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

ULONG Rayman3XInput_DirectInputDevice8A::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

ULONG Rayman3XInput_DirectInputDevice8A::Release()
{
	ULONG c = InterlockedDecrement(&m_refCount);
	if (c == 0)
	{
		delete this;
	}
	return c;
}

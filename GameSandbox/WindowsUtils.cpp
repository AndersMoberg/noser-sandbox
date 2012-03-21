// WindowsUtils.cpp
// Nolan Check
// Created 3/20/2012

#include "WindowsUtils.hpp"

DoNothingObject::DoNothingObject()
	: m_refCount(1)
{ }

DoNothingObject::~DoNothingObject()
{ }

ComPtr<DoNothingObject> DoNothingObject::Create()
{
	ComPtr<DoNothingObject> p;
	*p.Receive() = new DoNothingObject;
	return p;
}

HRESULT DoNothingObject::QueryInterface(REFIID riid, void** ppvObject)
{
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	if (__uuidof(IUnknown) == riid) {
		*ppvObject = this;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG DoNothingObject::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

ULONG DoNothingObject::Release()
{
	ULONG c = InterlockedDecrement(&m_refCount);
	if (c == 0) {
		delete this;
	}
	return c;
}
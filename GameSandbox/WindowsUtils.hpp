// WindowsUtils.hpp
// Nolan Check
// Created 3/16/2012

#ifndef _WINDOWSUTILS_HPP
#define _WINDOWSUTILS_HPP

#include <exception>

#include <ObjBase.h>

#define CHECK_HR(x) \
	do { \
		if (FAILED(x)) { \
			throw std::exception("Failed: " #x); \
		} \
	} while (0)

template<typename Interface>
class ComPtr
{
	template<typename InterfaceB> friend class ComPtr;

public:

	ComPtr()
		: m_ptr(NULL)
	{ }

	ComPtr(const ComPtr<Interface>& rhs)
		: m_ptr(rhs.m_ptr)
	{
		if (m_ptr) {
			m_ptr->AddRef();
		}
	}

	template<typename InterfaceB>
	ComPtr(const ComPtr<InterfaceB>& rhs)
		: m_ptr(rhs.m_ptr)
	{
		if (m_ptr) {
			m_ptr->AddRef();
		}
	}

	ComPtr<Interface>& operator=(const ComPtr<Interface>& rhs)
	{
		if (rhs.m_ptr != m_ptr)
		{
			if (m_ptr) {
				m_ptr->Release();
			}
			m_ptr = rhs.m_ptr;
			if (m_ptr) {
				m_ptr->AddRef();
			}
		}
		return *this;
	}

	~ComPtr()
	{
		if (m_ptr)
		{
			m_ptr->Release();
			m_ptr = NULL;
		}
	}

	void Release()
	{
		if (m_ptr)
		{
			m_ptr->Release();
			m_ptr = NULL;
		}
	}
	
	Interface** Receive() { return &m_ptr; }
	
	// This is a hack to prevent users from accidentally calling AddRef or
	// Release on the IUnknown object.
	class BlockIUnknownRefCounting : public Interface {
	private:
		STDMETHOD_(ULONG, AddRef)() = 0;
		STDMETHOD_(ULONG, Release)() = 0;
	};
	BlockIUnknownRefCounting* operator->() const {
		return reinterpret_cast<BlockIUnknownRefCounting*>(m_ptr);
	}

	operator Interface*() const { return m_ptr; }

private:

	Interface* m_ptr;

};

class DoNothingObject : public IUnknown
{

public:

	static ComPtr<DoNothingObject> Create();

	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

private:

	DoNothingObject();
	~DoNothingObject();

	ULONG m_refCount;

};

#endif

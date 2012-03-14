// WindowsUtils.hpp
// Nolan Check
// Created 1/10/2012

#ifndef _WINDOWSUTILS_HPP
#define _WINDOWSUTILS_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <ObjBase.h>

#define CHECK_HR(hr) \
	do { \
		if (FAILED(hr)) { \
			throw std::exception("Failed: " #hr); \
		} \
	} while (0)

template<class Interface>
inline void SafeRelease(Interface*& pi)
{
	if (pi)
	{
		pi->Release();
		pi = NULL;
	}
}

void Debug(const char* msg, ...);

template<typename Interface>
class ComPtr
{

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
		SafeRelease(m_ptr);
	}

	void Release()
	{
		SafeRelease(m_ptr);
	}

	Interface** Receive() { return &m_ptr; }

	class BlockIUnknownRefCounting : public Interface
	{
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

#endif

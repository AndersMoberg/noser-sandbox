// WindowsUtils.hpp
// Nolan Check
// Created 1/23/2012

#ifndef _WINDOWSUTILS_HPP
#define _WINDOWSUTILS_HPP

template<class Interface>
inline void SafeRelease(Interface*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

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
	
	class BlockIUnknownRefCounting : public Interface {
	private:
		STDMETHOD_(ULONG, AddRef)() = 0;
		STDMETHOD_(ULONG, Release)() = 0;
	};

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
		if (rhs.m_ptr) {
			rhs.m_ptr->AddRef();
		}
		if (m_ptr) {
			m_ptr->Release();
		}
		m_ptr = rhs.m_ptr;
		return *this;
	}

	~ComPtr() {
		SafeRelease(m_ptr);
	}

	void Release() {
		SafeRelease(m_ptr);
	}
	
	Interface** Receive() { return &m_ptr; }

	BlockIUnknownRefCounting* operator->() const {
		return reinterpret_cast<BlockIUnknownRefCounting*>(m_ptr);
	}

	operator Interface*() const { return m_ptr; }

private:

	Interface* m_ptr;

};

#endif

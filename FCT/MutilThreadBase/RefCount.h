#pragma once
#include<atomic>
namespace FCT {
	class RefCount {
	public:
		RefCount() : m_refCount(1) {}
		virtual ~RefCount() {};
		void addRef() {
			m_refCount++;
		}
		void release() {
			m_refCount--;
			if (m_refCount)
				return;
			delete this;
		}
		size_t count() {
			return m_refCount;
		}
	private:
		std::atomic<size_t> m_refCount;
	};
	inline void safeAddRef(RefCount* obj) {
		if (obj) obj->addRef();
	}
	inline void safeRelease(RefCount*& obj) {
		if (obj) obj->release();
        obj = nullptr;
	}

#define FCT_SAFE_RELEASE(obj) if (obj) obj->release(); obj = nullptr;
#define FCT_SAFE_ADDREF(p) if (p) { p->addRef(); }
}
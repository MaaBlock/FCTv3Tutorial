#ifndef FCT_MEMORY_CHEAK
#define FCT_MEMORY_CHEAK
#include <typeinfo>
#include <mutex>
#include <vector>
#include <iostream>

#include "../ThirdParty.h"
#ifndef FCT_USE_ANDROID
#define FCT_USE_MEMORY_CHECK
#endif
namespace FCT {
#if defined(FCT_DEBUG) && defined(FCT_USE_MEMORY_CHECK)
	struct _fct_object_t {
		void* pointer;
		std::string describe;
		int refCounted;
	};
	extern std::vector<_fct_object_t*> fct_object_list;
	extern std::string fct_object_info;
	extern std::mutex fct_object_mutex;
	extern int fct_object_count_delete_without_object;
	template <typename T, typename... Args>
	T* _fct_new(const Args&... arg) {
		T* ret = new T(arg...);
		_fct_object_t* object = new _fct_object_t;
		object->pointer = ret;
		object->refCounted = -1;
		object->describe = typeid(ret).name();
		fct_object_mutex.lock();
		fct_object_list.push_back(object);
		fct_object_mutex.unlock();
		return ret;
	}

	template <typename T, typename AUTO>
	T* _fct_news(AUTO size) {
		T* ret = new T[size];
		_fct_object_t* object = new _fct_object_t;
		object->pointer = ret;
		object->refCounted = -1;
		object->describe = typeid(ret).name();
		fct_object_mutex.lock();
		fct_object_list.push_back(object);
		fct_object_mutex.unlock();
		return ret;
	}
	template <typename T>
	void _fct_delete(T arg) {
		fct_object_mutex.lock();
		auto i = fct_object_list.begin();
		while (i != fct_object_list.end()) {
			if ((*i)->pointer == arg) {
				delete (*i);
				fct_object_list.erase(i);
				goto FinshWhile;
			}
			i++;
		}
		if (i == fct_object_list.end()) {
			fct_object_count_delete_without_object++;
		}
	FinshWhile:
		fct_object_mutex.unlock();
		delete arg;
	}
	template <typename T>
	void _fct_deletes(T arg) {
		auto i = fct_object_list.begin();
		for (; i != fct_object_list.end(); i++) {
			if ((*i)->pointer == arg) {
				delete (*i);
				fct_object_list.erase(i);
				goto FinshWhiles;
			}
		}
		fct_object_count_delete_without_object++;
	FinshWhiles:
		delete[] arg;
	}
	inline void _output_object_nums(std::ostream& out) {
		out << "当前对象总计:" << fct_object_list.size() << std::endl;
	}
	inline void _output_object(std::ostream& out) {
		//线程不安全
		for (auto i = fct_object_list.begin(); i != fct_object_list.end(); i++) {
			out << (*i)->describe << std::endl;
		}
		out << "不在object表里不delete的数量:" << fct_object_count_delete_without_object << std::endl;
		out << "未释放对象总计:" << fct_object_list.size() << std::endl;
	}
#endif
	inline void _add_object(void* ptr, const std::string& describe)
	{
		_fct_object_t* object = new _fct_object_t;
		object->pointer = ptr;
		object->refCounted = -1;
		object->describe = describe;
		fct_object_mutex.lock();
		fct_object_list.push_back(object);
		fct_object_mutex.unlock();
	}
	inline bool _remove_object(void* ptr)
	{
		auto i = fct_object_list.begin();
		for (; i != fct_object_list.end(); i++) {
			if ((*i)->pointer == ptr) {
				delete (*i);
				fct_object_list.erase(i);
				return true;
			}
		}
		fct_object_count_delete_without_object++;
		return false;
	}
	template<typename T>
	inline T* AllocatorNew(size_t size)
	{
#ifdef FCT_DEBUG
		auto ret = static_cast<T*>(operator new[](sizeof(T) * size));
		_add_object(ret,std::string(typeid(ret).name()) + std::string(" Chunk Size: ") + std::to_string(size));
		return ret;
#else
		return static_cast<T*>(operator new[](sizeof(T) * size));
#endif
	}
	template<typename T>
	inline void AllocatorDelete(T* ptr)
	{
		FCT::_remove_object(ptr);
		operator delete[](ptr);
	}
	template<typename T>
	inline T* New(T* ptr)
	{
		return new (ptr) T();
	}

	template<typename T, typename... Args>
	inline T* New(T* ptr, Args&&... args)
	{
		return new (ptr) T(std::forward<Args>(args)...);
	}
	template<typename T>
	inline void Destroy(T* ptr)
	{
		if (ptr) {
			ptr->~T();
		}
	}
}
#if defined(FCT_DEBUG) && defined(FCT_USE_MEMORY_CHECK)
#define FCT_NEW(type,...) FCT::_fct_new<type>(__VA_ARGS__)
#define FCT_NEWS(type,num) FCT::_fct_news<type>(num)
#define FCT_DELETE(args) FCT::_fct_delete<decltype(args)>(args)
#define FCT_DELETES(args) FCT::_fct_deletes<decltype(args)>(args)
#else
#define FCT_NEW(type,...) new type(__VA_ARGS__)
#define FCT_NEWS(type,num) new type[num]
#define FCT_DELETE(args) delete args
#define FCT_DELETES(args) delete[] args
#endif // FCT_DEBUG

#endif // FCT_MEMORY_CHEAK

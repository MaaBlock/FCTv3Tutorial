//
// Created by Administrator on 2025/5/8.
//
#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H
#include "../DebugTools/MemoryCheak.h"
#include "../MutilThreadBase/RefCount.h"
namespace FCT
{
    constexpr size_t SingleChunkObjectNums = 1024;
    template<typename T>
    class ObjectPoolChunk
    {
    public:
        ObjectPoolChunk() : m_objects(AllocatorNew<T>(SingleChunkObjectNums))
        {
            m_size = 0;
        }
        ObjectPoolChunk(const ObjectPoolChunk& other) = delete;
        ObjectPoolChunk(ObjectPoolChunk&& other) noexcept
            : m_size(other.m_size), m_objects(other.m_objects)
        {
            other.m_objects = nullptr;
            other.m_size = 0;
        }
        ~ObjectPoolChunk()
        {
            if (m_objects)
            {
                clear();
                AllocatorDelete(m_objects);
            }
        }
        template<typename... Args>
        T* addSingle(size_t index,Args... args)
        {
            m_size = index + 1;
            return New<T>(m_objects + index,
                          std::forward<Args>(args)...);
        }
        T* addSingle(size_t index)
        {
            m_size = index + 1;
            return New<T>(m_objects + index);
        }
        T* get(size_t index) const
        {
            return m_objects + index;
        }
        void clear()
        {
            if (m_objects)
            {
                for (size_t i = 0; i < m_size; ++i)
                {
                    Destroy(m_objects + i);
                }
                m_size = 0;
            }
        }
    protected:
        size_t m_size;
        T* m_objects;
    };
    template<typename T, typename... Args>
    class ObjectPool
    {
    public:
        using DestroyCallback = std::function<void(T*)>;
        explicit ObjectPool(Args... args) :
        m_size(0),
        m_destroyCallback(nullptr),
        m_constructorArgs(std::forward<Args>(args)...)
        {

        }
        ~ObjectPool()
        {

        }
        T* addSingle()
        {
            T* ret;
            if (m_size % SingleChunkObjectNums)
            {
                ret = std::apply([this](auto&&... args)
                {
                    return m_chunks[m_size / SingleChunkObjectNums]
                           .addSingle(m_size % SingleChunkObjectNums,
                           std::forward<decltype(args)>(args)...);
                }, m_constructorArgs);
            } else
            {
                m_chunks.emplace_back();
                ret = std::apply([this](auto&&... args)
                {
                    return m_chunks.back()
                           .addSingle(m_size % SingleChunkObjectNums,
                           std::forward<decltype(args)>(args)...);
                }, m_constructorArgs);
            }
            if constexpr (FCT::IsDeletableTrait<T>::value)
            {
                if (m_destroyCallback)
                {
                    size_t index = m_size;
                    ret->setDeleter([this,index](T* ptr)
                    {
                        m_destroyCallback(ptr);
                        m_freeIndices.push(index);
                    });
                }
            } else
            {
                //不应该出现这个 分支
            }
            m_size++;
            return ret;
        }
        T* alloc()
        {
            size_t index;
            if (m_freeIndices.pop(index))
            {

                return m_chunks[index / SingleChunkObjectNums].get(index % SingleChunkObjectNums);
            }
            else
            {
                return addSingle();
            }
        }
        void setDestroyCallback(DestroyCallback callback)
        {
            m_destroyCallback = std::move(callback);
        }
    protected:
        size_t m_size;
        std::vector<ObjectPoolChunk<T>> m_chunks;
        boost::lockfree::queue<size_t,boost::lockfree::capacity<1024>> m_freeIndices;
        DestroyCallback m_destroyCallback;
        std::tuple<Args...> m_constructorArgs;
    };
}
#endif //OBJECTPOOL_H

#pragma once

#include <atomic>
#include <utility>
#include <type_traits>
#include <mutex>
#include <functional>

namespace pgn {

    // Forward declarations
    class RefCounted;
    template <typename T> class Ref;
    template <typename T> class WeakRef;

    /**
     * Internal block to track if an object is still alive.
     */
    struct WeakRefBlock 
    {
        std::atomic<int> weakCount{1};
        void* objectPtr = nullptr; 
        std::mutex mtx;            

        void DecWeak() 
        {
            if (weakCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
                delete this;
        }
    };

    /**
     * Tag used to construct a Ref without incrementing the count (for internal use)
     */
    struct AdoptRefTag {};

    /**
     * Base class for all engine assets and layers.
     */
    class RefCounted 
    {
    public:
        RefCounted() = default;
        virtual ~RefCounted() 
        {
            WeakRefBlock* block = m_weakBlock.load(std::memory_order_relaxed);
            if (block) 
            {
                {
                    std::lock_guard<std::mutex> lock(block->mtx);
                    block->objectPtr = nullptr;
                }
                block->DecWeak();
            }
        }

        int GetRefCount() const { return m_refCount.load(std::memory_order_relaxed); }

        // Disable copying/assignment of the base class to prevent ref-count corruption
        RefCounted(const RefCounted&) = delete;
        RefCounted& operator=(const RefCounted&) = delete;
    
    private:

        void AddRef() const { m_refCount.fetch_add(1, std::memory_order_relaxed); }

        // Only increments if the object hasn't started destruction
        bool TryAddRef() const 
        {
            int count = m_refCount.load(std::memory_order_acquire);
            while (count > 0) 
            {
                if (m_refCount.compare_exchange_weak(count, count + 1, std::memory_order_acq_rel)) 
                    return true;
            }
            return false;
        }

        void Release() const 
        {
            if (m_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) 
                delete this;
        }

        WeakRefBlock* GetWeakBlock() const 
        {
            WeakRefBlock* block = m_weakBlock.load(std::memory_order_acquire);
            if (!block) 
            {
                WeakRefBlock* newBlock = new WeakRefBlock();
                newBlock->objectPtr = const_cast<RefCounted*>(this);
                
                if (m_weakBlock.compare_exchange_strong(block, newBlock, std::memory_order_acq_rel)) 
                    block = newBlock;
                else 
                    delete newBlock;
            }
            return block;
        }

    private:
        mutable std::atomic<int> m_refCount{0};
        mutable std::atomic<WeakRefBlock*> m_weakBlock{nullptr};

        template <typename U> friend class WeakRef;
        template <typename U> friend class Ref;
    };

    /**
     * pgn::WeakRef<T> - Weak Intrusive Smart Pointer
     */
    template <typename T>
    class WeakRef 
    {
    public:
        WeakRef() : m_ptr(nullptr), m_block(nullptr) {}

        WeakRef(const Ref<T>& strong) : m_ptr(strong.get()), m_block(nullptr) 
        {
            if (m_ptr) 
            {
                m_block = m_ptr->GetWeakBlock();
                m_block->weakCount.fetch_add(1, std::memory_order_relaxed);
            }
        }

        WeakRef(const WeakRef& other) : m_ptr(other.m_ptr), m_block(other.m_block) 
        {
            if (m_block) m_block->weakCount.fetch_add(1, std::memory_order_relaxed);
        }

        WeakRef(WeakRef&& other) noexcept : m_ptr(other.m_ptr), m_block(other.m_block) 
        {
            other.m_ptr = nullptr;
            other.m_block = nullptr;
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
        WeakRef(const WeakRef<U>& other) : m_ptr(other.m_ptr), m_block(other.m_block) 
        {
            if (m_block) m_block->weakCount.fetch_add(1, std::memory_order_relaxed);
        }

        ~WeakRef() { if (m_block) m_block->DecWeak(); }

        WeakRef& operator=(const WeakRef& other) 
        {
            if (this != &other) 
            {
                if (m_block) m_block->DecWeak();
                m_ptr = other.m_ptr;
                m_block = other.m_block;
                if (m_block) m_block->weakCount.fetch_add(1, std::memory_order_relaxed);
            }
            return *this;
        }

        WeakRef& operator=(WeakRef&& other) noexcept 
        {
            if (this != &other)
            {
                if (m_block) m_block->DecWeak();
                m_ptr = other.m_ptr;
                m_block = other.m_block;
                other.m_ptr = nullptr;
                other.m_block = nullptr;
            }
            return *this;
        }

        Ref<T> lock() const 
        {
            if (!m_block) return Ref<T>(nullptr);
            std::lock_guard<std::mutex> lock(m_block->mtx);
            if (m_block->objectPtr) 
            {
                T* obj = static_cast<T*>(m_block->objectPtr);
                if (obj->TryAddRef()) 
                    return Ref<T>(obj, AdoptRefTag{});
            }
            return Ref<T>(nullptr);
        }

        bool expired() const 
        {
            if (!m_block) return true;
            std::lock_guard<std::mutex> lock(m_block->mtx);
            return m_block->objectPtr == nullptr;
        }

        void* GetHashKey() const { return m_block; }

        template <typename U> bool operator==(const WeakRef<U>& other) const { return m_ptr == other.m_ptr; }
        template <typename U> bool operator!=(const WeakRef<U>& other) const { return m_ptr != other.m_ptr; }
        bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
        bool operator!=(std::nullptr_t) const { return m_ptr != nullptr; }

    private:
        T* m_ptr;
        WeakRefBlock* m_block;

        template <typename U> friend class WeakRef;
        template <typename U> friend class Ref;
    };

    /**
     * pgn::Ref<T> - Strong Intrusive Smart Pointer
     */
    template <typename T>
    class Ref 
    {
    public:
        Ref(T* ptr = nullptr) : m_ptr(ptr) { if (m_ptr) m_ptr->AddRef(); }
        Ref(T* ptr, AdoptRefTag) : m_ptr(ptr) {} 
        
        Ref(const Ref& other) : m_ptr(other.m_ptr) { if (m_ptr) m_ptr->AddRef(); }
        Ref(Ref&& other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
        Ref(const Ref<U>& other) : m_ptr(other.get()) { if (m_ptr) m_ptr->AddRef(); }

        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
        Ref(Ref<U>&& other) noexcept : m_ptr(other.get()) { other.m_ptr = nullptr; }

        ~Ref() { if (m_ptr) m_ptr->Release(); }

        Ref& operator=(const Ref& other) 
        {
            if (this != &other) 
            {
                if (m_ptr) m_ptr->Release();
                m_ptr = other.m_ptr;
                if (m_ptr) m_ptr->AddRef();
            }
            return *this;
        }

        Ref& operator=(Ref&& other) noexcept 
        {
            if (this != &other) 
            {
                if (m_ptr) m_ptr->Release();
                m_ptr = other.m_ptr;
                other.m_ptr = nullptr;
            }
            return *this;
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
        Ref& operator=(Ref<U>&& other) noexcept 
        {
            if (m_ptr != other.get()) 
            {
                if (m_ptr) m_ptr->Release();
                m_ptr = other.get();
                other.m_ptr = nullptr;
            }
            return *this;
        }

        /**
         * Relinquishes ownership of the pointer without decrementing the ref count.
         * Returns the raw pointer and sets the internal pointer to nullptr.
         */
        T* release() noexcept 
        {
            T* temp = m_ptr;
            m_ptr = nullptr;
            return temp;
        }

        T* get() const { return m_ptr; }
        T* operator->() const { return m_ptr; }
        T& operator*() const { return *m_ptr; }
        explicit operator bool() const { return m_ptr != nullptr; }

        template <typename U> bool operator==(const Ref<U>& other) const { return m_ptr == other.get(); }
        template <typename U> bool operator!=(const Ref<U>& other) const { return m_ptr != other.get(); }
        bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
        bool operator!=(std::nullptr_t) const { return m_ptr != nullptr; }

    private:
        mutable T* m_ptr;
        template <typename U> friend class Ref;
        template <typename U> friend class WeakRef;
    };

    template<typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args) { return Ref<T>(new T(std::forward<Args>(args)...)); }

    /**
     * Static cast for an lvalue Ref (increments ref count)
     */
    template <typename T, typename U>
    Ref<T> StaticRefCast(const Ref<U>& other) { return Ref<T>(static_cast<T*>(other.get())); }

    /**
     * Static cast for an rvalue Ref (moves pointer, 0 atomic ops)
     */
    template <typename T, typename U>
    Ref<T> StaticRefCast(Ref<U>&& other) noexcept { return Ref<T>(static_cast<T*>(other.release()), AdoptRefTag{}); }

    /**
     * Dynamic cast for an lvalue Ref
     */
    template <typename T, typename U>
    Ref<T> DynamicRefCast(const Ref<U>& other) 
    {
        if (auto* ptr = dynamic_cast<T*>(other.get())) 
            return Ref<T>(ptr);
        return Ref<T>(nullptr);
    }

    /**
     * Dynamic cast for an rvalue Ref
     */
    template <typename T, typename U>
    Ref<T> DynamicRefCast(Ref<U>&& other) noexcept 
    {
        if (auto* ptr = dynamic_cast<T*>(other.get())) 
        {
            other.release(); 
            return Ref<T>(ptr, AdoptRefTag{});
        }
        return Ref<T>(nullptr);
    }
}

// STL compatibility
namespace std 
{
    template <typename T>
    struct hash<pgn::Ref<T>> 
    {
        size_t operator()(const pgn::Ref<T>& ref) const noexcept { return hash<T*>{}(ref.get()); }
    };

    template <typename T>
    struct hash<pgn::WeakRef<T>> 
    {
        size_t operator()(const pgn::WeakRef<T>& ref) const noexcept { return hash<void*>{}(ref.GetHashKey()); }
    };
}
#pragma once

#include <utility>
#include <functional>

namespace Infra
{
    template<typename T>
    class ResPtr
    {
    public:
        ResPtr(T* p) // NOLINT(*-explicit-constructor)
            : _ptr(p)
            , _cleanup(DefaultDeleter)
        {
        }

        ResPtr(T* p, const std::function<void(T*)>& cleanup)
            : _ptr(p)
            , _cleanup(cleanup)
        {
        }

        ResPtr(const ResPtr& rhs) = delete;

        ResPtr& operator=(const ResPtr& rhs) = delete;

        ResPtr(ResPtr&& rhs) noexcept : _ptr(rhs._ptr)
        {
            rhs._ptr = nullptr;
        }

        ResPtr& operator=(ResPtr&& rhs) noexcept
        {
            if (&rhs == this)
                return *this;

            CleanUp(_ptr);
            _ptr = rhs._ptr;
            rhs._ptr = nullptr;

            return *this;
        }

        ~ResPtr()
        {
            CleanUp(_ptr);
        }

        T& operator*() const
        {
            return *_ptr;
        }

        T* operator->() const
        {
            return _ptr;
        }

        bool operator==(const ResPtr& rhs) const
        {
            return _ptr == rhs._ptr;
        }

        bool operator==(const T* p) const
        {
            return _ptr == p;
        }

        explicit operator bool() const
        {
            return _ptr != nullptr;
        }

    public:
        T* Release()
        {
            T* temp = _ptr;
            _ptr = nullptr;
            return temp;
        }

        T* Get()
        {
            return _ptr;
        }

        const T* Get() const
        {
            return _ptr;
        }

        void FreeOldAndReset(const T* newRes)
        {
            const T* temp = _ptr;
            _ptr = newRes;
            CleanUp(temp);
        }

        T* GetOldAndReset(const T* newRes)
        {
            const T* temp = _ptr;
            _ptr = newRes;
            return temp;
        }

        template<typename... Args>
        static ResPtr Make(Args&&... args, const std::function<void(T*)>& cleanup = DefaultDeleter)
        {
            T* p = new T(std::forward<Args>(args)...);
            return ResPtr(p, cleanup);
        }

        static void DefaultDeleter(T* p)
        {
            delete p;
        }

    private:

        void CleanUp(T* target)
        {
            if (target == nullptr)
                return;

            if (_cleanup != nullptr)
                _cleanup(target);
        }

    private:
        T* _ptr;
        std::function<void(T*)> _cleanup;
    };
}

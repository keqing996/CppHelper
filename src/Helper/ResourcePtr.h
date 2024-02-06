#pragma once

namespace Helper
{
    template<typename T>
    class ResPtr
    {
    public:
        ResPtr() = default;

        ResPtr(const T* p): _ptr(p) // NOLINT(*-explicit-constructor)
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

            delete _ptr;
            _ptr = rhs._ptr;
            rhs._ptr = nullptr;

            return *this;
        }

        ~ResPtr()
        {
            delete _ptr;
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
            delete temp;
        }

        T* GetOldAndReset(const T* newRes)
        {
            const T* temp = _ptr;
            _ptr = newRes;
            return temp;
        }

    private:
        T* _ptr = nullptr;
    };
}

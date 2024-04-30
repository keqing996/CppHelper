#pragma once

#include <functional>
#include "NonCopyable.hpp"

namespace Infra
{
    class ScopeGuard : public NonCopyable
    {
    public:
        ScopeGuard(ScopeGuard&& other) = delete;

        ~ScopeGuard()
        {
            _cleanup();
        }

        template<class Func>
        ScopeGuard(const Func& cleanup) // NOLINT(*-explicit-constructor)
            : _cleanup(cleanup)
        {
        }

    private:
        std::function<void()> _cleanup;
    };
}
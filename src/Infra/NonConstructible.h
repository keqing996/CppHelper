#pragma once

namespace Infra
{
    class NonConstructible
    {
    public:
        NonConstructible() = delete;
        ~NonConstructible() = delete;
    };
}
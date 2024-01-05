#pragma once

namespace Helper
{
    class NonConstructible
    {
    public:
        NonConstructible() = delete;
        ~NonConstructible() = delete;
    };
}
#pragma once

#include <string>

namespace Infra
{
    class CommandLine
    {
    public:
        CommandLine() = default;

    private:
        class Option
        {
        public:
            Option(const std::string& name, char shortName, const std::string& desc)
                : _fullName(name)
                , _shortName(shortName)
                , _desc(desc)
            {
            }

        public:
            const std::string& GetFullName()
            {
                return _fullName;
            }

            char GetShortName()
            {
                return _shortName;
            }

            const std::string GetDesc()
            {
                return _desc;
            }

        public:
            virtual bool HasValue() = 0;
            virtual bool Settle() = 0;

        private:
            std::string _fullName;
            char _shortName;
            std::string _desc;
        };

        class OptionNoValue: Option
        {
        public:
            OptionNoValue(const std::string& name, char shortName, const std::string& desc)
                : Option(name, shortName, desc)
                , _settle(false)
            {
            }

        public:
            bool HasValue() override
            {
                return false;
            }

            bool Set()
            {
                _settle = true;
                return true;
            }

            bool Settle() override
            {
                return _settle;
            }

        private:
            bool _settle;
        };

        template<typename T>
        class OptionWithValue : Option
        {
        public:


        public:

        private:

        };

    public:
        void AddOption(const std::string& fullName, char shortName, const std::string& desc);

        template<typename T>
        void AddOption(const std::string& fullName, char shortName, const std::string& desc)

    private:

    };
}
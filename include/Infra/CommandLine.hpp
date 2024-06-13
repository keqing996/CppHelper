#pragma once

#include <string>
#include <sstream>
#include <vector>

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
                , _settle(false)
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

            const std::string& GetDesc()
            {
                return _desc;
            }

            bool Settle() const
            {
                return _settle;
            }

        public:
            virtual bool HasValue() = 0;

        protected:
            std::string _fullName;
            char _shortName;
            std::string _desc;
            bool _settle;
        };

        class OptionNoValue: Option
        {
        public:
            OptionNoValue(const std::string& name, char shortName, const std::string& desc)
                : Option(name, shortName, desc)
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
        };

        template<typename T>
        class OptionSingleValue : Option
        {
        public:
            OptionSingleValue(const std::string& name, char shortName, const std::string& desc)
                : Option(name, shortName, desc)
            {
            }

            OptionSingleValue(const std::string& name, char shortName, const std::string& desc, T defaultValue)
                : Option(name, shortName, desc)
                , _value(defaultValue)
            {
            }

        public:
            bool HasValue() override
            {
                return true;
            }

            void SetValue(const std::string& str)
            {
                std::istringstream ss(str);
                if (!(ss >> _value && ss.eof()))
                    throw std::bad_cast();

                _settle = true;
            }

            const T& GetValue()
            {
                return _value;
            }

        private:
            T _value;
        };

        class OptionMultiValue : Option
        {
        public:
            OptionMultiValue(const std::string& name, char shortName, const std::string& desc)
                : Option(name, shortName, desc)
            {
            }

        public:
            bool HasValue() override
            {
                return true;
            }

            void AddValue(const std::string& str)
            {
                _values.push_back(str);
            }

            size_t ValueCount() const
            {
                return _values.size();
            }

        private:
            std::vector<std::string> _values;
        };



    public:
        void AddOption(const std::string& fullName, char shortName, const std::string& desc);

        template<typename T>
        void AddOption(const std::string& fullName, char shortName, const std::string& desc)

    private:

    };
}
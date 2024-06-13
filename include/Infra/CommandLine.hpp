#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <functional>

namespace Infra
{
    class CommandLine
    {
    public:
        CommandLine() = default;

    private:
        template<typename T>
        static T Convert(const std::string& str)
        {
            if constexpr (std::is_same_v<T, std::string>)           return str;
            if constexpr (std::is_same_v<T, int>)                   return std::stoi(str);
            if constexpr (std::is_same_v<T, float>)                 return std::stof(str);
            if constexpr (std::is_same_v<T, double>)                return std::stod(str);
            if constexpr (std::is_same_v<T, long>)                  return std::stol(str);
            if constexpr (std::is_same_v<T, unsigned long>)         return std::stoul(str);
            if constexpr (std::is_same_v<T, long long>)             return std::stoll(str);
            if constexpr (std::is_same_v<T, unsigned long long>)    return std::stoull(str);
            if constexpr (std::is_same_v<T, long double>)           return std::stold(str);

            T result;
            std::istringstream strStream(str);
            if (!(strStream >> result && strStream.eof()))
                throw std::bad_cast();

            return result;
        }

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

        class OptionSingleValue : Option
        {
        public:
            OptionSingleValue(const std::string& name, char shortName, const std::string& desc)
                : Option(name, shortName, desc)
            {
            }

            OptionSingleValue(const std::string& name, char shortName, const std::string& desc, const std::string& defaultValue)
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
                _value = str;
                _settle = true;
            }

            template<typename T>
            const T& GetValue(const std::function<T(const std::string&)>& converter)
            {
                return converter(_value);
            }

            template<typename T>
            const T& GetValue()
            {
                return Convert<T>(_value);
            }

        private:
            std::string _value;
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

            const std::vector<std::string>& GetValue() const
            {
                return _values;
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
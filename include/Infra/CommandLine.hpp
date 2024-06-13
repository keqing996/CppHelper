#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>

namespace Infra
{
    class CommandLine
    {
    public:
        CommandLine() = default;

        ~CommandLine()
        {
            for (auto& [key, pOption]: _optionMap)
                delete pOption;
        }

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

        enum class OptionType
        {
            NoValue,
            SingleValue,
            MultiValue
        };

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
            virtual OptionType Type() = 0;

        protected:
            std::string _fullName;
            char _shortName;
            std::string _desc;
            bool _settle;

        };

        class OptionNoValue: public Option
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

            OptionType Type() override
            {
                return OptionType::NoValue;
            }

            bool Set()
            {
                _settle = true;
                return true;
            }
        };

        class OptionSingleValue : public Option
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

            OptionType Type() override
            {
                return OptionType::SingleValue;
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

        class OptionMultiValue : public Option
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

            OptionType Type() override
            {
                return OptionType::MultiValue;
            }

            void AddValue(const std::string& str)
            {
                _values.push_back(str);
                _settle = true;
            }

            size_t ValueCount() const
            {
                return _values.size();
            }

            const std::vector<std::string>& GetValuesStringRaw() const
            {
                return _values;
            }

            template<typename T>
            T GetValueAt(int index)
            {
                return Convert<T>(_values[index]);
            }

            template<typename T>
            T GetValueAt(int index, const std::function<T(const std::string&)>& converter)
            {
                return converter(_values[index]);
            }

        private:
            std::vector<std::string> _values;
        };

    public:
        template<OptionType type>
        void AddOption(const std::string& fullName, char shortName, const std::string& desc)
        {
            Option* pOption = nullptr;

            if constexpr (type == OptionType::NoValue)
                pOption = new OptionNoValue(fullName, shortName, desc);
            else if constexpr (type == OptionType::SingleValue)
                pOption = new OptionSingleValue(fullName, shortName, desc);
            else if constexpr (type == OptionType::MultiValue)
                pOption = new OptionMultiValue(fullName, shortName, desc);

            _optionMap[fullName] = pOption;
        }

        void AddOption(const std::string& fullName, char shortName, const std::string& desc, const std::string& defaultValue)
        {
            _optionMap[fullName] = new OptionSingleValue(fullName, shortName, desc, defaultValue);
        }

    private:
        std::unordered_map<std::string, Option*> _optionMap;
    };
}
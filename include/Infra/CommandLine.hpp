#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <optional>
#include <Infra/Assert.hpp>

namespace Infra
{
    enum class CmdOptionType
    {
        NoValue,
        SingleValue,
        MultiValue
    };

    class CommandLine
    {
    public:
        CommandLine() = default;

        ~CommandLine()
        {
            for (auto& pOption: _allOptions)
                delete pOption;
        }

    public:
        class Option
        {
        public:
            Option(const std::string& desc)
                : _desc(desc)
                , _settle(false)
            {
            }

        public:

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
            virtual CmdOptionType Type() = 0;

        protected:
            std::string _desc;
            bool _settle;

        };

        class OptionNoValue: public Option
        {
        public:
            OptionNoValue(const std::string& desc)
                : Option(desc)
            {
            }

        public:
            bool HasValue() override
            {
                return false;
            }

            CmdOptionType Type() override
            {
                return CmdOptionType::NoValue;
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
            OptionSingleValue(const std::string& desc)
                : Option(desc)
            {
            }

        public:
            bool HasValue() override
            {
                return true;
            }

            CmdOptionType Type() override
            {
                return CmdOptionType::SingleValue;
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
            OptionMultiValue(const std::string& desc)
                : Option(desc)
            {
            }

        public:
            bool HasValue() override
            {
                return true;
            }

            CmdOptionType Type() override
            {
                return CmdOptionType::MultiValue;
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

        template<CmdOptionType type>
        static Option* CreateOption(const std::string& desc)
        {
            Option* pOption = nullptr;

            if constexpr (type == CmdOptionType::NoValue)
                pOption = new OptionNoValue(desc);
            else if constexpr (type == CmdOptionType::SingleValue)
                pOption = new OptionSingleValue(desc);
            else if constexpr (type == CmdOptionType::MultiValue)
                pOption = new OptionMultiValue(desc);

            return pOption;
        }

    public:
        void SetExitWhenErrorInput(bool doAbort)
        {
            _exitWhenErrorInput = doAbort;
        }

        void SetErrorInputExitCode(int code)
        {
            _errorInputExitCode = code;
        }

        void SetHelpPrintMessageFunc(const std::function<void()>& func)
        {
            _printHelpMessageFunc = func;
        }

        template<CmdOptionType type>
        void AddOption(const std::string& fullName, char shortName, const std::string& desc)
        {
            Option* pOption = CreateOption<type>(desc);

            ASSERT_MSG(_fullNameOptionMap.find(fullName) != _fullNameOptionMap.end(), "Command line duplicate full name option");
            ASSERT_MSG(_shortNameOptionMap.find(shortName) != _shortNameOptionMap.end(), "Command line duplicate short name option");

            _allOptions.push_back(pOption);
            _fullNameOptionMap[fullName] = pOption;
            _shortNameOptionMap[shortName] = pOption;
        }

        template<CmdOptionType type>
        void AddOption(const std::string& fullName, const std::string& desc)
        {
            Option* pOption = CreateOption<type>(desc);

            ASSERT_MSG(_fullNameOptionMap.find(fullName) != _fullNameOptionMap.end(), "Command line duplicate full name option");

            _allOptions.push_back(pOption);
            _fullNameOptionMap[fullName] = pOption;
        }

        template<CmdOptionType type>
        void AddOption(char shortName, const std::string& desc)
        {
            Option* pOption = CreateOption<type>(desc);

            ASSERT_MSG(_shortNameOptionMap.find(shortName) != _shortNameOptionMap.end(), "Command line duplicate short name option");

            _allOptions.push_back(pOption);
            _shortNameOptionMap[shortName] = pOption;
        }

        void Parse(int argc, char** argv)
        {
            if (argc == 2 && (argv[1] == "-h" || argv[1] == "-help"))
            {
                PrintHelpMessage();
                std::exit(0);
            }

            _invalidInputRecord.clear();
            int index = 1;

            auto ProcessOption = [&](Option* pOption) -> void
            {
                switch (pOption->Type())
                {
                    case CmdOptionType::NoValue:
                    {
                        auto* pNoValueOption = dynamic_cast<OptionNoValue*>(pOption);
                        pNoValueOption->Set();
                        break;
                    }
                    case CmdOptionType::SingleValue:
                    {
                        auto* pSingleValueOption = dynamic_cast<OptionSingleValue*>(pOption);
                        if (index + 1 < argc)
                        {
                            pSingleValueOption->SetValue(argv[index + 1]);
                            index++;
                        }
                        break;
                    }
                    case CmdOptionType::MultiValue:
                    {
                        auto* pMultiValueOption = dynamic_cast<OptionMultiValue*>(pOption);
                        while (index + 1 < argc)
                        {
                            std::string next(argv[index + 1]);
                            if (GetFullName(next) || GetShortName(next))
                                break;

                            pMultiValueOption->AddValue(next);
                            index++;
                        }
                        break;
                    }
                }
            };

            auto TryProcessFullName = [&](const std::string& str) -> bool
            {
                const auto fullName = GetFullName(str);
                if (!fullName)
                    return false;

                const auto& key = *fullName;
                if (const auto itr = _fullNameOptionMap.find(key); itr == _fullNameOptionMap.end())
                {
                    if (_exitWhenErrorInput)
                    {
                        std::cout << "Invalid option: " << key << std::endl;
                        std::exit(_errorInputExitCode);
                    }

                    _invalidInputRecord.push_back(str);
                }
                else
                {
                    ProcessOption(itr->second);
                }

                return true;
            };

            auto TryProcessShortName = [&](const std::string& str) -> bool
            {
                const auto fullName = GetShortName(str);
                if (!fullName)
                    return false;

                const auto key = *fullName;
                if (const auto itr = _shortNameOptionMap.find(key); itr == _shortNameOptionMap.end())
                {
                    if (_exitWhenErrorInput)
                    {
                        std::cout << "Invalid option: " << key << std::endl;
                        std::exit(_errorInputExitCode);
                    }

                    _invalidInputRecord.push_back(str);
                }
                else
                {
                    ProcessOption(itr->second);
                }

                return true;
            };

            for (; index < argc; index++)
            {
                std::string str(argv[index]);

                if (TryProcessFullName(str))
                    continue;

                if (TryProcessShortName(str))
                    continue;

                _invalidInputRecord.push_back(str);
            }
        }

        const std::vector<std::string>& GetInvalidInput() const
        {
            return _invalidInputRecord;
        }

    private:
        std::optional<std::string> GetFullName(const std::string& input)
        {
            if (input.size() > 2 && input[0] == '-' && input[1] == '-')
                return input.substr(2);

            return std::nullopt;
        }

        std::optional<char> GetShortName(const std::string& input)
        {
            if (input.size() == 2 && input[0] == '-' && std::isalpha(input[1]))
                return input[1];

            return std::nullopt;
        }

        void PrintHelpMessage()
        {
            if (!_printHelpMessageFunc)
                _printHelpMessageFunc();
            else
            {

            }
        }

    private:
        // Error handle
        bool _exitWhenErrorInput = false;
        int _errorInputExitCode = 1;
        std::vector<std::string> _invalidInputRecord;

        // Options
        std::vector<Option*> _allOptions;
        std::unordered_map<std::string, Option*> _fullNameOptionMap;
        std::unordered_map<char, Option*> _shortNameOptionMap;

        // Help message
        std::function<void()> _printHelpMessageFunc;
    };
}
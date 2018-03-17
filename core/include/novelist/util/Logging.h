/**********************************************************
 * @file   Logging.h
 * @author jan
 * @date   3/14/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_LOGGING_H
#define NOVELIST_LOGGING_H

#include <string_view>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <cstring>
#include <QDebug>

#undef NOVELIST_CURRENT_FUNCTION
#ifdef __clang__
#define NOVELIST_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__)
#define NOVELIST_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define NOVELIST_CURRENT_FUNCTION __FUNCSIG__
#endif
#undef NOVELIST_CURRENT_FUNCTION_NAME
#ifdef __clang__
#define NOVELIST_CURRENT_FUNCTION_NAME __FUNCTION__
#elif defined(__GNUC__)
#define NOVELIST_CURRENT_FUNCTION_NAME __FUNCTION__
#elif defined(_MSC_VER)
#define NOVELIST_CURRENT_FUNCTION_NAME __FUNCTION__
#endif

namespace novelist::log {
    namespace internal {
        template<class T>
        constexpr std::size_t computeMagicValue() noexcept
        {
            // Get start position of template type in function string.
            // We assume the template type will always be long int for this purpose. Then the found position is
            // adjusted for the difference in function names to type_name().
            // This should work reasonably well on any compiler. Fingers crossed.
            static_assert(std::is_same_v<T, long int>);

            // TODO: Make s constexpr as soon as https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66639 is resolved.
            std::string_view const s = NOVELIST_CURRENT_FUNCTION;
            auto const pos = s.find("long int");
            auto adjustedPos = pos;
            adjustedPos += std::string_view("std::string_view").size() - std::string_view("std::size_t").size();
            adjustedPos += std::string_view("novelist::log::type_name").size() - std::string_view("novelist::log::internal::computeMagicValue").size();
            return adjustedPos;
        }
    }

    /**
     * Get the fully qualified type name as a string
     * @tparam T Type
     * @return String view of the qualified type name
     */
    template<class T>
    constexpr std::string_view type_name() noexcept
    {
        auto const magicValue = internal::computeMagicValue<long int>();
        std::string_view s = NOVELIST_CURRENT_FUNCTION;

        // Magic number gives the start of the template type within the function string. The end is marked by either
        // ';' or ']' (gcc, clang) or '>' (MVC). In the latter case we need to make sure to find only '>' that doesn't
        // match a previous opening '<', or otherwise the result will be wrong for template types.
        size_t angularBracketCount = 0;
        size_t i = magicValue;
        for (; i < s.size(); ++i)
        {
            auto const c = s.at(i);
            if (c == ';' || c == ']')
                break;
            else if (c == '<')
                angularBracketCount++;
            else if (c == '>')
            {
                if (angularBracketCount == 0)
                    break;
                angularBracketCount--;
            }
        }

        return std::string_view(s.begin() + magicValue, i - magicValue);
    }

    namespace internal {
        template <class T, class... Ts>
        constexpr QTextStream& formatValues(QTextStream& stream, T value, Ts... values) noexcept
        {
            stream << std::string(type_name<T>()).c_str() << ": " << "[" << value << "]";
            if constexpr (sizeof...(values) > 0)
            {
                stream << ", ";
                formatValues(stream, std::forward<Ts>(values)...);
            }
            return stream;
        }

        template <class T, class... Ts>
        constexpr QTextStream& format(QTextStream& stream, T value, Ts... values) noexcept
        {
            stream << value;
            if constexpr (sizeof...(values) > 0)
                format(stream, std::forward<Ts>(values)...);
            return stream;
        }

        constexpr std::pair<size_t, size_t> extractNamespace(std::string_view const& prettyFunction, std::string_view funcName) noexcept
        {
            size_t endIdx = prettyFunction.find(funcName);
            while (endIdx + funcName.size() < prettyFunction.size() && prettyFunction.at(endIdx + funcName.size()) != '(')
                endIdx = prettyFunction.find(funcName, endIdx + funcName.size());
            size_t startIdx = endIdx;
            size_t parenthesesCount = 0;
            while(startIdx > 0)
            {
                char c = prettyFunction.at(startIdx);
                if (c == ')')
                    parenthesesCount++;
                else if (c == '(')
                    parenthesesCount--;
                else if (std::isspace(c) && parenthesesCount == 0)
                    break;
                --startIdx;
            }
            return {startIdx, endIdx};
        }
    }

    /**
     * Level to use for log messages
     */
    enum class LogLevel {
        Debug,
        Info,
        Warn,
        Critical,
        Fatal,
    };

    /**
     * Log the call to the current function.
     * @details This has to be called like this:
     *          log::traceCall(NOV_CURRENT_FUNCTION, NOV_CURRENT_FUNCTION_NAME, arg1, arg2, ...)
     *          where arg1, arg2 etc. are the function arguments. To make this less cumbersome, there are the macros
     *          NOV_TRACECALL_NOARG() and NOV_TRACECALL(arg1, arg2, ...).
     * @tparam level Log level, defaults to info
     * @tparam Ts Types of arguments
     * @param prettyFunction Fully qualified function name
     * @param funcName Unqualified function name
     * @param values Function arguments
     */
    template <LogLevel level = LogLevel::Info, class... Ts>
    constexpr void traceCall(std::string_view const& prettyFunction, std::string_view funcName, Ts... values) noexcept
    {
        QString str;
        QTextStream ts(&str);
        auto [startIdx, endIdx] = internal::extractNamespace(prettyFunction, funcName);
        ts << std::string(prettyFunction.substr(startIdx, endIdx - startIdx)).c_str() << std::string(funcName).c_str() << "( ";
        if constexpr (sizeof...(values) > 0)
            internal::formatValues(ts, std::forward<Ts>(values)...);
        ts << " )";
        if constexpr (level == LogLevel::Debug)
            qDebug(str.toStdString().c_str());
        else if constexpr (level == LogLevel::Info)
            qInfo(str.toStdString().c_str());
        else if constexpr (level == LogLevel::Warn)
            qWarning(str.toStdString().c_str());
        else if constexpr (level == LogLevel::Critical)
            qCritical(str.toStdString().c_str());
        else
            qFatal(str.toStdString().c_str());
    }

/**
 * Trace the call to the current function without arguments
 */
#define NOV_TRACECALL_NOARG() novelist::log::traceCall(NOVELIST_CURRENT_FUNCTION, NOVELIST_CURRENT_FUNCTION_NAME)

/**
 * Trace the call to the current function with arguments
 */
#define NOV_TRACECALL(...) novelist::log::traceCall(NOVELIST_CURRENT_FUNCTION, NOVELIST_CURRENT_FUNCTION_NAME, ##__VA_ARGS__)

    /**
     * Print arbitrary things to the log file
     * @tparam level Log level
     * @tparam T Argument type
     * @tparam Ts More argument types
     * @param value First value
     * @param values Other values
     */
    template <LogLevel level, class T, class... Ts>
    constexpr void print(T value, Ts... values) noexcept
    {
        QString str;
        QTextStream ts(&str);
        internal::format(ts, value, std::forward<Ts>(values)...);
        if constexpr (level == LogLevel::Debug)
            qDebug(str.toStdString().c_str());
        else if constexpr (level == LogLevel::Info)
            qInfo(str.toStdString().c_str());
        else if constexpr (level == LogLevel::Warn)
            qWarning(str.toStdString().c_str());
        else if constexpr (level == LogLevel::Critical)
            qCritical(str.toStdString().c_str());
        else
            qFatal(str.toStdString().c_str());
    }

    /**
     * Print arbitrary things to the log file with debug level
     * @tparam Ts Types
     * @param values Values
     */
    template <class... Ts>
    constexpr void debug(Ts... values) noexcept
    {
        return print<LogLevel::Debug>(std::forward<Ts>(values)...);
    }

    /**
     * Print arbitrary things to the log file with info level
     * @tparam Ts Types
     * @param values Values
     */
    template <class... Ts>
    constexpr void info(Ts... values) noexcept
    {
        return print<LogLevel::Info>(std::forward<Ts>(values)...);
    }

    /**
     * Print arbitrary things to the log file with warning level
     * @tparam Ts Types
     * @param values Values
     */
    template <class... Ts>
    constexpr void warn(Ts... values) noexcept
    {
        return print<LogLevel::Warn>(std::forward<Ts>(values)...);
    }

    /**
     * Print arbitrary things to the log file with critical level
     * @tparam Ts Types
     * @param values Values
     */
    template <class... Ts>
    constexpr void critical(Ts... values) noexcept
    {
        return print<LogLevel::Critical>(std::forward<Ts>(values)...);
    }

    /**
     * Print arbitrary things to the log file with fatal level
     * @tparam Ts Types
     * @param values Values
     */
    template <class... Ts>
    constexpr void fatal(Ts... values) noexcept
    {
        return print<LogLevel::Fatal>(std::forward<Ts>(values)...);
    }

}

#endif //NOVELIST_LOGGING_H

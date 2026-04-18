#pragma once

#include <string>

#ifndef KO_DISABLE_LOGGER
#include <fmt/format.h>
#endif

namespace ko {

/// Struct representing different log levels
enum LogLevel {
    LOG_LEVEL_OFF = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_DEBUG = 3,
};

class Logger {
public:
    /**
     * Constructs a nameless logger object.
     */
    Logger()
      : Logger(std::string()) {}

    /**
     * Constructs a logger object with a given name.
     *
     * @param name The given name
     */
    explicit Logger(const std::string& name)
#ifndef KO_DISABLE_LOGGER
      : m_logLevel(getGlobalLogLevel())
      , m_name(name)
#endif
    {
        (void)name;
    }

    /**
     * Sets the local log level.
     *
     * @param logLevel The new local log level
     */
    void setLogLevel(LogLevel logLevel);

    /**
     * Returns the local log level.
     *
     * @return The local log level
     */
    [[nodiscard]] LogLevel getLogLevel() const {
#ifndef KO_DISABLE_LOGGER
        return this->m_logLevel;
#else
        return LogLevel::LOG_LEVEL_OFF;
#endif
    }

    /**
     * Prints a given DEBUG logger message.
     *
     * @tparam Args Type of the arguments
     * @param fmt The message string to be formatted
     * @param args The arguments to be inserted into the message
     */
    template <typename... Args>
    void DEBUG(const std::string& fmt, Args... args) const {
#ifndef KO_DISABLE_LOGGER
        LOG(LOG_LEVEL_DEBUG, "DEBUG", fmt, args...);
#else
        (void)fmt;
        ((void)args, ...);
#endif
    }

    /**
     * Prints a given INFO logger message.
     *
     * @tparam Args Type of the arguments
     * @param fmt The message string to be formatted
     * @param args The arguments to be inserted into the message
     */
    template <typename... Args>
    void INFO(const std::string& fmt, Args... args) const {
#ifndef KO_DISABLE_LOGGER
        LOG(LOG_LEVEL_INFO, "INFO", fmt, args...);
#else
        (void)fmt;
        ((void)args, ...);
#endif
    }

    /**
     * Prints a given ERROR logger message.
     *
     * @tparam Args Type of the arguments
     * @param fmt The message string to be formatted
     * @param args The arguments to be inserted into the message
     */
    template <typename... Args>
    void ERROR(const std::string& fmt, Args... args) const {
#ifndef KO_DISABLE_LOGGER
        LOG(LOG_LEVEL_ERROR, "ERROR", fmt, args...);
#else
        (void)fmt;
        ((void)args, ...);
#endif
    }

    /**
     * Return the global log level.
     *
     * @return The global log level
     */
    static LogLevel getGlobalLogLevel();

    /**
     * Sets the global log level.
     *
     * @param logLevel The new global log level
     */
    static void setGlobalLogLevel(LogLevel logLevel);

    /**
     * Destructor.
     */
    virtual ~Logger() = default;

private:
    /**
     * Prints the name of the logger object.
     */
    void printName() const;

#ifndef KO_DISABLE_LOGGER
    /// The local log level
    LogLevel m_logLevel;
    /// The logger object's name
    std::string m_name;
    /// The global log level
    static LogLevel m_globalLogLevel;

    /**
     * Prints a given logger message.
     *
     * @tparam Args Type of the arguments
     * @param level The message's log level
     * @param levelName The message's log level name
     * @param fmt The message string to be formatted
     * @param args The arguments to be inserted into the message
     */
    template <typename... Args>
    void LOG(LogLevel level, const std::string& levelName, const std::string& fmt, Args... args) const {
        if (getLogLevel() >= level) {
            //fmt::print(stderr, "[{:<7}] ", levelName);
            fmt::print(stderr, "[LOG][{}] ", levelName);
            printName();
            fmt::print(stderr, fmt, args...);
            fmt::print(stderr, "\n");
        }
    }
#endif
};
} // namespace ko
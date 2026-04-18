#include "Logger.hpp"

#ifndef KO_LOGGER_DEFAULT_LEVEL
#define KO_LOGGER_DEFAULT_LEVEL LOG_LEVEL_DEBUG
#endif

namespace ko {
#ifndef KO_DISABLE_LOGGER
LogLevel Logger::m_globalLogLevel(KO_LOGGER_DEFAULT_LEVEL);
#endif

void Logger::setLogLevel(LogLevel logLevel) {
#ifndef KO_DISABLE_LOGGER
    this->m_logLevel = logLevel;
#else
    (void)logLevel;
#endif
}

LogLevel Logger::getGlobalLogLevel() {
#ifndef KO_DISABLE_LOGGER
    return m_globalLogLevel;
#else
    return LOG_LEVEL_OFF;
#endif
}

void Logger::setGlobalLogLevel(LogLevel logLevel) {
#ifndef KO_DISABLE_LOGGER
    m_globalLogLevel = logLevel;
#else
    (void)logLevel;
#endif
}

void Logger::printName() const {
#ifndef KO_DISABLE_LOGGER
    if (!this->m_name.empty()) {
        fmt::print(stderr, "({}) ", this->m_name);
    }
#endif
}
} // namespace ko

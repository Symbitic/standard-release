/**
 * @file standard-release/log/ilog.h
 * @brief Interface for logging.
 */
#pragma once

#include <ostream>
#include <string>

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"

namespace StandardRelease {

class ILoggerPrivate;

/**
 * @brief Interface for logging backends.
 *
 * A logging service decoupled from the backend.
 */
class STANDARDRELEASE_EXPORT ILogger
{
public:
#if 0
    /**
     * @brief Verbosity level.
     * @note Verbosity::Debug prints all messages.
     */
    enum VerbosityLevel {
        /** Print all messages. */
        Debug,
        /** Print all information, warning, and error messages. */
        Info,
        /** Print all warning and error messages. */
        Warning,
        /** Print only error messages. */
        Error
    };

    enum MessageType {};
    enum Message {};
    enum LogLevel {
    };

    // usbEntries() will only be called if driverUsb category is enabled
    // qCDebug(driverUsb) << "devices: " << usbEntries();

#endif

    // https://doc.qt.io/qt-5/qtglobal.html#QtMessageHandler-typedef
    // https://doc.qt.io/qt-5/qtglobal.html#qInstallMessageHandler
    // https://doc.qt.io/qt-5/qmessagelogger.html
    // https://stackoverflow.com/questions/17595957/operator-overloading-in-c-for-logging-purposes

    /**
     * @brief Construct an empty logger.
     */
    ILogger();

    ILogger();

    /** Current status. */
    Error error() const;

    /**
     * @brief Open the logging backend.
     */
    virtual void open() = 0;

    /**
     * @brief Pipe to an output stream.
     */
    operator std::ostream() const;

    /**
     * @brief Pipe input to the backend for formatted printing.
     */
    template<typename T>
    ILogger &operator<<(const T &input);

protected:
private:
    ILoggerPrivate *d;

#if 0

// static ILogger* logger = new ILogger();

// allows usage of both factory method and variable in qCX macros
    QLoggingCategory &operator()() { return *this; }
    const QLoggingCategory &operator()() const { return *this; }

    void init(const char *category, QtMsgType severityLevel);

#    define qCDebug(category, ...)                                                                 \
        for (bool qt_category_enabled = category().isDebugEnabled(); qt_category_enabled;          \
             qt_category_enabled = false)                                                          \
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,                 \
                       category().categoryName())                                                  \
                .debug(__VA_ARGS__)

    void debug(const char *msg, ...) const Q_ATTRIBUTE_FORMAT_PRINTF(2, 3);
    typedef const QLoggingCategory &(*CategoryFunction)();

    void debug(const QLoggingCategory &cat, const char *msg, ...) const Q_ATTRIBUTE_FORMAT_PRINTF(3, 4);
#    define qDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

    inline QDebug(QString *string) : stream(new Stream(string)) {}
    inline QDebug &operator<<(QChar t) { putUcs4(t.unicode()); return maybeSpace(); }

// This is global, outside the class
template <class T>
inline QDebug operator<<(QDebug debug, const QList<T> &list)
{
    return QtPrivate::printSequentialContainer(debug, "" /*for historical reasons*/, list);
}
template <class Key, class T>
inline QDebug operator<<(QDebug debug, const QMap<Key, T> &map)
{
    const bool oldSetting = debug.autoInsertSpaces();
    debug.nospace() << "QMap(";
    for (typename QMap<Key, T>::const_iterator it = map.constBegin();
         it != map.constEnd(); ++it) {
        debug << '(' << it.key() << ", " << it.value() << ')';
    }
    debug << ')';
    debug.setAutoInsertSpaces(oldSetting);
    return debug.maybeSpace();
}

#endif

#if 0
    /**
     * @brief Returns the version major.
     */
    int major() const;

    /**
     * @brief Returns the version minor.
     */
    int minor() const;

    /**
     * @brief Returns the version patch number.
     */
    int patch() const;

    /**
     * @brief Different ways to increment the version number.
     */
    enum Increment
    {
        /** Increment the major (`1.2.3` -> `2.0.0` ). */
        Major,
        /** Increment the minor (`1.2.1` -> `1.3.0` ). */
        Minor,
        /** Increment the patch (`1.2.0` -> `1.2.1` ). */
        Patch,
        /** No increment was applied. */
        None,
    };

    /**
     * @brief Increment to the next specified version.
     * @param[in] inc Which increment to apply.
     */
    void increment(Increment inc);

    /**
     * @brief Compares two versions to determine which kind of increment was
     * applied.
     * @param[in] v Previous version.
     * @returns Which increment was performed.
     */
    Increment incrementType(const SemVer &v) const;

    /**
     * @brief Parse a version string.
     * @param[in] version SemVer-compatible string (e.g. "1.2.3").
     * @returns `true` if parsed correctly, `false` otherwise.
     */
    bool parse(const std::string &version);

    /**
     * @brief Convert to a string.
     */
    std::string str() const;

    /** @brief String conversion operator. */
    operator const std::string() const;

    /** @brief Input operator. */
    friend std::ostream &operator<<(std::ostream &out, const SemVer &v);

    /** @brief Move operator. */
    void operator=(const SemVer &v);
    /** eq operator. */
    bool operator==(const SemVer &v);
    /** ne operator. */
    bool operator!=(const SemVer &v);

    /** gt operator. */
    friend bool operator>(const SemVer &v1, const SemVer &v2);
    /** gte operator. */
    friend bool operator>=(const SemVer &v1, const SemVer &v2);
    /** lt operator. */
    friend bool operator<(const SemVer &v1, const SemVer &v2);
    /** lte operator. */
    friend bool operator<=(const SemVer &v1, const SemVer &v2);

private:
    int m_major;
    int m_minor;
    int m_patch;
#endif
};

}

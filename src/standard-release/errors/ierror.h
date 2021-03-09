/**
 * @file "standard-release/errors/ierror.h"
 * @brief Generic error.
 */
#pragma once

#include "standard-release/global/global.h"
#include <ostream>
#include <string>

namespace StandardRelease {

/**
 * @brief Because encapsulating errors works so much better than exceptions.
 */
class STANDARDRELEASE_EXPORT IError
{
public:
    /**
     * @brief Error codes.
     */
    enum ErrorCode
    {
        Success = 0,
        InternalError = 1,
        LastError,

        UnknownError = 255,
    };

    /**
     * @brief Default constructor for an error.
     */
    IError();

    /**
     * @brief Expanded constructor.
     * @param code Error code.
     * @param extra Extra description to add to message().
     */
    IError(ErrorCode code, const std::string extra = "");

    IError(const IError &) = default;

    /**
     * @brief Human-readable error description.
     * @returns Error description.
     */
    std::string message() const;

    /**
     * @brief Set the current error value.
     * @param code Error code.
     * @param extra Extra error description (optional).
     */
    void setError(ErrorCode code, const std::string extra = "");

    /**
     * @brief Pipe the error description to an output stream (e.g. `std::cout`).
     */
    friend std::ostream &operator<<(std::ostream &output, const IError &err)
    {
        output << err.message();
        return output;
    }

    /**
     * @brief Reassign an error value.
     */
    void operator=(const IError &err)
    {
        m_code = err.m_code;
        m_msg = err.m_msg;
    }

    /**
     * @brief Convert to a string.
     */
    operator const std::string() const
    {
        return message();
    }

    /**
     * @brief Boolean comparison.
     */
    operator bool() const
    {
        return code() != Success;
    }

    bool operator==(const ErrorCode &error_code)
    {
        return code() == error_code;
    }

    bool operator!=(const ErrorCode &error_code)
    {
        return code() != error_code;
    }

protected:
    // void setDescription();
    // void setString();
    // void setStr();

private:
    int code() const;

    int m_code;
    std::string m_msg;
};

}

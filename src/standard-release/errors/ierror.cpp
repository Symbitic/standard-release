#include "ierror.h"

using namespace StandardRelease;

IError::IError()
    : IError(ErrorCode::Success)
{
}

IError::IError(ErrorCode code, const std::string extra)
    : m_code(code)
    , m_msg(extra)
{
}

std::string IError::message() const
{
    std::string msg = "";
    switch (m_code) {
        case IError::Success:
            msg = "No error";
            break;
        case IError::InternalError:
            msg = "Internal logic error (this should not happen in production)";
            break;
        case IError::UnknownError:
        default:
            msg = "Unknown error";
            break;
    }
    return m_msg.length() > 0 ? (msg + " - " + m_msg) : msg;
}

int IError::code() const
{
    return m_code;
}

void IError::setError(ErrorCode code, const std::string extra)
{
    m_code = code;
    m_msg = extra;
}

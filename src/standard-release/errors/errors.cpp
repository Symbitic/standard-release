#include "errors.h"

using namespace StandardRelease;

Error::Error()
    : Error(ErrorCode::Success)
{
}

Error::Error(ErrorCode code, const std::string extra)
    : m_code(code)
    , m_msg(extra)
{
}

std::string Error::message() const
{
    std::string msg = "";
    switch (m_code) {
        case Error::Success:
            msg = "No error";
            break;
        case InternalError:
            msg = "Internal logic error (this should not happen in production)";
            break;
        case Error::EmptyPath:
            msg = "Required path not given";
            break;
        case Error::PathNotFound:
            msg = "Path does not exist";
            break;
        case Error::ErrorOpeningGitRepo:
            msg = "Unable to open the git repository";
            break;
        case Error::NotReady:
            msg = "Not ready";
            break;
        case Error::GitInvalidSpec:
            msg = "Unable to resolve spec";
            break;
        case Error::GitBadSignature:
            msg = "Unable to create signature";
            break;
        case Error::ErrorCreatingTag:
            msg = "Error creating tag";
            break;
        case Error::GitNoRemotes:
            msg = "No git remotes found";
            break;
        case Error::VersionInvalid:
            msg = "Invalid version string";
            break;
        case Error::ConventionalUnrecognizedType:
            msg = "Invalid commit type";
            break;
        case Error::ConfigFileInvalid:
            msg = "Invalid config file";
            break;
        case Error::ConfigFileNotFound:
            msg = "Config file not found";
            break;
        case Error::UnknownError:
        default:
            msg = "Unknown error";
            break;
    }
    return m_msg.length() > 0 ? (msg + " - " + m_msg) : msg;
}

Error::ErrorCode Error::code() const
{
    return m_code;
}

void Error::setError(ErrorCode code, const std::string extra)
{
    m_code = code;
    m_msg = extra;
}

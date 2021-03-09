/**
 * @file "standard-release/errors/error.h"
 * @brief Error handling.
 */
#pragma once

#include "standard-release/global/global.h"
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

namespace StandardRelease {

static inline std::string FILE_NOT_FOUND(const std::string &file)
{
    return "Error opening '" + file + "'";
}

static inline std::string DIRECTORY_NOT_FOUND(const std::string &dir)
{
    return dir + " does not exist";
}

/**
 * @brief Base class for all errors.
 */
struct Exception : public std::exception
{
    Exception(const std::string &msg)
        : std::exception()
        , m_msg(msg)
    {
    }

    const char *what() const throw()
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};

}

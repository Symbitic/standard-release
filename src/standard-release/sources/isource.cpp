#include "isource.h"
#include <filesystem>
#include <iostream>
#include <system_error>

using namespace StandardRelease;

struct StandardRelease::ISourcePrivate
{
    std::string filename;
    SemVer version;
    Error error;
};

ISource::ISource()
    : d(new ISourcePrivate) {};

std::string ISource::filename() const
{
    return d->filename;
}

Error ISource::error() const
{
    return d->error;
}

void ISource::setFilename(const std::string &filename)
{
    d->filename = filename;
}

void ISource::setError(const Error error)
{
    d->error = error;
}

void ISource::setVersion(const SemVer &version)
{
    d->version = version;
}

SemVer ISource::version() const
{
    return d->version;
}

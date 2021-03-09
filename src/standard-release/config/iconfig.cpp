#include "iconfig.h"

using namespace StandardRelease;

struct StandardRelease::IConfigPrivate
{
    Error error;
    std::map<std::string, std::string> data;

    IConfigPrivate()
        : data()
        , error()
    {
    }
};

IConfig::IConfig()
    : d(new IConfigPrivate)
{
}

void IConfig::setError(const Error error)
{
    d->error = error;
}

Error IConfig::error() const
{
    return d->error;
}

void IConfig::setData(const std::map<std::string, std::string> data)
{
    d->data = data;
}

std::string IConfig::operator[](const std::string key)
{
    return value(key);
}

std::string IConfig::value(const std::string &key) const
{
    const int count = d->data.count(key);
    std::string ret = "";
    if (count != 0) {
        ret = d->data[key];
    }
    return ret;
}

std::vector<std::string> IConfig::keys() const
{
    std::vector<std::string> names;
    for (const auto &[key, value] : d->data) {
        names.push_back(key);
    }
    return names;
}

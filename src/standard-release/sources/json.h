#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include "standard-release/semver/semver.h"
#include "standard-release/sources/isource.h"
#include <string>

namespace StandardRelease {

class JsonFilePrivate;

/**
 * @brief package.json
 */
class STANDARDRELEASE_EXPORT JsonFile : public ISource
{
public:
    JsonFile();

    bool detect(const std::string &dirname);
    bool save();

    std::vector<std::string> filenames() const;

protected:
    bool readFile(const std::string &filename);

private:
    JsonFilePrivate *d;
};

};

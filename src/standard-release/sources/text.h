#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include "standard-release/semver/semver.h"
#include "standard-release/sources/isource.h"
#include <string>

namespace StandardRelease {

class TextFilePrivate;

/**
 * @brief VERSION.txt
 */
class STANDARDRELEASE_EXPORT TextFile : public ISource
{
public:
    TextFile();

    bool detect(const std::string &dirname);
    bool save();

    std::vector<std::string> filenames() const;

protected:
    bool readFile(const std::string &filename);

private:
    TextFilePrivate *d;
};

};

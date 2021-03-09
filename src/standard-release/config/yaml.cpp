#include "standard-release/config/yaml.h"
#include "standard-release/errors/error.h"
#include "yaml-cpp/yaml.h"

using namespace StandardRelease;

struct StandardRelease::YamlConfigPrivate
{
    std::string fileName;
};

std::string INVALID_YAML_FILE(const std::string &file)
{
    return file + " is not a valid YAML file";
}

std::string MISSING_REQUIRED_KEY(const std::string &key, const std::string &file)
{
    return "Missing required '" + key + "' in " + file;
}

YamlConfig YamlConfig::parse(const std::string &file)
{
    YamlConfig config(file);
    config.parse();
    return config;
}

/*
YAML::Node node = YAML::LoadFile("config.yaml");
auto name = node["name"].as<std::string>();
*/

YamlConfig::YamlConfig()
    : IConfig()
    , d(new YamlConfigPrivate)
{
}

YamlConfig::YamlConfig(const std::string &fileName)
    : YamlConfig()
{
    d->fileName = fileName;
}

std::string YamlConfig::filename() const
{
    return d->fileName;
}

void YamlConfig::setFilename(const std::string &fileName)
{
    d->fileName = fileName;
}

bool YamlConfig::parse()
{
    YAML::Node node;
    std::map<std::string, std::string> data;

    try {
        node = YAML::LoadFile(filename());
    } catch (YAML::BadFile e) {
        setError(Error::ConfigFileNotFound);
        throw Exception(FILE_NOT_FOUND(filename()));
        return false;
    } catch (YAML::ParserException e) {
        setError(Error::ConfigFileInvalid);
        throw Exception(INVALID_YAML_FILE(filename()));
        return false;
    }

    if (!node["releaseType"]) {
        throw Exception("Config file missing 'releaseType'");
    }

    for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        auto key = it->first.as<std::string>();
        auto value = it->second.as<std::string>();
        data[key] = value;
    }

    setData(data);

    return true;
}

// BadFile
// ParserException
/*
semantic-release
.bumpedrc
ci-publish
conventional-github-releaser
https://github.com/bumped/bumped/blob/master/.bumpedrc

https://www.npmjs.com/package/release-it
https://www.npmjs.com/package/semantic-release

*/

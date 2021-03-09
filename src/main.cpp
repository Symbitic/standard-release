#include "standard-release/changelog/changelog.h"
#include "standard-release/commits/conventional.h"
#include "standard-release/config/yaml.h"
#include "standard-release/errors/error.h"
#include "standard-release/errors/errors.h"
#include "standard-release/git/hooks.h"
#include "standard-release/git/repository.h"
#include "standard-release/semver/semver.h"
#include "standard-release/sources/json.h"
#include "standard-release/sources/text.h"
#include "standard-release/standard-release.h"
#include <cstring>
#include <initializer_list>
#include <iostream>

using namespace StandardRelease;

template<typename... Args>
static inline bool hasoption(std::string &arg, Args &&...args)
{
    std::initializer_list<std::string> params { std::forward<Args>(args)... };
    for (auto param : params) {
        if (arg == param) {
            return true;
        }
    }
    return false;
}

// Install git hooks and create config file (if needed).
static void init(Main &program)
{
    try {
        std::cout << "Init\n";
        // program.init();
    } catch (Exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

// Invoked by hooks to check if commit is conventional commit format.
static void lint(Main &program, const std::string &msg)
{
    try {
        std::cout << "lint\n";
        // program.lint(msg);
    } catch (Exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

static void release(Main &program)
{
    try {
        program.release();
    } catch (Exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

static void help(int argc, const char **argv)
{
    std::cout << "Usage: " << argv[0] << " [options] <mode>" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "  -c, --config <file>  Configuration file." << std::endl
              << "  -r, --repo <dir>     Git repository." << std::endl
              << "  -i, --init           Enable init mode." << std::endl
              << "  -l, --lint           Enable lint mode." << std::endl
              << "  -h, --help           Print usage." << std::endl
              << std::endl
              << "Modes:" << std::endl
              << "  init                 Install git hooks." << std::endl
              << "  lint                 Lint a git message." << std::endl
              << "  release (default)    Create a new release." << std::endl;

    exit(EXIT_SUCCESS);
}

int main(int argc, const char **argv)
{
    int ret = 0;
    bool success;
    enum Mode
    {
        Default,
        Init,
        Lint,
        Help,
    };
    Mode mode = Default;
    int modeCount = 0;
    std::string configFile;
    std::string repoDir;
    std::string lintMsg;
    Main program;
    std::error_code code;

    repoDir = std::filesystem::current_path(code);

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (hasoption(arg, "init", "-i", "--init")) {
            mode = Mode::Init;
            modeCount++;
        } else if (hasoption(arg, "-h", "--help")) {
            mode = Mode::Help;
            modeCount++;
        } else if (hasoption(arg, "lint", "-l", "--lint")) {
            mode = Mode::Lint;
            modeCount++;
            if (i == argc - 1 || argv[i + 1][0] == '-') {
                std::cerr << "Missing lint message\n";
                exit(EXIT_FAILURE);
            }
            lintMsg = argv[i + 1];
            i++;
        } else if (hasoption(arg, "release")) {
            mode = Mode::Default;
            modeCount++;
            continue;
        } else if (hasoption(arg, "-c", "--config")) {
            if (i == argc - 1 || argv[i + 1][0] == '-') {
                std::cerr << "Missing file for '--config'\n";
                exit(EXIT_FAILURE);
            }
            configFile = argv[i + 1];
            i++;
        } else if (hasoption(arg, "-r", "--repo")) {
            if (i == argc - 1 || argv[i + 1][0] == '-') {
                std::cerr << "Missing directory for '--repo'\n";
                exit(EXIT_FAILURE);
            }
            repoDir = argv[i + 1];
            i++;
        } else {
            std::cerr << "Unrecognized option '" << arg << "'\n";
            exit(EXIT_FAILURE);
        }
    }

    if (modeCount > 1) {
        std::cerr << "Error: Only one mode may be given" << std::endl;
        exit(EXIT_FAILURE);
    }

    bool repoFound = GitRepository::isRepo(repoDir);
    if (!repoFound) {
        std::cerr << "Error: " << repoDir << " is not a git repository" << std::endl;
        exit(EXIT_FAILURE);
    }

    program.setDirname(repoDir);

    try {
        // Do not read config file in lint or help mode.
        if (mode != Mode::Lint && mode != Mode::Help) {
            program.readConfigFile(configFile);
        }
    } catch (Exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    switch (mode) {
        case Mode::Init:
            init(program);
            break;
        case Mode::Help:
            help(argc, argv);
            break;
        case Mode::Lint:
            lint(program, lintMsg);
            break;
        case Mode::Default:
        default:
            release(program);
            break;
    }

    return ret;
}

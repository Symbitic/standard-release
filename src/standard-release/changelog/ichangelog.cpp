#include "ichangelog.h"
#include "cmark.h"

using namespace StandardRelease;

struct StandardRelease::IChangelogPrivate
{
    Error error;
    std::string filename;
    IConventionalCommit::Commits commits;
    bool exists;
    std::string content;
};

IChangelog::IChangelog()
    : d(new IChangelogPrivate)
{
}

IChangelog::IChangelog(const std::string filename)
    : d(new IChangelogPrivate)
{
    setFilename(filename);
}

IChangelog::~IChangelog()
{
    // TODO: close open file.
    delete d;
}

void IChangelog::setError(const Error error)
{
    d->error = error;
}

void IChangelog::setContent(const std::string content)
{
    d->content = content;
}

void IChangelog::setFilename(const std::string filename)
{
    // TODO: Should relative paths be resolved here or elsewhere?
    // If not here, then may need check to see if filename is null.
    d->filename = filename;
}

Error IChangelog::error() const
{
    return d->error;
}

std::string IChangelog::filename() const
{
    return d->filename;
}

bool IChangelog::exists() const
{
    return d->exists;
}

IConventionalCommit::Commits IChangelog::commits() const
{
    return d->commits;
}

std::string IChangelog::content() const
{
    return d->content;
}

void IChangelog::addEntry(const SemVer version, const IConventionalCommit::Commit commit)
{
    // NOTE: currently pushes to end of list; only works if we don't read
    // the already-existing commits from changelog.
    d->commits.push_back(commit);
}

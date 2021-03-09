# standard-release

A tool for creating and publishing software releases using a simple, standardized workflow.

Completely automates the entire release process using [SemVer](https://semver.org/) and [Conventional Commits](https://www.conventionalcommits.org/).

## Using

When creating a new project, or cloning an existing project, call `standard-release init`. This will install the git hooks and create a config file if none exists.

Call `standard-release` to create a new release. Standard release will then parse all commit messages to determine the next version number. It then:

1. Adds a new entry to CHANGELOG.
2. Sets the new version number in the language-specific version file.
3. Commits all changed files.
4. Creates a new git tag.
5. Pushes the new git tag and commits to the origin repository.
6. Publishes the new release to GitHub Releases, NPM, and other release managers. (**TODO**)

## Overview

standard-release revolves around three concepts:

1. Git Hooks
2. Semantic Versioning
3. Conventional Commits

### Git Hooks

Git hooks are scripts that Git executes before or after certain events such as commit, push, and receive.
standard-release uses git hooks for enforcing the Conventional Commits style.
That way we can always determine the current semantic version by parsing the history of commit messages.

### Semantic Versioning

Semantic Versioning (SemVer) is a standard for assigning version numbers in a human and machine-readable way. Under this scheme, version numbers convey meaning about what has changed from one version to the next.

### Conventional Commits

Conventional Commits is a specification for formatting Git commit messages. By describing git messages in a format readable by humans and machines, it is possible to determine the next semantic version that should be used for the next release.

## License

Copyright © Alex Shaw 2021

Available under the MIT license.

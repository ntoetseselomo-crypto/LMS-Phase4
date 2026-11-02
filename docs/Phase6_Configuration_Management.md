# Phase 6: Configuration Management

## 1. Version Control System

The LMS project uses Git for version control and GitHub
for remote repository hosting.

## 2. Branching Strategy

The project follows a feature-branch workflow.

- main: Contains stable and tested code.
- feature/*: Used for individual features, improvements,
  documentation and bug fixes.

Developers create a feature branch from main, make changes,
and commit them using meaningful commit messages.

When the work is complete, the feature branch is pushed to
GitHub and a Pull Request is opened against main.

The changes are reviewed and the automated CI pipeline
must pass before merging.

## 3. Commit Convention

Commit messages should briefly describe the change.

Examples:
- Add authentication validation
- Fix assignment deadline validation
- Add automated repository tests
- Update Phase 6 documentation

## 4. Continuous Integration

GitHub Actions automatically runs when code is pushed
to main or master and when a Pull Request targets these
branches.

The pipeline:
1. Checks out the repository.
2. Installs project dependencies.
3. Configures the project using CMake.
4. Builds the LMS and test executable.
5. Runs automated tests using CTest.

A successful pipeline confirms that the project builds
and its automated tests pass in the CI environment.

## 5. Release Management

Stable project versions are identified using Git tags.

The project uses semantic versioning:
- Major version: Significant incompatible changes.
- Minor version: New functionality.
- Patch version: Bug fixes.

Example release tag: v1.0.0

## 6. Configuration Management Practices

- Source code and documentation are tracked in Git.
- Generated build files and temporary files are excluded.
- Changes are committed with meaningful messages.
- Feature branches are merged through Pull Requests.
- Stable versions are marked with release tags.

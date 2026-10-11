# Contributing

Prism Desktop is an experimental preview and desktop shell project. Changes should keep module boundaries clear, preserve the separation between application logic and QML, and avoid changing system desktop configuration during development.

1. Open an issue or discuss a substantial change before implementing it.
2. Build out of source and run `ctest --test-dir build --output-on-failure`.
3. Run `clang-format --dry-run --Werror` on changed C++ files and `qmllint qml/*.qml` on QML files.
4. For C++ service or model changes, configure with `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` and run `clang-tidy` with `clang-analyzer-*` and `bugprone-*` checks. On this GCC 16/Clang setup, add `--removed-arg=-mno-direct-extern-access` for the GCC-only flag.
5. Include tests for behavior changes and report any GUI behavior you could not verify.
6. Keep commits focused. Do not include build output or credentials.

Contributions must be compatible with GPL-3.0-or-later. See [LICENSE](LICENSE) and the repository's [development rules](AGENTS.md).

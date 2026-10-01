# Co-op branch validation

The fork's `Co-op branch checks` workflow builds pushes to `coop/**` with the reusable [Engine build](../.github/workflows/engine-build.yml) workflow. It builds Visual Studio 2022 Win32 and x64 in Debug and Release, runs CTest, and uploads runtime artifacts. [Building OpenTS](BUILDING.md) owns the supported targets and runtime requirements.

A passing workflow proves compilation and its CTest checks. Shared-house play and mission transitions also need runtime checks with matching clients. Record the tested commit and distinguish original mission play from modified missions used to exercise a transition.

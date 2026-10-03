# Co-op branch validation

The fork's `Engine` workflow builds pushes to `main`; `Co-op branch checks` also builds pushes to `coop/**` with the reusable [Engine build](../.github/workflows/engine-build.yml) workflow. It builds Visual Studio 2022 Win32 and x64 in Debug and Release, runs CTest, and uploads runtime artifacts. [Building OpenTS](BUILDING.md) owns the supported targets and runtime requirements.

A passing workflow proves compilation and its CTest checks. Shared-house play and mission transitions also need runtime checks with matching clients. Record the tested commit and distinguish original mission play from modified missions used to exercise a transition.

The force-control and UI-shell harnesses run without game assets. They check wrapping assignments, invalid matrices, variant preselection, the real chooser's rendering, and physical checkbox clicks. The net-packet harness checks the master's setup packet and rejects an unterminated scenario name or a non-master sender.

On October 3, 2026, an experimental clang-cl Win32 Release build was run under Wine with a locally modified GDI1 variant declaring two forces. One player selected the variant, switched between its forces by selecting infantry, saw separate credits and production queues, and produced infantry from the second force. This checks that fixture's local behavior; it does not establish full campaign completion, real LAN play, Windows gameplay, or x64 runtime behavior.

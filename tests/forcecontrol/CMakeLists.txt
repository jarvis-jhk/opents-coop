# The force control helpers have no engine dependencies, so the harness compiles them alone.
# The entry point stays here so the recursive glob building OpenTS cannot pick it up.

opents_add_test(ForceControl
    NAME forcecontrol
    SOURCES forcecontroltest.cpp
    ENGINE
        forcecontrol.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)

####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FswManager.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FswManager.cpp"
)

# Uncomment and add any modules that this component depends on, else
# they might not be available when cmake tries to build this component.

set(MOD_DEPS
  Ports
  Types
)

register_fprime_module()

set(UT_MOD_DEPS
  Os_Stubs
)

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FswManager.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FswManagerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FswManagerTestMain.cpp"
  "${FPRIME_FRAMEWORK_PATH}/Os/Baremetal/Mutex.cpp"
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()

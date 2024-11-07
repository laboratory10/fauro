####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PrmDb.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/PrmDb.cpp"
)

if(FPRIME_ARDUINO)
    target_use_arduino_libraries("SD")
endif()

register_fprime_module()

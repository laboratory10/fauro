####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FauroPackets.xml"
  "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FauroTopology.cpp"
)
set(MOD_DEPS
  Fw/Logger
  Arduino/ArduinoTime
  Arduino/Drv/StreamDriver
  Os/Baremetal/TaskRunner
)

target_use_arduino_libraries("SD")

register_fprime_module()

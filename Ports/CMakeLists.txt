set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FauroPorts.fpp"
)

set(MOD_DEPS
  Types
  Fw
)

register_fprime_module()

module Ports { 
  @ Port for requesting the system mode
  port sysModeRequest(
    ref $mode: Types.SYS_MODE @< a place to put the current system mode
    ref $valid: Fw.ParamValid @< a place to indicate whether the parameter value is valid
  )
}

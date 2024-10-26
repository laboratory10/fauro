module Ports { 
  @ Port for requesting the system mode
  port SysModeRequest(
    ref $mode: Types.SYS_MODE @< a place to put the current system mode
    ref $valid: Fw.ParamValid @< a place to indicate whether the parameter value is valid
  )

  @ Port for notifying another component that it is now ok to handle a fata
  port FatalCheck(
  )
}

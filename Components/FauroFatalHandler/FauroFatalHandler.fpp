module Components {

  @ Handles FATAL calls
  passive component FauroFatalHandler {

    @ FATAL event receive port
    sync input port FatalReceive: Svc.FatalEvent

  }

}

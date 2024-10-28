module Components {

    @ Handles FATAL calls
    passive component FauroFatalHandler {

        ########################################################################
        # Ports                                                                #
        ########################################################################

        @ FATAL event receive port
        sync input port fatalReceive: Svc.FatalEvent

        @ FATAL check receive port
        sync input port fatalCheck: Ports.FatalCheck
  
  }

}

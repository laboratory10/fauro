module Components {
    @ The Power Manager reports power subsyelemetry.
    passive component PowerManager {

        ########################################################################
        # Ports                                                                #
        ########################################################################

        @ Port for rate group scheduler to invoke
        sync input port schedIn: Svc.Sched

        ########################################################################
        # Telemetry                                                            #
        ########################################################################

        telemetry BAT_VOLTAGE: F32 format "{.2f} volts" @< volts

        ########################################################################
        # Standard Ports: Channels, Events, Commands, and Parameters           #
        ########################################################################
        
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

    }
}
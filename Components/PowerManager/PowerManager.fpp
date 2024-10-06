module Components {
    @ The Power Manager reports power subsyelemetry.
    passive component PowerManager {

        sync input port schedIn: Svc.Sched
        telemetry BAT_VOLTAGE: F32 format "{.2f} volts" @< volts

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
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
module Components {
    @ Component that controls and configures the FAURO clock
    passive component FauroTime {

        ########################################################################
        # Ports                                                                #
        ########################################################################

        @ Port for other components to retrieve time
        sync input port timeGetPort: Fw.Time

        ########################################################################
        # Commands                                                             #
        ########################################################################

        @Command to sync boot time with time since unix epoch
        guarded command TIME_SET(epoch_offset_seconds: U32)

        ########################################################################
        # Events                                                               #
        ########################################################################

        event TIME_FROM_EPOCH_SET() severity activity high format \
        "FAUROs clock is updated to have knowledge of time since the unix epoch"

        ########################################################################
        # Standard Ports: Channels, Events, Commands, and Parameters           #
        ########################################################################
        
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
}
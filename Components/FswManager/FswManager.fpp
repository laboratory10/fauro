module Components {

    @ FswManager provides Flight Software commands, telemetry, and state info
    passive component FswManager {

        ########################################################################
        # Ports                                                                #
        ########################################################################
        
        @ Port for rate group scheduler to invoke
        sync input port schedIn: Svc.Sched
        
        @ Port for other components to request the current system mode
        sync input port modeRequest: Ports.SysModeRequest

        ########################################################################
        # Commands                                                             #
        ########################################################################

        @Command to compute and report the image size and checksum
        guarded command FSW_IMAGE_CRC()

        @Command to initiate a hardware reset
        guarded command FSW_RESET()

        ########################################################################
        # Parameters                                                           #
        ########################################################################
        
        @ Parameter to define the current system mode
        param SYS_MODE: Types.SYS_MODE default Types.SYS_MODE.STARTUP

        ########################################################################
        # Telemetry                                                            #
        ########################################################################

        telemetry SYS_MODE: Types.SYS_MODE
        telemetry RAM_AVAILABLE: I32 format "{} bytes" @< bytes

        ########################################################################
        # Events                                                               #
        ########################################################################

        event IMAGE_CRC_RESULT(
            $size: U32 @< Size of the image in bytes
            checksum: U32 @< Checksum value
        ) severity activity high format \
        "FSW Image is {} bytes with checksum 0x{x}"

        event RESET_INITIATED() severity fatal format \
        "A FSW RESET has been initiated"

        event SYS_MODE_CHANGED(
            mode: Types.SYS_MODE @< Current system mode
        ) severity activity high format \
        "The current system mode has changed to {}"

        event SYS_MODE_CHECK_FAILED(
            mode: Types.SYS_MODE @< Value returned for sys mode
            valid: Fw.ParamValid @< validity returned
        ) severity warning low format \
        "SYS_MODE requested for telemetry invalid. Value:{} / Validity:{}"

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
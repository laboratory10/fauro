module Components {
    @ The FswManager is the component that provides Flight Software commands, telemetry, and state information.
    passive component FswManager {

        enum SYS_MODE_ENUM {
            STARTUP, @<The mode enter upon boot
            FLIGHT, @<The mode intended to be used with flight
            RECOVERY, @<The mode entered after a flight has concluded
            TEST @<A mode for testing off-nominal situations prevented in other modes
        }

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        sync input port schedIn: Svc.Sched

        guarded command FSW_IMAGE_CRC()

        guarded command FSW_RESET()

        event FSW_IMAGE_CRC_RESULT(
                                    $size: U32 @< Size of the image in bytes
                                    checksum: U32 @< Checksum value
                                  ) severity activity high format "FSW Image is {} bytes and has a checksum of 0x{x}"

        event FSW_RESET_INITIATED(
                                  ) severity fatal format "A FSW RESET has been initiated"

        event FSW_SYS_MODE_CHANGED(
                                    mode: SYS_MODE_ENUM @< Current system mode
                                  ) severity activity high format "The current system mode has changed to {}"

        # @ Example async command
        # async command COMMAND_NAME(param_name: U32)

        # @ Example telemetry counter
        # telemetry ExampleCounter: U64

        # @ Example event
        # event ExampleStateEvent(example_state: Fw.On) severity activity high id 0 format "State set to {}"

        # @ Example port: receiving calls from the rate group
        # sync input port run: Svc.Sched

        # @ Example parameter
        # param PARAMETER_NAME: U32

        @ Parameter to define the current system mode
        param SYS_MODE: SYS_MODE_ENUM default SYS_MODE_ENUM.STARTUP

        telemetry SYS_MODE: SYS_MODE_ENUM

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
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
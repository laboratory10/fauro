module Components {
    @ The TelemetryManager is sent events and telemetry from all places within the system to prepare the for downlink via radio, serial, or SD storage as applicable.
    passive component TelemetryManager {

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        ##TODO should any of these ports be guarded? see: https://nasa.github.io/fprime/UsersGuide/user/port-comp-top.html#a-quick-look-at-port-kinds-defined-in-a-components-usage
        
        @ Polling for receiving data
        sync input port schedIn: Svc.Sched

        @ Indicates the driver has connected to the UART device
        output port ready: Drv.ByteStreamReady

        @Allocate new buffer
        output port allocate: Fw.BufferGet

        @return the allocated buffer
        output port deallocate: Fw.BufferSend

        @ Takes data to transmit out the UART device
        guarded input port $send: Drv.ByteStreamSend

        @ Takes data to transmit out the UART device
        output port $recv: Drv.ByteStreamRecv

        
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
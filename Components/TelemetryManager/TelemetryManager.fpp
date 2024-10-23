module Components {
    @ The TelemetryManager is sent events and telemetry from all places within the system to prepare the for downlink via radio, serial, or SD storage as applicable.
    passive component TelemetryManager {

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

        @Last RSSI from received ground transmission
        telemetry LAST_RSSI: I16 format "{} dBm" @< dBm

        event TELEMETRY_LOG_OPENED(filename: string size 15) severity activity high format "A new log file was opened with a name: {}"
        event TELEMETRY_LOG_OPEN_FAIL(filename: string size 15) severity warning low format "A new log with the name {} could not be opened"
        event TELEMETRY_LOG_CLOSED(filename: string size 15) severity activity high format "Attempting to close a log file named {}"

        output port modeRequest: Ports.sysModeRequest

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
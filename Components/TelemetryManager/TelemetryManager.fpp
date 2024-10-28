module Components {
    @ Prepare data for downlink via radio/serial or record to SD storage
    passive component TelemetryManager {

        ########################################################################
        # Ports                                                                #
        ########################################################################
        
        @ Polling for receiving data
        sync input port schedIn: Svc.Sched

        @ FATAL event receive port
        output port fatalPing: Ports.FatalCheck

        @ Port to request the SYS_MODE parameter
        output port modeRequest: Ports.SysModeRequest

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

        ########################################################################
        # Telemetry                                                            #
        ########################################################################

        telemetry LAST_RSSI: I16 format "{} dBm" @< dBm

        ########################################################################
        # Events                                                               #
        ########################################################################

        event TELEMETRY_LOG_OPENED(filename: string size 15) \
        severity activity high format \
        "A new log file was opened with a name: {}"
        
        event TELEMETRY_LOG_OPEN_FAIL(filename: string size 15) \
        severity warning low format \
        "A new log with the name {} could not be opened"
        
        event TELEMETRY_LOG_CLOSED(filename: string size 15) \
        severity activity high format \
        "Attempting to close a log file named {}"


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
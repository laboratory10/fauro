module Components {
    @ Only component allowed to read/write from the I2C bus and the one tasked with updating GNC telemetry and commanding
    passive component GncManager {

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        sync input port gncUpdate: Svc.Sched

        telemetry ACCEL_X: F32 @< Earth G
        telemetry ACCEL_Y: F32 @< Earth G
        telemetry ACCEL_Z: F32 @< Earth G
        telemetry GYRO_X: F32 @< radians/second
        telemetry GYRO_Y: F32 @< radians/second
        telemetry GYRO_Z: F32 @< radians/second
        telemetry MAG_X: F32 @< micro Tesla
        telemetry MAG_Y: F32 @< micro Tesla
        telemetry MAG_Z: F32 @< micro Tesla
        telemetry BAROMETRIC_ALTITUDE: F32 @< meters

        telemetry GPS_FIX: bool @< boolean
        telemetry GPS_QUALITY: bool @< boolean
        telemetry GPS_SATELLITES: U8 @< count
        telemetry GPS_TIME: U32 @< date/time in U32
        telemetry GPS_LATITUDE: F64 @< degrees
        telemetry GPS_LONGITUDE: F64 @< degrees
        telemetry GPS_SPEED: F32 @< knots
        telemetry GPS_COURSE: F32 @< degrees
        telemetry GPS_ALTITUDE: F32 @< meters

        telemetry IMU_TEMP: I32 @< degrees C
        telemetry BAROMETRIC_TEMP: I32 @< degrees C
        telemetry BAROMETRIC_PRESSURE: F32 @< hPA
        
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
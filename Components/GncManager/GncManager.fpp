module Components {
    @ Manages GNC commands and telemetry and sole allowable user of I2C bus.
    passive component GncManager {

        ########################################################################
        # Ports                                                                #
        ########################################################################

        @ Port for rate group scheduler to invoke
        sync input port gncUpdate: Svc.Sched

        ########################################################################
        # Parameters                                                           #
        ########################################################################

        @ Parameter to define the current altimeter setting
        param ALTIMETER_SETTING: F64 default 1013.2075

        ########################################################################
        # Telemetry                                                            #
        ########################################################################

        telemetry ALTIMETER_SETTING: F64 format "{.2f} hPA" @< hPA

        telemetry ACCEL_X: F32 format "{.2f} G" @< Earth G
        telemetry ACCEL_Y: F32 format "{.2f} G" @< Earth G
        telemetry ACCEL_Z: F32 format "{.2f} G" @< Earth G
        telemetry GYRO_X: F32 format "{.2f} rad/s" @< radians/second
        telemetry GYRO_Y: F32 format "{.2f} rad/s" @< radians/second
        telemetry GYRO_Z: F32 format "{.2f} rad/s" @< radians/second
        telemetry MAG_X: F32 format "{.2f} microTesla" @< micro Tesla
        telemetry MAG_Y: F32 format "{.2f} microTesla" @< micro Tesla
        telemetry MAG_Z: F32 format "{.2f} microTesla" @< micro Tesla
        telemetry BAROMETRIC_ALTITUDE: F32 format "{.2f} meters" @< meters

        telemetry GPS_FIX: bool @< boolean
        telemetry GPS_QUALITY: bool @< boolean
        telemetry GPS_SATELLITES: U8 @< count
        telemetry GPS_DATE: string size 11 @< gps date in string form
        telemetry GPS_TIME: string size 11 @< gps time in string form
        telemetry GPS_LATITUDE: F64 format "{.4f} degrees" @< degrees
        telemetry GPS_LONGITUDE: F64 format "{.4f} degrees" @< degrees
        telemetry GPS_SPEED: F32 format "{.2f} knots" @< knots
        telemetry GPS_COURSE: F32 format "{.2f} degrees" @< degrees
        telemetry GPS_ALTITUDE: F32 format "{.2f} meters" @< meters

        telemetry IMU_TEMP: I32 format "{} degrees C" @< degrees C
        telemetry BAROMETRIC_TEMP: I32 format "{} degrees C" @< degrees C
        telemetry BAROMETRIC_PRESSURE: F32 format "{.2f} hPA" @< hPA

        ########################################################################
        # Events                                                               #
        ########################################################################

        event ALTIMETER_CHANGED (
            alt_setting: F64 @< Current altimeter setting
        ) severity activity high format \
        "The current altimeter setting has changed to {} hPA"

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
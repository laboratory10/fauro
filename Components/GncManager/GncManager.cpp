// ======================================================================
// \title  GncManager.cpp
// \author laboratory10
// \brief  cpp file for GncManager component implementation class
// ======================================================================

#include "Components/GncManager/GncManager.hpp"
#include "FpConfig.hpp"
#include <Adafruit_BMP3XX.h>
#include <Adafruit_GPS.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

namespace Components {

  Adafruit_LSM6DSOX imu;
  Adafruit_LIS3MDL mag;
  Adafruit_GPS gps(&Wire);
  Adafruit_BMP3XX bmp;

  U8 last_gps = 0;

  sensors_event_t acc_reading, gyro_reading, mag_reading, temp_reading;

  #define TIMESTAMP_LENGTH  11
  #define GPS_READ_INTERVAL 6 //1 second at 6Hz

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  GncManager ::
    GncManager(const char* const compName) :
      GncManagerComponentBase(compName)
  {

  }

  GncManager ::
    ~GncManager()
  {

  }

  void GncManager ::
    initialize()
  {
    FW_ASSERT(imu.begin_I2C());
    FW_ASSERT(mag.begin_I2C());
    FW_ASSERT(gps.begin(0x10));
    FW_ASSERT(bmp.begin_I2C());

    imu.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
    imu.setAccelDataRate(LSM6DS_RATE_52_HZ);
    imu.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
    imu.setGyroDataRate(LSM6DS_RATE_52_HZ);
    mag.setDataRate(LIS3MDL_DATARATE_40_HZ);
    mag.setRange(LIS3MDL_RANGE_12_GAUSS);
    mag.setPerformanceMode(LIS3MDL_MEDIUMMODE);
    mag.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    mag.setIntThreshold(500);
    mag.configInterrupt(false, false, true, true, false, true);
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    // Set up oversampling and filter initialization
    FW_ASSERT(bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X));
    FW_ASSERT(bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X));
    FW_ASSERT(bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3));
    FW_ASSERT(bmp.setOutputDataRate(BMP3_ODR_50_HZ));
    //Need to read the bmp sensor once on init
    //Otherwise the first time it reports it won't be a good measurement
    //We don't care about return here. This is only done to init the sensor
    (void)bmp.performReading();
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void GncManager ::
    gncUpdate_handler(
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    //Must be read several times per task loop 
    //Otherwise GPS data buffers overflow
    (void)gps.read();
    (void)gps.read();
    (void)gps.read();
    (void)gps.read();
    (void)gps.read();

    if (gps.newNMEAreceived()) {
      //I advise against checking this return value and generating a warning EVR
      //GPS fails to parse frequently
      //There is no downside to continuing without warning
      //Generating EVRs will make countless nuisance EVRs
      (void)gps.parse(gps.lastNMEA());
    }

    if (bmp.performReading()) {
      //pressure is given in pascals but we want to provide hPa
      this->tlmWrite_BAROMETRIC_PRESSURE(bmp.pressure/100.0);
      this->tlmWrite_BAROMETRIC_TEMP(bmp.temperature);

      //Remember, the Adafruit BMP library is using this formula
      //altitude = 44330.0 * (1.0 - pow(atmospheric (hPa) / seaLevel, 0.1903));
      Fw::ParamValid isValid;
      F64 alt_setting = this->paramGet_ALTIMETER_SETTING(isValid);
      if (isValid != Fw::ParamValid::UNINIT && 
          isValid != Fw::ParamValid::INVALID &&
          alt_setting != 0) {
        float alt = pow(bmp.pressure/100.0/alt_setting, 0.1903);
        alt = 44330.0 * (1 - alt);
        this->tlmWrite_BAROMETRIC_ALTITUDE(alt);
        this->tlmWrite_ALTIMETER_SETTING(alt_setting);
      } else {
        this->log_WARNING_HI_ALTIMETER_SETTING_INVALID(alt_setting, isValid);
      }
    } else {
      this->log_WARNING_LO_BMP_READING_FAILED();
    }

    if (imu.getEvent(&acc_reading, &gyro_reading, &temp_reading)) {
      this->tlmWrite_ACCEL_X(acc_reading.acceleration.x/SENSORS_GRAVITY_EARTH);
      this->tlmWrite_ACCEL_Y(acc_reading.acceleration.y/SENSORS_GRAVITY_EARTH);
      this->tlmWrite_ACCEL_Z(acc_reading.acceleration.z/SENSORS_GRAVITY_EARTH);
      this->tlmWrite_GYRO_X(gyro_reading.gyro.x);
      this->tlmWrite_GYRO_Y(gyro_reading.gyro.y);
      this->tlmWrite_GYRO_Z(gyro_reading.gyro.z);
      this->tlmWrite_IMU_TEMP(temp_reading.temperature);
    } else {
      this->log_WARNING_LO_IMU_READING_FAILED();
    }
    
    if (mag.getEvent(&mag_reading)) {
      this->tlmWrite_MAG_X(mag_reading.magnetic.x);
      this->tlmWrite_MAG_Y(mag_reading.magnetic.y);
      this->tlmWrite_MAG_Z(mag_reading.magnetic.z);
    } else {
      this->log_WARNING_LO_MAG_READING_FAILED();
    }    

    //reading GPS blocks the receiver, so only do this every second or two
    if (last_gps > GPS_READ_INTERVAL) {
      last_gps = 0;
      this->tlmWrite_GPS_FIX(gps.fix);
      this->tlmWrite_GPS_QUALITY(gps.fixquality);
      this->tlmWrite_GPS_SATELLITES(gps.satellites);

      char gdate[TIMESTAMP_LENGTH];
      char gtime[TIMESTAMP_LENGTH];
      I8 return_value;
      return_value = snprintf(gdate, TIMESTAMP_LENGTH, "%04d-%02d-%02d", 
                              gps.year+2000, gps.month, gps.day);
      FW_ASSERT(return_value > 0);
      FW_ASSERT(return_value < TIMESTAMP_LENGTH);
      return_value = snprintf(gtime, TIMESTAMP_LENGTH, "%02d:%02d:%02dZ", 
                              gps.hour, gps.minute, gps.seconds);
      FW_ASSERT(return_value > 0);
      FW_ASSERT(return_value < TIMESTAMP_LENGTH);
      this->tlmWrite_GPS_DATE(gdate);
      this->tlmWrite_GPS_TIME(gtime);
      
      this->tlmWrite_GPS_LATITUDE(gps.latitudeDegrees);
      this->tlmWrite_GPS_LONGITUDE(gps.longitudeDegrees);
      this->tlmWrite_GPS_SPEED(gps.speed);
      this->tlmWrite_GPS_COURSE(gps.angle);
      this->tlmWrite_GPS_ALTITUDE(gps.altitude);
    } else {
      last_gps++;
    }
  }

  void GncManager ::parameterUpdated(FwPrmIdType id) {
    
    // Read back the parameter value
    Fw::ParamValid isValid;
    F64 altimeter_setting = this->paramGet_ALTIMETER_SETTING(isValid);
    // NOTE: isValid is always VALID in parameterUpdated as it was just set
    FW_ASSERT(isValid == Fw::ParamValid::VALID, isValid);

    // Check the parameter ID is expected
    if (PARAMID_ALTIMETER_SETTING == id) {
      this->log_ACTIVITY_HI_ALTIMETER_CHANGED(altimeter_setting);
      this->tlmWrite_ALTIMETER_SETTING(altimeter_setting);
    }
    
  }

}

// ======================================================================
// \title  GncManager.cpp
// \author snow
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

Adafruit_LSM6DSOX imu;
Adafruit_LIS3MDL mag;
Adafruit_GPS gps(&Wire);
Adafruit_BMP3XX bmp;

sensors_event_t accel_reading, gyro_reading, mag_reading, temp_reading;

#define GRAVITATIONAL_ACC      (9.80665) //On Earth at least...

namespace Components {

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
    imu.begin_I2C();
    mag.begin_I2C();
    gps.begin(0x10);
    bmp.begin_I2C();

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
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
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
    //Must be read several times per task loop otherwise GPS data buffers overflow
    (void)gps.read();
    (void)gps.read();
    (void)gps.read();
    (void)gps.read();
    (void)gps.read();

    if (gps.newNMEAreceived()) {
      //I advise against checking this return value and generating a warning ETL
      //GPS fails to parse frequently
      //There is no downside to continuing without warning
      //Generating etls will make countless nuisance ETLs
      (void)gps.parse(gps.lastNMEA());
    }

    bmp.performReading();
    imu.getEvent(&accel_reading, &gyro_reading, &temp_reading);
    mag.getEvent(&mag_reading);

    //pressure is given in pascals but we want to provide hPa
    this->tlmWrite_BAROMETRIC_PRESSURE(bmp.pressure / 100.0);
    this->tlmWrite_ACCEL_X(accel_reading.acceleration.x / GRAVITATIONAL_ACC);
    this->tlmWrite_ACCEL_Y(accel_reading.acceleration.y / GRAVITATIONAL_ACC);
    this->tlmWrite_ACCEL_Z(accel_reading.acceleration.z / GRAVITATIONAL_ACC);
    this->tlmWrite_GYRO_X(gyro_reading.gyro.x);
    this->tlmWrite_GYRO_Y(gyro_reading.gyro.y);
    this->tlmWrite_GYRO_Z(gyro_reading.gyro.z);
    this->tlmWrite_MAG_X(mag_reading.magnetic.x);
    this->tlmWrite_MAG_Y(mag_reading.magnetic.y);
    this->tlmWrite_MAG_Z(mag_reading.magnetic.z);

    this->tlmWrite_GPS_FIX(gps.fix);
    this->tlmWrite_GPS_QUALITY(gps.fixquality);
    this->tlmWrite_GPS_SATELLITES(gps.satellites);
    this->tlmWrite_GPS_LATITUDE(gps.latitudeDegrees);
    this->tlmWrite_GPS_LONGITUDE(gps.longitudeDegrees);
    this->tlmWrite_GPS_SPEED(gps.speed);
    this->tlmWrite_GPS_COURSE(gps.angle);
    this->tlmWrite_GPS_ALTITUDE(gps.altitude);

    this->tlmWrite_IMU_TEMP(temp_reading.temperature);
    this->tlmWrite_BAROMETRIC_TEMP(bmp.temperature);

    //Remember, the Adafruit BMP library is using this formula
    //altitude = 44330.0 * (1.0 - pow(atmospheric (hPa) / seaLevel, 0.1903));
    Fw::ParamValid isValid;
    F64 altimeter_setting = this->paramGet_ALTIMETER_SETTING(isValid);
    if (isValid) {
      float alt = pow(bmp.pressure/100.0/altimeter_setting, 0.1903);
      alt = 44330.0 * (1 - alt);
      this->tlmWrite_BAROMETRIC_ALTITUDE(alt);
    } else {
      //todo error
    }
  }

  void GncManager ::parameterUpdated(FwPrmIdType id) {
    
    // Read back the parameter value
    Fw::ParamValid isValid;
    F64 altimeter_setting = this->paramGet_ALTIMETER_SETTING(isValid);
    // NOTE: isValid is always VALID in parameterUpdated as it was just properly set
    FW_ASSERT(isValid == Fw::ParamValid::VALID, isValid);

    // Check the parameter ID is expected
    if (PARAMID_ALTIMETER_SETTING == id) {
      this->log_ACTIVITY_HI_GNC_ALTIMETER_SETTING_CHANGED(altimeter_setting);
      this->tlmWrite_ALTIMETER_SETTING(altimeter_setting);
    }
    
  }

}

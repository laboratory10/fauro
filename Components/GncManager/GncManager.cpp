// ======================================================================
// \title  GncManager.cpp
// \author snow
// \brief  cpp file for GncManager component implementation class
// ======================================================================

#include "Components/GncManager/GncManager.hpp"
#include "FpConfig.hpp"
#include <Adafruit_BMP3XX.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_BMP3XX bmp;
boolean test1 = false;
boolean test2 = false;
boolean test3 = false;
boolean test4 = false;
boolean test5 = false;
boolean test6 = false;

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
    bmp.begin_I2C();
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
    bmp.performReading();
    //pressure is given in pascals but we want to provide hPa
    this->tlmWrite_BAROMETRIC_PRESSURE(bmp.pressure / 100);
  }

}

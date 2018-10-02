#include <cstdio> // printf()
#include <unistd.h> // sleep()
#include "Adafruit_ADS1015.h"

// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

int main()
{
  printf("Hello!\n");
  
  printf("Single-ended readings from AIN0 with >3.0V comparator\n");
  printf("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)\n");
  printf("Comparator Threshold: 1000 (3.000V)\n");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  // The data sample rate (SPS: Sample per second) can be set between 128 and 3300 SPS
  // for the ADS1015 with a default value of 1600 SPS and between 8 and 860 SPS for the
  // ADS1115 with a default value of 128 SPS.
  // As the library was first written to be used with the ADS1015, it is these values
  // that are used for the settings:
  //                               ADS1015  ADS1115
  //                               -------  -------
  // ads.setSps(SPS_128);  // SPS:     128        8
  // ads.setSps(SPS_250);  // SPS:     250       16
  // ads.setSps(SPS_490);  // SPS:     490       32
  // ads.setSps(SPS_920);  // SPS:     920       64
  // ads.setSps(SPS_1600); // SPS:    1600      128 (default)
  // ads.setSps(SPS_2400); // SPS:    2400      250
  // ads.setSps(SPS_3300); // SPS:    3300      475
  // ads.setSps(SPS_860);  // SPS:    3300      860 ADS1115 specific
  
  // Setup 3V comparator on channel 0
  ads.startComparator_SingleEnded(0, 1000);

  while(1)
  {
    int16_t adc0;

    // Comparator will only de-assert after a read
    adc0 = ads.getLastConversionResults();
    printf("AIN0: %d\n", adc0);
    
    usleep(100 * 1000);
  }
}

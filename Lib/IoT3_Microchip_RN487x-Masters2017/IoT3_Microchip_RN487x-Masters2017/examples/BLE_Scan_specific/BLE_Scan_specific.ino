// BLE_Scan_whiteList.ino
// Author: M16946
// Date: 2017/01/13

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include <RN487x_BLE.h>

#define debugSerial SerialUSB
#define bleSerial Serial1
#define SERIAL_TIMEOUT  10000

// MAC address added to the white list
const char* peerAddressToScan = "FADF301CAC4F" ;

void initLed()
{
  pinMode(LED_BUILTIN, OUTPUT) ;
  pinMode(LED_RED, OUTPUT) ;
  pinMode(LED_GREEN, OUTPUT) ;
  pinMode(LED_BLUE, OUTPUT) ;  
  pinMode(BUTTON, INPUT_PULLUP) ;
}

void turnBlueLedOn()
{
  digitalWrite(LED_BUILTIN, HIGH) ;
}

void turnBlueLedOff()
{
  digitalWrite(LED_BUILTIN, LOW) ;
}

#define COMMON_ANODE  // LED driving
void setRgbColor(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef COMMON_ANODE
  red = 255 - red ;
  green = 255 - green ;
  blue = 255 - blue ;
#endif

  analogWrite(LED_RED, red) ;
  analogWrite(LED_GREEN, green) ;
  analogWrite(LED_BLUE, blue) ;
}

void initTemperature()
{
  pinMode(TEMP_SENSOR, INPUT) ;
  //Set ADC resolution to 12 bits
  analogReadResolution(12) ;  
}

float getTemperature()
{
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0 ;
  float temp = (mVolts - 500.0) / 100.0 ;
  return temp ;
}

void setup()
{
  while ((!debugSerial) && (millis() < SERIAL_TIMEOUT)) ;
  
	debugSerial.begin(115200) ;

  initLed() ;
  initTemperature() ;

  // Set the optional debug stream
  rn487xBle.setDiag(debugSerial) ;
  // Initialize the BLE hardware
  rn487xBle.hwInit() ;
  // Open the communication pipe with the BLE module
  bleSerial.begin(rn487xBle.getDefaultBaudRate()) ;
  // Assign the BLE serial port to the BLE library
  rn487xBle.initBleStream(&bleSerial) ;
  // Finalize the init. process
  if (rn487xBle.swInit())
  {
    setRgbColor(0, 255, 0) ;
    debugSerial.println("Init. procedure done!") ;
  }
  else
  {
    setRgbColor(255, 0, 0) ;
    debugSerial.println("Init. procedure failed!") ;
    while(1) ;
  }

  // >> Configuring the BLE
  // First enter in command/configuration mode
  rn487xBle.enterCommandMode() ;
  // Stop advertising before starting the demo
  rn487xBle.stopAdvertising() ;
  // Set the advertising output power (range: min = 5, max = 0)
  rn487xBle.setAdvPower(3) ;
  // Remove GATT services
  rn487xBle.setDefaultServices(NO_SERVICE) ;
  // Set passive scan and does not filter out duplicate scan results
  rn487xBle.setSupportedFeatures(PASSIVE_SCAN_BMP | NO_DUPLICATE_SCAN_BMP) ;
  // Take into account the settings by issuing a reboot
  rn487xBle.reboot() ;
  rn487xBle.enterCommandMode() ;
  // Clear the white list
  rn487xBle.clearWhiteList() ;
  // Add the MAC address to scan in the white list
  rn487xBle.addMacAddrWhiteList(true, peerAddressToScan) ;
  // Halt advertisement
  rn487xBle.stopAdvertising() ;

  // Start scanning
  rn487xBle.startScanning() ;

  debugSerial.println("Starter Kit as a Central with filtering a device added to the white list when performing a scan") ;
  debugSerial.println("===============================================================================================") ;
}

void loop()
{
  // Display the result of the scanning
  if (bleSerial.available())
  {
    debugSerial.print((char)bleSerial.read()) ;
  }
}


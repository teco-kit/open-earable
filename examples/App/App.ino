/* 
 * This sketch let's you control OpenEarable via the provided dashboard or edge-ml.org (embedded ML no-code framework).
 * 
 * OpenEarable Dashboard: openearable.github.io/dashboard/
 * edge-ml: app.edge-ml.org
 * 
 * Firmware-version: 1.4.1
 * Release-date: 17.06.2024
*/

#include "Arduino.h"
#include "OpenEarable.h"

// Set DEBUG to true in order to enable debug print
#define DEBUG true

// Change name to OELeft or OERight before flashing ("OpenEarable" if left as default value)
String d_name = "OpenEarable";

void setup()
{
#if DEBUG
  Serial.begin(115200);
  delay(5000);
  open_earable.debug(Serial);
  delay(5000);
#endif

    open_earable.begin(d_name);
}

void loop()
{
  // Update and then sleep
    open_earable.update();
}

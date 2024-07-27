#include <TMCStepper.h>
#include <motor.h>


#define SERIAL_PORT Serial1 // HardwareSerial port 
#define DRIVER_ADDRESS 0b11111110 // TMC5160 Driver address according to NAI/NAO pins

#define R_SENSE 0.075f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

bool shaft = 0;   // ONLY NEEDED FOR CHANGING DIRECTION VIA UART, NO NEED FOR DIR PIN FOR THIS
int STALL_VALUE = 5;
unsigned long vTarget = 2000;
long accel = 50;
long vActual = 0;
int microstep = 1;

TMC5160Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);

void setup() {
  SERIAL_PORT.begin(115200);      // INITIALIZE UART to TMC5160
  Serial.begin(115200);
  delay(500);
  Serial.println(F("Serial Initialized"));

  driver.begin();                // Initialize driver 
 
  driver.toff(3);                // Enables driver in software 
  driver.tbl(2);                 // Adjusts blanking time 
  driver.hend(0);                // Adjusts hysteresis end effective value to -3 
  // driver.hstrt(4);               // Adjusts hysteresis minimum value to 1, so overall is -3+4=1 
  driver.hstrt(0);             // Recommended hysteresis minimum value for spreadCycle 
 
  driver.TPWMTHRS(1);
 
  driver.rms_current(1000);       // Set motor RMS current in mA, 1000 for thruster rotating/1500 for thruster up and down
  Serial.print("RMS Current has been set to:");
  Serial.println(driver.rms_current());
 
  driver.microsteps(microstep);        // Set microsteps to 1/256 
 
  driver.pwm_autoscale(true);    // Needed for stealthChop AT
  driver.pwm_autograd(true);     // Helpful for stealthChop AT
  //driver.en_spreadCycle(true);   // Enable SpreadCycle mode, only for TMC2209
 
  grabber_talk();
 
  driver.pwm_freq(1);         // This is the default value? (%01) 
 
  //driver.SGTHRS(250);   // Set stall detection threshold, only for TMC2209


}

// This function now only applicable to TMC2209, can try if have TMC5160 version
// void stall_guard(int stall_value) 
// {
//   // different stall value based on closing or opening 
//   static uint32_t last_time = 0;
//   uint32_t ms = millis();
 
//   if ((ms - last_time) > 100 && driver.VACTUAL() != 0) { // run every 0.1s
//     last_time = ms;
 
//     int load = (int) driver.SG_RESULT(); 
// #ifdef BB_DEBUG
//     Serial.print("Status: ");
//     Serial.println(load);
// #endif // BB DEBUG
//     if (load && load < stall_value)
//     {
//       //driver.VACTUAL(0);
//       Serial.println("Stall detected");
//     }
//   }
// }

// testing, to take out 
void grabber_talk()
{
  auto versionG = driver.version();
  if (versionG == 0x21) {
    Serial.println("Grabber talking through UART.");
  }
  else {
    Serial.print("\nTesting connection...");
    uint8_t result = driver.test_connection();
    if (result) {
      Serial.println("failed!");
      Serial.print("Likely cause: ");

      switch (result) {
        case 1:
          Serial.println("loose connection");
          break;
        case 2:
          Serial.println("no power");
          break;
      }
    }
    else {
      Serial.println("UART not working, unknown error.");
    }
  }
}


void loop() {
  // uint16_t msread = driver.microsteps();
  // Serial.print(F("Read microsteps via UART to test UART receive : "));
  // Serial.println(msread);
  
  //driver.VACTUAL();
  //Serial.println(driver.VACTUAL());
    for (long i = 7500; i >=0; i = i - 2*accel){              // increase speed to zero
    //driver.VACTUAL();
    Serial.println((signed) driver.VACTUAL());
    // Serial << TMCdriver.VACTUAL() << endl;
    delay(200);
  }

  //driver.VACTUAL();
     for (long i = -7500; i <=0; i = i + 2*accel){              // Decrease speed to zero
     //driver.VACTUAL(i);
     Serial.println((signed) driver.VACTUAL());
     // Serial << TMCdriver.VACTUAL() << endl;
     delay(200);
  }
  
  // if (driver.VACTUAL() < vTarget)
  //  {
  //    vActual = driver.VACTUAL() + 2*accel;
  //    driver.VACTUAL(vActual);
  //  }
  //  else
  //  {
  //    Serial.println("Max speed");
  //  }

  //Serial.println(driver.SG_RESULT());

  //  delay(200); // Delay for motor to run at constant velocity

  //  for (long i = 0; i <= vTarget; i = i + 2*accel){             // Speed up to maxSpeed
  //    driver.VACTUAL(i);                                     // Set motor speed
  //    // Serial << TMCdriver.VACTUAL() << endl;
  //    delay(200);
  //  }
  
  //  for (long i = vTarget; i >=0; i = i - 2*accel){              // Decrease speed to zero
  //    driver.VACTUAL(i);
  //    // Serial << TMCdriver.VACTUAL() << endl;
  //    delay(200);
  //  }   

  //shaft = !shaft;
  //driver.shaft(shaft);
}


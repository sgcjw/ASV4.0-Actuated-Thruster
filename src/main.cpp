#include <Arduino.h>
#include <TMC5160.h>

const uint8_t UART_TX_EN_PIN = 5;   // Differential transceiver TX enable pin
const uint32_t UART_BAUDRATE = 115200; // UART baudrate : up to 750kbps with default TMC5160 clock

TMC5160_UART_Transceiver motor = TMC5160_UART_Transceiver(UART_TX_EN_PIN, Serial1, 0, UART_BAUDRATE); //Use Serial 1 ; address 0 (NAI LOW)

void setup() {
  // USB/debug serial coms
  Serial.begin(115200);

  // Init TMC serial bus @ 250kbps
  Serial1.begin(115200);
  Serial1.setTimeout(5); // TMC5130 should answer back immediately when reading a register.

  // status LED
  pinMode(LED_BUILTIN, OUTPUT);

   // This sets the motor & driver parameters /!\ run the configWizard for your driver and motor for fine tuning !
  TMC5160::PowerStageParameters powerStageParams; // defaults.
  TMC5160::MotorParameters motorParams;
  motorParams.globalScaler = 98; // Adapt to your driver and motor (check TMC5160 datasheet - "Selecting sense resistors")
  motorParams.irun = 31;
  motorParams.ihold = 16;

  motor.begin(powerStageParams, motorParams, TMC5160::NORMAL_MOTOR_DIRECTION);

  // ramp definition
  motor.setRampMode(TMC5160::POSITIONING_MODE);
  motor.setMaxSpeed(400);
  motor.setAcceleration(500);

  Serial.println("starting up");
}


// checking connection
void loop()
{
  uint32_t now = millis();
  static unsigned long t_dirchange, t_echo;
  static bool dir;

  // every n seconds or so...
  if ( now - t_dirchange > 3000 )
  {
    t_dirchange = now;

    // reverse direction
    dir = !dir;
    motor.setTargetPosition(dir ? 200 : 0);  // 1 full rotation = 200s/rev
  }

  // print out current position
  if( now - t_echo > 100 )
  {
    t_echo = now;

    // get the current target position
    float xactual = motor.getCurrentPosition();
    float vactual = motor.getCurrentSpeed();
    Serial.print("current position : ");
    Serial.print(xactual);
    Serial.print("\tcurrent speed : ");
    Serial.println(vactual);
  }
}


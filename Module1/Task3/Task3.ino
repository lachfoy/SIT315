const int R_LED_PIN = 13;
const int B_LED_PIN = 11;
const int PIR_PIN = 2;
const int SW_PIN = 3;

volatile static int motion = LOW;
volatile static int switch_state = LOW;

void setup()
{
  // set the input and output pins
  pinMode(PIR_PIN, INPUT);
  pinMode(SW_PIN, INPUT);
  pinMode(R_LED_PIN, OUTPUT);
  pinMode(B_LED_PIN, OUTPUT);

  // attach interrupt function to the PIR pin
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), pir_interrupt, CHANGE);
  
  // attach interrupt function to the switch
  attachInterrupt(digitalPinToInterrupt(SW_PIN), sw_interrupt, CHANGE);
  
  Serial.begin(9600); // set up serial communication
}

void pir_interrupt()
{
  motion = !motion; // switch motion state
  digitalWrite(R_LED_PIN, motion);
  const char* debug_msg = motion ? "Motion detected" : "No motion detected";
  Serial.println(debug_msg);
}

void sw_interrupt()
{
  switch_state =!switch_state;
  digitalWrite(B_LED_PIN, switch_state);
  const char* debug_msg = switch_state ? "Switch on" : "Switch off";
  Serial.println(debug_msg);
}

void loop()
{
}

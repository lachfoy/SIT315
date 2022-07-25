const int LED_PIN = 13;
const int PIR_PIN = 2;

volatile int motion = 0;

void setup()
{
  // set the input and output pins
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // attach interrupt function to the PIR pin
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), interrupt_ISR, CHANGE);
  
  Serial.begin(9600); // set up serial communication
}

void interrupt_ISR()
{
  motion = !motion; // switch motion state
  digitalWrite(LED_PIN, motion);
  const char* debug_msg = motion ? "Motion detected" : "No motion detected";
  Serial.println(debug_msg);
}

void loop()
{
}

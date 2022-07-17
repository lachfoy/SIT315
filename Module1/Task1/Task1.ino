const int LED = 7;
const int PIR = 10;

int motion = 0;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(PIR, INPUT);  

  Serial.begin(9600);
}

void loop()
{
  motion = digitalRead(PIR);  // read input value

  if (motion)
  {
    digitalWrite(LED, HIGH); // turn LED on
    Serial.println("Motion detected");
  }
  else
  {
    digitalWrite(LED, LOW); // turn LED off
    Serial.println("No motion detected");
  }

  delay(10);
}

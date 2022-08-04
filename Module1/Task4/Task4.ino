const int SW1_PIN = 3;
const int SW2_PIN = 2;
const int SW3_PIN = 1;
const int LED1_PIN = 13;
const int LED2_PIN = 12;
const int LED3_PIN = 11;
const int LED4_PIN = 10;

volatile int SW1_state;
volatile int SW2_state;
volatile int SW3_state;

void setup()
{
  // set the input pins
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SW3_PIN, INPUT_PULLUP);
  
  // set the starting states
  SW1_state = digitalRead(SW1_PIN);
  SW2_state = digitalRead(SW2_PIN);
  SW3_state = digitalRead(SW3_PIN);
  
  // set the output pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  
  noInterrupts();
  // configure Pin Change Interrupt Control Register
  PCICR |= B00000100; // enable PCIE2 (Interrupts on Port D)
  
  // set the Pin Change Mask to enable interrupts on pin 1, 2 and 3
  PCMSK2 |= B0001110;
  
  // Timer
  // Reset timer 1 registers
  TCCR1A = 0;
  TCCR1B = 0;
  
  // initalize counter to 0
  TCNT1 = 0; 
  
  // (16*10^6) / (1*1024) - 1 = 15624
  OCR1A = 15624;  // set Compare Match register to 1hz
  
  TCCR1B |= (1 << WGM12);   // CTC mode

  TCCR1B |= (1 << CS12);    // 256 prescaler 

  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

  interrupts();
  
  Serial.begin(9600); // set up serial communication
}

void loop()
{
  digitalWrite(LED1_PIN, SW1_state);
  digitalWrite(LED2_PIN, SW2_state);
  digitalWrite(LED3_PIN, SW3_state);
}

ISR (PCINT2_vect) 
{
  SW1_state = PIND & B00001000;
  SW2_state = PIND & B00000100;
  SW3_state = PIND & B00000010;
  Serial.println("Port D interrupt triggered!");
}

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LED4_PIN, digitalRead(LED4_PIN) ^ 1);
  Serial.println("Timer interrupt triggered!");
}

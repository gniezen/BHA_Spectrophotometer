
#define PIN_LED A5

class TSL1406
{
public:
  byte pin_si, pin_clk, pin_analogOut;
  TSL1406(byte pin_si, byte pin_clk, byte pin_ao) {
    this->pin_si = pin_si;
    this->pin_clk = pin_clk;
    this->pin_analogOut = pin_ao;
    
    pinMode(pin_si, OUTPUT);
    pinMode(pin_clk, OUTPUT);
    pinMode(pin_ao, INPUT);

    // do some clocking to initialize the sensor to 'zero'
    for (int i=0;i<260;i++)
      clockPulse();
    digitalWrite(pin_si, 1);
    clockPulse();
    digitalWrite(pin_si, 0);
    for (int i=0;i<260;i++)
      clockPulse();    
  }
  
  void clockPulse() {
    digitalWrite(pin_clk,1);
    digitalWrite(pin_clk,0);
  }
  
  void read(int* output)
  {
    digitalWrite(pin_si,1);
    clockPulse();
    digitalWrite(pin_si,0);
    
    for (int i=0;i<768;i++) {
      delayMicroseconds(20);
      int val = analogRead(pin_analogOut);
      clockPulse();
      output[i] = val;
    }

    digitalWrite(pin_si,1);
    clockPulse();
    digitalWrite(pin_si,0);
    
    for (int i=0;i<260;i++) {
      clockPulse();
    }
  }
};

const byte PIN_SI = 2;
const byte PIN_CLK = 3;
const byte PIN_AO = A0;
TSL1406 sensor(PIN_SI,PIN_CLK, PIN_AO);



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
    
  // Define various ADC prescaler:
  const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
  const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  
  // To set up the ADC, first remove bits set by Arduino library, then choose 
  // a prescaler: PS_16, PS_32, PS_64 or PS_128:
  ADCSRA &= ~PS_128;  
  ADCSRA |= PS_32; // <-- Using PS_32 makes a single ADC conversion take ~30 us

  Serial.begin(57600);
  Serial.println("Spectrophotometer v1");
  
  pinMode(A5, OUTPUT);
  digitalWrite(A5, 1);
 
}

void printOutput(int *data, int offset, int count)
{
  for(int i=0;i<count;i++) {
    Serial.print(offset+i);
    Serial.print(";\t");
    Serial.println(data[i]);
  }
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, 1);   // turn the LED on (HIGH is the voltage level)
  delay(400);              // wait for a second
  digitalWrite(13, 0);    // turn the LED off by making the voltage LOW
  delay(400);              // wait for a second
  
  int values[768];
  sensor.read(values);
  printOutput(values,0, 768); 
}



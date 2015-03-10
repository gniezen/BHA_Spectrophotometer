
#define LED_PIN A5

/*

TSL1406R readout method:

The TSL1406 is an array of sensors. Each sensor has a capacitor that is filled by the sensor photodiode, and one that is used as buffering capacitor. 
Whenever the HOLD is set HIGH, the buffering capacitor takes over the charge from the sensing capacitor 

In order to control exposure time of this sensor, the following method is used:
- Set SI/HOLD on high, moving the charges to the holding capacitors
- Send 769 clock ticks, during which integration starts after the 18th tick. 
- Wait X amount of time for integration, allowing for light to be received by the sensor
- Set SI/HOLD on high again. The holding capacitors now contain the charge that we want to know about
- Readout.

*/
class TSL1406
{
public:
  byte pin_si, pin_clk, pin_analogOut;
  int exposureTime;
  
  TSL1406(byte pin_si, byte pin_clk, byte pin_ao) {
    this->pin_si = pin_si;
    this->pin_clk = pin_clk;
    this->pin_analogOut = pin_ao;
    
    pinMode(pin_si, OUTPUT);
    pinMode(pin_clk, OUTPUT);
    pinMode(pin_ao, INPUT);
    
    exposureTime = 5;
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
    
    for (int i=0;i<769;i++) {
      clockPulse();
    }
    
    delay(exposureTime);

    digitalWrite(pin_si,1);
    clockPulse();
    digitalWrite(pin_si,0);
  
    for (int i=0;i<768;i++) {
      int val = analogRead(pin_analogOut);
      clockPulse();
      output[i] = val;
    }
 
  }
};

const byte PIN_SI = 2;
const byte PIN_CLK = 3;
const byte PIN_AO = A0;
TSL1406 sensor(PIN_SI,PIN_CLK, PIN_AO);

String buffer;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  Serial.begin(57600);
  Serial.println("Spectrophotometer v1");
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);
 
}

void printOutput(int *data, int offset, int count)
{
  Serial.println("start");
  
  for(int i=0;i<count;i++) {
//    Serial.print(offset+i);
  //  Serial.print(";");
    Serial.println(data[i]);
  }
}

void readSensor(){
  int values[768];
  sensor.read(values);
  printOutput(values,0, 768); 
}

// the loop function runs over and over again forever
void loop() {
  while (Serial.available()>0) {
    char c = (char)Serial.read();
  
    if (c == '\n') {
      if (buffer.startsWith("read")) {
        readSensor();
      }
      else if(buffer.startsWith("led")) {
        int v=buffer.substring(3).toInt();
        digitalWrite(LED_PIN,v);
      }
      else if(buffer.startsWith("exp")) {
        sensor.exposureTime = buffer.substring(3).toInt();
      }
      else {
        Serial.println("Unknown cmd.");
      }
      buffer="";
    } else buffer+=c;
  }  
 

  static byte led_val=0;  
  digitalWrite(13, led_val); 
  led_val=1-led_val;
  delay(100);  
}



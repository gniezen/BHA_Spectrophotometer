
import processing.serial.*;

Serial port;      // The serial port
int exposureTime = 1; // 1 ms
float updateSpeed = 2000; // 1000 ms

int lastSpectrumUpdate=0;

boolean ledState=true;
PFont defaultFont;
PFont titleFont;

String buffer;

int SpectrumSize = 768;
int[] spectrumData;
int spectrumValueIndex=0;
int spectraCount=0;

void setup() {
  size(800, 400);
  // create a font with the third font available to the system:
  defaultFont = createFont(PFont.list()[0], 14);
  titleFont = createFont(PFont.list()[4], 20);
  textFont(defaultFont);

  // List all the available serial ports:
  printArray(Serial.list());

  String portName = Serial.list()[0];
  port = new Serial(this, portName, 57600);

  spectrumData = new int[SpectrumSize];
  lastSpectrumUpdate=millis();

  ledState=true;
  port.write("led 1\n");
}


void readSpectrum() {
  println("Updating spectrum");
  port.write("read\n");
}

void drawSpectrum() {
  int xstart=10, ystart=height-30;
  float yscale = 360/1024.0f * 3;
  for (int i=1;i<spectrumData.length;i++) {
    line(i+xstart, ystart - spectrumData[i-1] * yscale, 
        i+xstart+1, ystart - spectrumData[i] * yscale);
  }
}


void draw() {
  int time = millis();

  if (lastSpectrumUpdate + updateSpeed < time) {
    lastSpectrumUpdate=time;
    readSpectrum();
  }

  background(255);
  text("U=decrease exposure, I=increase exposure", 10, 55);
  text("Exposure time: " + exposureTime + " ms", 10, 90);

  text("Measurements: " + spectraCount, 10, 390);

  text("LED = " + ( ledState ? "ON" : "OFF" ), 700, 55);
  text("L = Toggle LED", 700, 70);
  
  textFont(titleFont);
  fill(0, 255, 0);
  text("BioHackAcademy Spectrophotometer", 10, 20);
  textFont(defaultFont);
  fill(0);
  
  drawSpectrum();
}


void serialEvent(Serial port) {
  while (port.available () >0) {
    char c = port.readChar();
    if (c == '\n' && buffer.length() > 0) {
      char first=buffer.charAt(0);
      if (first >= '0' && first <= '9') {
        int value = Integer.parseInt(buffer.trim());//substring(0,buffer.length()-1));
        spectrumData[spectrumValueIndex++] = value;
        if (spectrumValueIndex==SpectrumSize) {
          spectrumValueIndex=0;
          spectraCount ++;
        }
      } else if(buffer=="start") {
        spectrumValueIndex=0; // align again
      }
      buffer="";
    } else {
      buffer+=c;
    }
  }
}

void keyPressed() {
  if (key >= 'A' && key <= 'Z')
    key += 'a'-'A'; // make lowercase

  if (key == 'i' || key == 'u') {
    if (key == 'i') exposureTime += 3;
    else exposureTime -= 3;
    exposureTime = max(exposureTime,1);
    
    port.write("exp " + max(1, (int)exposureTime) + "\n");
  }

  if (key == 'l') {
    ledState=!ledState;
    port.write("led " + ( ledState ? "1" : "0" )+ "\n");
  }
  
  if (key == ' ') {
    readSpectrum();
  }
}





#include <Wire.h>

#define SDA_PIN 4
#define SCL_PIN 5

const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;

int isOn = 1;
int freeze = 0;

int control_pin = 5;
int control_pin2 = 12;
int red_pin = 3;
int blue_pin = 6;
int green_pin = 9;

int hue = 0;
int brightness = 0;

int interval = 70;

struct rgb {
  int r;
  int g;
  int b;
};

rgb cols;


void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  Wire.begin(I2C_SLAVE); // new syntax: join i2c bus (address required for slave)
  Wire.onReceive(receiveEvent); // register event
  Serial.println("I2C is up.");

  pinMode(control_pin, OUTPUT);
  pinMode(control_pin2, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

}

void loop() {

  if (freeze == 1) {
    Serial.println("Frozen!");
   
    writePins(1, cols.r, cols.g, cols.b);
    delay(4000);
    Serial.println("Fading...");
    rgb target_cols = hue_to_rgb(hue);
  
    //difference between actual values (cols) and target values
    int red_step = (target_cols.r - cols.r) / 25;
    int green_step = (target_cols.g - cols.g) / 25;
    int blue_step = (target_cols.b - cols.b) / 25;

    for (int i = 0; i < 15; i++) {
      cols.r = cols.r + red_step;
      cols.g = cols.g + green_step;
      cols.b = cols.b + blue_step;

      writePins(1, cols.r, cols.g, cols.b);

      delay(interval);
    }
    freeze = 0;
    Serial.println("Freeze is over");

  }

  if (isOn == 1) {

    cols = hue_to_rgb(hue);
    //Serial.println("Hue RGB: " + String(hue) + " " + String(cols.r) + " " + String(cols.g) + " " + String(cols.b));

    //invert colours
    
    writePins(1, cols.r, cols.g, cols.b);

  }

  else {
    writePins(0, 0, 0, 0);
  }

  delay(interval); //Pyramid should be displaying at ~30Hz
  hue = hue + 1;
  if (hue > 257) {

    hue = 0;
  }

}

void receiveEvent(size_t howMany) {

  char buf[howMany];
  Wire.readBytes(buf, howMany);

  //To extract substrings from a char array in C, you can use the strtok() function, which is part of the standard C library.
  //The strtok() function splits a string into tokens based on a delimiter character, which in your case is the dash -

  //yes I used chatGPT for this.
  Serial.println(buf);
  int wasOn = isOn;
  int new_brightness;
  
  sscanf(buf, "%d-%d-%d-%d-%d", &isOn, &new_brightness, &cols.r, &cols.g, &cols.b);

  if (isOn == 1 and wasOn == 1 and brightness == new_brightness) {
    freeze = 1;
  }

  brightness=new_brightness;

}

rgb hue_to_rgb(int hue) {
  //source: https://arduino.stackexchange.com/questions/75446/rgb-led-color-control-with-a-single-value

  int val = 255;
  int sat = 255;

  unsigned int H_accent = hue / 43;
  unsigned int bottom = ((255 - sat) * val) >> 8;
  unsigned int top = val;
  unsigned char rising  = ((top - bottom)  * (hue % 43   )  )  /  43  +  bottom;
  unsigned char falling = ((top - bottom)  * (43 - hue % 43)  )  /  43  +  bottom;

  int r = 0;
  int g = 0;
  int b = 0;

  switch (H_accent) {
    case 0:
      r = top;
      g = rising;
      b = bottom;
      break;

    case 1:
      r = falling;
      g = top;
      b = bottom;
      break;

    case 2:
      r = bottom;
      g = top;
      b = rising;
      break;

    case 3:
      r = bottom;
      g = falling;
      b = top;
      break;

    case 4:
      r = rising;
      g = bottom;
      b = top;
      break;

    case 5:
      r = top;
      g = bottom;
      b = falling;
      break;
  }
  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  rgb cols;
  cols.r = r;
  cols.g = g;
  cols.b = b;
  return (cols);
}

void writePins(int control, int r, int g, int b) {

  char str[30];
  sprintf(str, "Input: %d %d %d %d %d", control, brightness, r, g, b);
  Serial.println(str);
  
  r = 255 - r;
  g = 255 - g;
  b = 255 - b;
  r = map(brightness, 0, 100, 255, r);
  g = map(brightness, 0, 100, 255, g);
  b = map(brightness, 0, 100, 255, b);
  
  
  sprintf(str, "Writing: %d %d %d %d", control, r, g, b);
  Serial.println(str);

  digitalWrite(control_pin, control);
  digitalWrite(control_pin2, control);
  analogWrite(red_pin, r);
  analogWrite(blue_pin, g);
  analogWrite(green_pin, b);

}

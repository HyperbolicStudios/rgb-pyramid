#include <TimeLib.h>

int control_pin = 5;
int red_pin = 3;
int blue_pin = 6;
int green_pin = 9;

int hue = 0;

struct rgb {
  int r;
  int g;
  int b;
};

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  //setTime(hr,min,sec,day,mnth,yr);
  setTime(6,59,55,1,1,2023);
  pinMode(control_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

}

void loop() {
  time_t t = now();

  if(hour(t) >= 7 and (hour(t) < 22 or (hour(t) <= 22 and minute(t) < 30))) {
    
    rgb cols = hue_to_rgb(hue);
    Serial.println("Hue RGB: " + String(hue) + " " + String(cols.r) + " " + String(cols.g) + " " + String(cols.b));

    //invert colours
    cols.r = 255-cols.r;
    cols.g = 255-cols.g;
    cols.b = 255-cols.b;
    digitalWrite(control_pin, HIGH);
    analogWrite(red_pin,cols.r);
    analogWrite(blue_pin, cols.b);
    analogWrite(green_pin, cols.g);

  }

  else {
    digitalWrite(control_pin, LOW);
    analogWrite(red_pin, 0);
    analogWrite(blue_pin, 0);
    analogWrite(green_pin, 0);
  }

  delay(70); //Pyramid should be displaying at ~30Hz
  hue = hue+1;
  if (hue > 257) {

    hue = 0;
  }

}

rgb hue_to_rgb(int hue) {
    //source: https://arduino.stackexchange.com/questions/75446/rgb-led-color-control-with-a-single-value

    int val = 255;
    int sat = 255;
    
    unsigned int H_accent = hue/43;
    unsigned int bottom = ((255 - sat) * val)>>8;
    unsigned int top = val;
    unsigned char rising  = ((top-bottom)  *(hue%43   )  )  /  43  +  bottom;
    unsigned char falling = ((top-bottom)  *(43-hue%43)  )  /  43  +  bottom;

    int r = 0;
    int g = 0;
    int b = 0;

    switch(H_accent) {
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
    return(cols);
}

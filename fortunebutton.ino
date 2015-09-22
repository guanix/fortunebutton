const auto led = 13;
const auto sdi = 14;
const auto clk = 15;
const auto lat = 16;
const auto blank = 17;

const int buttonPins[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};
const int buttonCount = 12;

void writeLEDs(unsigned int bits);
bool sweep();
int serialCommand();

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  pinMode(sdi, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(lat, OUTPUT);
  pinMode(blank, OUTPUT);
  
  digitalWrite(led, LOW);
  digitalWrite(clk, LOW);
  digitalWrite(lat, LOW);
  digitalWrite(blank, HIGH);
  
  for (ptrdiff_t i = 0; i < buttonCount; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void writeLEDs(unsigned int bits) {
    // blank
  digitalWrite(blank, HIGH);  

  // shift out
  shiftOut(sdi, clk, MSBFIRST, (bits & 0xff00) >> 8);
  shiftOut(sdi, clk, MSBFIRST, bits & 0xff);
  
  // latch in
  digitalWrite(lat, HIGH);
  digitalWrite(lat, LOW);
  
  // unblank
  digitalWrite(blank, LOW);
}

int serialCommand() {
  if (!Serial) {
    return -1;
  }
  
  while (Serial.available()) {
    auto c = Serial.read();
    
    if (c == 1 || c == 0) {
      return c;
    }
  }
  
  return -1;
}

bool sweep() {
  for (ptrdiff_t i = 0; i < buttonCount; i++) {
    if (i == buttonCount - 1) {
      writeLEDs((1 << (i-1)) | (1 << i));
    } else {
      writeLEDs((1 << i) | (1 << (i + 1)));
    }
    delay(45);
  }
  
  if (serialCommand() == 1) {
    return true;
  }
  
  for (ptrdiff_t i = buttonCount - 1; i >= 0; i--) {
    if (i == buttonCount - 1) {
      writeLEDs((1 << (i-1)) | (1 << i));
    } else {
      writeLEDs((1 << i) | (1 << (i + 1)));
    }
    delay(45);
  }

  if (serialCommand() == 1) {
    return true;
  }
  
  writeLEDs(0);
  delay(200);

  if (serialCommand() == 1) {
    return true;
  }

  return false;
}

void loop() {
  if (sweep()) {
     digitalWrite(led, HIGH);
     writeLEDs(0);
     
     while (true) {
       // we were told to exit sweep
       // in this mode, we will light up a button as it is being pressed, and send also send the button number over serial
       for (ptrdiff_t i = 0; i < buttonCount; i++) {
         // debounce
         if (digitalRead(buttonPins[i]) == LOW) {
           delay(100);
           if (digitalRead(buttonPins[i]) == LOW) {
             writeLEDs(1 << i);
             if (Serial) {
               Serial.write(i + 1);
             }
             
             while (digitalRead(buttonPins[i]) == LOW) {
               if (serialCommand() == 0) {
                 writeLEDs(0);
                 digitalWrite(led, LOW);
                 return;
               }
             }
             writeLEDs(0);
           }
         }
         
         if (serialCommand() == 0) {
           writeLEDs(0);
           digitalWrite(led, LOW);
           return;
         }
       }
     }
  }
}


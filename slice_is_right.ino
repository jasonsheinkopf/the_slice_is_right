// 7/13/19

#define sliceSense 5                // IR sensor to check slice
#define vidPin 2                    // pin tells video to switch
#define lcdPin 3                    // pin tells LCD to display weblink
#define magPin 7
#define numSlices 16                // number of slices
#define vidLength 10                // number of seconds of win video
#define buzzer 8                    // buzzer pin

byte revs = 0;                      // revolutions
byte black = 100;                   // sensor reading for line
byte white = 50;                    // sensor reading for background
byte slice = 1;                     // which slice the counter is on
byte startCount = 2;                // after how many revs the game begins
byte revCheck;                      // checks if rev sense is legit
bool revLine = false;               // true if the sensor is on the revSense line
byte sliceCheck;                    // check is slice sense is legit
byte checkAmount = 100;             // number of times lines sense is legit
bool sliceLine = false;             // true if the sensor is on the sliceSense line
bool game = false;                  // true if the revs > startcount
bool clockWise = true;              // true if spun clockwise
bool directionKnown = false;        // true if spin direction is known
bool wheelReady = true;             // wheel is ready to spin
unsigned long checkTime;            // stores time pointer entered slice
int stillTime = 1000;               // milliseconds that mean whell is still


void setup() {
  pinMode(sliceSense, INPUT);
  pinMode(vidPin, OUTPUT);
  pinMode(lcdPin, OUTPUT);
  pinMode(magPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  //readSensor();
  //readDigital();
  //fullProgram();
  fullMagProgram();
  //buzzTest();
}

void buzzTest() {
  byte noteLength = 75;
  for (byte i = 8; i > 3; i--) {
    tone(buzzer, 100 * i, noteLength);
    delay(100);
  }
  delay(5000);
}

void fullMagProgram() {
  if ((digitalRead(sliceSense) == 1) && (sliceLine == false)) {
    sliceCheck++;
  }
  if (sliceCheck == checkAmount) {
    sliceLine = true;
    sliceCheck = 0;
  }
  if ((digitalRead(sliceSense) == 0) && (sliceLine == true)) {
    Serial.print("Slices = ");
    Serial.println(slice);
    slice++;
    wheelReady = false;
    if (slice == numSlices + 1) {
      revs++;
      Serial.print("Revs = ");
      Serial.println(revs);
      slice = 1;
    }
    sliceLine = false;
    checkTime = millis();
    if (revs >= startCount) {
      game = true;
    }
  }
  if (((millis() - checkTime) > stillTime) && game == false && wheelReady == false) {
    Serial.print("Not enough revs");
    for (byte i = 0; i < 3; i++) {
      byte noteLength = 75;
      tone(buzzer, 100, noteLength);
      delay(noteLength * 2);
    }
    resetGame();
  }
  if (((millis() - checkTime) > stillTime) && game == true) {
    if (digitalRead(magPin) == LOW) {
      debug();
      byte noteLength = 75;
      for (byte i = 1; i < 9; i++) {
        tone(buzzer, 100 * i, noteLength);
        delay(100);
      }
      Serial.println("You won, but don't think you're all great.");
      digitalWrite(vidPin, HIGH);
      delay(1000);
      Serial.println("Video Triggered");
      digitalWrite(vidPin, LOW);
      delay(vidLength * 1000);
      digitalWrite(lcdPin, HIGH);
      Serial.println("LCD Triggered");
      delay(1000);
      digitalWrite(lcdPin, LOW);
      Serial.println("");
      Serial.println("Here we go, again!");
    }
    else {
      byte noteLength = 75;
      for (byte i = 8; i > 3; i--) {
        tone(buzzer, 100 * i, noteLength);
        delay(100);
      }
      Serial.println("Wow, you're bad at this.");
      Serial.println("");
      Serial.println("Here we go, again!");
    }
    debug();
    resetGame();
  }
}

void resetGame() {
  game = false;
  revs = 0;
  slice = 1;
  revLine = false;
  sliceLine = false;
  directionKnown = false;
  clockWise = true;
  wheelReady = true;
}

void debug() {
  Serial.print("tDif = ");
  Serial.print(millis() - checkTime);
  Serial.print(" | gm = ");
  Serial.print(game);
  Serial.print(" | inLn = ");
  Serial.print(revLine);
  Serial.print(" | outLn = ");
  Serial.print(sliceLine);
  Serial.print(" | mag = ");
  Serial.print(digitalRead(magPin));
  Serial.print(" | rvs = ");
  Serial.println(revs);
}

void readDigital() {
  Serial.print("sliceSense = ");
  Serial.print(digitalRead(sliceSense));
  Serial.print(" | magPin = ");
  Serial.println(digitalRead(magPin));
  //delay(10);
}

/*
  void fullProgram() {
  revCount();                       // counts revolutions
  sliceCountClockwise();            // counts current slice
  checkWin();
  }

  void readSensor() {
  Serial.print("dirSense = ");
  Serial.print(analogRead(dirSense));
  Serial.print(" | revSense = ");
  Serial.print(analogRead(revSense));
  Serial.print(" | sliceSense = ");
  Serial.println(analogRead(sliceSense));
  delay(100);
  }

  void revCount() {
  if (revs < startCount) {
    if ((digitalRead(revSense) == 1) && (revLine == false)) {
      revCheck++;
    }
    if (revCheck == checkAmount) {
      revLine = true;
      revCheck = 0;
    }
    if ((digitalRead(revSense) == 0) && (revLine == true)) {
      Serial.print("Revs = ");
      Serial.println(revs);
      revs++;
      revLine = false;
    }
  }
  }

  void revCountAnalog() {
  if (revs < startCount) {
    if ((analogRead(revSense) > black) && (revLine == false)) {
      revLine = true;
    }
    if ((analogRead(revSense) < white) && (revLine == true)) {
      Serial.print("Revs = ");
      Serial.println(revs);
      revs++;
      revLine = false;
    }
  }
  }

  void sliceCountClockwise() {
  if ((revs >= startCount)) {
    if ((digitalRead(dirSense) == 1) && directionKnown == false) {
      clockWise = false;
      directionKnown = true;
      slice = numSlices - 1;
      Serial.println("counter");
    }
    if ((digitalRead(sliceSense) == 1) && (sliceLine == false)) {
      sliceCheck++;
    }
    if (sliceCheck == checkAmount) {
      Serial.print("Slice = ");
      Serial.println(slice);
      directionKnown = true;
      if (clockWise == true) {
        slice++;
        if (slice == numSlices + 1) {
          slice = 1;
        }
      }
      else {
        slice --;
        if (slice == 0) {
          slice = numSlices;
        }
      }
      sliceLine = true;
      sliceCheck = 0;
    }
    if ((digitalRead(sliceSense) == 0) && (sliceLine == true)) {
      sliceLine = false;
      checkTime = millis();
      game = true;
    }
  }
  }

  void sliceCountClockwiseAnalog() {
  if ((revs >= startCount)) {
    if ((analogRead(dirSense) > black) && directionKnown == false) {
      clockWise = false;
      directionKnown = true;
      slice = numSlices - 1;
      Serial.println("counter");
    }
    if ((analogRead(sliceSense) > black) && (sliceLine == false)) {
      Serial.print("Slice = ");
      Serial.println(slice);
      directionKnown = true;
      if (clockWise == true) {
        slice++;
        if (slice == numSlices + 1) {
          slice = 1;
        }
      }
      else {
        slice --;
        if (slice == 0) {
          slice = numSlices;
        }
      }
      sliceLine = true;
    }
    if ((analogRead(sliceSense) < white) && (sliceLine == true)) {
      sliceLine = false;
      checkTime = millis();
      game = true;
    }
  }
  }

  void checkWin() {
  if (((millis() - checkTime) > stillTime) && game == true) {
    if (clockWise == true) {
      slice--;
      if (slice == 0) {
        slice = 16;
      }
    }
    else {
      slice++;
      if (slice == 17) {
        slice = 1;
      }
    }
    Serial.print("Slice = ");
    Serial.print(slice);
    if (slice == win1 | slice == win2 | slice == win3) {
      win();
    }
    else {
      lose();
    }
    debug();
    resetGame();

  }
  }

  void win() {
  Serial.println("You won, but don't think you're all great.");
  digitalWrite(vidPin, HIGH);
  delay(1000);
  Serial.println("Video Triggered");
  digitalWrite(vidPin, LOW);
  delay(vidLength * 1000);
  digitalWrite(lcdPin, HIGH);
  Serial.println("LCD Triggered");
  delay(1000);
  digitalWrite(lcdPin, LOW);
  Serial.println("");
  Serial.println("Here we go, again!");

  }

  void lose() {
  Serial.println("Wow, you're bad at this.");
  //delay(3000);
  Serial.println("");
  Serial.println("Here we go, again!");
  }
*/

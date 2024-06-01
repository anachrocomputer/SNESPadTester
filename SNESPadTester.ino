/* SNESPadTester --- test program for the SNES game controller 2024-05-31 */
/* Copyright (c) 2024 John Honniball */

/* Released under the GNU Public Licence (GPL) */

// Connections to SNES controller
#define CLOCK_PIN    2
#define LATCH_PIN    3
#define DATA_PIN     4

// Direct port I/O defines for Arduino with ATmega328
// Change these if running on Mega Arduino
#define SNESOUT      PORTD
#define SNESIN       PIND
#define LATCH_BIT    (1 << LATCH_PIN)
#define CLOCK_BIT    (1 << CLOCK_PIN)
#define DATA_BIT     (1 << DATA_PIN)

// Bits returned by 'readSNES':
#define NES_A        1
#define NES_B        2
#define NES_SELECT   4
#define NES_START    8
#define NES_N       16
#define NES_S       32
#define NES_W       64
#define NES_E      128


void setup(void)
{
  Serial.begin(9600);
  
  Serial.println("SNES Pad Tester");
  Serial.println("John Honniball, May 2024");
  
  // Set up I/O pins for the SNES controller
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);
  
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
}


void loop(void)
{
  for (;;) {
    const unsigned long int snesbits = readSNES(16);

    Serial.print(snesbits, BIN);
    Serial.print(" ");
    Serial.print(snesbits, DEC);
    Serial.println();

    delay(20);
  }
}


/* readSNES --- read the SNES controller */

unsigned long int readSNES(const unsigned int bits)
{
  // With digitalRead/digitalWrite: 150us
  // With direct port I/O: 24us
//unsigned long int before, after;
  unsigned long int snesbits = 0L;
  unsigned int i;
  
//before = micros();
  
#ifdef SLOW_READSNES
  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(LATCH_PIN, LOW);
  
  for (i = 0; i < bits; i++) {
    if (digitalRead(DATA_PIN) == LOW)
      snesbits |= (1L << i);
      
    digitalWrite(CLOCK_PIN, HIGH);
    digitalWrite(CLOCK_PIN, LOW);
  }
#else
  SNESOUT |= LATCH_BIT;
  delayMicroseconds(2);
  SNESOUT &= ~LATCH_BIT;
  
  for (i = 0; i < bits; i++) {
    delayMicroseconds(2);
    
    if ((SNESIN & DATA_BIT) == 0)
      snesbits |= (1L << i);
      
    SNESOUT |= CLOCK_BIT;
    delayMicroseconds(2);
    SNESOUT &= ~CLOCK_BIT;
  }
#endif
  
//after = micros();
//Serial.print("readSNES: ");
//Serial.print(after - before, DEC);
//Serial.println("us");
  
  return (snesbits);
}

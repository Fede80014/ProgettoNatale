#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configurazione della Seriale Bluetooth (RX, TX)
const int BT_RX = 2;
const int BT_TX = 3;
SoftwareSerial BTSerial(BT_RX, BT_TX); 

// Configurazione Pin dei componenti
const int ledPin = 4;       // Pin dei LED
const int buzzerPin = 8;    // Pin del buzzer

// Pin del sensore a ultrasuoni
const int trigPin = 5;      
const int echoPin = 6;      

// Configurazione LCD con I2C 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variabili di stato
bool statoLed = false;
int indiceFrase = 0;

// Soglia di attivazione in centimetri
const int SOGLIA_DISTANZA_CM = 30; 

// Elenco delle frasi da mostrare sul display LCD
String frasi[] = {
  "Buon Natale",
  "Happy Chrismas",
  "Felici Feste",
  "Buon anno"
};
const int totaleFrasi = 4;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Configurazione pin ultrasuoni
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Avvia le comunicazioni seriali
  Serial.begin(9600);
  BTSerial.begin(9600);
  
  // Inizializza l'LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("In attesa BT...");
}

void loop() {
  // 1. Controllo automatico tramite sensore a ultrasuoni HC-SR04
  long distanza = calcolaDistanzaCM();
  
  // Se la distanza è valida e inferiore alla soglia
  if (distanza > 0 && distanza < SOGLIA_DISTANZA_CM) {
    attivaRilevamentoAutomatico();
    delay(2000); // Pausa per evitare attivazioni ripetute troppo rapide
  }

  // 2. Controllo dei comandi in arrivo dal Bluetooth
  if (BTSerial.available() > 0) {
    char comando = BTSerial.read();
    
    Serial.print("Comando ricevuto: ");
    Serial.println(comando);

    if (comando == 'L') {
      gestisciLuci();
    } 
    else if (comando == 'C') {
      suonaMelodia();
    } 
    else if (comando == 'S') {
      prossimaFraseLCD();
    }
  }
}

// Funzione per calcolare la distanza con l'HC-SR04
long calcolaDistanzaCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long durata = pulseIn(echoPin, HIGH, 30000); // Timeout di sicurezza a 30ms
  long distanza = durata * 0.034 / 2; // Formula per convertire il tempo in cm
  return distanza;
}

void gestisciLuci() {
  statoLed = !statoLed;
  digitalWrite(ledPin, statoLed);
}

void prossimaFraseLCD() {
  indiceFrase++;
  if (indiceFrase >= totaleFrasi) {
    indiceFrase = 0;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(frasi[indiceFrase]);
}

void suonaMelodia() {
  int note[] = {659, 659, 659, 659, 659, 659, 659, 789, 523, 587, 659, 689, 689, 689, 689, 659, 659, 659, 587, 587, 659, 587, 783};
  int durate[] = {200, 200, 400, 200, 200, 400, 200, 200, 200, 200, 400, 200, 200, 400, 200, 200, 200, 200, 400, 400, 200, 200, 400};

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suono Melodia..");

  int numNote = sizeof(note) / sizeof(note[0]);

  for (int i = 0; i < numNote; i++) {
    tone(buzzerPin, note[i]);
    delay(durate[i]);
    noTone(buzzerPin);
    delay(50);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(frasi[indiceFrase]);
}

void attivaRilevamentoAutomatico() {
  statoLed = true;
  digitalWrite(ledPin, HIGH);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(frasi[indiceFrase]);

  suonaMelodia();
}

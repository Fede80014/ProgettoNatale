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
const int sensorPin = A1;   // Sensore di prossimità/distanza sulla porta A1

// Configurazione LCD con I2C (indirizzo 0x27, display 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variabili di stato
bool statoLed = false;
int indiceFrase = 0;

// Soglia di attivazione del sensore su A1 (regolabile in base al sensore usato)
const int SOGLIA_PROSSIMITA = 500; 

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
  pinMode(sensorPin, INPUT);
  
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
  // 1. Controllo automatico tramite sensore su porta A1
  int valoreSensore = analogRead(sensorPin);
  if (valoreSensore > SOGLIA_PROSSIMITA) {
    // Azione automatica al rilevamento di vicinanza
    attivaRilevamentoAutomatico();
    delay(2000); // Pausa per evitare attivazioni ripetute troppo rapide
  }

  // 2. Controllo dei comandi in arrivo dal Bluetooth
  if (BTSerial.available() > 0) {
    char comando = BTSerial.read();
    
    // Mostra sul monitor seriale del PC il comando ricevuto
    Serial.print("Comando ricevuto: ");
    Serial.println(comando);

    // Gestione dei comandi in base alla lettera inviata dall'app
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

void gestisciLuci() {
  statoLed = !statoLed;
  digitalWrite(ledPin, statoLed);
}

void prossimaFraseLCD() {
  indiceFrase++;
  if (indiceFrase >= totaleFrasi) {
    indiceFrase = 0; // Ricomincia se arriva alla fine
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(frasi[indiceFrase]);
}

void suonaMelodia() {
  int note[] = {659, 659, 659, 659, 659, 659, 659, 789, 523, 587, 659, 689, 689, 689, 689, 659, 659, 659, 587, 587, 659, 587, 783 };
  int durate[] = {200, 200, 400, 200, 200, 400, 200, 200, 200, 200, 400, 200, 200, 400 , 200, 200, 200, 200, 400, 400, 200, 200, 400};

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suono Melodia..");

  for (int i = 0; i < 23; i++) {
    tone(buzzerPin, note[i]);
    delay(durate[i]);
    noTone(buzzerPin);
    delay(50);
  }
  
  // Ripristina la frase sul display alla fine della canzone
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(frasi[indiceFrase]);
}

void attivaRilevamentoAutomatico() {
  // Accende le luci
  statoLed = true;
  digitalWrite(ledPin, HIGH);
  
  // Mostra direttamente la frase sullo schermo LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(frasi[indiceFrase]);

  // Suona la melodia
  suonaMelodia();
}
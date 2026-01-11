// ============================================================
// PROJE: REFLEKS USTASI (HATASIZ FİNAL KODU)

// --- PIN TANIMLARI ---
constexpr int DATA_PIN  = 11;   
constexpr int CLOCK_PIN = 13;   
constexpr int LATCH_P1  = 12; 
constexpr int LATCH_P2  = 7;  

constexpr int BUTTON_PIN_1 = 2;
constexpr int BUTTON_PIN_2 = 3;

constexpr int RED_LED_PIN   = 4; 
constexpr int WHITE_LED_PIN = 5;
constexpr int BUZZER_PIN    = 6;

// --- OYUN DEĞİŞKENLERİ ---
unsigned int scorePlayer1 = 0;
unsigned int scorePlayer2 = 0;
const int WIN_SCORE = 3; // 3 puana ulaşan kazanır!

// --- 7-SEGMENT HARİTASI (Ortak Anot) ---
byte SEGMENT_MAP[10] = {
  B11000000, B11111001, B10100100, B10110000, B10011001,
  B10010010, B10000010, B11111000, B10000000, B10010000
};

// --- EKRAN FONKSİYONLARI ---
void showDigitOnLatch(int value, int latchPin) {
  if (value < 0) value = 0;
  if (value > 9) value = 9;
  digitalWrite(latchPin, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, SEGMENT_MAP[value]);
  digitalWrite(latchPin, HIGH);
}

void displayScore(int s1, int s2) {
  showDigitOnLatch(s1, LATCH_P1);
  showDigitOnLatch(s2, LATCH_P2);
}

// --- SES VE IŞIK EFEKTLERİ ---
void beepSuccess() { tone(BUZZER_PIN, 1200, 150); }
void beepFail() { tone(BUZZER_PIN, 400, 200); }

void winLedEffect() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(WHITE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(WHITE_LED_PIN, LOW);
    delay(100);
  }
}

// --- BUTON KONTROLÜ (INPUT_PULLUP İÇİN GÜNCELLENDİ) ---
int checkButtons() {
  // PULLUP modunda basılan buton 0 (LOW) döndürür
  if (digitalRead(BUTTON_PIN_1) == LOW) return 1;
  if (digitalRead(BUTTON_PIN_2) == LOW) return 2;
  return 0;
}

// --- SETUP ---
void setup() {
  pinMode(DATA_PIN,  OUTPUT); pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_P1,  OUTPUT); pinMode(LATCH_P2,  OUTPUT);
  pinMode(RED_LED_PIN,   OUTPUT); pinMode(WHITE_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN,    OUTPUT);
  
  
  pinMode(BUTTON_PIN_1, INPUT_PULLUP); 
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  
  randomSeed(analogRead(A0));
  displayScore(scorePlayer1, scorePlayer2);
  winLedEffect(); 
}

// --- ANA DÖNGÜ (MİLLİS() MİMARİSİ) ---
void loop() {
  // 1. KAZANAN KONTROLÜ (GÜNCEL SESLİ)
  if (scorePlayer1 >= WIN_SCORE || scorePlayer2 >= WIN_SCORE) {
    // Neşeli bir bitiş melodisi (Do-Mi-Sol-Do)
    int melody[] = {262, 330, 392, 523}; 
    for (int i = 0; i < 4; i++) {
      tone(BUZZER_PIN, melody[i], 200);
      delay(250);
    }
    noTone(BUZZER_PIN);

    winLedEffect(); // Işıklar dans etsin

    scorePlayer1 = 0; 
    scorePlayer2 = 0;
    displayScore(scorePlayer1, scorePlayer2);
    delay(2000); 
  }

  // 2. RASTGELE BEKLEME (millis() ile bloklamayan)
  unsigned long waitTime = random(1500, 3500);
  unsigned long startWait = millis();
  while (millis() - startWait < waitTime);

  // 3. IŞIK SEÇİMİ
  bool isWhite = random(0, 100) >= 50;
  if (isWhite) digitalWrite(WHITE_LED_PIN, HIGH);
  else         digitalWrite(RED_LED_PIN, HIGH);

  // 4. REAKSİYON ÖLÇÜMÜ (1.5 Saniye Bekleme)
  unsigned long reactionStart = millis();
  int winner = 0;
  while (millis() - reactionStart < 1500) {
    winner = checkButtons();
    if (winner != 0) break; 
  }

  // 5. PUANLAMA (Eksiye Düşmeme Algoritması)
  if (winner != 0) {
    if (isWhite) {
      if (winner == 1) scorePlayer1++; else scorePlayer2++;
      beepSuccess();
    } else {
      // Skor 0 ise eksiye düşürme kuralı
      if (winner == 1 && scorePlayer1 > 0) { scorePlayer1--; beepFail(); }
      else if (winner == 2 && scorePlayer2 > 0) { scorePlayer2--; beepFail(); }
      else { beepFail(); } 
    }
  }

  // 6. TEMİZLİK VE GÜNCELLEME
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(WHITE_LED_PIN, LOW);
  displayScore(scorePlayer1, scorePlayer2);
  
  unsigned long roundEnd = millis();
  while(millis() - roundEnd < 1000); 
} // <--- Eksik olan parantez burasıydı!
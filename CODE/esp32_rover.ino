// ════════════════════════════════════════════════════════════════
//  ESP32 ROVER — 3-MODE CONTROL  |  WiFi + HC-SR04
//
//  HC-SR04 Wiring:
//    VCC  → VIN  (5V)
//    GND  → GND
//    TRIG → D13  (GPIO13)
//    ECHO → D34  (GPIO34)  ← input-only pin, safe
//
//  Motor pins, PWM, all UNCHANGED from original.
// ════════════════════════════════════════════════════════════════

#include <WiFi.h>

// ── WiFi ──────────────────────────────────────────────────────
const char* ssid     = "atoz";
const char* password = "givemeyourip";
WiFiServer server(80);

// ── Motor Pins (UNCHANGED) ────────────────────────────────────
const int IN1=26, IN2=27, IN3=14, IN4=12, ENA=25, ENB=33;

// ── HC-SR04 Pins ──────────────────────────────────────────────
#define TRIG_PIN 13
#define ECHO_PIN 34

// ── PWM ───────────────────────────────────────────────────────
const int CH1=0, CH2=1, PWM_FREQ=30000, PWM_RES=8;

// ── Global State ──────────────────────────────────────────────
int  currentMode  = 1;        // 1=Manual  2=Avoid  3=SmartExplore
String cmd        = "stop";

// Combo wiggle timing
unsigned long comboTimer = 0;
bool comboFlip = true;

// Smart turn memory (alternates direction each obstacle)
int turnDir = 1;  // +1 = right, -1 = left

// ═════════════════════════════════════════════════════════════
//  MOTOR FUNCTIONS
// ═════════════════════════════════════════════════════════════
void motorsOff() {
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
}
void goForward(int s=150) {
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);  ledcWrite(CH1,s);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);  ledcWrite(CH2,s);
}
void goReverse(int s=150) {
  digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH); ledcWrite(CH1,s);
  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH); ledcWrite(CH2,s);
}
void doSpinRight(int s=150) {
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);  ledcWrite(CH1,s);
  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH); ledcWrite(CH2,s);
}
void doSpinLeft(int s=150) {
  digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH); ledcWrite(CH1,s);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);  ledcWrite(CH2,s);
}

// ═════════════════════════════════════════════════════════════
//  HC-SR04 DISTANCE (cm)
// ═════════════════════════════════════════════════════════════
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long us = pulseIn(ECHO_PIN, HIGH, 26000); // 26ms = ~4.5m max
  if (us == 0) return 300;                  // no echo = clear
  return us * 0.034 / 2;
}

// ═════════════════════════════════════════════════════════════
//  SETUP
// ═════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  int mpins[] = {IN1,IN2,IN3,IN4};
  for (int p : mpins) pinMode(p, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  ledcSetup(CH1, PWM_FREQ, PWM_RES); ledcAttachPin(ENA, CH1);
  ledcSetup(CH2, PWM_FREQ, PWM_RES); ledcAttachPin(ENB, CH2);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());
  server.begin();
}

// ═════════════════════════════════════════════════════════════
//  WEB PAGE
// ═════════════════════════════════════════════════════════════
void sendPage(WiFiClient& c, long dist) {
  String ip    = WiFi.localIP().toString();
  String dStr  = (dist >= 290) ? String("---") : String(dist) + String(" cm");
  String dCol  = (dist < 15)  ? "#ff3366" : (dist < 30) ? "#ffcc00" : "#00ff88";
  String mLabel= (currentMode==1) ? "MANUAL" : (currentMode==2) ? "AVOID" : "SMART";

  c.println(F("HTTP/1.1 200 OK"));
  c.println(F("Content-type:text/html"));
  c.println(F("Connection: close"));
  c.println();

  /* ── HTML HEAD ── */
  c.print(F("<!DOCTYPE html><html lang='en'><head>"));
  c.print(F("<meta charset='UTF-8'>"));
  c.print(F("<meta name='viewport' content='width=device-width,initial-scale=1'>"));
  c.print(F("<title>ESP32 Rover</title>"));
  c.print(F("<style>"));

  // Reset & base
  c.print(F("*{margin:0;padding:0;box-sizing:border-box}"));
  c.print(F("body{background:#07071a;color:#c8d8ff;font-family:'Segoe UI',Arial,sans-serif;min-height:100vh;overflow-x:hidden}"));
  c.print(F(":root{--neon:#00f0ff;--purple:#7b2fff;--stop:#ff3366;--safe:#00ff88;--warn:#ffcc00;--card:#0e0e2a;--border:#1e1e50}"));

  // Header
  c.print(F(".header{background:linear-gradient(135deg,#090924 0%,#12123a 100%);padding:14px 20px;border-bottom:2px solid var(--neon);text-align:center;position:relative}"));
  c.print(F(".header h1{font-size:1.55em;letter-spacing:4px;font-weight:900;background:linear-gradient(90deg,var(--neon),var(--purple));-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text}"));
  c.print(F(".header sub{font-size:0.68em;color:#5566aa;letter-spacing:1px}"));

  // Status bar
  c.print(F(".sbar{display:flex;justify-content:space-around;padding:10px 6px;background:#0b0b22;border-bottom:1px solid var(--border)}"));
  c.print(F(".si{text-align:center}.si .sl{font-size:0.58em;color:#5566aa;text-transform:uppercase;letter-spacing:1.5px;display:block}.si .sv{font-size:1em;font-weight:700}"));

  // Tabs
  c.print(F(".tabs{display:flex;gap:8px;padding:12px;background:#090920}"));
  c.print(F(".tab{flex:1;padding:9px 4px;border-radius:8px;font-size:0.7em;font-weight:700;letter-spacing:1.5px;cursor:pointer;text-decoration:none;text-align:center;transition:all .2s;border:1px solid var(--border);color:#6677bb;background:var(--card)}"));
  c.print(F(".tab.active{background:linear-gradient(135deg,var(--purple),var(--neon));color:#fff;border-color:transparent;box-shadow:0 0 18px rgba(0,240,255,.35)}"));

  // Content area
  c.print(F(".content{padding:16px}"));

  // D-Pad
  c.print(F(".dpad{display:grid;grid-template-columns:repeat(3,78px);grid-template-rows:repeat(3,78px);gap:10px;justify-content:center;margin:8px auto 4px}"));
  c.print(F(".db{display:flex;align-items:center;justify-content:center;border-radius:14px;font-size:1.6em;text-decoration:none;color:#ccdeff;background:var(--card);border:1px solid var(--border);transition:all .15s;box-shadow:inset 0 2px 4px rgba(0,0,0,.4),0 4px 14px rgba(0,0,0,.3);user-select:none}"));
  c.print(F(".db:active{transform:scale(.92);box-shadow:0 0 16px var(--neon);border-color:var(--neon);background:#1a1a44}"));
  c.print(F(".db.fwd{grid-column:2;grid-row:1}.db.lft{grid-column:1;grid-row:2}.db.ctr{grid-column:2;grid-row:2}.db.rgt{grid-column:3;grid-row:2}.db.rev{grid-column:2;grid-row:3}"));
  c.print(F(".db.ctr{font-size:1em;color:var(--stop);border-color:#401030;background:#180818}"));
  c.print(F(".db.ctr:active{background:var(--stop);color:#fff;border-color:var(--stop)}"));

  // Combo button
  c.print(F(".combo{display:block;max-width:250px;margin:14px auto 4px;padding:14px;border-radius:14px;background:linear-gradient(90deg,var(--purple),var(--neon));color:#fff;font-size:.9em;font-weight:700;letter-spacing:2.5px;text-align:center;text-decoration:none;box-shadow:0 0 22px rgba(123,47,255,.45);transition:all .2s}"));
  c.print(F(".combo:active{transform:scale(.97);box-shadow:0 0 30px var(--neon)}"));

  // Direction labels
  c.print(F(".dlabels{text-align:center;font-size:.6em;color:#445588;letter-spacing:1.5px;margin-top:6px}"));

  // Auto mode panel
  c.print(F(".apanel{background:var(--card);border:1px solid var(--border);border-radius:18px;padding:22px;text-align:center}"));
  c.print(F(".apanel h2{font-size:1.1em;letter-spacing:2px;margin-bottom:8px}"));
  c.print(F(".apanel p{font-size:.78em;color:#6677aa;margin-bottom:16px}"));
  c.print(F(".dgauge{font-size:3em;font-weight:900;letter-spacing:2px;text-shadow:0 0 20px currentColor;margin:8px 0}"));
  c.print(F(".badge{display:inline-block;padding:6px 18px;border-radius:20px;font-size:.8em;font-weight:700;letter-spacing:1px;margin-top:8px}"));
  c.print(F(".bsafe{background:rgba(0,255,136,.12);color:var(--safe);border:1px solid var(--safe)}"));
  c.print(F(".bwarn{background:rgba(255,204,0,.12);color:var(--warn);border:1px solid var(--warn)}"));
  c.print(F(".bdanger{background:rgba(255,51,102,.12);color:var(--stop);border:1px solid var(--stop)}"));
  c.print(F(".runring{display:inline-block;width:12px;height:12px;border-radius:50%;background:var(--safe);margin-right:8px;box-shadow:0 0 8px var(--safe);animation:blink 1.2s infinite}"));
  c.print(F("@keyframes blink{0%,100%{opacity:1}50%{opacity:.25}}"));
  c.print(F(".stopbtn{display:inline-block;margin-top:18px;padding:10px 26px;border-radius:10px;background:#200a14;border:1px solid var(--stop);color:var(--stop);text-decoration:none;font-size:.82em;font-weight:700;letter-spacing:1px;transition:all .2s}"));
  c.print(F(".stopbtn:active{background:var(--stop);color:#fff}"));

  c.print(F("</style></head><body>"));

  /* ── HEADER ── */
  c.print(F("<div class='header'>"));
  c.print(F("<h1>⚡ ESP32 ROVER</h1>"));
  c.print(F("<sub>"));
  c.print("📡 " + ip);
  c.print(F("</sub></div>"));

  /* ── STATUS BAR ── */
  c.print(F("<div class='sbar'>"));

  // Mode
  c.print(F("<div class='si'><span class='sl'>MODE</span><span class='sv' style='color:var(--neon)'>"));
  c.print(mLabel);
  c.print(F("</span></div>"));

  // Distance
  c.print(F("<div class='si'><span class='sl'>DISTANCE</span><span class='sv' style='color:"));
  c.print(dCol);
  c.print(F("'>"));
  c.print(dStr);
  c.print(F("</span></div>"));

  // State
  c.print(F("<div class='si'><span class='sl'>STATE</span><span class='sv' style='color:#aabbdd'>"));
  String cmdUp = cmd; cmdUp.toUpperCase();
  c.print(cmdUp);
  c.print(F("</span></div>"));

  c.print(F("</div>"));

  /* ── MODE TABS ── */
  c.print(F("<div class='tabs'>"));
  c.print(F("<a href='/mode1' class='tab"));
  if(currentMode==1) c.print(F(" active"));
  c.print(F("'>🕹️ MANUAL</a>"));
  c.print(F("<a href='/mode2' class='tab"));
  if(currentMode==2) c.print(F(" active"));
  c.print(F("'>🛡️ AVOID</a>"));
  c.print(F("<a href='/mode3' class='tab"));
  if(currentMode==3) c.print(F(" active"));
  c.print(F("'>🤖 SMART</a>"));
  c.print(F("</div>"));

  /* ── CONTENT ── */
  c.print(F("<div class='content'>"));

  if (currentMode == 1) {
    /* ── D-PAD ── */
    c.print(F("<div class='dpad'>"));
    c.print(F("<a href='/fwd'   class='db fwd'>&#9650;</a>"));
    c.print(F("<a href='/lft'   class='db lft'>&#9668;</a>"));
    c.print(F("<a href='/stop'  class='db ctr'>&#9632;</a>"));
    c.print(F("<a href='/rgt'   class='db rgt'>&#9658;</a>"));
    c.print(F("<a href='/rev'   class='db rev'>&#9660;</a>"));
    c.print(F("</div>"));
    c.print(F("<div class='dlabels'>FWD &nbsp; LEFT &nbsp; STOP &nbsp; RIGHT &nbsp; REV</div>"));
    c.print(F("<a href='/combo' class='combo'>&#9889; SPIN COMBO &#9889;</a>"));

  } else if (currentMode == 2) {
    /* ── OBSTACLE AVOIDANCE PANEL ── */
    c.print(F("<div class='apanel'>"));
    c.print(F("<h2 style='color:var(--neon)'>🛡️ OBSTACLE AVOIDANCE</h2>"));
    c.print(F("<p>Rover auto-navigates, halts and turns on detection</p>"));
    c.print(F("<div class='dgauge' style='color:"));
    c.print(dCol);
    c.print(F("'>"));
    c.print((dist>=290) ? String("---") : String(dist));
    if(dist<290) c.print(F(" cm"));
    c.print(F("</div>"));
    String cls = (dist<15) ? "bdanger" : (dist<30) ? "bwarn" : "bsafe";
    String lbl = (dist<15) ? "&#9888; OBSTACLE!" : (dist<30) ? "&#9889; CAUTION" : "&#10003; CLEAR";
    c.print("<div class='badge " + cls + "'>" + lbl + "</div>");
    c.print(F("<br><br><span class='runring'></span>"));
    c.print(F("<span style='color:#6677aa;font-size:.8em'>Autonomous — alternating turns on obstacle</span>"));
    c.print(F("<br><a href='/mode1' class='stopbtn'>&#9632; SWITCH TO MANUAL</a>"));
    c.print(F("</div>"));
    c.print(F("<script>setTimeout(()=>location.reload(),1500)</script>"));

  } else {
    /* ── SMART EXPLORER PANEL ── */
    c.print(F("<div class='apanel'>"));
    c.print(F("<h2 style='color:var(--purple)'>🤖 SMART EXPLORER</h2>"));
    c.print(F("<p>Speed adapts to distance &bull; Smart alternate turns &bull; Turbo on open path</p>"));
    c.print(F("<div class='dgauge' style='color:"));
    c.print(dCol);
    c.print(F("'>"));
    c.print((dist>=290) ? String("---") : String(dist));
    if(dist<290) c.print(F(" cm"));
    c.print(F("</div>"));

    // Speed indicator
    int spd = 0;
    if(dist >= 290) spd = 200;
    else if(dist > 35) spd = 200;
    else if(dist > 15) spd = map(dist, 15, 35, 70, 150);
    else spd = 0;

    c.print(F("<div style='font-size:.75em;color:#6677aa;margin:6px 0'>"));
    c.print(F("SPEED: "));
    c.print(String(spd));
    c.print(F(" / 200</div>"));

    // Speed bar
    c.print(F("<div style='background:#111133;border-radius:6px;height:8px;margin:6px auto;max-width:220px'>"));
    c.print(F("<div style='background:linear-gradient(90deg,var(--purple),var(--neon));height:8px;border-radius:6px;width:"));
    c.print(String(spd * 100 / 200));
    c.print(F("%'></div></div>"));

    String cls2 = (dist<15) ? "bdanger" : (dist<35) ? "bwarn" : "bsafe";
    String lbl2 = (dist<15) ? "&#9888; EMERGENCY AVOID" : (dist<35) ? "&#128034; SLOW — OBJECT NEAR" : "&#128640; TURBO — PATH CLEAR";
    c.print("<div class='badge " + cls2 + "' style='margin-top:10px'>" + lbl2 + "</div>");

    c.print(F("<br><br><span class='runring'></span>"));
    c.print(F("<span style='color:#6677aa;font-size:.8em'>Exploring — turn memory active</span>"));
    c.print(F("<br><a href='/mode1' class='stopbtn'>&#9632; SWITCH TO MANUAL</a>"));
    c.print(F("</div>"));
    c.print(F("<script>setTimeout(()=>location.reload(),1500)</script>"));
  }

  c.print(F("</div></body></html>"));
}

// ═════════════════════════════════════════════════════════════
//  MAIN LOOP
// ═════════════════════════════════════════════════════════════
void loop() {
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.flush();

    /* ── Mode switching ── */
    if      (req.indexOf("/mode1") != -1) { currentMode=1; cmd="stop"; motorsOff(); }
    else if (req.indexOf("/mode2") != -1) { currentMode=2; cmd="auto"; }
    else if (req.indexOf("/mode3") != -1) { currentMode=3; cmd="smart"; }

    /* ── Mode 1 commands ── */
    if (currentMode == 1) {
      if      (req.indexOf("/fwd")   != -1) { cmd="fwd"; }
      else if (req.indexOf("/rev")   != -1) { cmd="rev"; }
      else if (req.indexOf("/lft")   != -1) { cmd="lft"; }
      else if (req.indexOf("/rgt")   != -1) { cmd="rgt"; }
      else if (req.indexOf("/combo") != -1) { cmd="combo"; }
      else if (req.indexOf("/stop")  != -1) { cmd="stop"; motorsOff(); }
    }

    long dist = getDistance();
    sendPage(client, dist);
    client.stop();
  }

  /* ══ EXECUTE MOVEMENT ══ */
  unsigned long now = millis();

  // ── MODE 1 : Manual button control ──────────────────────────
  if (currentMode == 1) {
    if      (cmd == "fwd")   goForward();
    else if (cmd == "rev")   goReverse();
    else if (cmd == "lft")   doSpinLeft();
    else if (cmd == "rgt")   doSpinRight();
    else if (cmd == "combo") {
      // Fast alternating spin every 280ms
      if (now - comboTimer > 280) {
        comboTimer = now;
        comboFlip  = !comboFlip;
      }
      comboFlip ? doSpinLeft(230) : doSpinRight(230);
    }
    else motorsOff();
  }

  // ── MODE 2 : Obstacle Avoidance ─────────────────────────────
  //    Rover goes forward, stops & reverses when obstacle < 25cm,
  //    then alternates turning direction each time.
  else if (currentMode == 2) {
    long d = getDistance();
    if (d < 25) {
      motorsOff();      delay(150);
      goReverse(160);   delay(500);
      turnDir *= -1;                    // alternate L / R each time
      (turnDir > 0) ? doSpinRight(160) : doSpinLeft(160);
      delay(600);
    } else {
      goForward(145);
    }
  }

  // ── MODE 3 : Smart Explorer ──────────────────────────────────
  //    Speed is proportional to distance — far = turbo, near = creep.
  //    Obstacle: reverse + alternate turn + pseudo-random turn time.
  //    No extra pins or sensors — all using same HC-SR04.
  else if (currentMode == 3) {
    long d = getDistance();
    if (d < 15) {
      // Emergency stop & back up
      motorsOff();    delay(120);
      goReverse(185); delay(550);
      // Alternate + randomise turn duration for less predictable wander
      turnDir *= -1;
      int turnMs = 350 + (int)(millis() % 450);   // 350–800ms
      (turnDir > 0) ? doSpinRight(175) : doSpinLeft(175);
      delay(turnMs);
    } else if (d < 35) {
      // Getting close — slow down, speed maps 70–150
      int spd = map((int)d, 15, 35, 70, 150);
      goForward(spd);
    } else {
      // Clear path — full turbo
      goForward(200);
    }
  }
}

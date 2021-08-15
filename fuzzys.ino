// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <MQ2.h>
#define AC_LOAD 6

// Replace with your network credentials
const char* ssid = "POCO X3 NFC";
const char* password = "asdfg12345";
//definisi potensio
int potPin= A0;
//definisi pin out MQ2
int pinAout = A0;
MQ2 dht(pinAout);
int suhu;
int timer;
int s0=0;
int s1=1;
int s2=2;
float T;//data gas asap

//float t = 0.0;//data gas asap
float h = 0.0;//data gas LPG
String s = "";//data status LED
int output;
//int dimming = 128;  // Dimming level (0-128)  0 = ON, 128 = OFF
float dimming;
byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float celsius, fahrenheit;

float dingin, hangat, panas;
float sedikit, sedang, banyak;
float lb, sd, ke;
float hasillb, hasilsd, hasilke;
float Coglb, Cogsd, Cogke;
float Cogxlb, Cogxsd, Cogxke;
float Komlb, Komsd, Komke;
float Penlb, Pensd, Penke;
float Totalkom, Totalpen;
float Cog;
float lb1, lb2, lb3;
float sd1, sd2, sd3;
float ke1, ke2, ke3;

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    

// Updates MQ2 readings every 5 seconds
const long interval = 5000;  

//tampilan website
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: fantasy;
     display: inline-block;
     text-align: center;
    }
   .topnav { 
    font-size: 4.0rem; 
  }
    p { font-size: 3.0rem; }
 body {  
    margin: 0;
  }

    h2 { font-size: 3.0rem; font-family: fantasy; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 3.0rem;
      font-family: Futura;
      vertical-align:middle;
      padding-bottom: 15px;
    }

.content { 
    padding: 20px; 
  }
  .card { 
    background-color: #e6e6fa; 
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); 
  }
  .cards { 
    max-width: 1000px; 
    margin: 0 auto; 
    display: grid; 
    grid-gap: 3rem; 
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    

    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body bgcolor="#fffafa">
  <div class="topnav">
    <font font-color="#ffffff">MQ2</font>
    <font color="#008b8b">Server</font>
    <hr width="50%" align="center">
  </div>

 <div class="content">
    <div class="cards">
      <div class="card">
    <p>
    <i class="fas fa-smoking" style="color:#ffa500;"></i> 
    <span class="dht-labels" >Smoke</span><br>
    <span id="temperature" >%TEMPERATURE%</span>
    <sup class="units">ppm</sup>
  </p>
</div>

   <div class="card">
    <p>
    <i class="fas fa-burn" style="color:#008b8b;"></i> 
    <span class="dht-labels">SUHU</span><br>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">ppm</sup>
    </p>
 </div>
<div class="card">
    <p>
    <i class="fas fa-dumpster-fire" style="color:#800000;"></i> 
    <span class="dht-labels">STATUS</span><br>
    <span id="status">%STATUS%</span>
    </p>
 </div>
</div>
  </div>
  %BUTTONPLACEHOLDER%
  <script>function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
    else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
    xhr.send();
  }
  </script>
</body>

<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 5000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 5000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("status").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/status", true);
  xhttp.send();
}, 5000 ) ;
</script>
</html>)rawliteral";

// Penggantian Placeholder data MQ2
String processor(const String& var){

  //test pin
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<label class=\"switch\"><br><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label><h4>Tes Koneksi dengan LED</h4>";
   return buttons;
  }
  
  //Gas Asap
  if(var == "TEMPERATURE"){
    return String(T);
  }

  //Gas LPG
  else if(var == "HUMIDITY"){
    return String(h);
  }
   // Status
   else if(var == "STATUS"){
    return String(s);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(4, OUTPUT);//PIN LED OUTPUT
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  dht.begin(); 
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(T).c_str());
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(s).c_str());
  });
   // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  server.begin();

}
 
void loop(){  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the MQ2 values
    previousMillis = currentMillis;
    // membaca gas asap sebagai ppm (the default)
    float newT = dht.readSmoke();

    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      
      digitalWrite(s0,LOW);
      digitalWrite(s1,LOW);
      digitalWrite(s2,LOW);
      T = analogRead(pinAout);
     
      Serial.println(T);
    }
    // membaca SUHU 
    digitalWrite(s0,LOW);
    digitalWrite(s1,LOW);
    digitalWrite(s2,HIGH);
    float newH = analogRead(pinAout);
    
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      T = newH;
      Serial.println(h);
    }

    
   //======================
   //   fuzzyfikasi suhu
   //======================
   
   if(T <= 25){
   dingin = 1;
   hangat = 0;
   panas = 0;
   }
   else if(T >= 35){
   panas = 1;
   hangat = 0;
   dingin = 0;
   }
   else if(T == 30){
   hangat = 1;
   dingin = 0;
   panas = 0;
   }
   else if((T > 25)&&(T < 30)){
   hangat = (T - 25)/5;
   dingin = 1-(T - 25)/5; 
   panas = 0;
   }
   else if((T > 30)&&(T < 35)){
   panas = (T - 30)/5;
   hangat = 1-(T - 30)/5;
   dingin = 0; 
   }
   
   Serial.print("panas= ");   
   Serial.println(panas);
   Serial.print("hangat= ");   
   Serial.println(hangat);
   Serial.print("dingin= ");   
   Serial.println(dingin);
      
   //===============================
   //       fuzzyfikasi gas
   //===============================
   
   if(h <= 150){
   sedikit = 1;
   sedang = 0;
   banyak = 0;
   }
   else if(h >= 300){
   banyak = 1;
   sedang = 0;
   sedikit = 0;
   }
   else if(h == 200){
   banyak = 0; 
   sedang = 1;
   sedikit = 0;
   }
   else if((h > 150)&&(h < 200)){
   sedang = (h - 150)/50;
   sedikit = 1-(h - 150)/50;
   banyak = 0;
   }
   else if((h > 200)&&(h < 300)){
   sedang = (h - 200)/100;
   banyak = 1-(h - 200)/100;
   sedikit = 0; 
   }
   
 //  Serial.print("Sedikit= ");   
 //  Serial.println(sedikit);
 //  Serial.print("sedang= ");   
 //  Serial.println(sedang);
 //  Serial.print("banyak= ");   
 //  Serial.println(banyak);
    
   
   
   
   //==================================
   //           fuzzy rule
   //===================================
      
   //============================================================================
   if((T <= 25 )&&(h <= 150)){ //1A
    if(dingin < sedikit){
    lb = dingin;
    }
    else if(dingin > sedikit){
    lb = sedikit;
    }
    else if(dingin == sedikit){
    lb = sedikit;
    }
    
    hasillb = lb;
    hasilsd = 0;
    hasilke = 0;
     
   }
   
   //=========================================================================
   
   else if((T <= 25 )&&(h > 150)&&(h < 200)){ //1AB
    if(dingin < sedikit){
    lb1 = dingin;
    }
    else if(dingin > sedikit){
    lb1 = sedikit;
    }
    else if(dingin == sedikit){
    lb1 = sedikit;
    }
    
    if(dingin < sedang){
    lb2 = dingin;
    }
    else if(dingin > sedang){
    lb2 = sedang;
    }
    else if(dingin == sedang){
    lb2 = sedang ;
    }
    
    if(lb1 < lb2){    
     hasillb = lb2;   
    }
    else if(lb1 > lb2){    
     hasillb = lb1;   
    }
    else if(lb1 == lb2){    
     hasillb = lb1;   
    }
    
    hasilsd = 0;
    hasilke = 0;
     
   }
   
   //========================================================================
    else if((T <= 25 )&&(h > 200)&&(h < 300)){ //1BC
    if(dingin < sedang){
    lb = dingin;
    }
    else if(dingin > sedang){
    lb = sedang;
    }
    else if(dingin == sedang){
    lb = sedang;
    }
    
    if(dingin < banyak){
    sd = dingin;
    }
    else if(dingin > banyak){
    sd = banyak;
    }
    else if(dingin == banyak){
    sd = banyak;
    }
    
    hasillb = lb;   
    hasilsd = sd;
    hasilke = 0;
     
   }
   
   //=======================================================================
    else if((T <= 25 )&&(h >= 300)){ //1C

    if(dingin < banyak){
    sd = dingin;
    }
    else if(dingin > banyak){
    sd = banyak;
    }
    else if(dingin == banyak){
    sd = banyak;
    }
    
    hasillb = 0;   
    hasilsd = sd;
    hasilke = 0;
     
   }
   
   
   //==========================================================================
   
    else if((T > 25 )&&(T < 30 )&&(h <= 150)){ //12A

    if(dingin < sedikit){
    lb1 = dingin;
    }
    else if(dingin > sedikit){
    lb1 = sedikit;
    }
    else if(dingin == sedikit){
    lb1 = sedikit;
    }
    
    
    if(hangat > sedikit){
    lb2 = sedikit;
    }
    else if(hangat < sedikit){
    lb2 = hangat;
    }
    else if(hangat == sedikit){
    lb2 = hangat;
    }
    
    
    if(lb1 < lb2){    
     hasillb = lb2;   
    }
    else if(lb1 > lb2){    
     hasillb = lb1;   
    }
    else if(lb1 == lb2){    
     hasillb = lb1;   
    }
    
    hasilsd = 0;
    hasilke = 0;
     
   }
   
   //=========================================================================
   
    else if((T > 25 )&&(T < 30 )&&(h > 150)&&(h < 200)){ //12AB

    if(dingin < sedikit){
    lb1 = dingin;
    }
    else if(dingin > sedikit){
    lb1 = sedikit;
    }
    else if(dingin == sedikit){
    lb1 = sedikit;
    }
    
    
    if(dingin < sedang){
    lb2 = dingin;
    }
    else if(dingin > sedang){
    lb2 = sedang;
    }
    else if(dingin == sedang){
    lb2 = sedang;
    }
    
    
    if(hangat < sedikit){
    lb3 = dingin;
    }
    else if(hangat > sedikit){
    lb3 = sedikit;
    }
    else if(hangat == sedikit){
    lb3 = sedikit;
    }
    
    
    if(hangat < sedang){
    sd = hangat;
    }
    else if(hangat > sedang){
    sd = sedang;
    }
    else if(hangat == sedang){
    sd = sedang;
    }
    
    
   if((lb1 > lb2)&&(lb1 > lb3)){                          
   hasillb = lb1;
   }
   else if((lb1 > lb2)&&(lb1 == lb3)){
   hasillb = lb1;
   }
   else if((lb1 == lb2)&&(lb1 > lb3)){
   hasillb = lb1;
   }
   else if((lb1 == lb2)&&(lb1 == lb3)){
   hasillb = lb1;
   }
   else if((lb2 > lb1)&&(lb2 > lb3)){
   hasillb = lb2;
   }
   else if((lb2 > lb1)&&(lb2 == lb3)){
   hasillb = lb2;
   }
   else if((lb2 == lb1)&&(lb2 > lb3)){
   hasillb = lb2;
   }
   else if((lb2 == lb1)&&(lb2 == lb3)){
   hasillb = lb2;
   }
   else if((lb3 > lb1)&&(lb3 > lb2)){
   hasillb = lb3;
   }
   else if((lb3 > lb1)&&(lb3 == lb2)){
   hasillb = lb3;
   }
   else if((lb3 == lb1)&&(lb3 > lb2)){
   hasillb = lb3;
   }
   else if((lb3 == lb1)&&(lb3 == lb2)){
   hasillb = lb3;
   }

   hasilsd = sd;
   hasilke = 0;
    
   }
   
   //===============================================================================
   
   else if((T > 25)&&(T < 30)&&(h > 200)&&(h < 300)){ //12BC
   if (dingin < sedang){
   lb = dingin;
   }
   else if(dingin > sedang){
   lb = sedang;
   }
   else if(dingin == sedang){
   lb = sedang;
   }

   if(dingin < banyak){
   sd1 = dingin;
   }
   else if(dingin > banyak){
   sd1 = banyak;
   }
   else if(dingin == banyak){
   sd1 = banyak;
   }

   if(hangat < sedang){
   sd2 = hangat;
   }
   else if(hangat > sedang){
   sd2 = sedang;
   }
   else if(hangat == sedang){
   sd2 = sedang;
   }

   if(hangat < banyak){
   ke = hangat;
   }
   else if(hangat > banyak){
   ke = banyak;
   }
   else if(hangat == banyak){
   ke = banyak;
   }

   if(sd1 > sd2){
   hasilsd = sd1;
   }
   else if(sd2 > sd1){
   hasilsd = sd2;
   }
   else if(sd2 == sd1){
   hasilsd = sd2;
   }

   hasillb = lb;
   hasilke = ke;

   }

   //=======================================================================

   else if((T > 25)&&(T < 30)&&(h >= 300)){ //12C
     
   if(dingin < banyak){
   sd = dingin;
   }
   else if(dingin > banyak){
   sd = banyak;
   }
   else if(dingin == banyak){
   sd = banyak;
   }

   if(hangat > banyak){
   ke = banyak;
   }
   else if(hangat < banyak){
   ke = hangat;
   }
   else if(hangat == banyak){
   ke = hangat;
   }
   
   hasilsd = sd;
   hasilke = ke;
   hasillb = 0;
   }

   //===============================================================
   else if ((T == 30) && (h <= 150)){ //2A
   if (hangat > sedikit){     
   lb = sedikit;
   }
   else if (hangat < sedikit){
   lb = hangat;
   }
   else if (hangat == sedikit){
   lb = hangat;
   }

   hasillb = lb;
   hasilsd = 0;
   hasilke = 0;
   }
   
   //============================================================
   else if ((T == 30) && (h > 150) && (h < 200)){ //2AB
   if (hangat < sedikit){
   lb = hangat;
   }
   else if (hangat > sedikit){
   lb = sedikit;
   }
   else if (hangat == sedikit){
   lb = sedikit;
   }
   

   if (hangat < sedang){
   sd = hangat;
   }
   else if (hangat > sedikit){
   sd = sedikit;
   }
   else if (hangat == sedikit){
   sd = sedikit;
   }

   hasillb = lb;
   hasilsd = sd;
   hasilke = 0;
   
   }

//===================================================================
   else if ((T == 30) && (h > 200) && (h < 300)){ //2BC
   if (hangat < sedang){
   sd = hangat;
   }
   else if (hangat > sedang){
   sd = sedang;
   }
   else if (hangat == sedang){
   sd = sedang;
   }

   if (hangat < banyak){
   ke = hangat;
   }
   else if (hangat > banyak){
   ke = banyak;
   }
   else if (hangat == banyak){
   ke = banyak;
   }

   hasilsd = sd;
   hasilke = ke;
   hasillb = 0;
   
   }

 //=============================================================
   else if ((T == 30) && (h >= 300)){  //2C
   if (hangat < banyak){
   ke = hangat;
   }
   else if (hangat > banyak){
   ke = banyak;
   }
   else if (hangat == banyak){
   ke = banyak;
   }

   hasilke = ke;
   hasilsd = 0;
   hasillb = 0;

}


//====================================================================

   else if ((T > 30) && (T < 35) && (h <= 150)){ //23A
   if (hangat < sedikit){ 
   lb = hangat;
   }
   else if (hangat > sedikit){
   lb = sedikit;
   }
   else if (hangat == sedikit){
   lb = sedikit;
   }
   
   if (panas < sedikit){
   sd = panas;
   }
   else if (panas > sedikit){
   sd = sedikit;
   }
   else if (panas == sedikit){
   sd = sedikit;
   }

   hasillb = lb;
   hasilsd = sd;
   hasilke = 0;
   }

//=======================================================================

   else if ((T > 30) && (T < 35) && (h > 150) && (h < 200)){  //23AB
   if (hangat < sedikit){
   lb = hangat;
   }
   else if (hangat > sedikit){
   lb = sedikit;
   }
   else if (hangat == sedikit){
   lb = sedikit;
   }

   if (hangat < sedang){
   sd1 = hangat;
   }
   else if (hangat > sedang){
   sd1 = sedang;
   }
   else if (hangat == sedang){
   sd1 = sedang;
   }
   
   if (panas < sedikit){
   sd2 = panas;
   }
   else if (panas > sedikit){
   sd2 = sedikit;
   }
   else if (panas == sedikit){
   sd2 = sedikit;
   }

   if (panas < sedang){
   ke = panas;
   }
   else if (panas > sedang){
   ke = sedang;
   }
   else if (panas == sedang){
   ke = sedang;
   }

   if (sd1 < sd2){
   hasilsd = sd2;
   }
   else if (sd1 > sd2){
   hasilsd = sd1;
   }
   else if (sd1 == sd2){
   hasilsd = sd1;
   }

   hasilke = ke;
   hasillb = lb;
   }

//=========================================================================

   else if ((T > 30) && (T < 35) && (h > 200) && (h < 300)){ //23BC
   if (hangat < sedang){
   sd = hangat;
   }
   else if (hangat > sedang){
   sd = sedang;
   }
   else if (hangat == sedang){
   sd = sedang;
   }

   if (hangat < banyak){
   ke1 = hangat;
   }
   else if (hangat > banyak){
   ke1 = banyak;
   }
   else if (hangat == banyak){
   ke1 = banyak;
   }

   if (panas < sedang){
   ke2 = panas;
   }
   else if (panas > sedang){
   ke2 = sedang;
   }
   else if (panas == sedang){
   ke2 = sedang;
   }

   if (panas < banyak){
   ke3 = panas;
   }
   else if (panas > banyak){
   ke3 = banyak;
   }
   else if (panas == banyak){
   ke3 = banyak;
   }
   
   if ((ke1 > ke2) && (ke1 > ke3)){
   hasilke = ke1;
   }
   else if ((ke1 > ke2) && (ke1 == ke3)){
   hasilke = ke1;
   }
   else if ((ke1 == ke2) && (ke1 > ke3)){
   hasilke = ke1;
   }
   else if ((ke1 == ke2) && (ke1 == ke3)){
   hasilke = ke1;
   }
   else if ((ke2 > ke1) && (ke2 > ke3)){
   hasilke = ke2;
   }
   else if ((ke2 > ke1) && (ke2 == ke3)){
   hasilke = ke2;
   }
   else if ((ke2 == ke1) && (ke2 > ke3)){
   hasilke = ke2;
   }
   else if ((ke2 == ke1) && (ke2 == ke3)){
   hasilke = ke2;
   }

   else if ((ke3 > ke1) && (ke3 > ke2)){
   hasilke = ke3;
   }
   else if ((ke3 > ke1) && (ke3 == ke2)){
   hasilke = ke3;
   }
   else if ((ke3 == ke1) && (ke3 > ke2)){
   hasilke = ke3;
   }
   else if ((ke3 == ke1) && (ke3 == ke2)){
   hasilke = ke3;
   }

   hasilsd = sd;
   hasillb = 0;
   }

//===========================================================================

   else if ((T > 30) && (T < 35) && (h >= 300)){ //23C
   if (hangat < banyak){
   ke1 = hangat;
   }
   else if (hangat > banyak){
   ke1 = banyak;
   }
   else if (hangat == banyak){
   ke1 = banyak;
   }

   if (panas < banyak){
   ke2 = panas;
   }
   else if (panas > banyak){
   ke2 = banyak;
   }
   else if (panas == banyak){
   ke2 = banyak;
   }

   if (ke1 < ke2){
   hasilke = ke2;
   }
   else if (ke1 > ke2){
   hasilke = ke1;
   }
   else if (ke1 == ke2){
   hasilke = ke1;
   }

   hasilsd = 0;
   hasillb = 0;
   }

//===========================================================================

  else if ((T >= 35) && (h <= 150)){  //3A
   if (panas < sedikit){
   sd = panas;
   }
   else if (panas > sedikit){
   sd = sedikit;
   }
   else if (panas == sedikit){
   sd = sedikit;
   }

   hasilsd = sd;
   hasillb = 0;
   hasilke = 0;
   }

//============================================================================

else if ((T >= 35) && (h > 150) && (h < 200)){  //3AB
   if (panas < sedikit){
   sd = panas;
   }
   else if (panas > sedikit){
   sd = sedikit;
   }
   else if (panas == sedikit){
   sd = sedikit;
   }

   if (panas < sedang){
   ke = panas;
   }
   else if (panas > sedang){
   ke = sedang;
   }
   else if (panas == sedang){
   ke = sedang;
   }

   hasilsd = sd;
   hasilke = ke;
   hasillb = 0;
   }
   
   
//======================================================================   
   
else if ((T >= 35) && (h > 200) && (h < 300)){ //3BC
   if (panas < sedang){
   ke1 = panas;
   }
   else if (panas > sedang){
   ke1 = sedang;
   }
   else if (panas == sedang){
   ke1 = sedang;
   }

   if (panas < banyak){
   ke2 = panas;
   }
   else if (panas > banyak){
   ke2 = banyak;
   }
   else if (panas == banyak){
   ke2 = banyak;
   }

   if (ke1 < ke2){
   hasilke = ke2;
   }
   else if (ke1 > ke2){
   hasilke = ke1;
   }
   else if (ke1 == ke2){
   hasilke = ke1;
   }

   hasillb = 0;
   hasilsd = 0;
   }

//=========================================================================

else if ((T >= 35) && (h >= 300)){ //3C
   if (panas < banyak){
   ke = panas;
   }
   else if (panas > banyak){
   ke = banyak;
   }
   else if (panas == banyak){
   ke = banyak;
   }

   hasilke = ke;
   hasilsd = 0;
   hasillb = 0;
}


//======================================================================

else if ((T <= 25) && (h == 200)){ //1B
   if (dingin < sedang){
   lb = dingin;
   }
   else if (dingin > sedang){
   lb = sedang;
   }
   else if (dingin == sedang){
   lb = sedang;
   }

   hasillb = lb;
   hasilsd = 0;
   hasilke = 0;
}

//===================================================================

else if ((T > 25) && (T < 30) && (h == 200)){ //12B
   if (dingin < sedang){
   lb = dingin;
   }
   else if (dingin > sedang){
   lb = sedang;
   }
   else if (dingin == sedang){
   lb = sedang;
   }

   if (hangat < sedang){
   sd = hangat;
   }
   else if (hangat > sedang){
   sd = sedang;
   }
   else if (hangat == sedang){
   sd = sedang;
   }

   hasillb = lb;
   hasilsd = sd;
   hasilke = 0;
}

//==========================================================

else if ((T == 30) && (h == 200)){ //2B
   if (hangat < sedang){
   sd = hangat;
   }
   else if (hangat > sedang){
   sd = sedang;
   }
   else if (hangat == sedang){
   sd = sedang;
   }

   hasilsd = sd;
   hasillb = 0;
   hasilke = 0;
   
}


//================================================================

else if ((T > 30) && (T < 35) && (h == 200)){ //23B
   if (hangat < sedang){ 
   sd = hangat;
   }
   else if (hangat > sedang){
   sd = sedang;
   }
   else if (hangat == sedang){
   sd = sedang;
   }

   if (panas < sedang){
   ke = panas;
   }
   else if (panas > sedang){
   ke = sedang;
   }
   else if (panas == sedang){
   ke = sedang;
   }
   
   hasilsd = sd;
   hasilke = ke;
   hasillb = 0;
}



//================================================================

else if ((T >= 35) && (h == 200)){  //3B
   if (panas < sedang){
   ke = panas;
   }
   else if (panas > sedang){
   ke = sedang;
   }
   else if (panas == sedang){
   ke = sedang;
   }

   hasilke = ke;
   hasilsd = 0;
   hasillb = 0;

}



//==================================================
//               Defuzzifikasi
//==================================================


//0-10-20-30
Coglb = 60;
Cogxlb = 4;

//40-50-60-70
Cogsd = 220;
Cogxsd = 4;

//80-90-100-110-120
Cogke = 500;
Cogxke = 5;

/*
//0-10-20-30-40-50
Coglb = 150;
Cogxlb = 6;

//60-70-80-90-100-110-120
Cogsd = 630;
Cogxsd = 7;

//130-140-150-160-170-180-190-200
Cogke = 1320;
Cogxke = 8;
*/
//Komlb = Coglb * hasillb;
//Komsd = Cogsd * hasilsd;
//Komke = Cogke * hasilke;

//Totalkom = Komlb + Komsd;
//Totalkom = Totalkom + Komke;

//Penlb = hasillb * Cogxlb;
//Pensd = hasilsd * Cogxsd;
//Penke = hasilke * Cogxke;

//Totalpen = Penlb + Pensd;
//Totalpen = Totalpen + Penke;

Totalkom = (Coglb * hasillb) + (Cogsd * hasilsd) + (Cogke * hasilke); 

Totalpen = (hasillb * Cogxlb) + (hasilsd * Cogxsd) + (hasilke * Cogxke);

Cog = Totalkom / Totalpen;
   
   dimming = 128 - Cog;
   
   Serial.print("Totalkom= ");
   Serial.println(Totalkom);
   Serial.print("Totalpen= ");
   Serial.println(Totalpen);
   Serial.print("COG= ");
   Serial.println(Cog);
   Serial.println(" ");
   
  //kondisi LED nyala dengan parameter asap
      if(Cog<33){
        digitalWrite(5, HIGH);
        s="AMAN";
        }

       else if(Cog>33 && Cog<66){
        digitalWrite(4, HIGH);
        s="SIAGA";
        }
        else{
         digitalWrite(2, HIGH);
          s="BAHAYA";
        }
   
   delay(2000);
  }
}

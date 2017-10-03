// Using one webhook to pass 4 variables to Thingspeak
const String key = "XXXXXXXXXXXX"; // Thingspeak API write key
int soil1;
int soil2;
int soil3;
int soil4;
int temp; 
int network; 
int battery; 
String devicename; 

void setup() {
  pinMode(D0, OUTPUT);      //Soil sensor power pin
  Serial.begin(9600); 
}

void loop() {
  digitalWrite(D0, HIGH);
  delay(500);
  soil1 = analogRead(A4);    //EC-5 
  delay(500);
  soil2= analogRead(A1);    //10-HS 
   delay(500);
  soil3 = analogRead(A2);    //EC-5 
  delay(500);
  soil4= analogRead(A3);    //10-HS  
  digitalWrite(D0, LOW);
  
 
  
  temp = 68;
  FuelGauge fuel;//dummy value for temperature sensor
  battery = fuel.getSoC(); 
  devicename = "PISB2";
 
    CellularSignal sig = Cellular.RSSI();                               //Creates cellular object for measuring Network Strength
    network = sig.rssi;                                                 //Gets network strength in dBm
    if (network < 0){
        network *= -1;                                                  //Ensures that network strength is a positive number as per Ubidots requirements
    }
    
// publish to AWS
      Particle.publish("AWSPISB", "{ \"devicename\": \"" + String(devicename) + "\"," +
        "\"soil1\": \"" + String(soil1) + "\"," +
       "\"soil2\": \"" + String(soil2) + "\"," +
          "\"soil3\": \"" + String(soil3) + "\"," +
       "\"soil4\": \"" + String(soil4) + "\"," +
       "\"temp\": \"" + String(temp) + "\"," +
       "\"network\": \"" + String(network) + "\"," +
       "\"battery\": \"" + String(battery) + "\" }",PRIVATE); 
       
  // Publish to Thingspeak
  Particle.publish("TSPISB", "{ \"1\": \"" + String(soil1) + "\"," +
       "\"2\": \"" + String(soil2) + "\"," +
       "\"3\": \"" + String(soil3) + "\"," +
       "\"4\": \"" + String(soil4) + "\"," +
        "\"5\": \"" + String(temp) + "\"," +
         "\"6\": \"" + String(network) + "\"," +
          "\"7\": \"" + String(battery) + "\"," +
       "\"k\": \"" + key + "\" }", PRIVATE);
       
  // Print values to serial monitor
  Serial.print("Soil1: ");
  Serial.println(soil1);
  Serial.print("Soil2: ");
  Serial.println(soil2);
Serial.print("Soil3: ");
  Serial.println(soil3);
  Serial.print("Soil4: ");
  Serial.println(soil4);
  Serial.print("Network Strength: ");
  Serial.println(network);
  Serial.print("Battery: ");
  Serial.println(battery);
  
  
   delay(45000); // Wait 45 seconds before next loop
  
  // 7.5 minute sleep
  System.sleep(SLEEP_MODE_DEEP,450); //it's in seconds, not milliseconds 7.5 minutes
}
  
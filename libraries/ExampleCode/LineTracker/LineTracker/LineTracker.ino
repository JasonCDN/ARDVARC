const int tracker_pin = A0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Welcome to Line Tracker!");
  Serial.println("------------------------");
  Serial.println("IR Sensor Readings: ");
}

void loop()
{
  Serial.println(analogRead(tracker_pin));
  delay(100);
}

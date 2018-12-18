/*************************************************************
Download latest Blynk library here:
https://github.com/blynkkk/blynk-library/releases/latest

Blynk is a platform with iOS and Android apps to control
Arduino, Raspberry Pi and the likes over the Internet.
You can easily build graphic interfaces for all your
projects by simply dragging and dropping widgets.

Downloads, docs, tutorials: http://www.blynk.cc
Sketch generator:           http://examples.blynk.cc
Blynk community:            http://community.blynk.cc
Follow us:                  http://www.fb.com/blynkapp
http://twitter.com/blynk_app

Blynk library is licensed under MIT license
This example code is in public domain.

*************************************************************

Blynk can provide your device with time data, like an RTC.
Please note that the accuracy of this method is up to several seconds.

App project setup:
RTC widget (no pin required)
Value Display widget on V1
Value Display widget on V2

WARNING :
For this example you'll need Time keeping library:
https://github.com/PaulStoffregen/Time

This code is based on an example from the Time library:
https://github.com/PaulStoffregen/Time/blob/master/examples/TimeSerial/TimeSerial.ino
*************************************************************/

/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define BLYNK_MAX_SENDBYTES 256
//#define BLYNK_MAX_READBYTES 1024
/*------D-Wido-----*/
/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial
// These are the interrupt and control pins for СС3000
#define ADAFRUIT_CC3000_IRQ   7
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

#include <SPI.h>
#include <Adafruit_CC3000.h>
#include <BlynkSimpleCC3000.h>
#include <TimeLib.h>

// Your WiFi credentials.
// Choose wifi_sec from WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
char ssid[] = "Livebox-f1fc";
char pass[] = "aqwzsxedc4321";
int wifi_sec = WLAN_SEC_WPA2;

/*------F-Wido-----*/

char auth[] = "09ee489ef08649fd9f4c5d01ad92e610";

#define pinRelayChauffeau 9 // Définir la pin utilisée
bool etatChauffeau = false;

BlynkTimer timer;

BLYNK_CONNECTED() {
	//get data stored in virtual pin V0 from server
	Blynk.syncAll();
}

BLYNK_WRITE(V12) {
	int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

	if (pinValue == 1 && etatChauffeau == false) {
		//Serial.println("Ouvert");
		//fenetreOuverte = true;
		changeChauffage(true);
	}
	else if (pinValue == 0 && etatChauffeau == true) {
		//Serial.println("Fermé");
		//fenetreOuverte = false;
		changeChauffage(false);
	}
}


bool changeChauffage(bool etat) {
	if (etat)
	{
		digitalWrite(pinRelayChauffeau, HIGH);
		etatChauffeau = true;
		Blynk.virtualWrite(V12, etatChauffeau);
	}
	else
	{
		digitalWrite(pinRelayChauffeau, LOW);
		etatChauffeau = false;
		Blynk.virtualWrite(V12, etatChauffeau);
	}
}

void setup()
{
	Serial.begin(9600);

	if (!cc3000.begin())
	{
		//Serial.println(F("Couldn't begin()! Check your wiring?"));
		while (1);
	}
	while (!cc3000.checkDHCP())
	{
		delay(100); // ToDo: Insert a DHCP timeout!
	}


	pinMode(pinRelayChauffeau, OUTPUT);
	digitalWrite(pinRelayChauffeau, LOW);

	Blynk.begin(auth, ssid, pass, wifi_sec, "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8442);

	/*
	table.onOrderChange([](int indexFrom, int indexTo) {
		//Serial.print("Reordering: ");
		//Serial.print(indexFrom);
		//Serial.print(" => ");
		//Serial.print(indexTo);
		//Serial.println();
	});
	*/

}

void loop()
{
	Blynk.run();
}

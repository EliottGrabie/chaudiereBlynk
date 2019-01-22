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
//#include <Blynk.h>
#define BLYNK_MAX_SENDBYTES 256
//#define BLYNK_MAX_READBYTES 1024

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


char auth[] = "0ab7fa399d5c4956a4517ea871514f36";
char ssid[] = "Livebox-680C";
char pass[] = "aqwzsxedc1234";


#define pinRelayChauffeau D1 // Définir la pin utilisée
bool etatChauffeau = false;

BlynkTimer timer;
WidgetRTC rtc;

int jour = 1;
/*
union uJour
{
	char a;
	byte b[8];
};
*/


float TempNodeSalon = 0;
float TempNode1 = 0;
float TempNode2 = 0;
float TempNode3 = 0;

float tempMin = 0;
float tempMax = 0;

int mode = 0;

int dManuel = 0;

TimeInputParam* lCreneau[6];

//Fonction--------------------------

BLYNK_CONNECTED() {
	//get data stored in virtual pin V0 from server
	//Blynk.syncVirtual(V0);
	//Blynk.syncVirtual(V5);
	Blynk.syncAll();
}

//Btn On Off
BLYNK_WRITE(V12) {
	int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
	//Serial.println(pinValue);
	if (pinValue == 1) {
		//Serial.println("Ouvert");
		//fenetreOuverte = true;
		changeChauffage(true);
	}
	else if (pinValue == 0) {
		//Serial.println("Fermé");
		//fenetreOuverte = false;
		changeChauffage(false);
	}
}

//Brdge 
BLYNK_WRITE(V5) {		//TempNodeSalon
	TempNodeSalon = param.asFloat(); //pinData variable will store value that came via Bridge
}

BLYNK_WRITE(V6) {		//TempNodeSalon
	TempNode1 = param.asFloat(); //pinData variable will store value that came via Bridge
}

BLYNK_WRITE(V7) {		//TempNodeSalon
	TempNode2 = param.asFloat(); //pinData variable will store value that came via Bridge
}

BLYNK_WRITE(V8) {		//TempNodeSalon
	TempNode3 = param.asFloat(); //pinData variable will store value that came via Bridge
}

BLYNK_WRITE(V14) {		//TempMAx
	tempMin = param.asFloat(); //pinData variable will store value that came via Bridge
}
BLYNK_WRITE(V15) {		//TempMAx
	tempMax = param.asFloat(); //pinData variable will store value that came via Bridge
}

BLYNK_WRITE(V16) {		//mode
	mode = param.asInt(); //pinData variable will store value that came via Bridge
}

BLYNK_WRITE(V20) {		//Creneau 1
	TimeInputParam* pt = new TimeInputParam(param);
	//TimeInputParam t(param);
	lCreneau[0] = pt; //pinData variable will store value that came via Bridge
}
BLYNK_WRITE(V21) {		//Creneau 1
	TimeInputParam* pt = new TimeInputParam(param);
	//TimeInputParam t(param);
	lCreneau[1] = pt; //pinData variable will store value that came via Bridge
}
BLYNK_WRITE(V22) {		//Creneau 1
	TimeInputParam* pt = new TimeInputParam(param);
	//TimeInputParam t(param);
	lCreneau[2] = pt; //pinData variable will store value that came via Bridge
}
BLYNK_WRITE(V23) {		//Creneau 1
	TimeInputParam* pt = new TimeInputParam(param);
	//TimeInputParam t(param);
	lCreneau[3] = pt; //pinData variable will store value that came via Bridge
}
BLYNK_WRITE(V24) {		//Creneau 1
	TimeInputParam* pt = new TimeInputParam(param);
	//TimeInputParam t(param);
	lCreneau[4] = pt; //pinData variable will store value that came via Bridge
}
BLYNK_WRITE(V25) {		//Creneau 1
	TimeInputParam* pt = new TimeInputParam(param);
	//TimeInputParam t(param);
	lCreneau[5] = pt; //pinData variable will store value that came via Bridge
}


String makeHeure(int index) {

	int startHeure = index * 2;
	int stopHeure = (index * 2) + 2;
	String heure = "";
	heure = heure + startHeure;
	heure = heure + ":00 - ";
	heure = heure + stopHeure;
	heure = heure + ":00";
	return heure;
}

bool checkCreneau() {
	
	bool etat = false;
	int arraySize = sizeof(lCreneau) / sizeof(lCreneau[0]);
	for (size_t i = 0; i < arraySize; i++)
	{
		if (lCreneau[i]->isWeekdaySelected(dayOfWeek(now()))) {
			if (lCreneau[i]->getStartHour() <= hour() && lCreneau[i]->getStopHour() >= hour()) {
				if (lCreneau[i]->getStartHour() == hour())
				{
					if(lCreneau[i]->getStartMinute() <= minute()){ etat = true; }
				}
				else if (lCreneau[i]->getStopHour() == hour())
				{
					if(lCreneau[i]->getStopMinute() > minute()){ etat = true; }
				}
				else
				{
					etat = true;
				}
			}
		}
	}
	return etat;
	
}

// Digital clock display of the time
void checkClock()
{
	//Blynk.virtualWrite(V2, jSemaine);
	switch (mode)
	{
	case 1:			//Mode On
		//Mode interupteur
		if (hour() == 0) {
			Blynk.virtualWrite(V16, 2);		//retour calendar
			//ou juste Off ???
		}
		if (TempNodeSalon < tempMax) {
			changeChauffage(true);
		}
		//changeChauffage(true);
		break;
	case 2:		//Mode Calendar
		if (checkCreneau())
		{
			changeChauffage(true);
		}
		else if (TempNodeSalon <= tempMin)
		{
			changeChauffage(true);
		}
		else
		{
			changeChauffage(false);
		}
		break;
	case 3:		//Mode Off 
		if (TempNodeSalon <= tempMin) {
			changeChauffage(true);
		}
		else
		{
			changeChauffage(false);
		}
		
		break;
				
	default:
		break;
	}

	//CheckTemp
	if (TempNodeSalon > tempMax) {
		changeChauffage(false);
	}
}

bool changeChauffage(bool etat) {

	if (etatChauffeau != etat) {
		if (etat && (TempNodeSalon <= tempMax))
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

	
}

//Start------------------------------
void setup()
{
	// Debug console
	Serial.begin(115200);
	Serial.println("STAAAAART");
	
	pinMode(pinRelayChauffeau, OUTPUT);
	digitalWrite(pinRelayChauffeau, LOW);
	
	//Blynk.begin(auth, ssid, pass,  "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8442);
	Blynk.begin(auth, ssid, pass,  "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8080);
	
	// Begin synchronizing time
	rtc.begin();

	//delay(1000);
	// Display digital clock every 300000 = 5 min
	timer.setInterval(6000L, checkClock);
}

void loop()
{
	Blynk.run();
	timer.run();
}

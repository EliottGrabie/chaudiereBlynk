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

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


char auth[] = "0ab7fa399d5c4956a4517ea871514f36";
char ssid[] = "Livebox-680C";
char pass[] = "aqwzsxedc1234";


#define pinRelayChauffeau 1 // Définir la pin utilisée
bool etatChauffeau = false;

BlynkTimer timer;
WidgetRTC rtc;

int jour = 1;
bool semaine[7][12] = {
	//	0	1		2		3		4	5		6		7		8	9		10		11		
	{ true, false ,false, false, false ,false, false, false ,false, false, false ,false },	//Lundi
{ false, true ,false, false, false ,false, false, false ,false, false, false ,true },	//Mardi
{ false, false, true, false, false, false, false, false, false, false, true, false },	//Mercredi
{ false, false, false, true, false, false, false, false, false, true, false, false },	//Jeudi
{ false, false, false, false, true, false, false, false, true, false, false, false },	//Vendredi
{ false, false, false, false, false, true, false, true, false, false, false, false },	//Samedi
{ false, false, false, false, false, false, true, false, false, false, false, false },	//Dimanche
};

WidgetTable table;
BLYNK_ATTACH_WIDGET(table, V11);

float TempNode1 = 0;

//Fonction--------------------------

BLYNK_CONNECTED() {
	//get data stored in virtual pin V0 from server
	//Blynk.syncVirtual(V0);
	//Blynk.syncVirtual(V5);
	Blynk.syncAll();
}

BLYNK_WRITE(V0)	//Get calandar from Serveur
{
	//restoring value
	//semaine[][] = param.asInt();
	//Serial.print("syncVirtual : ");
	//Serial.println(param.asString());

	String retSemaine = param.asString();
	int iCar = 0;
	for (int col = 0; col < 12; col++) {

		//Serial.print("col : ");
		//Serial.println(col);

		char str[5] = "0x";

		str[2] = retSemaine[iCar + col];
		iCar++;
		str[3] = retSemaine[iCar + col];
		//iCar++;

		int num;
		Serial.print("str : ");
		Serial.println(str);

		sscanf(str, "%x", &num);

		Serial.print("num : ");
		Serial.println(num);
		unsigned int iToByte = byte(num);

		//iToByte -= 65;
		Serial.print("iToByte : ");
		Serial.println(iToByte);

		byte creneau = byte(iToByte);
		//Serial.print("creneau : ");
		//Serial.println(creneau,BIN);

		//Serial.println("creneau: ");
		for (int _jour = 0; _jour < 7; _jour++) {
			if (creneau & (1 << (6 - _jour)))
			{
				//Serial.print("1");
				semaine[_jour][col] = true;
			}
			else
			{
				//Serial.print("0");
				semaine[_jour][col] = false;
			}
		}

		Serial.println("");

		//delay(500);
	}
}

BLYNK_WRITE(V10) {
	actuTableau(param.asInt());
	jour = param.asInt();
}
//Btn On Off
BLYNK_WRITE(V12) {
	int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
	Serial.println(pinValue);
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

//Brdge 
BLYNK_WRITE(V5) {
	TempNode1 = param.asInt(); //pinData variable will store value that came via Bridge
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

void actuTableau(int _jour) {
	table.clear(); //Cleart Tableau
	_jour--;
	for (size_t i = 0; i < sizeof(semaine[_jour]); i++)	//Parcourt des tranche d'heure
	{
		String etat = "OFF";

		if (semaine[_jour][i]) {
			etat = "ON";
		}
		table.addRow(i, makeHeure(i), etat);
		//table.pickRow(i);
		if (semaine[_jour][i] == false) {
			Blynk.virtualWrite(V11, "deselect", i);
		}
	}

}

// Digital clock display of the time
void checkClock()
{

	String currentTime = String(hour()) + ":" + minute() + ":" + second();
	String currentDate = String(day()) + " " + month() + " " + year();

	String jSemaine;
	switch (dayOfWeek(now()))
	{
	case 1:
		jSemaine = "Dimmanche";
		break;
	case 2:
		jSemaine = "Lundi";
		break;
	case 3:
		jSemaine = "Mardi";
		break;
	case 4:
		jSemaine = "Mercredi";
		break;
	case 5:
		jSemaine = "Jeudi";
		break;
	case 6:
		jSemaine = "Vendredi";
		break;
	case 7:
		jSemaine = "Samedi";
		break;
	default:
		break;
	}

	Blynk.virtualWrite(V2, jSemaine);
	Serial.println(dayOfWeek(now()) - 2);
	Serial.println(int(hour() / 2));
	if (semaine[dayOfWeek(now()) - 2][int(hour() / 2)])
	{
		changeChauffage(true);
	}
	else
	{
		changeChauffage(false);
	}

}
/*
void printTableau() {
	for (size_t _jour = 0; _jour < 7; _jour++)
	{
		for (size_t crn = 0; crn < 12; crn++)
		{
			if (semaine[_jour][crn])
			{
				Serial.print("1");
			}
			else
			{
				Serial.print("0");
			}
		}
		Serial.println("");
	}
	Serial.println("");
}
*/
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
//Start------------------------------
void setup()
{
	// Debug console
	Serial.begin(115200);
	Serial.println("Start");

	pinMode(pinRelayChauffeau, OUTPUT);
	digitalWrite(pinRelayChauffeau, LOW);

	Serial.println("Blynk.begin");
	//Blynk.begin(auth, ssid, pass,  "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8442);
	Blynk.begin(auth, ssid, pass,  "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8080);
	
	// Begin synchronizing time
	rtc.begin();

	table.onOrderChange([](int indexFrom, int indexTo) {
		//Serial.print("Reordering: ");
		//Serial.print(indexFrom);
		//Serial.print(" => ");
		//Serial.print(indexTo);
		//Serial.println();
	});

	table.onSelectChange([](int index, bool selected) {

		String msg = "";

		int _jour = jour - 1;
		if (selected) {
			semaine[_jour][index] = true;
			table.updateRow(index, makeHeure(index), "ON");
		}
		else {
			semaine[_jour][index] = false;
			Blynk.virtualWrite(V11, "update", index, makeHeure(index), "OFF");
		}

		// send to serveur pour save
		for (int col = 0; col < 12; col++) {		//castcalandar to ???
			byte bToInt = 0;
			//Serial.print("creneau : ");
			for (int _jour2 = 0; _jour2 < 7; _jour2++) {
				//Serial.print(semaine[_jour2][col]);
				if (semaine[_jour2][col]) {
					bToInt |= 1 << (6 - _jour2);
				}
			}
			//Serial.println("");
			//Serial.print("bToInt : ");
			//Serial.println(bToInt, BIN);
			char car = char(bToInt);
			char hexa[3];

			sprintf(hexa, "%.2X", car);
			//Serial.println(hexa,BIN);
			msg += hexa;



			//delay(500);
		}
		Serial.print("msg : ");
		Serial.println(msg);
		/*Serial.print("  Size : ");
		Serial.println(sizeof(msg));*/
		Blynk.virtualWrite(V0, msg);		//Put calandar to serveur

	});

	Blynk.virtualWrite(V10, jour);
	actuTableau(jour);
	//delay(1000);
	// Display digital clock every 300000 = 5 min
	Serial.println("finsetup");
	timer.setInterval(6000L, checkClock);
}

void loop()
{
	Blynk.run();
	timer.run();
}

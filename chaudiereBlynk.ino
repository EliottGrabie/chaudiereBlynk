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

BLYNK_CONNECTED() {
	//get data stored in virtual pin V0 from server
	Blynk.syncVirtual(V0);
}
/*
BLYNK_WRITE(V0)
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
		
		str[2] = retSemaine[iCar+col];
		iCar++;
		str[3] = retSemaine[iCar+col];
		//iCar++;
				
		int num;
		//Serial.print("str : ");
		//Serial.println(str);

		sscanf(str, "%x", &num);

		//Serial.print("num : ");
		//Serial.println(num);
		unsigned int iToByte = byte(num);
		
		//iToByte -= 65;
		//Serial.print("iToByte : ");
		//Serial.println(iToByte);

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
		
		//Serial.println("");
		
		//delay(500);
	}
}
*/
BLYNK_WRITE(V10) {
	actuTableau(param.asInt());
	jour = param.asInt();
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


String makeHeure(int index) {

	String heure = "";
	heure = heure + (index * 2);
	heure = heure + ":00 - ";
	heure = heure + ((index * 2) + 2);
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

	Blynk.virtualWrite(V1, jSemaine);
	//Serial.println(dayOfWeek(now()) - 2);
	//Serial.println(int(hour() / 2));
	if (semaine[dayOfWeek(now()) - 2][int(hour()/2)])
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
	table.onSelectChange([](int index, bool selected) {

		String msg = "";

		int _jour = jour-1;
		if (selected) {
			semaine[_jour][index] = true;
			table.updateRow(index, makeHeure(index), "ON");
		}
		else{
			semaine[_jour][index] = false;
			Blynk.virtualWrite(V11, "update", index, makeHeure(index), "OFF");
		}
		
		// send to serveur pour save
		for (int col = 0; col < 12; col++) {
			byte bToInt = 0;
			//Serial.print("creneau : ");
			for (int _jour2 = 0; _jour2 < 7; _jour2++) {
				//Serial.print(semaine[_jour2][col]);
				if (semaine[_jour2][col]) {
					bToInt |= 1 << (6-_jour2);
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

		}
		//Serial.print("msg : ");
		//Serial.println(msg);

		Blynk.virtualWrite(V0, msg);

	});

	Blynk.virtualWrite(V10, jour);
	actuTableau(jour);
	//delay(1000);
	// Display digital clock every 300000 = 5 min
	timer.setInterval(30000L, checkClock);
}

void loop()
{
	Blynk.run();
	timer.run();
}

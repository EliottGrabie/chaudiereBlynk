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
#define BLYNK_PRINT Serial


#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "0ab7fa399d5c4956a4517ea871514f36";

#define W5100_CS  10
#define SDCARD_CS 4

BlynkTimer timer;

WidgetRTC rtc;

WidgetTable table;
BLYNK_ATTACH_WIDGET(table, V11);

int rowIndex = 0;

byte mac[] = { 0xAD, 0x01, 0xAA, 0xBB, 0xCC, 0x03};

				//	1		2		3		4	5		6		7		8	9		10		11		12	
bool Lundi[12] = { false, false ,false, true, false ,false, false, false ,false, false, true ,false };
bool Mardi[12] = { false, false ,false, false, false ,false, false, false ,false, false, false ,false };
bool Mercredi[12] = { false, false ,false, false, false ,false, false, false ,false, false, false ,false };
bool Jeudi[12] = { false, false ,false, false, false ,false, false, false ,false, false, false ,false };
bool Vendredi[12] = { false, false ,false, false, false ,false, false, false ,false, false, false ,false };
bool Samedi[12] = { false, false ,false, false, false ,false, false, false ,false, false, false ,false };
bool Dimanche[12] = { false, false ,false, false, false ,false, false, false ,false, false, false ,false };


//// Add Crenau
//bool jour[7] = { 0, 0, 0, 0, 0, 0, 0};
//int startHeure = 0;
//int startMin = 0;
//int stopHeure = 0;
//int stopMin = 0;

//// Button add
//BLYNK_WRITE(V10) {
//	if (param.asInt()) {
//		table.addRow(rowIndex, "Test row", millis() / 1000);
//		//table.pickRow(rowIndex);
//		rowIndex++;
//	}
//}
//
//// Button delete
//BLYNK_WRITE(V11) {
//	if (param.asInt()) {
//		table.clear();
//		rowIndex = 0;
//	}
//}
//
////Horloge
//BLYNK_WRITE(V12) {
//	TimeInputParam t(param);
//
//	// Process start time
//
//	if (t.hasStartTime())
//	{
//		Serial.println(String("Start: ") +
//			t.getStartHour() + ":" +
//			t.getStartMinute());
//
//		startHeure = t.getStartHour;
//		startMin = t.getStartMinute;
//	}
//
//	// Process stop time
//
//	if (t.hasStopTime())
//	{
//		Serial.println(String("Stop: ") +
//			t.getStopHour() + ":" +
//			t.getStopMinute());
//
//		stopHeure = t.getStopHour;
//		startMin = t.getStopMinute;
//	}
//	
//	for (int i = 1; i <= 7; i++) {
//		if (t.isWeekdaySelected(i)) {
//			Serial.println(String("Day ") + i + " is selected");
//
//			jour[i] = true;
//		}
//	}
//
//	Serial.println();
//}

void actuTableau(int Jour) {
	table.clear();
	switch (Jour)
	{
	case 1: // Item 1
		for (size_t i = 0; i < sizeof(Lundi); i++)
		{
			String etat = "OFF";
			int startHeure = i * 2;
			int stopHeure = (i * 2) + 2;
			String heure = "";
			heure = "";
			heure = heure + startHeure;
			heure = heure + ":00 - ";
			heure = heure + stopHeure;
			heure = heure + ":00";
			if (Lundi[i]) {
				etat = "ON";
			}
			table.addRow(i, heure, etat);

			if (Lundi[i] == false) {
				Blynk.virtualWrite(V11, "deselect", i);
			}
			
		}

		break;
	case 2: // Item 1

		break;
	case 3: // Item 2

		break;
	case 4: // Item 3

		break;
	case 5: // Item 1

		break;
	case 6: // Item 1

		break;
	case 7: // Item 1

		break;

	}

}

BLYNK_WRITE(V10) {
	actuTableau(param.asInt());
	switch (param.asInt())
	{
	case 1: // Item 1
		Serial.println("Item 1 selected");
		break;
	case 2: // Item 2
		Serial.println("Item 2 selected");
		break;
	case 3: // Item 3
		Serial.println("Item 3 selected");
		break;
	default:
		Serial.println("Unknown item selected");
	}
}

// Digital clock display of the time
void clockDisplay()
{
	// You can call hour(), minute(), ... at any time
	// Please see Time library examples for details

	String currentTime = String(hour()) + ":" + minute() + ":" + second();
	String currentDate = String(day()) + " " + month() + " " + year();
	//Serial.print("Current time: ");
	//Serial.print(currentTime);
	//Serial.print(" ");
	//Serial.print(currentDate);
	//Serial.println();

	// Send time to the App
	Blynk.virtualWrite(V1, currentTime);
	// Send date to the App
	Blynk.virtualWrite(V2, currentDate);

	
}

void setup()
{
	// Debug console
	Serial.begin(9600);

	pinMode(SDCARD_CS, OUTPUT);
	digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card

	//Blynk.begin(auth);
	// You can also specify server:
	//Blynk.begin(auth, "blynk-cloud.com", 8442);
	//Blynk.begin(auth, IPAddress(192,168,1,100), 8442);
	Blynk.begin(auth, "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8442);

	// Begin synchronizing time
	//rtc.begin();

	// Other Time library functions can be used, like:
	//   timeStatus(), setSyncInterval(interval)...
	// Read more: http://www.pjrc.com/teensy/td_libs_Time.html

	table.onOrderChange([](int indexFrom, int indexTo) {
		Serial.print("Reordering: ");
		Serial.print(indexFrom);
		Serial.print(" => ");
		Serial.print(indexTo);
		Serial.println();
	});

	table.onSelectChange([](int index, bool selected) {
		Serial.print("Item ");
		Serial.print(index);
		Serial.print(selected ? " marked" : " unmarked");
		//TODO change tableau
	});
	
	// Display digital clock every 10 seconds
	timer.setInterval(1000L, Main);
}

void loop()
{
	Blynk.run();
	timer.run();
}

void Main() {
	//printf("Test");
}
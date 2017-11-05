#define sprint Serial.print 
#define sprintln Serial.println
#include <IRremote.h>

#define IR_RCVR_PIN 11
IRrecv ir_receiver(IR_RCVR_PIN);
decode_results results;

void setup() {
	Serial.begin(9600);
	ir_receiver.enableIRIn(); // Start the receiver
}

void loop() {
	if (ir_receiver.decode(&results)) {
		dump(&results);
		ir_receiver.resume(); // Receive the next value
	}
}

int c = 1;

void dump(decode_results *results) {
	int count = results->rawlen;
	sprintln(c);
	c++;
	sprintln("For IR Scope: ");
	for (int i = 1; i < count; i++) {
		sprint("0x");
		sprint((unsigned int)results->rawbuf[i], HEX);
		sprint(" ");
	}

	sprintln("");
	sprintln("For Arduino sketch: ");
	sprint("unsigned int raw[");
	sprint(count, DEC);
	sprint("] = {");
	for (int i = 1; i < count; i++) {
		sprint("0x");
		sprint((unsigned int)results->rawbuf[i], HEX);
		sprint(",");
	}
	sprint("};");
	sprintln("");
	sprint("irsend.sendRaw(raw,");
	sprint(count, DEC);
	sprint(",38);");
	sprintln("");
	sprintln("");
}
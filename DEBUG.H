
// ----------------------------------
#ifdef _DEBUG
void DBG(String msg)    { Serial.print(msg); }
void DBG(byte data)     { Serial.print(data, HEX); Serial.print(" "); }
void DBG(int num)       { Serial.print(num); Serial.print(" "); }
void DBGLN(String msg)  { Serial.println(msg); }
void DBGLN(byte data)   { Serial.print(data, HEX); Serial.println(" "); }
void DBGLN(int num)     { Serial.print(num); Serial.println(" "); }
void Dump(uint8_t len, uint8_t *buf)
{
	Serial.print("len = ");
	Serial.println(len);
	for (int i = 0; i<len; i++) {
		Serial.print(buf[i], HEX);
		Serial.print(", ");
	}
	Serial.println("\n");
}
#else
# define DBG(a)
# define DBGLN(a)
# define Dump(a,b)
#endif

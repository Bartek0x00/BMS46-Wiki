#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Data

struct StatusBit {
	const char *name;
	uint8_t byte_idx;
	uint8_t mask;
	uint8_t value;
};
const struct StatusBit STATUS_BITS[] = {
	{}
};

struct ErrorLocation {
	uint8_t ort;
	const char *text;
	uint8_t uw_1;
	uint8_t uw_2;
	uint8_t uw_3;
};
const struct ErrorLocation ERROR_LOCATIONS[] = {
	{}
};

struct ErrorType {
	uint8_t idx;
	const char *text;
};
const struct ErrorType ERROR_TYPES[] = {
	{}
};

struct DataFactor {
	uint8_t idx;
	const char *text;
	const char *unit;
	float factor_a;
	float factor_b;
	float factor_c;	
};
const struct DataFactor DATA_FACTORS[] = {
	{}
};

struct JobResult {
	uint8_t id;
	const char *status_text;
};
const struct JobResult JOB_RESULTS[] = {
	{0xA0, "OKAY"},
	{0xA1, "BUSY"},
	{0xA2, "ERROR_ECU_REJECTED"},
	{0xB0, "ERROR_ECU_PARAMETER"},
	{0xB1, "ERROR_ECU_FUNCTION"},
	{0xB2, "ERROR_ECU_NUMBER"},
	{0xFF, "ERROR_ECU_NACK"},
	{0x00, "ERROR_ECU_UNKNOWN_STATUSBYTE"}
};

// Builtin

extern void xconnect(void);
extern void xstopf(void);
extern void xreps(uint8_t repetition);
extern void xsetpar(uint8_t[] param, uint8_t len);
extern void atsp(uint8_t a, uint8_t b);
extern void xreset(void);
extern void xsend(uint8_t *request, int req_len, uint8_t *response_out);
extern void ergs(const char *result_name, const char *value);
extern const char *tabget(const char *table_name, uint8_t lookup_byte);

// Job Code

void ISN_LESEN()
{
	uint8_t req_payload[] = {0x12, 0x04, 0x00};
	uint8_t response[256];
	char hex_buffer[512];

	xsend(req_payload, sizeof(req_payload), response);

	const char *status_text = JOB_RESULTS[response[3]];

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint8_t check_bytes[4];
	memcpy(check_bytes, &response[41], 4);

	if (check_bytes[0] == 0xFF) {
		ergs("ISN_LESEN_WERT", "77");
		return;
	}

	for (int i = 0; i < sizeof(response); i++)
		sprintf(&hex_buffer[i * 2], "%02X", bytes[i]);

	char isn_substring[5];
	strncpy(isn_substring, &hex_buffer[7], 4);
	isn_substring[4] = '\0';

	ergs("ISN_LESEN_WERT", isn_substring);
}

struct {
	char job_status[64];
	uint8_t out_data[1024];
	uint32_t out_len;
	char data_unit[4];
} RamLesenResult;
struct RamLesenResult RAM_LESEN(uint32_t addr, uint32_t len)
{
	struct RamLesenResult result;

	strcpy(result.data_unit, "HEX");
	result.out_len = 0;

	uint8_t payload[8];
	payload[0] = 0x12;
	payload[1] = 0x09;
	payload[2] = 0x06;
	payload[3] = 0x00;
	payload[4] = (addr >> 16) & 0xFF;
	payload[5] = (addr >> 8)  & 0xFF;
	payload[6] = (addr >> 0)  & 0xFF;
	payload[7] = len & 0xFF;

	uint8_t response_buff[2048];
	uint32_t response_len = 0;

	xsend(payload, sizeof(payload), response_buff, &response_len);

	const char *status_text = JOB_RESULTS[response_buffer[0]];
	strncpy(result.job_status, status_text, sizeof(result.job_status) - 1);

	if (response_len > 3) {
		result.out_len = response_len - 3;
		memcpy(result.out_data, &response_buff[3], result.out_len);
	}
	
	return result;
}

struct {
	char job_status[64];
	uint8_t out_data[1024];
	uint32_t out_len;
	char data_unit[4];
} RomLesenResult;
struct RomLesenResult ROM_LESEN(uint32_t addr, uint32_t len)
{
	struct RomLesenResult result;

	strcpy(result.data_unit, "HEX");
	result.out_len = 0;

	uint8_t payload[8];
	payload[0] = 0x12;
	payload[1] = 0x09;
	payload[2] = 0x06;
	payload[3] = 0x00;
	payload[4] = (addr >> 16) & 0xFF;
	payload[5] = (addr >> 8)  & 0xFF;
	payload[6] = (addr >> 0)  & 0xFF;
	payload[7] = len & 0xFF;

	uint8_t response_buff[2048];
	uint32_t response_len = 0;

	xsend(payload, sizeof(payload), response_buff, &response_len);

	const char *status_text = JOB_RESULTS[response_buffer[0]];
	strncpy(result.job_status, status_text, sizeof(result.job_status) - 1);

	if (response_len > 3) {
		result.out_len = response_len - 3;
		memcpy(result.out_data, &response_buff[3], result.out_len);
	}
	
	return result;
}

struct IdentFrame {
	char bmw_no[8];
	uint16_t hw_no;
	char code_index[3];
	char diag_index[3];
	char bus_index[3];
	char prod_week[3];
	char prod_year[3];
	char supplier_no[11];
	char sw_no[3];
	char change_no[3];
	char prod_no[10];
}

struct IdentFrame IDENT()
{
	struct IdentFrame result;

	uint8_t request[] = {0x12, 0x04, 0x00};
	uint8_t response_buff[64];
	uint32_t response_len = 0;

	xsend(request, sizeof(request), response_buff, &response_len);

	const char *status_text = JOB_RESULTS[response_buff[0]];
	// Status code checks got truncated

	memcpy(result.bmw_no, 		&response[3],  7);
	result.hw_no = *(uint16_t *)&response[10];
	memcpy(result.code_index,	&response[12], 2);
	memcpy(result.diag_index, 	&response[14], 2);
	memcpy(result.bus_index, 	&response[16], 2);
	memcpy(result.prod_week, 	&response[18], 2);
	memcpy(result.prod_year, 	&response[20], 2);
	memcpy(result.supplier_no, 	&response[22], 10);
	memcpy(result.sw_no, 		&response[32], 2);
	memcpy(result.change_no, 	&response[34], 2);
	memcpy(result.prod_no, 		&response[36], 9);

	return result
}

void EDIC_RESET(void)
{
	xreset();
	ergs("JOB_STATUS", "OKAY");
}

void INITIALISIERUNG(void)
{
	xconnect();
	xstopf();
	xreps(2);

	uint8_t proto_params[] = {
		0x06, 0x00, 0x80, 0x25,
		0x12, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xD0, 0x07,
		0x64, 0x00, 0x32, 0x00,
		0x00, 0x00
	};
	xsetpar(proto_params, sizeof(proto_params));

	uint8_t response_len_config[] = {0xFF, 0xFF, 0x00, 0x00};
	xawlen(response_len_config, sizeof(response_len_config));

	atsp(1, 4);

	ergs("JOB_STATUS", "DONE");
}

void INFO(void)
{
	ergs("ECU", "BMS 46");
	ergs("ORIGIN", "BMW TP-421 Weber");
	ergs("REVISION", "0.152");
	ergs("AUTHOR", "Hecht, EE-34 - Entwicklungsversion");
	ergs("COMMENT", "");
	ergs("SPRACHE", "Deutsch");
	ergs("JOB_STATUS", "DONE");
}

void STATUS_CODIER_CHECKSUMME(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x04, 0x0A};
	int32_t checksum_value = 0;

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	checksum_value = response[2];

	ergs("STATUS_CHECKSUMME_WERT", checksum_value);
	ergs("JOB_STATUS", status_text);
}

// Engine_RPM
void STATUS_MOTORDREHZAHL(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t raw_rpm = (response_data[3] << 8) | response_data[4];
	float engine_rpm = raw_rpm * 0.15625f;

	ergs("STATUS_MOTORDREHZAHL_WERT", engine_rpm);
	ergs("STATUS_MOTORDREHZAHL_EINH", "1/min");
}

// Engine_Coolant
void STATUS_MOTORTEMPERATUR(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t raw_temp = (response_data[5] << 8) | response_data[6];
	float engine_temp = (raw_temp * 0.75f) / 256.f - 48.f;

	ergs("STATUS_MOTORTEMPERATUR_WERT", engine_temp);
	ergs("STATUS_MOTORTEMPERATUR_EINH", "Grad C");
}

// Engine_IAT (Intake Air Temperature)
void STATUS_ANSAUGLUFTTEMPERATUR(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t raw_temp = (response_data[7] << 8) | response_data[8];
	float iat_temp = (raw_temp * 0.75f) / 256.f - 48.f;

	ergs("STATUS_ANSAUGLUFTTEMPERATUR_WERT", iat_temp);
	ergs("STATUS_ANSAUGLUFTTEMPERATUR_EINH", "Grad C");
}

// Engine_Load
void STATUS_LASTSIGNAL_WRGL(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t raw_load = response_data[9];
	float load = raw_load * 3.f;

	ergs("STATUS_LASTSIGNAL_WRGL_WERT", load);
	ergs("STATUS_LASTSIGNAL_WRGL_EINH", "mg/AS/Zylinder");
}

// Engine_Battery
void STATUS_UBATT(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t raw_voltage = response_data[10];
	float voltage = raw_voltage * 0.0948f;

	ergs("STATUS_UBATT_WERT", load);
	ergs("STATUS_UBATT_EINH", "V");
}

// Vehicle_Speed
void STATUS_GESCHWINDIGKEIT(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t raw_speed = response_data[11];
	float vehicle_speed = raw_speed * 1.f;

	ergs("STATUS_GESCHWINDIGKEIT_WERT", load);
	ergs("STATUS_GESCHWINDIGKEIT_EINH", "km/h");
}

// Engine_Throttle_Raw
void STATUS_DKWINKEL(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t raw_throttle = (response_data[12] << 8) | response_data[13];
	float throttle_voltage = (raw_throttle * 5.f) / 65472.f;

	ergs("STATUS_DKWINKEL_WERT", throttle_voltage);
	ergs("STATUS_DKWINKEL_EINH", "V");
}

// Engine_MAF_Raw
void STATUS_LASTSIGNAL_SPG(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t maf_raw = (response_data[14] << 8) | response_data[15];
	float maf = (maf_raw * 5.f) / 65535.f;

	ergs("STATUS_LASTSIGNAL_SPG_WERT", maf_raw);
	ergs("STATUS_LASTSIGNAL_SPG_EINH", "V");
}

// Engine_Ignition_Advance_Cyl1
void STATUS_ZUENDWINKEL(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t ignition_raw = (response_data[16] << 8) | response_data[17];
	float ignition = (ignition_raw - 30.f) * 0.75f;

	ergs("STATUS_ZUENDWINKEL_WERT", ignition);
	ergs("STATUS_ZUENDWINKEL_EINH", "Grad KW");
}

// Engine_STFT
void STATUS_LAMBDAREGELFAKTOR(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t stft_raw = response_data[17];
	float stft = stft_raw / 128.f;

	ergs("STATUS_LAMBDAREGELFAKTOR_WERT", stft);
	ergs("STATUS_LAMBDAREGELFAKTOR_EINH", "--");
}

// get_EVAP_duty()
void STATUS_TE_TASTVERHAELTNIS(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t evap_raw = response_data[18];
	float evap = (evap_raw * 100.f) / 256.f;

	ergs("STATUS_TE_TASTVERHAELTNIS_WERT", evap);
	ergs("STATUS_TE_TASTVERHAELTNIS_EINH", "%");
}

// Engine_Injection_CYL1
void STATUS_EINSPRITZZEIT_P_U(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t injection_raw = (response_data[19] << 8) | response_data[20];
	float injection = (injection_raw * 4.f) / 1000.f;

	ergs("STATUS_EINSPRITZZEIT_P_U_WERT", injection);
	ergs("STATUS_EINSPRITZZEIT_P_U_EINH", "ms");
}

// Engine_Theoritical_MAF
void STATUS_ML(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t ml_raw = (response_data[21] << 8) | response_data[22];
	float ml = ml_raw / 5.f;

	ergs("STATUS_ML_WERT", ml);
	ergs("STATUS_ML_EINH", "kg/h");
}

// Engine_ICV_Correction
void STATUS_LEERLAUFINTEGRATOR(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t ll_raw = (response_data[23] << 8) | response_data[24];
	float ll = (ll_raw * 51.2f) / 65536.f;

	ergs("STATUS_LEERLAUFINTEGRATOR_WERT", ll);
	ergs("STATUS_LEERLAUFINTEGRATOR_EINH", "kg/h");
}

// Engine_ICV_Target
void STATUS_LEERLAUFSTELLER_SOLL_LUFTMASSE(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t ll_raw = (response_data[25] << 8) | response_data[26];
	float ll = ll_raw / 80.f;

	ergs("STATUS_LEERLAUFSTELLER_SOLL_LUFTMASSE_WERT", ll);
	ergs("STATUS_LEERLAUFSTELLER_SOLL_LUFTMASSE_EINH", "kg/h");
}

// Engine_LTAT
void STATUS_ADAP_LEERLAUFLUFT(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t ll_raw = (response_data[27] << 8) | response_data[28];
	float ll = ll_raw / 80.f;

	ergs("STATUS_ADAP_LEERLAUFLUFT_WERT", ll);
	ergs("STATUS_ADAP_LEERLAUFLUFT_EINH", "kg/h");
}

// Engine_LTFT
void STATUS_ADAP_GEM_MUL(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t gem_raw = (response_data[29] << 8) | response_data[30];
	float gem = gem_raw / 32768.f;

	ergs("STATUS_ADAP_GEM_MUL_WERT", gem);
	ergs("STATUS_ADAP_GEM_MUL_EINH", "--");
}

// Engine_LTFT_Additive
void STATUS_ADAP_GEM_ADD(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t gem_raw = (response_data[31] << 8) | response_data[32];
	float gem = gem_raw / 78.4f;

	ergs("STATUS_ADAP_GEM_ADD_WERT", gem);
	ergs("STATUS_ADAP_GEM_ADD_EINH", "--");
}

// Engine_RPM_Idle_Target
void STATUS_SOLLDREHZAHL_LLR(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t idle_rpm_raw = (response_data[33] << 8) | response_data[34];
	float idle_rpm = idle_rpm_raw * 10240.f / 65536.f;

	ergs("STATUS_SOLLDREHZAHL_LLR_WERT", idle_rpm);
	ergs("STATUS_SOLLDREHZAHL_LLR_EINH", "1/min");
}

// Vehicle_eeprom_throttle_adaptation
void STATUS_ADAP_DKP(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t throttle_adap_raw = response_data[35];
	float throttle_adap = (throttle_adap_raw * 100.f) / 256.f;

	ergs("STATUS_ADAP_DKP_WERT", throttle_adap);
	ergs("STATUS_ADAP_DKP_EINH", "%");
}


void STATUS_BSZ(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t bsz_ticks_raw = (response_data[36] << 8) | response_data[37];
	float bsz = bsz_ticks_raw * 0.1f;

	ergs("STATUS_BSZ_WERT", bsz);
	ergs("STATUS_BSZ_EINH", "h");
}

// Engine_Lambda1_Voltage
void STATUS_US(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t us_raw = (response_data[38] << 8) | response_data[39];
	float us = (us_raw * 1.25f) / 65535.f;

	ergs("STATUS_US_WERT", bsz);
	ergs("STATUS_US_EINH", "h");
}

// Engine_Throttle_Voltage_Raw
void STATUS_UDK_REF(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t udk_raw = (response_data[40] << 8) | response_data[41];
	float udk = (udk_raw * 5.5f) / 65472.f;

	ergs("STATUS_UDK_REF_WERT", udk);
	ergs("STATUS_UDK_REF_EINH", "V");
}

// Engine_MAF_Raw
void STATUS_UHFM_REF(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t uhfm_raw = (response_data[42] << 8) | response_data[43];
	float uhfm = (uhfm_raw * 5.5f) / 65472.f;

	ergs("STATUS_UHFM_REF_WERT", uhfm);
	ergs("STATUS_UHFM_REF_EINH", "V");
}

// Engine_Radiator_Temp_Target
void STATUS_TKA(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t tka_raw = (response_data[44] << 8) | response_data[45];
	float tka = (tka_raw * 0.75f) / 256.f - 48.f;

	ergs("STATUS_TKA_WERT", tka);
	ergs("STATUS_TKA_EINH", "Grad C");
}

// Engine_EFAN_PWM
void STATUS_EL_TV(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t efan_raw = response_data[46];
	float efan = (efan_raw * 100.f) / 256.f;

	ergs("STATUS_EL_TV_WERT", efan);
	ergs("STATUS_EL_TV_EINH", "%");
}

// Engine_Thermostat_PWM
void STATUS_KFK_TV(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t thermostat_raw = response_data[47];
	float thermostat = thermostat_raw * 1.f;

	ergs("STATUS_KFK_TV_WERT", thermostat);
	ergs("STATUS_KFK_TV_EINH", "%");
}

// Engine_Throttle_Percentage
void STATUS_W_FPDK(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t throttle_raw = response_data[48];
	float throttle = (throttle_raw * 100.f) / 256.f;

	ergs("STATUS_W_FPDK_WERT", throttle);
	ergs("STATUS_W_FPDK_EINH", "%");
}

// Engine_EVAP_Factor
void STATUS_FTEA(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t evap_raw = (response_data[53] << 8) | response_data[54];
	float evap = evap_raw / 32768.f;

	ergs("STATUS_FTEA_WERT", evap);
	ergs("STATUS_FTEA_EINH", "-");
}

// Engine_Lambda1_Heater_PWM
void STATUS_LAMBDA_TV(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t heater_raw = (response_data[55] << 8) | response_data[56];
	float heater = (heater_raw * 100.f) / 3120.f;

	ergs("STATUS_LAMBDA_TV_WERT", heater);
	ergs("STATUS_LAMBDA_TV_EINH", "%");
}

// Engine_ICV_PWM
void STATUS_TAU(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t icv_raw = (response_data[57] << 8) | response_data[58];
	float icv = (icv_raw * 100.f) / 4092.f;

	ergs("STATUS_TAU_WERT", icv);
	ergs("STATUS_TAU_EINH", "%");
}

// Engine_Battery
void STATUS_UB_KL15(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t ubatt_raw = response_data[60];
	float ubatt = ubatt_raw * 0.0948f;

	ergs("STATUS_UB_KL15_WERT", ubatt);
	ergs("STATUS_UB_KL15_EINH", "V");
}

// Engine_AA_CYL1
void STATUS_LUT1_W(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t accel_raw = (response_data[61] << 8) | response_data[62];
	float accel = accel_raw / 256.f;

	ergs("STATUS_LUT1_W_WERT", accel);
	ergs("STATUS_LUT1_W_EINH", "1/s^2");
}

// Engine_AA_CYL2
void STATUS_LUT2_W(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t accel_raw = (response_data[61] << 8) | response_data[62];
	float accel = accel_raw / 256.f;

	ergs("STATUS_LUT2_W_WERT", accel);
	ergs("STATUS_LUT2_W_EINH", "1/s^2");
}

// Engine_AA_CYL3
void STATUS_LUT3_W(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t accel_raw = (response_data[61] << 8) | response_data[62];
	float accel = accel_raw / 256.f;

	ergs("STATUS_LUT3_W_WERT", accel);
	ergs("STATUS_LUT3_W_EINH", "1/s^2");
}

// Engine_AA_CYL4
void STATUS_LUT4_W(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request = {0x12, 0x05, 0x0B, 0x03};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t accel_raw = (response_data[61] << 8) | response_data[62];
	float accel = accel_raw / 256.f;

	ergs("STATUS_LUT4_W_WERT", accel);
	ergs("STATUS_LUT4_W_EINH", "1/s^2");
}

void STATUS_UEBERDREH(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x05, 0x0B, 0xA2};

	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;

	uint16_t nmaxsp_raw = (response_data[3] << 8) | response_data[4];
	float nmaxsp = nmaxsp_raw / 6.4f;
	ergs("STATUS_UEBERDREH_NMAXSP", nmaxsp);

	uint16_t nmaxbs_raw = (response_data[5] << 8) | response_data[6];
	float nmaxbs = nmaxbs_raw * 0.1f;
	ergs("STATUS_UEBERDREH_NMAXBS", nmaxbs);

	uint16_t nmaxhz_raw = response_data[7];
	float nmaxhz = nmaxhz_raw * 1.f;
	ergs("STATUS_UEBERDREH_NMAXHZ", nmaxhz);
}

void STEUERN_EV_1(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x54, 0x01, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_EV_2(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x53, 0x01, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_EV_3(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x52, 0x01, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_EV_4(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x51, 0x01, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_EV_1_AUS(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x54, 0x00, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_EV_2_AUS(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x53, 0x00, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_EV_3_AUS(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x52, 0x00, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_EV_4_AUS(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x51, 0x00, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_DISA(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x4E, 0x01, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_SSP(void)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x36, 0x01, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_E_LU(uint8_t duty)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x59, duty, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}

void STEUERN_LSH(uint8_t duty)
{
	uint8_t response_data[256];
	uint32_t response_len = 0;

	uint8_t request[] = {0x12, 0x07, 0x0C, 0x21, duty, 0x01};
	
	do {
		xsend(request, sizeof(request), response_data, &response_len);
		const char *status_text = JOB_RESULTS[response_data[0]];
		ergs("STATUS_TEXT", status_text);
	} while (strcmp(status_text, "BUSY") == 0);

	ergs("JOB_STATUS", status_text);

	if (strcmp(status_text, "OKAY") != 0)
		return;
}



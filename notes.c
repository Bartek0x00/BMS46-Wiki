// PSEUDOCODE

// Voltage multiplier x0.068

// Engine_RPM addr 0x00FF8334
// Engine_RPM multiplier x46.36363636

// Engine_RPM Med vs High limit (6200 RPM) 0x917f

// Engine_LOAD addr 0x00FF837C
// Engine_LOAD multiplier x0.715

// Engine_Throttle multiplier x0.5625
// Engine_Throttle move_toward_multiplier x(128 / 255)

// Engine_Coolant_Temp addr 0x00FF832A
// Engine_Coolant_Temp move_toward_multiplier x(48 / 255)
// Engine_Coolant_Temp multiplier x0.75 - 48 (Celsius)

// Engine_Intake_Air_Temp multiplier x - 40 (Celsius)

// Ignition_Advance multiplier x0.75 - 22.5

typedef unsigned char byte;

union Reg {
	unsigned int l;
	unsigned short w;
	byte b;
};

Reg D0;
Reg D1;
Reg D2;
Reg D3;
Reg D4;
Reg D5;
Reg D6;
Reg D7;

Reg A0;
Reg A1;
Reg A2;
Reg A3;
Reg A4;
Reg A5;
Reg A6;
Reg A7

// 0x0e1e()

void FUN_000028d8(uint8_t cond, uint32_t myst_24bit, uint8_t flag, uint8_t length)
{
	if ((cond == 255) || (cond == 162)) {
		BYTE_00ff83aa = 0;
		PTR_00ff83a6->mode = cond;
		return;
	}

	if (cond != 160) {
		BYTE_00ff83aa = 0;
		PTR_00ff83a6->mode = 162;
		return;
	}

	PTR_00ff83a6->mode = 160;
	PTR_00ff83a6->type = PTR_00ff83a2->type;
	PTR_00ff83a6->addr_med = (uint8_t)(myst_24bit >> 16);
	PTR_00ff83a6[5] = (uint8_t)(myst_24bit >> 8);
	PTR_00ff83a6[6] = (uint8_t)(myst_24bit >> 0);
	PTR_00ff83a6[7] = length
	PTR_00ff83a6[8] = flag;
	BYTE_00ff83aa = 0;
	return;
}

void FUN_000021e0()
{
	word *pos = (uint32_t)(
		((uint32_t)PTR_00ff83a2[4] << 16) |
		((uint32_t)PTR_00ff83a2[5] << 8) |
		((uint32_t)PTR_00ff83a2[6] << 0)
	);

	uint8_t out_len;

	if (PTR_00ff83a2[3] == 0) {
		uint8_t in_range = check_in_range(pos, 4, PTR_00ff83a2[7], &out_len);
		if (in_range != 0xff) {
			
		}
	}
}

void verify_checksums()
{
	UINT_00ff8822 = (uint32_t)(
		((uint32_t)PTR_00ff83a2[4] << 16) |
		((uint32_t)PTR_00ff83a2[5] << 8) |
		((uint32_t)PTR_00ff83a2[6] << 0)
	);

	if (!((UINT_00ff8822 < 0xff0000) && PTR_00ff83a2[3])) {
		unknown = PTR_00ff83a2[7];
		flag = 1;
		myst_24bit = UINT_00ff8822;
		cond = 160;
		FUN_000028d8(cond, myst_24bit, flag, unknown);
		return;
	}
	
	flag = 
	//char *empty  == 0xFFFFFFFF...
	
	//char *0x3fe6 == "01119700"
	//char *0x3fee == "01119700"
	//char *0x3ff6 == "01119700"
	//word *0x3ffe == 0x5aa5
	
	if (!strncmp(0x3fe6, empty, 24) ||
		strncmp(0x3fe6, 0x3fee, 8) ||
		strncmp(0x3fe6, 0x3ff6, 8) ||
		*0x3ffe != 0x5aa5) {
		cVar2 = 9;
	
	//char *0x3fcb0 == "01119700F000"
	//char *0x3fcbc == "01119700F000"
	//char *0x3fcc8 == "01119700F000"

	} else if (
		!strncmp()) {
		
	}
}

uint16_t FUN_00017f0e(uint16_t target, uint16_t current, uint16_t factor)
{
	if (factor == 0)
		return current;
	
	uint32_t tmp = ((target - current) * factor) / 256;
	
	if ((current == target) || tmp) {
		return (current + tmp);
	}

	if (current >= target) {
		return (current - 1);
	} else {
		return (current + 1);
	}
}

struct Byte_Map2D {
	byte *in;
	byte *out;
	word size;
	byte values[size];
	byte data[size];
};

struct Word_Map2D {
	word *in;
};

struct Byte_Map3D {
	byte *in_y;
	byte *in_x;
	byte *out;
	byte y;
	byte x;
	byte y_values[y];
	byte x_values[x];
	byte data[y * x];
};

struct FUN_00011300_t {
	byte *func;
	byte ...;
};

void *FUN_00011300(FUN_00011300_t *ptr)
{
	D0.w = SR.w;
	SR.w = 0x2700;

	A0.l = 0xff81a0;

	A2.l = ptr;
	
	if (*(0xff81e0)) {
		*(0xff81a0) = ptr->func;
		*(0xff81a4) = ptr->...;
		
		if (*(0xff81a4) > *(0xff81e0))
			*(0xff81a4) = *(0xff81e0);

		return SR;
	}
		
	for (int i = 0; i < 10; ++i) {
		if (*(0xff81a0) == ptr) {
			*(0xff8196) = ptr;
			FUN_00011450(0x4000);
			return SR;
		}
		
		if (*(0xff81a0) == 0)
			A1.l = ;
	}
}

byte lookup_closest3D(Byte_Map3D *ptr)
{
	enum {
		NOT_FOUND = 0,
		INEXACT_X = 1,
		INEXACT_Y = 2,
		FOUND = 3
	};
	
	D0.b = *ptr->in_y;
	D1.b = *ptr->in_x;

	D2.b = ptr->y;
	D3.b = ptr->x;

	A2.l = &ptr->metadata[ptr->y - 1];
	A3.l = A2.l + 1;

	A4.l = &ptr->metadata[ptr->y - 1 + ptr->x];
	A5.l = A4.l + 1;
	
	D4.l = FOUND;

	if (*ptr->in_y > ptr->metadata[0]) {
		--D2.l;
		if (*ptr->in_y < ptr->metadata[ptr->y - 1]) {
			while (--D2.l != -1) {
				if (*(--A2.l) == *ptr->in_y)
					break;
			}

			if (A2.l[0] != *ptr->in_y)
				D4.l = INEXACT_Y;
		}
	} else {
		D2.l = 0;
		A2.l = &ptr->metadata[0];
	}

	if (*ptr->in_x > ptr->metadata[ptr->y]) {
		if (*ptr->in_x < ptr->metadata[ptr->y - 1 + ptr->x]) {
			if (*ptr->in_x > *(--A4.l))
				D4.l -= 2;

			while (*ptr->in_x < *(--A4.l));
		}
	} else {
		A4.l = A3.l;
	}

	A5.l += (A4.l - A3.l) + (D2.w * D3.w);
	
	switch (D4.b) {
	case NOT_FOUND:
	{
		//D2 = (next_y - exact y) * candidate_val
		D2.l = (A2.l[1] - D0.b) * A5.l[0];
		//D4 = exact y - candidate_y
		D4.l = D0.b - A2.l[0];
		//D5 = next_x - exact x
		D5.b = A4.l[1] - D1.b;
		
		//D0 = (candidate_val * y) + (val[x] * (distance_y))
		// * fwd_distance_x
		D0.l = (
			(A5.l[0] * D2.w) + 
			(A5.l[D3.w] * D4.w)
		) * D5.l;
		
		//D1 = exact x - candidate_x
		D1.b -= A4.l[0];
		
		//D7 = next_val * y
		D7.b = A5.l[1] * D2.w;
		
		//D6 = () * bwd_distance_x
		D6.l = (
			(
				A5.l[D3.w + 1] * 
				D4.w
			) + D7.l
		) * D1.l;

		D0.l += D6.l;
		D2.b += D4.b;
		D1.l = (D1.b + D5.b) * D2.w;

		if (D1.l == 0)
			return;

		*ptr->out = ((uint64_t)D0.l + (D1.l / 2))  / D1.l;
		break;
	}
	
	case INEXACT_X:
	{
		//D2 = next_x - candidate_x
		D2.b = A4.l[1] - A4.l[0];
		
		//if x's are equal, set D2 = 1
		//to not have to deal with 0 division later
		if (D2.b == 0)
			D2.l = 1;
		
		//D0 = deviation 
		//(difference, exact x subtract candidate_x)
		D0.b = D1.b - A4.l[0];
		//D3 = half the distance between
		//candidate_x and next_x
		D3.l = D2.l >> 1;
		//D4 = candidate_val
		D4.b = A5.l[0];
		//D5 = next_val
		D5.b = A5.l[1];
		//D1 = true
		D1.l = true;

		//difference between next_val and candidate_val
		D5.l -= D4.l;
		//if it was decreasing
		//get abs of difference
		//and set is_increasing to false
		if (D5 < 0) {	
			D5.l = ~D5.l;
			D1.w = false;
		}
		
		//multiply deviation by difference of val's
		//then add half the distance
		//then divide by distance
		//magically get abs offset from candidate_x
		D0.l = ((D0.l * D5.w) + D3.l) / D2.w;
		if (D1.l == false)
			D0.l = ~D0.l;
		//if it was increasing, add off to candidate_val
		//else subtract off from candidate
		*ptr->out = D0.l + D4.l;
		break;
	}

	case INEXACT_Y:
	{
		//D2 =  next_y - candidate_y
		D2.b = A2.l[1] - A2.l[0];
		
		//if y's are equal, set D2 = 1
		//to not have to deal with 0 divison later
		if (D2.b == 0)
			D2.l = 1;
		
		//D0 = deviation
		//(difference, exact y subtract candidate_y)
		D0.b -= A2.l[0];
		//D6 = half the distance between
		//candidate_y and next_y
		D6.l = D2.l >> 1;
		//D4 = candidate_val
		D4.b = A5.l[0];
		//D5 = next_val
		D5.b = A5.l[D3.w];
		//D1 = true
		D1.l = true;

		//difference between next_val and candidate_val
		D5.l -= D4.l;
		//if it was decreasing
		//get abs of difference
		//and set is_incrementing to false
		if (D5 < 0) {
			D5.l =  ~D5.l;
			D1.w = false;
		}
		
		//multiply deviation by difference of val's
		//then add half the distance
		//then divide by distance
		//magically get abs offset from candidate_y
		D0.l = ((D0.l * D5.w) + D6.l) / D2.w;
		if (D1.l == false)
			D0.l = ~D0.l;
		
		//if it was increasing, add off to candidate_val
		//else subtract off from candidate
		*ptr->out = D0.b + D4.b;
		break;
	}

	case FOUND:
	{
		*ptr->out = A5.l[0];
		break;
	}
}

byte lookup_closest2D(Byte_Map2D *ptr)
{
	D0.b = *ptr->in;
	A1.l = ptr->out;
	D1.w = ptr->size;
	A2.l = &ptr->data[0];

	if (ptr->values[0] >= *ptr->in) {
		*ptr->out = ptr->data[0];
		return ptr->data[0];
	}

	if (ptr->values[ptr->size - 1] <= *ptr->in) {
		*ptr->out = ptr->data[ptr->size - 1];
		return ptr->data[ptr->size - 1];
	}

	for (i = size - i; ptr->values[i] > *ptr->in; --i);

	val = ptr->values[i + 1] - ptr->values[i];
	if (val == 0)
		val = 1;
	
	tmp = *ptr->in - ptr->values[i];
	D3.l = val >> 1;
}

byte lookup_roundup2D(Byte_Map2D *ptr)
{
	A1.l = ptr;
	A2.l = ptr + 10;
	A3.l = ptr + ptr->size + 10;

	A0.l = ptr->in;
	D1.b = *ptr->in;

	if (ptr->values[0] >= *ptr->in) {
		*ptr->out = ptr->data[0];
		return ptr->data[0];
	}
	
	if (ptr->values[ptr->size - 1] <= *ptr->in) {
		*ptr->out = ptr->values[ptr->size - 1];
		return ptr->values[ptr->size - 1];
	}

	for (i = ptr->size - 1; ptr->values[i] > *ptr->in; --i);
	
	if (ptr->values[i] == *ptr->in) {
		*ptr->out = ptr->values[i];
		return ptr-values[i];
	}

	*ptr->out = ptr->values[i + 1];
	return ptr->values[i + 1];
}

word FUN_00015abc(Word_Map2D *ptr)
{
	D0.w = *ptr->in;
	A1.l = ptr->out;

	D1.w = *ptr->size;
	A2.l = &ptr->data[0];
	
	if (ptr->values[0] >= *ptr->in) {
		*ptr->out = ptr->data[0];
		return ptr->data[0];
	}

	if (ptr->values[*ptr->size * 2 - 2] <= *ptr->in) {
		*ptr->out = ptr->data[*ptr->size * 2 - 2];
		return ptr->data[*ptr->size * 2 - 2];
	}

	for (i = size * 2 - 3; ptr->values[i] > *ptr->in; --i);

	tmp
}

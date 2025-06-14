/*
 * Copyright (C) 2007-2022 Siemens AG
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************
 *
 * @author Daniel.Peintner.EXT@siemens.com
 * @version 2022-03-08 
 * @contact Richard.Kuntschke@siemens.com
 *
 * <p>Code generated by EXIdizer</p>
 * <p>Schema: V2G_CI_MsgDef.xsd</p>
 *
 *
 ********************************************************************/



#include "DecoderChannel.h"

#include "EXIConfig.h"
#include "EXIOptions.h"
#include "BitInputStream.h"
#include "EXITypes.h"
#include "MethodsBag.h"
/*#include "v2gEXICoder.h"*/
#include "ErrorCodes.h"

#if MEMORY_ALLOCATION == DYNAMIC_ALLOCATION
#include "DynamicMemory.h"
#endif /* DYNAMIC_ALLOCATION */

#ifndef DECODER_CHANNEL_C
#define DECODER_CHANNEL_C

/* unsigned long == 64 bits, 10 * 7bits = 70 bits */
#define MAX_OCTETS_FOR_UNSIGNED_INTEGER_64 10
/* unsigned int == 32 bits, 5 * 7bits = 35 bits */
#define MAX_OCTETS_FOR_UNSIGNED_INTEGER_32 5

/* buffer for reading (arbitrary) large integer values */
static uint8_t maskedOctets[MAX_OCTETS_FOR_UNSIGNED_INTEGER_64];


static int _decodeUnsignedInteger(bitstream_t* stream, exi_integer_t* iv, int negative) {
	int errn = 0;
	int i = 0;
	uint8_t b;
	int k;

	do {
		/* Read the next octet */
		errn = decode(stream, &b);
		if (errn == 0) {
			if(i < MAX_OCTETS_FOR_UNSIGNED_INTEGER_64) {
				/* the 7 least significant bits hold the actual value */
				maskedOctets[i++] = (b & 127);
			} else {
				errn = EXI_UNSUPPORTED_INTEGER_VALUE;
			}
		}
	} while( errn == 0 && b >= 128 ); /* no more octets ? */

	if ( errn == 0 ) {
		/* For negative values, the Unsigned Integer holds the
		 * magnitude of the value minus 1 */

		switch(i) {
		case 1: /* 7 bits */
			if (negative) {
				iv->val.int8 =  (int8_t)(-( b + 1));
				iv->type = EXI_INTEGER_8;
			} else {
				iv->val.uint8 = b;
				iv->type = EXI_UNSIGNED_INTEGER_8;
			}
			break;
		case 2: /* 14 bits */
			iv->val.uint16 = 0;
			for (k = i-1; k >= 0 ; k--) {
				iv->val.uint16  = (uint16_t)((iv->val.uint16 << 7) | maskedOctets[k]);
			}
			if (negative) {
				iv->val.int16 = (int16_t)( -( iv->val.uint16 + 1 ));
				iv->type = EXI_INTEGER_16;
			} else {
				iv->type = EXI_UNSIGNED_INTEGER_16;
			}
			break;
		case 3: /* 21 bits */
		case 4: /* 28 bits */
			iv->val.uint32 = 0;
			for (k = i-1; k >= 0 ; k--) {
				iv->val.uint32 = (iv->val.uint32 << 7) | maskedOctets[k];
			}
			if (negative) {
				iv->val.int32 = (-(int32_t)(iv->val.uint32 + 1));
				if (iv->val.int32 <= INT16_MAX && iv->val.int32 >= INT16_MIN ) {
					iv->type = EXI_INTEGER_16;
				} else {
					iv->type = EXI_INTEGER_32;
				}
			} else {
				if (iv->val.uint32 <= UINT16_MAX) {
					iv->type = EXI_UNSIGNED_INTEGER_16;
				} else {
					iv->type = EXI_UNSIGNED_INTEGER_32;
				}
			}
			break;
		case 5: /* 35 bits */
		case 6: /* 42 bits */
		case 7: /* 49 bits */
		case 8: /* 56 bits */
		case 9: /* 63 bits */
		case 10: /* 70 bits */
			iv->val.uint64 = 0;
			for (k = i-1; k >= 0 ; k--) {
				iv->val.uint64  = (iv->val.uint64 << 7) | maskedOctets[k];
			}
			if (negative) {
				if (i > 9) {
					/* too large */
					return EXI_UNSUPPORTED_INTEGER_VALUE;
				}
				iv->val.int64 = (-(int64_t)(iv->val.uint64 + 1));
				if (iv->val.int64 <= INT32_MAX && iv->val.int64 >= INT32_MIN ) {
					iv->type = EXI_INTEGER_32;
				} else {
					iv->type = EXI_INTEGER_64;
				}
			} else {
				if (iv->val.uint64 <= UINT32_MAX) {
					iv->type = EXI_UNSIGNED_INTEGER_32;
					/* iv->val.uint32 = iv->val.uint64;*/
				} else {
					iv->type = EXI_UNSIGNED_INTEGER_64;
				}
			}
			break;
		default:
			errn = EXI_UNSUPPORTED_INTEGER_VALUE;
			break;
		}
	}

	return errn;
}

int decodeUnsignedInteger(bitstream_t* stream, exi_integer_t* iv) {
	return _decodeUnsignedInteger(stream, iv, 0);
}

int decodeUnsignedInteger16(bitstream_t* stream, uint16_t* uint16) {
	unsigned int mShift = 0;
	int errn = 0;
	uint8_t b = 0;
	*uint16 = 0;

	do {
		/* 1. Read the next octet */
		errn = decode(stream, &b);
		/* 2. Multiply the value of the unsigned number represented by the 7
		 * least significant
		 * bits of the octet by the current multiplier and add the result to
		 * the current value */
		*uint16 = (uint16_t)(*uint16 + ((b & 127) << mShift));
		/* 3. Multiply the multiplier by 128 */
		mShift += 7;
		/* 4. If the most significant bit of the octet was 1, go back to step 1 */
	} while (errn == 0 && (b >> 7) == 1);

	return errn;
}

int decodeUnsignedInteger32(bitstream_t* stream, uint32_t* uint32) {
	/* 0XXXXXXX ... 1XXXXXXX 1XXXXXXX */
	unsigned int mShift = 0;
	int errn = 0;
	uint8_t b = 0;
	*uint32 = 0;

	do {
		/* 1. Read the next octet */
		errn = decode(stream, &b);
		/* 2. Multiply the value of the unsigned number represented by the 7
		 * least significant
		 * bits of the octet by the current multiplier and add the result to
		 * the current value */
		*uint32 += (uint32_t)((b & 127) << mShift);
		/* 3. Multiply the multiplier by 128 */
		mShift += 7;
		/* 4. If the most significant bit of the octet was 1, go back to step 1 */
	} while (errn == 0 && (b >> 7) == 1);

	return errn;
}

int decodeUnsignedIntegerSizeT(bitstream_t* stream, size_t* sizeT) {
	int errn = 0;

	/* TODO is there a better way to detect the actual size of size_t */
	if(SIZE_MAX == UINT16_MAX) {
		/* 16bit */
		uint16_t uint16;
		errn = decodeUnsignedInteger16(stream, &uint16);
		if(errn == 0) {
			*sizeT = (size_t)uint16;
		}
	} else if(SIZE_MAX == UINT32_MAX) {
		/* 32bit */
		uint32_t uint32;
		errn = decodeUnsignedInteger32(stream, &uint32);
		if(errn == 0) {
			*sizeT = (size_t)uint32;
		}
	} else {
		/* 64bit */
		uint64_t uint64;
		errn = decodeUnsignedInteger64(stream, &uint64);
		if(errn == 0) {
			*sizeT = (size_t)uint64;
		}
	}

	return errn;
}



/**
 * Decode an arbitrary precision non negative integer using a sequence of
 * octets. The most significant bit of the last octet is set to zero to
 * indicate sequence termination. Only seven bits per octet are used to
 * store the integer's value.
 */
int decodeUnsignedInteger64(bitstream_t* stream, uint64_t* uint64) {
	unsigned int mShift = 0;
	int errn = 0;
	uint8_t b = 0;
	*uint64 = 0L;

	do {
		errn = decode(stream, &b);
		*uint64 += ((uint64_t) (b & 127)) << mShift;
		mShift += 7;
	} while (errn == 0 && (b >> 7) == 1);

	return errn;
}


void _reverseArray(uint8_t *array, int number) {
    int x, t;
    number--;

    for(x = 0; x < number; x ++, number --) {
        t = array[x];
        array[x] = array[number];
        array[number] = t;
    }
}

/**
 * Decode an arbitrary precision non negative integer using a sequence of
 * octets. The most significant bit of the last octet is set to zero to
 * indicate sequence termination. Only seven bits per octet are used to
 * store the integer's value.
 */
int decodeUnsignedIntegerBig(bitstream_t* stream, size_t size, uint8_t* data, size_t* len) {
	int errn = 0;
	uint8_t b = 0;
	unsigned int mShift1 = 0;
	unsigned int mShift2 = 0;
	unsigned int mShift3 = 0;
	unsigned int mShift4 = 0;
	unsigned int nBytesRead = 0;
	unsigned int nBitsAvailable = 0;
	uint64_t uint64_1 = 0;
	uint64_t uint64_2 = 0;
	uint64_t uint64_3 = 0;
	uint64_t uint64_4 = 0;

	*len = 0;

	do {
		errn = decode(stream, &b);
		nBytesRead++;
		nBitsAvailable += 7;

		if(nBytesRead <= 8) {
			uint64_1 += ((uint64_t) (b & 127)) << mShift1;
			mShift1 += 7;
		} else if(nBytesRead <= 16) {
			uint64_2 += ((uint64_t) (b & 127)) << mShift2;
			mShift2 += 7;
		} else if(nBytesRead <= 24) {
			uint64_3 += ((uint64_t) (b & 127)) << mShift3;
			mShift3 += 7;
		} else if(nBytesRead <= 32) {
			uint64_4 += ((uint64_t) (b & 127)) << mShift4;
			mShift4 += 7;
		} else {
			return -1; /* too large */
		}
	} while (errn == 0 && (b >> 7) == 1);

	/* shift actual data into array */
	if(uint64_4 != 0) {
		/* 7 octets for uint64_1 */
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 1 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 2 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 3 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 4 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 5 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 6 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 7 */

		/* 7 octets for uint64_2 */
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 1 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 2 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 3 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 4 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 5 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 6 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 7 */

		/* 7 octets for uint64_3 */
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_3 & 0xFF); /* 1 */
		uint64_3 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_3 & 0xFF); /* 2 */
		uint64_3 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_3 & 0xFF); /* 3 */
		uint64_3 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_3 & 0xFF); /* 4 */
		uint64_3 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_3 & 0xFF); /* 5 */
		uint64_3 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_3 & 0xFF); /* 6 */
		uint64_3 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_3 & 0xFF); /* 7 */

		/* remaining octets of uint64_4 */
		while (uint64_4 != 0 && errn == 0) {
			if(*len >= size) {
				return EXI_ERROR_OUT_OF_BOUNDS;
			}
			data[(*len)++] = uint64_4 & 0xFF;
			uint64_4 >>= 8;
		}
	} else if(uint64_3 != 0) {
		/* 7 octets for uint64_1 */
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 1 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 2 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 3 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 4 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 5 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 6 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 7 */

		/* 7 octets for uint64_2 */
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 1 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 2 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 3 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 4 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 5 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 6 */
		uint64_2 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_2 & 0xFF); /* 7 */

		/* remaining octets of uint64_3 */
		while (uint64_3 != 0 && errn == 0) {
			if(*len >= size) {
				return EXI_ERROR_OUT_OF_BOUNDS;
			}
			data[(*len)++] = uint64_3 & 0xFF;
			uint64_3 >>= 8;
		}

	} else if(uint64_2 != 0) {
		/* 7 octets for uint64_1 */
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 1 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 2 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 3 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 4 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 5 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 6 */
		uint64_1 >>= 8;
		if(*len >= size) {
			return EXI_ERROR_OUT_OF_BOUNDS;
		}
		data[(*len)++] = (uint8_t)(uint64_1 & 0xFF); /* 7 */
		/* remaining octets of uint64_2 */
		while (uint64_2 != 0 && errn == 0) {
			if(*len >= size) {
				return EXI_ERROR_OUT_OF_BOUNDS;
			}
			data[(*len)++] = uint64_2 & 0xFF;
			uint64_2 >>= 8;
		}
	} else if(uint64_1 != 0) {
		while (uint64_1 != 0 && errn == 0) {
			if(*len >= size) {
				return EXI_ERROR_OUT_OF_BOUNDS;
			}
			data[(*len)++] = uint64_1 & 0xFF;
			uint64_1 >>= 8;
		}
	}

	_reverseArray(data, *len);

	return errn;
}

int decodeInteger(bitstream_t* stream, exi_integer_t* iv) {
	int b;
	int errn = decodeBoolean(stream, &b);
	if (errn == 0) {
		errn = _decodeUnsignedInteger(stream, iv, b);
	}

	return errn;
}


/**
 * Decode an arbitrary precision integer using a sign bit followed by a
 * sequence of octets. The most significant bit of the last octet is set to
 * zero to indicate sequence termination. Only seven bits per octet are used
 * to store the integer's value.
 */
int decodeInteger16(bitstream_t* stream, int16_t* int16) {
	int b;
	uint16_t uint16;
	int errn = decodeBoolean(stream, &b);

	if (errn == 0) {
		if (b) {
			/* For negative values, the Unsigned Integer holds the
			 * magnitude of the value minus 1 */
			errn = decodeUnsignedInteger16(stream, &uint16);
			*int16 = (int16_t)(-(uint16 + 1));
		} else {
			/* positive */
			errn = decodeUnsignedInteger16(stream, &uint16);
			*int16 = (int16_t)(uint16);
		}
	}

	return errn;
}

/**
 * Decode an arbitrary precision integer using a sign bit followed by a
 * sequence of octets. The most significant bit of the last octet is set to
 * zero to indicate sequence termination. Only seven bits per octet are used
 * to store the integer's value.
 */
int decodeInteger32(bitstream_t* stream, int32_t* int32) {
	int b;
	uint32_t uint32;
	int errn = decodeBoolean(stream, &b);

	if (errn == 0) {
		if (b) {
			/* For negative values, the Unsigned Integer holds the
			 * magnitude of the value minus 1 */
			errn = decodeUnsignedInteger32(stream, &uint32);
			*int32 = (-(int32_t)(uint32 + 1));
		} else {
			/* positive */
			errn = decodeUnsignedInteger32(stream, &uint32);
			*int32 = (int32_t)(uint32);
		}
	}

	return errn;
}

/**
 * Decode an arbitrary precision integer using a sign bit followed by a
 * sequence of octets. The most significant bit of the last octet is set to
 * zero to indicate sequence termination. Only seven bits per octet are used
 * to store the integer's value.
 */
int decodeInteger64(bitstream_t* stream, int64_t* int64) {
	int b;
	uint64_t uint64;
	int errn = decodeBoolean(stream, &b);

	if (errn == 0) {
		if (b) {
			/* For negative values, the Unsigned Integer holds the
			 * magnitude of the value minus 1 */
			errn = decodeUnsignedInteger64(stream, &uint64);
			*int64 = (-(int64_t)(uint64 + 1));
		} else {
			/* positive */
			errn = decodeUnsignedInteger64(stream, &uint64);
			*int64 = (int64_t)(uint64);
		}
	}

	return errn;
}

/**
 * Decode an arbitrary precision integer using a sign bit followed by a
 * sequence of octets. The most significant bit of the last octet is set to
 * zero to indicate sequence termination. Only seven bits per octet are used
 * to store the integer's value.
 */
int decodeIntegerBig(bitstream_t* stream, int* negative, size_t size, uint8_t* data, size_t* len) {
	int errn = decodeBoolean(stream, negative);

	if (errn == 0) {
		if (*negative) {
			/* For negative values, the Unsigned Integer holds the
			 * magnitude of the value minus 1 */
		} else {
			/* positive */
		}
		errn = decodeUnsignedIntegerBig(stream, size, data, len);
	}

	return errn;
}

/**
 * Decode a Float datatype as two consecutive Integers.
 * The first Integer represents the mantissa of the floating point
 * number and the second Integer represents the base-10 exponent
 * of the floating point number.
 */
int decodeFloat(bitstream_t* stream, exi_float_me_t* f) {
	int errn = decodeInteger64(stream, &f->mantissa);
	if (errn == 0) {
		errn = decodeInteger16(stream, &f->exponent);
	}
	return errn;
}

/**
 * Decode a decimal represented as a Boolean sign followed by two Unsigned
 * Integers. A sign value of zero (0) is used to represent positive Decimal
 * values and a sign value of one (1) is used to represent negative Decimal
 * values The first Integer represents the integral portion of the Decimal
 * value. The second positive integer represents the fractional portion of
 * the decimal with the digits in reverse order to preserve leading zeros.
 */
int decodeDecimal(bitstream_t* stream, exi_decimal_t* d) {
	int errn = decodeBoolean(stream, &d->negative);
	if (errn == 0) {
		errn = decodeUnsignedInteger(stream, &d->integral);
		if (errn == 0) {
			errn = decodeUnsignedInteger(stream, &d->reverseFraction);
		}
	}

	return errn;
}

/**
 * Decode a sequence of characters for a given length.
 */
int decodeStringOnly(bitstream_t* stream, size_t len, exi_string_t* s) {
	int errn = 0;
	unsigned int extraChar = 0;
#if STRING_REPRESENTATION == STRING_REPRESENTATION_ASCII
	extraChar = 1; /* null terminator */
#endif /* STRING_REPRESENTATION_ASCII */

	if ( (len + extraChar) > s->size) {
#if MEMORY_ALLOCATION == STATIC_ALLOCATION
		errn = EXI_ERROR_OUT_OF_STRING_BUFFER;
#endif /* STATIC_ALLOCATION */
#if MEMORY_ALLOCATION == DYNAMIC_ALLOCATION
		if(s->size > 0) {
			errn = exiFreeDynamicStringMemory(s);
			if(errn) {
				return errn;
			}
		}
		errn = exiAllocateDynamicStringMemory(s, (len + extraChar)); /*s->len*/
#endif /* DYNAMIC_ALLOCATION */
	}
	if(errn == 0) {
		errn = decodeCharacters(stream, len, s->characters, s->size);
		s->len = len;
	}

	return errn;
}

/**
 * Decode a length prefixed sequence of characters.
 */
int decodeString(bitstream_t* stream, exi_string_t* s) {
	int errn = decodeUnsignedIntegerSizeT(stream, &s->len);
	if (errn == 0) {
		errn = decodeStringOnly(stream, s->len, s);
	}
	return errn;
}

/**
 * Decode a sequence of characters according to a given length.
 * Each character is represented by its UCS [ISO/IEC 10646]
 * code point encoded as an Unsigned Integer
 */
int decodeCharacters(bitstream_t* stream, size_t len, exi_string_character_t* chars, size_t charsSize) {
	unsigned int i;
	int errn = 0;

	unsigned int extraChar = 0;
#if STRING_REPRESENTATION == STRING_REPRESENTATION_ASCII
	extraChar = 1; /* null terminator */
#endif /* STRING_REPRESENTATION_ASCII */

	if ( (len + extraChar) > charsSize) {
		errn = EXI_ERROR_OUT_OF_STRING_BUFFER;
		return errn;
	}

#if STRING_REPRESENTATION == STRING_REPRESENTATION_ASCII
	uint8_t b;
	for (i = 0; i < len && errn == 0; i++) {
		errn = decode(stream, &b);
		if(b < 128) {
			chars[i] = (exi_string_character_t)b;
		} else {
			errn = EXI_ERROR_STRINGVALUES_CHARACTER;
		}
	}
	/* null terminator \0 */
	chars[i] = '\0';

#endif /* STRING_REPRESENTATION_ASCII */
#if STRING_REPRESENTATION == STRING_REPRESENTATION_UCS
	for (i = 0; i < len && errn == 0; i++) {
		errn = decodeUnsignedInteger32(stream, &chars[i]);
	}
#endif /* STRING_REPRESENTATION_UCS */




	return errn;
}

int decodeRCSCharacters(bitstream_t* stream, size_t len, exi_string_character_t* chars, size_t charsSize, size_t rcsCodeLength, size_t rcsSize, const exi_string_character_t rcsSet[]) {
	unsigned int i;
	int errn = 0;
	uint32_t uint32;
	unsigned int extraChar = 0;
#if STRING_REPRESENTATION == STRING_REPRESENTATION_ASCII
	uint8_t b;
	extraChar = 1; /* null terminator */
#endif /* STRING_REPRESENTATION_ASCII */

	if ( (len + extraChar) > charsSize) {
		errn = EXI_ERROR_OUT_OF_STRING_BUFFER;
		return errn;
	}


	for (i = 0; i < len && errn == 0; i++) {
		errn = decodeNBitUnsignedInteger(stream, rcsCodeLength, &uint32);
		if(errn == 0) {
			if ( uint32 == rcsSize ) {
				/* RCS deviation */
#if STRING_REPRESENTATION == STRING_REPRESENTATION_ASCII
				errn = decode(stream, &b);
				if(b < 128) {
					chars[i] = (exi_string_character_t)b;
				} else {
					errn = EXI_ERROR_STRINGVALUES_CHARACTER;
				}
#endif /* STRING_REPRESENTATION_ASCII */
#if STRING_REPRESENTATION == STRING_REPRESENTATION_UCS
				errn = decodeUnsignedInteger32(stream, &chars[i]);
#endif /* STRING_REPRESENTATION_UCS */

			} else {
				/* RCS match */
#if STRING_REPRESENTATION == STRING_REPRESENTATION_ASCII
				chars[i] = rcsSet[uint32];
#endif /* STRING_REPRESENTATION_ASCII */
#if STRING_REPRESENTATION == STRING_REPRESENTATION_UCS
				chars[i] = rcsSet[uint32];
#endif /* STRING_REPRESENTATION_UCS */
			}
		}
	}

	return errn;
}


/**
 * Decode a binary value as a length-prefixed sequence of octets.
 */
int decodeBinary(bitstream_t* stream, exi_bytes_t* bytes) {
	int errn = decodeUnsignedIntegerSizeT(stream, &bytes->len);
	if (errn == 0) {
		if (bytes->len > bytes->size) {
#if MEMORY_ALLOCATION == STATIC_ALLOCATION
			errn = EXI_ERROR_OUT_OF_BYTE_BUFFER;
#endif /* STATIC_ALLOCATION */
#if MEMORY_ALLOCATION == DYNAMIC_ALLOCATION
			errn = exiFreeDynamicBinaryMemory(bytes);
			if(errn == 0) {
				errn = exiAllocateDynamicBinaryMemory(bytes, bytes->len);
			}
#endif /* DYNAMIC_ALLOCATION */
		}

		errn = decodeBytes(stream, bytes->len, bytes->data);
	}

	return errn;
}


int decodeBytes(bitstream_t* stream, size_t len, uint8_t* data) {
	unsigned int i;
	int errn = 0;
	uint8_t b = 0;

	for (i = 0; i < len && errn == 0; i++) {
		errn = decode(stream, &b);
		data[i] = (uint8_t)b;
	}

	return errn;
}

/**
 * Decode Date-Time as sequence of values representing the individual
 * components of the Date-Time.
 */
int decodeDateTime(bitstream_t* stream, exi_datetime_type_t type, exi_datetime_t* datetime) {
	int errn = 0;

	datetime->type = type;

	datetime->year = 0;
	datetime->monthDay = 0;
	datetime->time = 0;
	datetime->presenceFractionalSecs = 0;
	datetime->fractionalSecs = 0;
	datetime->presenceTimezone = 0;
	datetime->timezone = 0;

	switch (type) {
	case EXI_DATETIME_GYEAR: /* Year, [Time-Zone] */
		errn = decodeInteger32(stream, &datetime->year);
		if (errn == 0) {
			datetime->year += DATETIME_YEAR_OFFSET;
		}
		break;
	case EXI_DATETIME_GYEARMONTH: /* Year, MonthDay, [TimeZone] */
	case EXI_DATETIME_DATE:
		errn = decodeInteger32(stream, &datetime->year);
		if (errn == 0) {
			datetime->year += DATETIME_YEAR_OFFSET;
			errn = decodeNBitUnsignedInteger(stream, DATETIME_NUMBER_BITS_MONTHDAY, &datetime->monthDay);
		}
		break;
	case EXI_DATETIME_DATETIME: /* Year, MonthDay, Time, [FractionalSecs], [TimeZone] */
		/* e.g. "0001-01-01T00:00:00.111+00:33"  */
		errn = decodeInteger32(stream, &datetime->year);
		if (errn == 0) {
			datetime->year += DATETIME_YEAR_OFFSET;
			errn = decodeNBitUnsignedInteger(stream, DATETIME_NUMBER_BITS_MONTHDAY, &datetime->monthDay);
			if (errn != 0) {
				break;
			}
		}
		/* no break */
	case EXI_DATETIME_TIME: /* Time, [FractionalSecs], [TimeZone] */
		/* e.g. "12:34:56.135"  */
		errn = decodeNBitUnsignedInteger(stream, DATETIME_NUMBER_BITS_TIME, &datetime->time);
		if (errn == 0) {
			errn = decodeBoolean(stream, &datetime->presenceFractionalSecs);
			if (errn == 0) {
				if (datetime->presenceFractionalSecs) {
					errn = decodeUnsignedInteger32(stream, &datetime->fractionalSecs);
				}
			}
		}
		break;
	case EXI_DATETIME_GMONTH: /* MonthDay, [TimeZone] */
		/* e.g. "--12" */
	case EXI_DATETIME_GMONTHDAY: /* MonthDay, [TimeZone] */
		/* e.g. "--01-28"  */
	case EXI_DATETIME_GDAY: /* MonthDay, [TimeZone] */
		/* "---16"  */
		errn = decodeNBitUnsignedInteger(stream, DATETIME_NUMBER_BITS_MONTHDAY, &datetime->monthDay );
		break;
	default:
		errn = EXI_UNSUPPORTED_DATETIME_TYPE;
		break;
	}

	if(errn == 0) {
		errn = decodeBoolean(stream, &datetime->presenceTimezone );
		if (errn == 0 && datetime->presenceTimezone) {
			errn = decodeNBitUnsignedInteger(stream, DATETIME_NUMBER_BITS_TIMEZONE, &datetime->timezone);
			datetime->timezone -= DATETIME_TIMEZONE_OFFSET_IN_MINUTES;
		}
	}

	return errn;
}



int decode(bitstream_t* stream, uint8_t* b) {
#if EXI_OPTION_ALIGNMENT == BIT_PACKED
	uint32_t bb;
	int errn =  readBits(stream, 8, &bb);
	if (errn == 0) {
		if (bb > 256) {
			errn = EXI_ERROR_UNEXPECTED_BYTE_VALUE;
		} else {
			*b = (uint8_t)bb;
		}
	}

	return errn;
#endif /* EXI_OPTION_ALIGNMENT == BIT_PACKED */
#if EXI_OPTION_ALIGNMENT == BYTE_ALIGNMENT
	int errn = 0;
#if EXI_STREAM == BYTE_ARRAY
	if ( (*stream->pos) < stream->size ) {
		*b = stream->data[(*stream->pos)++];
	} else {
		errn = EXI_ERROR_INPUT_STREAM_EOF;
	}
#endif /* EXI_STREAM == BYTE_ARRAY */
#if EXI_STREAM == FILE_STREAM
	*b = (uint8_t)(getc(stream->file));
	/* EOF cannot be used, 0xFF valid value */
	if ( feof(stream->file) || ferror(stream->file) ) {
		errn = EXI_ERROR_INPUT_STREAM_EOF;
	}
#endif /* EXI_STREAM == FILE_STREAM */

	return errn;
#endif /* EXI_OPTION_ALIGNMENT == BYTE_ALIGNMENT */
}

int decodeBoolean(bitstream_t* stream, int* b) {
#if EXI_OPTION_ALIGNMENT == BIT_PACKED
	uint32_t ub;
	int errn = readBits(stream, 1, &ub);
	*b = (ub == 0) ? 0 : 1;
	return errn;
#endif /* EXI_OPTION_ALIGNMENT == BIT_PACKED */
#if EXI_OPTION_ALIGNMENT == BYTE_ALIGNMENT
	uint8_t bb;
	int errn = decode(stream, &bb);
	*b = (bb == 0) ? 0 : 1;
	return errn;
#endif /* EXI_OPTION_ALIGNMENT == BYTE_ALIGNMENT */

}

/**
 * Decodes and returns an n-bit unsigned integer using the minimum number of
 * bytes required for n bits.
 */
int decodeNBitUnsignedInteger(bitstream_t* stream, size_t nbits, uint32_t* uint32) {
#if EXI_OPTION_ALIGNMENT == BIT_PACKED
	int errn = 0;
	if (nbits == 0) {
		*uint32 = 0;
	} else {
		errn= readBits(stream, nbits, uint32);
	}
	return errn;
#endif /* EXI_OPTION_ALIGNMENT == BIT_PACKED */
#if EXI_OPTION_ALIGNMENT == BYTE_ALIGNMENT
	size_t bitsRead = 0;
	uint8_t b;
	int errn = 0;
	*uint32 = 0;

	while (errn == 0 && bitsRead < nbits) {
		errn = decode(stream, &b);
		*uint32 = *uint32 + (uint32_t)(b << bitsRead);
		bitsRead = (bitsRead + 8);
	}

	return errn;
#endif /* EXI_OPTION_ALIGNMENT == BYTE_ALIGNMENT */
}



#endif


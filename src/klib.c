/*
 *                         OS-3o3 Operating System
 *
 *            some C functions equivalent for use in the kernel
 *
 *                      13 may MMXXIV PUBLIC DOMAIN
 *           The authors disclaim copyright to this source code.
 *
 *
 */

#include "klib.h"

k__int k__memcmp(void *str1, void *str2, k__int n)
{
	k__u32 i = 0;
	k__u8 *s1 = (k__u8 *)str1;
	k__u8 *s2 = (k__u8 *)str2;
	while (i < n) {
		if (s1[i] != s2[i]) {
			return s1[i] - s2[i];
		}
		i++;
	}
	return 0;
}

void *k__memset(void *str, k__int c, k__int n)
{
	char *s;
	
	s = (char *)str;
	while (n > 0) {
		*s = c;
		s++;
		n--;
	}
	return str;
}

utf8 *k__strcpy(utf8 *dst, const utf8 *src)
{
	utf8 *dstSave = dst;
	k__int c;
	do {
		c = *dst++ = *src++;
	} while (c);
	return dstSave;
}

k__int k__strlen(const utf8 *string)
{
	const utf8 *base = string;
	while (*string++)
		;
	return string - base - 1;
}

long k__strtol(const utf8 *s, utf8 **end, k__int base)
{
	k__int i;
	k__int ch, value = 0, neg = 0;

	if (s[0] == '-') {
		neg = 1;
		++s;
	}
	if (s[0] == '0' && s[1] == 'x') {
		base = 16;
		s += 2;
	}
	for (i = 0; i <= 8; ++i) {
		ch = *s++;
		if ('0' <= ch && ch <= '9')
			ch -= '0';
		else if ('A' <= ch && ch < base - 10 + 'A')
			ch = ch - 'A' + 10;
		else if ('a' <= ch && ch < base - 10 + 'a')
			ch = ch - 'a' + 10;
		else
			break;
		value = value * base + ch;
	}
	if (end)
		*end = (utf8 *)s - 1;
	if (neg)
		value = -(k__int)value;
	return value;
}

k__int k__atoi(const utf8 *s) { return k__strtol(s, (utf8 **)NULL, 10); }

utf8 *k__itoa(k__int num, utf8 *dst, k__int base)
{
	k__int negate = 0;
	k__int place;
	k__int un;
	k__int digit;
	utf8 c;
	utf8 text[20];

	un = num;
	if (un == 0) {
		k__strcpy(dst, "0");
		return dst;
	}
	if (base == 10 && num < 0) {
		un = -num;
		negate = 1;
	}
	text[16] = 0;
	text[15] = '0';
	for (place = 15; place >= 0; --place) {
		/*digit = un % base; JML FIXME subc */
		if (base == 10) {
			digit = un % 10;
		} else {
			digit = un & 0x0F;
			/* digit = un - ((un / 16) * 16);*/
		}
		/*assert(digit < base);*/
		if (un == 0 && place < 15 && base == 10 && negate) {
			c = '-';
			negate = 0;
		} else if (digit < 10)
			c = (utf8)('0' + digit);
		else if (digit >= base)
			c = 'G';
		else
			c = (utf8)('A' + digit - 10);
		text[place] = c;
		if (base == 10) {
			un = un / 10;
		} else {
			un = (un >> 4) & 0x0FFFFFFF;
		}
		/*un = un / ((unsigned k__int)base);*/
		if (un == 0 && negate == 0)
			break;
		if (place == 0)
			break;
	}
	k__strcpy(dst, text + place);
	return dst;
}

k__int k__sprintf2(utf8 *s, const utf8 *format, k__int arg0, k__int arg1, k__int arg2,
		k__int arg3, k__int arg4, k__int arg5, k__int arg6, k__int arg7)
{
	k__int argv[8];
	k__int argc = 0, width, length;
	utf8 f = 0, text[20], fill;

	argv[0] = arg0;
	argv[1] = arg1;
	argv[2] = arg2;
	argv[3] = arg3;
	argv[4] = arg4;
	argv[5] = arg5;
	argv[6] = arg6;
	argv[7] = arg7;

	for (;;) {
		f = *format++;
		if (f == 0)
			return argc;
		else if (f == '%') {
			width = 0;
			fill = ' ';
			f = *format++;
			while ('0' <= f && f <= '9') {
				width = width * 10 + f - '0';
				f = *format++;
			}
			if (f == '.') {
				fill = '0';
				f = *format++;
			}
			if (f == 0)
				return argc;

			if (f == 'd') {
				k__memset(s, fill, width);
				k__itoa(argv[argc++], text, 10);
				length = (k__int)k__strlen(text);
				if (width < length)
					width = length;
				k__strcpy(s + width - length, text);
			} else if (f == 'x' || f == 'f') {
				k__memset(s, '0', width);
				k__itoa(argv[argc++], text, 16);
				length = (k__int)k__strlen(text);
				if (width < length)
					width = length;
				k__strcpy(s + width - length, text);
			} else if (f == 'c') {
				*s++ = (utf8)argv[argc++];
				*s = 0;
			} else if (f == 's') {
				length = k__strlen((utf8 *)argv[argc]);
				if (width > length) {
					k__memset(s, ' ', width - length);
					s += width - length;
				}
				k__strcpy(s, (utf8 *)argv[argc++]);
			}
			s += k__strlen(s);
		} else {
			/*
		    if (f == '\n' && prev != '\r')
			*s++ = '\r';*/
			*s++ = f;
		}
		*s = 0;
	}
}

void k__printf2(const utf8 *format, k__int arg0, k__int arg1, k__int arg2, k__int arg3,
		k__int arg4, k__int arg5, k__int arg6, k__int arg7)
{
	utf8 buffer[128];
	k__sprintf2(buffer, format, arg0, arg1, arg2, arg3, arg4, arg5, arg6,
		    arg7);
	k__puts(buffer);
}

k__int k__rd8(k__int base, k__int offset)
{
	k__int align;
	base = base + offset;
	align = base & 3;
	base = base - align;
	return ((*((volatile k__int *)base)) >> (align * 8)) & 0xFF;
}

k__int k__rd16(k__int base, k__int offset)
{
	k__int align;
	base = base + offset;
	align = base & 3;
	base = base - align;
	if (align == 0) {
		return ((*((volatile k__int *)(base)))) & 0xFFFF;
	} else if (align == 1) {
		return ((*((volatile k__int *)(base))) >> 8) & 0xFFFF;
	} else if (align == 2) {
		return ((*((volatile k__int *)(base))) >> 16) & 0xFFFF;
	} else {
		return (((*((volatile k__int *)(base)) >> 24) & 0xFF) +
			(((*((volatile k__int *)(base + 4))) << 8) & 0xFF00));
	}
}
k__int k__rd32(k__int base, k__int offset)
{
	k__int align;
	base = base + offset;
	align = base & 3;
	base = base - align;
	if (align == 0) {
		return ((*((k__int volatile *)((base))))) & 0xFFFFFFFF;
	} else if (align == 1) {
		return (((*((volatile k__int *)((base)))) >> 8) & 0x00FFFFFF) +
		       (((*((volatile k__int *)((base + 4)))) << 24) & 0xFF000000);
	} else if (align == 2) {
		return (((*((volatile k__int *)((base)))) >> 16) & 0x0000FFFF) +
		       (((*((volatile k__int *)((base + 4)))) << 16) & 0xFFFF0000);
	} else {
		return (
		    (((*((volatile k__int *)((base)))) >> 24) & 0x000000FF) +
		    (((*((volatile k__int *)((base + 4)))) << 8) & 0xFFFFFF00));
	}
}

k__int k__wr32(k__int base, k__int offset, k__int value)
{
	*((k__int volatile *)((base + offset))) = value;
	return 0;
}

#include <_string.h>
#include <stdint.h>
#include <malloc.h>

void *__memset(void *s, int c, size_t n) {
	uint64_t c8;
	int8_t *p = (void*)&c8;
	
	for(int i = 0; i < 8; i++)
		p[i] = (int8_t)c;
	
	uint64_t* d = s;
	
	int count = n / 8;
	while(count--)
		*d++ = c8;
	
	uint8_t* d2 = (uint8_t*)d;
	count = n % 8;
	while(count--)
		*d2++ = c;
	
	return s;
}

void * __attribute__ (( noinline )) __memcpy ( void *dest, const void *src,
					       size_t len ) {
	void *edi = dest;
	const void *esi = src;
	int discard_ecx;

	/* We often do large dword-aligned and dword-length block
	 * moves.  Using movsl rather than movsb speeds these up by
	 * around 32%.
	 */
	__asm__ __volatile__ ( "rep movsl"
			       : "=&D" ( edi ), "=&S" ( esi ),
				 "=&c" ( discard_ecx )
			       : "0" ( edi ), "1" ( esi ), "2" ( len >> 2 )
			       : "memory" );
	__asm__ __volatile__ ( "rep movsb"
			       : "=&D" ( edi ), "=&S" ( esi ),
				 "=&c" ( discard_ecx )
			       : "0" ( edi ), "1" ( esi ), "2" ( len & 3 )
			       : "memory" );
	return dest;
}

/**
 * Copy memory area backwards
 *
 * @v dest		Destination address
 * @v src		Source address
 * @v len		Length
 * @ret dest		Destination address
 */
void * __attribute__ (( noinline )) __memcpy_reverse ( void *dest,
						       const void *src,
						       size_t len ) {
	void *edi = ( dest + len - 1 );
	const void *esi = ( src + len - 1 );
	int discard_ecx;

	/* Assume memmove() is not performance-critical, and perform a
	 * bytewise copy for simplicity.
	 */
	__asm__ __volatile__ ( "std\n\t"
			       "rep movsb\n\t"
			       "cld\n\t"
			       : "=&D" ( edi ), "=&S" ( esi ),
				 "=&c" ( discard_ecx )
			       : "0" ( edi ), "1" ( esi ),
				 "2" ( len )
			       : "memory" );
	return dest;
}

void * __memmove ( void *dest, const void *src, size_t len ) {

	if ( dest <= src ) {
		return __memcpy ( dest, src, len );
	} else {
		return __memcpy_reverse ( dest, src, len );
	}
}

int __memcmp(const void* v1, const void* v2, size_t size) {
	const uint64_t* d = v1;
	const uint64_t* s = v2;
	
	int count = size / 8;
	while(count--) {
		if(*d != *s)
			return *s - *d;
		
		s++;
		d++;
	}
	
	const uint8_t* d2 = (uint8_t*)d;
	const uint8_t* s2 = (uint8_t*)s;
	count = size % 8;
	while(count--) {
		if(*d2 != *s2)
			return *s2 - *d2;
		
		s2++;
		d2++;
	}
	
	return 0;
}

void __bzero(void* dest, size_t size) {
	uint64_t* d = dest;
	
	int count = size / 8;
	while(count--)
		*d++ = 0;
	
	uint8_t* d2 = (uint8_t*)d;
	count = size % 8;
	while(count--)
		*d2++ = 0;
}

size_t __strlen(const char* s) {
	int i = 0;
	while(s[i] != '\0')
		i++;
	
	return i;
}

char* __strstr(const char* haystack, const char* needle) {
	const char* hs = haystack;
	while(*hs != 0) {
		const char* s = hs;
		const char* d = needle;
		while(*d != '\0') {
			if(*s != *d)
				break;
			
			s++;
			d++;
		}
		
		if(*d == '\0')
			return (char*)hs;
		
		hs++;
	}
	
	return NULL;
}

char* __strchr(const char* s, int c) {
	char* ch = (char*)s;
	
	while(*ch != c && *ch != '\0')
		ch++;
	
	return *ch == c ? ch : NULL;
}

char* __strrchr(const char* s, int c) {
	char* ch = (char*)s;
	
	while(*ch != c && *ch != '\0')
		ch--;
	
	return *ch == c ? ch : NULL;
}

int __strcmp(const char* s, const char* d) {
	while(*s != '\0' && *d != '\0') {
		if(*s != *d)
			return *s - *d;
		
		s++;
		d++;
	}
	
	return *s - *d;
}

int __strncmp(const char* s, const char* d, size_t size) {
	if(size == 0)
		return 0;
	
	while(*s != '\0' && *d != '\0') {
		if(*s != *d)
			return *s - *d;
		
		s++;
		d++;
		
		if(--size == 0)
			return 0;
	}
	
	return *s - *d;
}

char* __strdup(const char* source) {
	int str_len =  __strlen(source) + 1;
	char* dest = malloc(str_len);
	__memcpy(dest, source, str_len);
	
	return dest;
}

long int __strtol(const char *nptr, char **endptr, int base) {
	if(base == 0) {
		if(nptr[0] == '0') {
			if((nptr[1] == 'x' || nptr[1] == 'X')) {
				base = 16;
				nptr += 2;
			} else {
				base = 8;
				nptr += 1;
			}
		} else {
			base = 10;
		}
	}
	
	long int value = 0;
	
	char* p = (char*)nptr;
	while(1) {
		switch(base) {
			case 16:
				if(*p >= 'a' && *p <= 'f') {
					value = value * base + *(p++) - 'a' + 10;
					continue;
				} else if(*p >= 'A' && *p <= 'F') {
					value = value * base + *(p++) - 'A' + 10;
					continue;
				}
			case 10:
				if(*p >= '0' && *p <= '9') {
					value = value * base + *(p++) - '0';
					continue;
				}
			case 8:
				if(*p >= '0' && *p <= '7') {
					value = value * base + *(p++) - '0';
					continue;
				}
		}
		
		if(endptr != NULL)
			*endptr = p;
		
		return value;
	}
}

long long int __strtoll(const char *nptr, char **endptr, int base) {
	if(base == 0) {
		if(nptr[0] == '0') {
			if((nptr[1] == 'x' || nptr[1] == 'X')) {
				base = 16;
				nptr += 2;
			} else {
				base = 8;
				nptr += 1;
			}
		} else {
			base = 10;
		}
	}
	
	long long int value = 0;
	
	char* p = (char*)nptr;
	while(1) {
		switch(base) {
			case 16:
				if(*p >= 'a' && *p <= 'f') {
					value = value * base + *(p++) - 'a' + 10;
					continue;
				} else if(*p >= 'A' && *p <= 'F') {
					value = value * base + *(p++) - 'A' + 10;
					continue;
				}
			case 10:
				if(*p >= '0' && *p <= '9') {
					value = value * base + *(p++) - '0';
					continue;
				}
			case 8:
				if(*p >= '0' && *p <= '7') {
					value = value * base + *(p++) - '0';
					continue;
				}
		}
		
		if(endptr != NULL)
			*endptr = p;
		
		return value;
	}
}
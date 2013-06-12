#include <config.h>
#include <reg.h>
#include <serial.h>
#include <main.h>
#include <types.h>
#include <lib.h>
#include <time.h>

///////////////////////////////////////////////////
#define ENOERROR		0	/* no error at all */
#define EINVAL		1	/* invalid argument */
#define ENOPARAMS		2	/* not enough parameters */
#define EMAGIC		3	/* magic value failed */
#define ECOMMAND		4	/* invalid command */
#define ENAN			5	/* not a number */
#define EALIGN		6	/* addres not aligned */
#define ERANGE		7	/* out of range */
#define ETIMEOUT		8	/* timeout exceeded */
#define ETOOSHORT		9	/* short file */
#define ETOOLONG		10	/* long file */
#define EAMBIGCMD		11	/* ambiguous command */
#define EFLASHERASE	12	/* can't erase flash block */
#define EFLASHPGM		13	/* flash program error */
#define ESERIAL		14	/* serial port error */

#define INT_BUF_SIZE 1024
#define	MAX_RETRIES 10

#define TEST_MAX_RETRIES do { 				                   \
	if(retries++ > MAX_RETRIES) {	 		                   \
		return -ETIMEOUT;				           \
	}						                   \
} while(0)

#define	DEC(c)	(((c) - ' ') & 077)		/* single character decode */
#define	IS_DEC(c) ( (((c) - ' ') >= 0) &&  (((c) - ' ') <= 077 + 1) )
/* #define IS_DEC(c) (1) */

#define OUT_OF_RANGE do {	\
	PutChar('\n');	\
        PutString(buf);	\
	PutChar('\n');	\
        return -ERANGE; \
} while(0)

#define PUT_CHAR(x) do {	             \
	if(bytesWritten < bufLen)	     \
	        bufBase[bytesWritten++] = x; \
} while(0)


void SerialInit(void)
{

   __REG(FFUART_BASE + FFLCR) = 0x00000003; // 8-bit, 1 stop, no parity 세팅.
   __REG(FFUART_BASE + FFFCR) = 0x00000007; // Reset tx, rx FIFO. clear. FIFO enable
   __REG(FFUART_BASE + FFIER) = 0x00000040; // UART Enable Interrupt
   __REG(FFUART_BASE + FFLCR) |= 0x00000080; // DLAB set=latch registers, DLAB clear=일반 포트.
   __REG(FFUART_BASE + FFDLL) = SERIAL_SPEED_RATE; // baud rate 설정.
   __REG(FFUART_BASE + FFLCR) &= 0xFFFFFF7F; // DLAB clear, 일반 포트로 전환.

   // Transmit Shift Register, Transmit Holding Register, FIFO에
   // 데이타가 없을때까지 기다린다.
   while(! __REG(FFUART_BASE + FFLSR) & 0x00000040 );

   return;
}

int  SerialInputString(char *s, const int len, const int timeout)
{

	u32 startTime, currentTime;
	char c;
	time_t now;
	int i;
	int numRead;
	int skipNewline = 1;
	int maxRead = len - 1;
	int ret;

	startTime = GetTime(&now);

	for(numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		
		while((__REG(FFUART_BASE  + FFLSR) & 0x00000001)==0) {
			currentTime = GetTime(&now);

			/* check timeout value */
			if((currentTime - startTime) >
			    (timeout)) {
			   //(timeout * TICKS_PER_SECOND)) {
				/* timeout */
				s[i++] = '\0';
				return(numRead);
			}
		}

		ret=GetChar(&c);

		/* check for errors */
		#if 0
		if(c < 0) {
			s[i++] = '\0';
			return c;
		}
		#else
		if(!ret) {
			s[i++] = '\0';
			return c;
		}
		#endif

		/* eat newline characters at start of string */
		if((skipNewline == 1) && (c != '\r') && (c != '\n'))
			skipNewline = 0;

		if(skipNewline == 0) {
			if((c == '\r') || (c == '\n')) {
				s[i++] = '\0';
				return(numRead);
			} else {
				s[i++] = (char)c;
				numRead++;
			}
		}
	}

	return(numRead);

}


int UUDecode(char *bufBase, int bufLen) {

	int n, linesReceived = 0;
	char ch, *p;
	int bytesWritten = 0, retries = 0;
	char buf[INT_BUF_SIZE];

	/* Search for header line. We don't care about the mode or filename */
	retries = 0;
	do {
		SerialInputString(buf, sizeof(buf), 6);
		TEST_MAX_RETRIES;
	}while(StrNCmp(buf, "begin ", 6) != 0);

	for (;;) {
		if (SerialInputString(p = buf, sizeof(buf), 2) == 0) {
			return -ETOOSHORT;
		}
		/* Status print to show where we are at right now */

		if((linesReceived++ & 0x007F) == 0) {
			PutChar('.');
		}

		/*
		 * `n' is used to avoid writing out all the characters
		 * at the end of the file.
		 */
		if ((n = DEC(*p)) <= 0)
			break;
		for (++p; n > 0; p += 4, n -= 3)
			if (n >= 3) {
				if (!(IS_DEC(*p) && IS_DEC(*(p + 1)) &&
				     IS_DEC(*(p + 2)) && IS_DEC(*(p + 3))))
                                	OUT_OF_RANGE;

				ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
				PUT_CHAR(ch);
				ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
				PUT_CHAR(ch);
				ch = DEC(p[2]) << 6 | DEC(p[3]);
				PUT_CHAR(ch);

			}
			else {
				if (n >= 1) {
					if (!(IS_DEC(*p) && IS_DEC(*(p + 1))))
						OUT_OF_RANGE;
					ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
					PUT_CHAR(ch);
				}
				if (n >= 2) {
					if (!(IS_DEC(*(p + 1)) &&
						IS_DEC(*(p + 2))))
						OUT_OF_RANGE;

					ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
					PUT_CHAR(ch);
				}
				if (n >= 3) {
					if (!(IS_DEC(*(p + 2)) &&
						IS_DEC(*(p + 3))))
						OUT_OF_RANGE;
					ch = DEC(p[2]) << 6 | DEC(p[3]);
					PUT_CHAR(ch);
				}
			}
	}
	PutChar('\n');
	if (SerialInputString(p = buf, sizeof(buf), 2) == 0 || (StrNCmp(buf, "end", 3))) {
		/* no "end" line */
		return -ETOOSHORT;
	}
	return(bytesWritten);
} /* UUDecode */

int Download(int base, int length)
{
	u32 *numRead=0;
	int retval;

	printf("\t* Translate image by using uuencode in host PC *\n");
	*numRead = UUDecode((char *)base, length);

	if(*numRead < 0) {
		/* something went wrong */
		retval = *numRead;
	}

	printf("\tReceived bytes : 0x%08lx",*numRead);
	return 0;

}


void PutChar(const char ch)
{
	// FIFO에 데이타가 없을때까지 기다린다.
	while ((__REG(FFUART_BASE + FFLSR) & 0x00000020) == 0 );

	__REG(FFUART_BASE + FFTHR) = ((u32)ch & 0xFF);

	if (ch=='\n') PutChar('\r'); // c=='\n'이면, 실제로는 "\n\r"을 출력.


}

void PutString(const char *ch) //문잘열을 출력하기 위해 구현해 놓았지만 printf문이 있어 거의 사용하지 않는다.
{
	while(*ch != '\0')
		PutChar(*ch++);
}


int GetChar(char * ch) //시리얼로 입력된 데이타를 가져 온다.
{
	if((__REG(FFUART_BASE + FFLSR) & 0x00000001)==0){ // FIFO에 데이타가 있을때.
		return 0;
	} else {

		(volatile char)*ch = __REG(FFUART_BASE + FFRBR);
		return 1;
	}
}




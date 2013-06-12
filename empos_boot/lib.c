#include <config.h>
#include <lib.h>
#include <serial.h>
#include <types.h>
#include <reg.h>

void MemCpy32(unsigned long *dest, unsigned long *src, int numWords)
{

	while(numWords--) {
		if((numWords & 0xffff) == 0x0)
			PutChar('.');

		*dest++ = *src++;
	}
}

void MemCpy(void *dest, void *src, int cnt){
	char *s1=dest, *s2=src, *endptr=(char *)dest+cnt;

	while (s1<endptr)
		*s1++ = *s2++;
	return;
}

void MemSet(void *dest, const char c, int len)
{
	char *dt = (char *) dest;

	while (len--)
		*dt++ = c;

	return;// dest;
}


int MemCmp(void *addr1, void *addr2, int len){
	volatile char *s1=addr1, *s2=addr2;
	volatile char *endptr = addr1+len;

	while ((u32)s1 < (u32)endptr){
		if (*s1++ != *s2++) return *(--s1) - *(--s2);
	}
	return 0;
}	// MemCmp.


//Text LCD를 제어와 관련
#define        CTRL    0
#define        WRITE   1
#define        READ    2

#define        YES     1
#define        NO      0

void user_wait(unsigned long delay_factor) // 1 = 0.075ms
{
        unsigned int i;
        for (i=0; i<delay_factor; i++);
	
}
void LCD_word(short Control, short Data)
{
	volatile u32 *dptr1;
	dptr1 = (volatile u32 *)0x10700000;
        *(dptr1 + Control) = Data;
	user_wait(0x4000);

}
void printLCD(int line, char *printchar)
{
        int i, len;
        char data;
        unsigned short wr_data, we_data ;

        switch (line) {
                case 0 : LCD_word(CTRL, 0x0402);
                        LCD_word(CTRL, 0x0002);
                         break;
                case 1 : LCD_word(CTRL, 0x04C0);
                        LCD_word(CTRL, 0x00C0);
                         break;
                default : break;
        }

        len = StrLen(printchar);

        for (i=0; i<len; i++) {

                data = (short)*(printchar + i);

                wr_data = data + 0x0500;
                we_data = data + 0x0100;


		user_wait(0xf000);
                LCD_word(WRITE, wr_data);
                LCD_word(WRITE, we_data);
		user_wait(0xf000);

        }
}

int IoTest()
{
	volatile u8 * addr8;
	volatile u16 * addr16;
//	volatile u32 *dptr1;
	char * data;

	//LED를 on
	addr8 = (volatile u8 *)0x10600000;
	*addr8 = 0xff;


	//7Segment를 on
	addr16 =  (volatile u16 *)0x10300000;
	*addr16 = 0xffff;
	addr16 =  (volatile u16 *)0x10400000;
	*addr16 = 0xffff;

        LCD_word(CTRL,0x0438);  // function set
        LCD_word(CTRL,0x0038);
        
	LCD_word(CTRL,0x040E); // display on
        LCD_word(CTRL,0x000E);

        LCD_word(CTRL,0x0401); // allclr
        LCD_word(CTRL,0x0001);

        LCD_word(CTRL,0x0402); // Ln11
        LCD_word(CTRL,0x0002);

        LCD_word(CTRL,0x0406);
        LCD_word(CTRL,0x0006);

	data = "Welcome to embedded!";	//LCD 에 글자를 입력한다.
	printLCD(0, data);
	data =  "EMPOS-II";

	printLCD(1, data);

	return true;

}

int StrCmp(char * dest, char *src)
{
	int ret;
	while(1)
	{
		if ((ret = *dest-*src) != 0 || *dest == '\0')
			return ret;
		dest++;
		src++;
	}
}

int StrLen(char *dest){
	volatile char *tmp = dest;

	if (!tmp) return -1;
	while (*tmp!=0) tmp++;
	return (tmp - dest);
}	// StrLen.

void StrCpy(char *dest, char *src){
	volatile char *s1=dest, *s2=src;

	while (*s2!=0) *s1++ = *s2++;
	*s1 = 0;
	return;
}	// StrCpy.


int StrNCmp(char *s1, char *s2, int len){
	int i;

	for(i = 0; i < len; i++){
		if(s1[i] != s2[i])	return ((int)s1[i]) - ((int)s2[i]);
		if(s1[i] == 0)		return 0;
	}
	return 0;
} // StrNCmp.


bool HexToInt(char *s, void *retval, int type){
	char	c;
	int		i;
	long	rval;

	if (!s || !retval) return false;
	if (!StrNCmp(s, "0x", 2)) s+=2;
	// fine int value.
	for (i=0, rval=0; i<type/4; i++){
		if (*s=='\0'){
			if (i==0) return false;
			else      break;
		}
		c = *s++;

		if      (c>='0' && c<='9') c-='0';
		else if (c>='a' && c<='f') c=c-'a'+10;
		else if (c>='A' && c<='F') c=c-'A'+10;
		else    return false;

		rval = rval<<4 | c;
	}
	// make retval.
	switch (type){
		case 8 :
			*(char *)retval = (char)rval;
			break;
		case 16 :
			*(short *)retval = (short)rval;
			break;
		case 32 :
			*(long *)retval = (long)rval;
			break;
		default :
			return false;
	}
	return true;
}	// HexToInt.


u32 StrToInt(char *ch)
{
	unsigned long int ret=0, tmp=0, i=1;

	if(ch[0] =='0' && ch[1]=='x') //16진수를 보내왔을 경우.
	{
		ch++;ch++;
		while(1)
		{
			if(*ch=='0' && i==1){
				ch++;
				continue;
			}

			switch(*ch){
			case 'a' :
				tmp=0xa; break;
			case 'b' :
				tmp=0xb; break;
			case 'c' :
				tmp=0xc; break;
			case 'd' :
				tmp=0xd; break;
			case 'e' :
				tmp=0xe; break;
			case 'f' :
				tmp=0xf; break;
			default :
				tmp =  *ch  - 0x30;
			}

			ret *=i;
			ret +=tmp;

			i =16;
			ch++;
			if (*ch == '\0')
				break;
  		}
		return ret;
	}else //10진수를 보내 왔을 경우
	{
		while(1)
		{
			if(*ch=='0' && i==1){
				ch++;
				continue;
			}

			tmp =  *ch  - 0x30;
			ret *=i;

			ret +=tmp;
			i =10;
			ch++;

			if (*ch == '\0')
				break;

		}
		return ret;
	}
	return 0;
}


void PrintChar(char *fmt, char c){
	PutChar(c);
	return;
}

int MOD(int num, int dev){		// num % dev	툴체인에 문제가 있어서  함수를 직접 구현하였다.

	int remainder = num;

	for(;;)
	{
		if(remainder<dev)
			return remainder;

		remainder -= dev;
	}

	return remainder;
}

int DEV(int num, int dev){		// num / dev	툴체인에 문제가 있어서  함수를 직접 구현하였다.

	int remainder = num;
	int mok =0;

	for(;;)
	{
		if(remainder<dev)
			return mok;

		mok++;
		remainder -= dev;
	}

	return mok;
}

void PrintDec(char *fmt, int l){

	int	i, j;
	char	c, *s=fmt, tol[10];
	bool	flag0=false, flagl=false;	// "%08lx"에서 '0', 'l'의 존재 여부.
	long	flagcnt=0;					// "%08lx"에서 "8"을 찾아서 long형으로.
	bool	leading_zero=true;			// long형의 data를 출력하기 위한 변수.
	long	divisor, result, remainder;

	// fmt의 "%08lx"에서 '0', '8', 'l'을 해석.
	for (i=0; (c=s[i]); i++){
		if (c=='d') break;
		else if (c>='1' && c<='9'){
			for (j=0; s[i]>='0' && s[i]<='9'; j++){
				tol[j] = s[i++];
			}
			tol[j] = '\0';
			i--;
			DecToLong(tol, &flagcnt);
		}
		else if (c=='0') flag0=true;
		else if (c=='l') flagl=true;
		else continue;
	}

	// 위의 flag에 따라 출력.
	if (flagcnt){
		if (flagcnt>9) flagcnt=9;

		//remainder = l%(Power(10, flagcnt));	// flagcnt보다 윗자리의 수는 걸러냄. 199에 flagcnt==2이면, 99만.
		remainder = MOD(l,Power(10, flagcnt));

		for (divisor=Power(10, flagcnt-1); divisor>0; divisor/=10){

			//result = remainder / divisor;
			result = DEV(remainder,divisor);

			//remainder %= divisor;
			  remainder =MOD(remainder, divisor);

			if (result!=0 || divisor==1) leading_zero = false;

			if (leading_zero==true){
				if (flag0)	PutChar('0');
				else		PutChar(' ');
			}
			else PutChar((char)(result)+'0');
		}
	} else {
		remainder = l;

		for (divisor=1000000000; divisor>0; divisor/=10){
			//result = remainder/divisor;
			result = DEV(remainder,divisor);

			//remainder %= divisor;
			remainder =MOD(remainder, divisor);

			if (result!=0 || divisor==1) leading_zero = false;
			if (leading_zero==false) PutChar((char)(result)+'0');
		}
	}

	return;
}


void PrintHex(char *fmt, int l){
	int		i, j;
	char	c, *s=fmt, tol[10];
	bool	flag0=false, flagl=false;	// flags.
	long	flagcnt=0;
	bool	leading_zero=true;
	char	uHex, lHex;
	int		cnt;						// "%5x"의 경우 5개만 출력하도록 출력한 개수.

	// fmt의 "%08lx"에서 '0', '8', 'l'을 해석.
	for (i=0; (c=s[i]); i++){
		if (c=='x') break;
		else if (c>='1' && c<='9'){
			for (j=0; s[i]>='0' && s[i]<='9'; j++){
				tol[j] = s[i++];
			}
			tol[j] = '\0';
			i--;
			DecToLong(tol, &flagcnt);
		}
		else if (c=='0') flag0=true;
		else if (c=='l') flagl=true;
		else continue;
	}

	s = (char *)(&l);
	l = SWAP32(l);		// little, big endian에 따라서.(big이 출력하기 쉬워 순서를 바꿈)
	
	// 위의 flag에 따라 출력.
	if (flagcnt){
		if (flagcnt&0x01){	// flagcnt가 홀수 일때, upper를 무시, lower만 출력.
			c = s[(8-(flagcnt+1))/2]; // 홀수 일때 그 위치를 포함하는 곳의 값을 가져 옵니다.
			
			// lower 4 bits를 가져와서 ascii code로.
			lHex = ((c>>0)&0x0f);
			if (lHex!=0) leading_zero=false;
			if (lHex<10) lHex+='0';
			else         lHex+='A'-10;

			// lower 4 bits 출력.
			if (leading_zero){
				if (flag0) PutChar('0');
				else       PutChar(' ');
			}
			else PutChar(lHex);
			
			flagcnt--;
		}

		// byte단위의 data를 Hex로 출력.
		for (cnt=0, i=(8-flagcnt)/2; i<4; i++){
			c = s[i];
				
			// get upper 4 bits and lower 4 bits.
			uHex = ((c>>4)&0x0f);
			lHex = ((c>>0)&0x0f);

			// upper 4 bits and lower 4 bits to '0'~'9', 'A'~'F'.
			// upper 4 bits를 ascii code로.
			if (uHex!=0) leading_zero = false;
			if (uHex<10) uHex+='0';
			else         uHex+='A'-10;

			// upper 4 bits 출력.
			if (leading_zero){
				if (flag0) PutChar('0');
				else       PutChar(' ');
			}
			else PutChar(uHex);
			
			// lower 4 bits를 ascii code로.
			if (lHex!=0) leading_zero = false;
			if (lHex<10) lHex+='0';
			else         lHex+='A'-10;

			// lower 4 bits 출력.
			if (leading_zero){
				if (flag0) PutChar('0');
				else       PutChar(' ');
			}
			else PutChar(lHex);
		}
	}
	else {
		for (i=0; i<4; i++){
			c = s[i];
	
			// get upper 4 bits and lower 4 bits.
			uHex = ((c>>4)&0x0f);
			lHex = ((c>>0)&0x0f);

			// upper 4 bits and lower 4 bits to '0'~'9', 'A'~'F'.
			if (uHex!=0) leading_zero = false;
			if (uHex<10) uHex+='0';
			else         uHex+='A'-10;
			if (!leading_zero) PutChar(uHex);
			
			if (lHex!=0 || i==3) leading_zero = false;
			if (lHex<10) lHex+='0';
			else         lHex+='A'-10;
			if (!leading_zero) PutChar(lHex);
		}
	}
	return;
}


void PrintString(char *fmt, char *s){
	if (!fmt || !s) return;
	while (*s) PutChar(*s++);
	return;
}

void printf(char *fmt, ...){
 int	i;
	va_list args;
	char	*s=fmt;
	char	format[10];		// fmt의 인자가 "%08lx"라면, "08l"를 임시로 기록.
 
	va_start(args, fmt);
	while (*s){
		if (*s=='%'){
			s++;
			// s에서 "%08lx"형식을 가져와 format에 기록. 나중에 출력함수에 넘겨줌.
			format[0] = '%';
			for (i=1; i<10;){
				if (*s=='c' || *s=='d' || *s=='x' || *s=='s' || *s=='%'){
					format[i++] = *s;
					format[i] = '\0';
					break;
				}
				else {
					format[i++] = *s++;
				}
			}
			// "%s", "%c", "%d", "%x"를 찾아 출력할 함수 호출.
			switch (*s++){
				case 'c' :
					PrintChar(format, va_arg(args, int));
					break;
				case 'd' :
					PrintDec(format, va_arg(args, int));
					break;
				case 'x' :
					PrintHex(format, va_arg(args, int));
					break;
				case 's' :
					PrintString(format, va_arg(args, char *));
					break;
				case '%' :
					PrintChar("%c", '%');
					break;
			}
		}
		else {
			PrintChar("%c", *s);
			s++;
		}
	}
	va_end(args);
	return;
}
bool DecToLong(char *s, long *retval){
	long remainder;
	if (!s || !s[0]) return false;

	for (*retval=0; *s; s++){
		if (*s < '0' || *s > '9') return false;
		remainder = *s - '0';
		*retval = *retval * 10 + remainder;
	}

	return true;
}

int Power(int num, int cnt){

	long retval=num;
	cnt--;

	while (cnt--){
		retval *= num;
	}
	return retval;
}

void ClearLine(void){
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
		"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	return;
}

void GetIPAddr(void)
{
	int i;
	u8 addr_info[20];
        u32  clientfullip, hostfullip;

	for(i=0;i<20;i++)
                addr_info[i] =  __REG8(CONFIG_ROM_BASE + i);

        hostfullip &=0x00000000;
        hostfullip |= addr_info[12] << 24;
        hostfullip |= addr_info[13] << 16;
        hostfullip |= addr_info[14] << 8;
        hostfullip |= addr_info[15] ;
        hostfullip = SWAP32(hostfullip);
        hostIP=hostfullip;

        clientfullip &=0x00000000;
        clientfullip |= addr_info[16] << 24;
        clientfullip |= addr_info[17] << 16;
        clientfullip |= addr_info[18] << 8;
        clientfullip |= addr_info[19] ;
        clientfullip = SWAP32(clientfullip);
        clientIP=clientfullip;
}


#ifndef SMART_TYPES_H
#define SMART_TYPES_H


#define	true	1
#define	false	0
#define	NULL	(void *)0
#define	va_start(ap, p)	(ap = (char *) (&(p)+1))
#define	va_arg(ap, type)	((type *) (ap += sizeof(type)))[-1]
#define	va_end(ap)
#define	HZ			(1)

typedef	unsigned long		u32;
typedef	unsigned short		u16;
typedef	unsigned char		u8;
typedef	int			bool;
typedef	char *va_list;


#endif

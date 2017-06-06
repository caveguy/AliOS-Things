#ifndef _FFCONF
#define _FFCONF         7211	/* Revision ID */

#define	_USE_LFN	    0		/* 0 to 3 */
#define	_MAX_LFN	    255		/* Maximum LFN length to handle (12 to 255) */
#define	_LFN_UNICODE	0	    /* 0:ANSI/OEM or 1:Unicode */
#define _VOLUMES	    1
#define	_MIN_SS		    512
#define	_MAX_SS		    512
#define _FS_REENTRANT	0		/* 0:Disable or 1:Enable */

typedef unsigned char  uint8;                   /* �޷���8λ���ͱ���                        */
typedef signed   char  int8;                    /* �з���8λ���ͱ���                        */
typedef unsigned short uint16;                  /* �޷���16λ���ͱ���                       */
typedef signed   short int16;                   /* �з���16λ���ͱ���                       */
typedef unsigned int   uint32;                  /* �޷���32λ���ͱ���                       */
typedef signed   int   int32;                   /* �з���32λ���ͱ���                       */
typedef float          fp32;                    /* �����ȸ�������32λ���ȣ�                 */
typedef double         fp64;                    /* ˫���ȸ�������64λ���ȣ�                 */
typedef unsigned long long uint64;
typedef long long   int64;

typedef unsigned char  u_int8;                   /* �޷���8λ���ͱ���                        */
typedef unsigned short u_int16;                  /* �޷���16λ���ͱ���                       */
typedef unsigned int   u_int32;                  /* �޷���32λ���ͱ��� */


#endif 

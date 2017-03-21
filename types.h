#ifndef TYPES_H
#define TYPES_H

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef char			s8;
typedef short			s16;
typedef int			s32;
typedef long long		s64;


typedef u8  __u8;
typedef s8  __s8;
typedef u16  __u16;
typedef s16  __s16;
typedef u32  __u32;
typedef s32  __s32;
typedef u64  __u64;
typedef s64  __s64;


typedef __u16  __le16;
typedef __u16  __be16;
typedef __u32  __le32;
typedef __u32  __be32;
typedef __u64  __le64;
typedef __u64  __be64;

#endif

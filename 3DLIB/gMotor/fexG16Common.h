#ifndef G16_H
#define G16_H

#define G16_ISUNIFORM 0x0100

#define G16_COMPRESS_METHOD_MASK 0x3   // 0011
#define G16_NONCOMPRESSED        0     // 0000
#define G16_COMPRESSED_BY_UCL    1     // 0001
#define G16_COMPRESSED_BY_LZO    2     // 0010

#define G16_PACK_METHOD_MASK     0xC   // 1100
#define G16_NONPACKED            0     // 0000
#define G16_PACKED_BY_2DWAVELET  4     // 0100
#define G16_PACKED_BY_3DWAVELET  8     // 1000

#define FGA_POLY_CONF_ALL    0       // - ���� ��� ������������
#define FGA_POLY_CONF_LU     1       // - ���� ����� ������� �����������
#define FGA_POLY_CONF_RB     2       // - ���� ������ ������ �����������
#define FGA_POLY_CONF_RU     3       // - ���� ������ ������� �����������
#define FGA_POLY_CONF_LB     4       // - ���� ����� ������ �����������

#endif
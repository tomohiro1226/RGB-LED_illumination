#ifndef __BIT_Field
#define __BIT_Field

#include <pic.h>

#define Red_Out		RGB_Out.BIT4.R // 赤 : デューティー比を操作する
#define Green_Out	RGB_Out.BIT4.G // 緑 :
#define Blue_Out	RGB_Out.BIT4.B // 青 :
#define c_15		RGB_Out.BIT4.C //15までのカウントで使用(Slow_shine)

#define speed bit_cnt.DL
#define count bit_cnt.DH

typedef union{ //１６階調4BITの為、ビットフィールドでメモリ節約
	unsigned int rgb;
	struct{
		unsigned int C : 4;
		unsigned int B : 4;
		unsigned int G : 4;
		unsigned int R : 4;
	}BIT4;
}RGB;

typedef union{ //メモリ節約のため
	unsigned char DX;
	struct{
		unsigned char DL : 4;
		unsigned char DH : 4;
	};
}__Data;


/*変数宣言*/
RGB RGB_Out; //RGBのデューティー比を決めて、出力(出力はタイマー割り込み)
__Data bit_cnt; //カウントで使用

#endif

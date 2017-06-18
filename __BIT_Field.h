#ifndef __BIT_Field
#define __BIT_Field

#include <pic.h>

#define Red_Out		RGB_Out.BIT4.R // �� : �f���[�e�B�[��𑀍삷��
#define Green_Out	RGB_Out.BIT4.G // �� :
#define Blue_Out	RGB_Out.BIT4.B // �� :
#define c_15		RGB_Out.BIT4.C //15�܂ł̃J�E���g�Ŏg�p(Slow_shine)

#define speed bit_cnt.DL
#define count bit_cnt.DH

typedef union{ //�P�U�K��4BIT�ׁ̈A�r�b�g�t�B�[���h�Ń������ߖ�
	unsigned int rgb;
	struct{
		unsigned int C : 4;
		unsigned int B : 4;
		unsigned int G : 4;
		unsigned int R : 4;
	}BIT4;
}RGB;

typedef union{ //�������ߖ�̂���
	unsigned char DX;
	struct{
		unsigned char DL : 4;
		unsigned char DH : 4;
	};
}__Data;


/*�ϐ��錾*/
RGB RGB_Out; //RGB�̃f���[�e�B�[������߂āA�o��(�o�͂̓^�C�}�[���荞��)
__Data bit_cnt; //�J�E���g�Ŏg�p

#endif

//	PIC�}�C�R���T���v���v���O����
//	P=12F675
//
//	����m�F�@INT�s�����̓G�b�W������
//
//
//

#define _XTAL_FREQ 4000000	//�g�p������g���ɉ������l���Z�b�g(delay�}�N���Ŏg�p)

#include <pic.h>		// �w�b�_�E�t�@�C��
#include "__BIT_Field.h"

#define	Red		GPIO5
#define	Green	GPIO4
#define	Blue	GPIO1

__CONFIG(CPD_OFF & CP_OFF & BOREN_OFF & MCLRE_OFF & PWRTE_ON & WDTE_OFF & FOSC_INTRCIO);
//�R���t�B�M�����[�V�����E�r�b�g�̐ݒ�
// EEPROM�R�[�h�v���e�N�g=OFF
// �v���O�����R�[�h�E�v���e�N�g=OFF
// �u���E���A�E�g���Z�b�g=OFF
// GP3/MCLR�s��=GP3
// �p���[�A�b�v�E�^�C�}=ON
// �E�H�b�`�h�b�O�E�^�C�}=OFF
// INTIO���U(4MHz)

bit sw_flg_push = 0;
bit sw_flg_hold = 0;
const RGB array[] = { 0x0DD0 , 0xC400, 0x2F20 , 0xF1A0 , 0x26E0 , 0xFF00, 0x7070}; //�e�ʃI�[�o�[�ŃO���[�o���ϐ��Ɋi�[
					/* ���F�@�@�I�����W�@�ΐF�@�@�s���N�@�F�@�@���F�@���F*/

void Slow_shine(const __Data ary ); //��...��
void Order_shine(void);//�� 256�F??
void ioport(void);		// I/O�|�[�g�ݒ�֐��̃v���g�^�C�v

void acceleration(unsigned char x){ //__delay_ms�𐧌� 0 = 0ms, 1 = 10ms
	while( x>0 ){
		__delay_ms(10);
		x--;
	}
}

void shine(){

	const __Data ary[] = { 0xA5 , 0x0A, 0x22 };//�\���^�C���𐧌�( �����ω������邱�Ƃŕ\�����ω�����)

	count = 0;

	do{
		sw_flg_push = 0;
		Slow_shine( ary[count] );
		count = ( count == 2 ) ? 0 : count + 1; //�z������ԂɑI��
	}while( !sw_flg_hold ); //PSW�𒷉��������܂�

	sw_flg_hold = 0;
}

void main(void)			// ���C���֐�
{

	OSCCAL=__osccal_val();		//�@�L�����u���[�V�����f�[�^�̏�����

	ioport();

	while(1){
		Order_shine();//�F�����X�ɕω�
		shine(); //�萔�̐F�̒l�����Á`���܂Œ������s��
	}
}

void ioport(void)		// I/O�|�[�g�ݒ�֐�
{
	GPIO = 0x00; //GPIO <- 0

	TRISIO = 0x04; //���o�͐ݒ� GP2�����

	ANSEL &= 0xF1; //GP1,GP2,GP4���f�W�^��I/O�ɐݒ�

	CMCON |= 0x07; // Comparator Off

	OPTION_REG=0x48;	// �^�C�}0�̃N���b�N�͓����N���b�N�A�v���X�P�[���͎g��Ȃ�

	INTCON = 0xB0;	// GIE,PEIE,T0IE,INTE,GPIE,T0IF,INTF,GPIF
}

void interrupt isr(void)	// ���荞�݊֐�
{
	static unsigned char	cnt = 0;
	unsigned char f = 0;
	unsigned char c = 0;

	if(T0IF & T0IE){ //�^�C�}�[���荞�݁H
		if(cnt == 16){//���Z�b�g
			cnt = 0;
			GPIO = 0x32;
		}

		if( Red_Out == cnt )Red = 0; //�f���[�e�B�[����쐬�@��
		if( Green_Out == cnt )Green = 0; //�@�V	 ��
		if( Blue_Out == cnt )Blue = 0; // �V	��
		cnt ++;
	}

	if(INTF & INTE){ //INT���荞�݁H
		T0IE = 0;			// �^�C�}0���荞�݂��~�߂�

		Red = 0;
		Green = 0;
		Blue = 0;

		do{
			__delay_ms(50);
			f = ( ++c==10 ) ? 0 : 1;
		}while( GPIO2 & f ); //�P��!? or ������!?

		if( f == 1){
			speed = (speed==4) ? 0 : speed+2;
			sw_flg_push = 1;
		}else{
			sw_flg_hold = 1;
			Red_Out = 0;
			Green_Out = 0;
			Blue_Out = 0;
		}



		cnt = 16;
		T0IE = 1;			// �^�C�}0���荞�݂��g��
		INTF = 0; //INT���荞�݃t���O��0�ɂ���
	}
	T0IF=0;			// �^�C�}0���荞�݃t���O��0�ɂ���
	TMR0=0x4E;		// �^�C�}0��50�����ɃZ�b�g
}
void Slow_shine(const __Data ary ){
	unsigned char i = 0;

	while( !sw_flg_hold & !sw_flg_push ){ /*���荞�ݐM��������܂Ŗ������[�v(���[�v�P)*/
		Red_Out = Green_Out = Blue_Out = c_15 = 0;
		while ( !sw_flg_hold & !sw_flg_push ) { //���荞�ݐM��������܂�
			if(array[i].BIT4.R > c_15)Red_Out++; //�ԐF�𖾂邭���Ă���
			if(array[i].BIT4.G > c_15)Green_Out++; //�ΐF �V
			if(array[i].BIT4.B > c_15)Blue_Out++; //�F �V
			if( ++c_15 == 0)break;
			acceleration( ary.DL ); //__delay
		}
		__delay_ms(300);

		c_15 = 15;
		while ( !sw_flg_hold & !sw_flg_push ) { //���荞�ݐM��������܂�
			if(Red_Out == c_15)Red_Out--; //�ԐF���Â����Ă���
			if(Green_Out == c_15)Green_Out--; //�ΐF �V
			if(Blue_Out == c_15)Blue_Out--; //�F �V
			if( --c_15 == 1)break;
			acceleration( ary.DH ); //__delay
		}
		i = ( i== 6 ) ? 0 : i+1;
	}/*���[�v�P*/
}

void Order_shine(void){ //RGB���ꂼ��̒l���������ω������Ă���

	char rf,gf,bf; //rgb�t���O

	Blue_Out = 15;//������
	Green_Out = Red_Out = 0;

	bf = 1;//������
	gf = rf = 0;

	while( !sw_flg_hold ){//���荞�ݐM��������܂Ń��[�v

		if( rf == 1){
			Red_Out--;
			Green_Out++;
			if( Red_Out==0 )rf = 0;
		}
		if( gf == 1){
			Green_Out--;
			Blue_Out++;
			if( Green_Out==0 )gf = 0;
		}
		if( bf == 1){
			Blue_Out--;
			Red_Out++;
			if( Blue_Out==0 )bf = 0;
		}

		if( Red_Out == 15 ) 	rf = 1; // �Ԃ��t���Ɍ�������t���O�𗧂Ă�
		if( Green_Out == 15 ) 	gf = 1; // ��	�V
		if( Blue_Out == 15 ) 	bf = 1; // ��	�V

		acceleration(6 - speed);//�_���X�s�[�h���R���g���[�� (speed �� MAX 4)
	}
	sw_flg_hold = 0;
}

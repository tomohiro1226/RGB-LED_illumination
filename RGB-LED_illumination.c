//	PICマイコンサンプルプログラム
//	P=12F675
//
//	動作確認　INTピン入力エッジ割込み
//
//
//

#define _XTAL_FREQ 4000000	//使用する周波数に応じた値をセット(delayマクロで使用)

#include <pic.h>		// ヘッダ・ファイル
#include "__BIT_Field.h"

#define	Red		GPIO5
#define	Green	GPIO4
#define	Blue	GPIO1

__CONFIG(CPD_OFF & CP_OFF & BOREN_OFF & MCLRE_OFF & PWRTE_ON & WDTE_OFF & FOSC_INTRCIO);
//コンフィギュレーション・ビットの設定
// EEPROMコードプロテクト=OFF
// プログラムコード・プロテクト=OFF
// ブラウンアウトリセット=OFF
// GP3/MCLRピン=GP3
// パワーアップ・タイマ=ON
// ウォッチドッグ・タイマ=OFF
// INTIO発振(4MHz)

bit sw_flg_push = 0;
bit sw_flg_hold = 0;
const RGB array[] = { 0x0DD0 , 0xC400, 0x2F20 , 0xF1A0 , 0x26E0 , 0xFF00, 0x7070}; //容量オーバーでグローバル変数に格納
					/* 水色　　オレンジ　緑色　　ピンク　青色　　黄色　紫色*/

void Slow_shine(const __Data ary ); //暗...明
void Order_shine(void);//明 256色??
void ioport(void);		// I/Oポート設定関数のプロトタイプ

void acceleration(unsigned char x){ //__delay_msを制御 0 = 0ms, 1 = 10ms
	while( x>0 ){
		__delay_ms(10);
		x--;
	}
}

void shine(){

	const __Data ary[] = { 0xA5 , 0x0A, 0x22 };//表示タイムを制限( これを変化させることで表示が変化する)

	count = 0;

	do{
		sw_flg_push = 0;
		Slow_shine( ary[count] );
		count = ( count == 2 ) ? 0 : count + 1; //配列を順番に選択
	}while( !sw_flg_hold ); //PSWを長押しされるまで

	sw_flg_hold = 0;
}

void main(void)			// メイン関数
{

	OSCCAL=__osccal_val();		//　キャリブレーションデータの書込み

	ioport();

	while(1){
		Order_shine();//色が次々に変化
		shine(); //定数の色の値をを暗～明まで調整を行う
	}
}

void ioport(void)		// I/Oポート設定関数
{
	GPIO = 0x00; //GPIO <- 0

	TRISIO = 0x04; //入出力設定 GP2を入力

	ANSEL &= 0xF1; //GP1,GP2,GP4をデジタルI/Oに設定

	CMCON |= 0x07; // Comparator Off

	OPTION_REG=0x48;	// タイマ0のクロックは内部クロック、プリスケーラは使わない

	INTCON = 0xB0;	// GIE,PEIE,T0IE,INTE,GPIE,T0IF,INTF,GPIF
}

void interrupt isr(void)	// 割り込み関数
{
	static unsigned char	cnt = 0;
	unsigned char f = 0;
	unsigned char c = 0;

	if(T0IF & T0IE){ //タイマー割り込み？
		if(cnt == 16){//リセット
			cnt = 0;
			GPIO = 0x32;
		}

		if( Red_Out == cnt )Red = 0; //デューティー比を作成　赤
		if( Green_Out == cnt )Green = 0; //　〃	 緑
		if( Blue_Out == cnt )Blue = 0; // 〃	青
		cnt ++;
	}

	if(INTF & INTE){ //INT割り込み？
		T0IE = 0;			// タイマ0割り込みを止める

		Red = 0;
		Green = 0;
		Blue = 0;

		do{
			__delay_ms(50);
			f = ( ++c==10 ) ? 0 : 1;
		}while( GPIO2 & f ); //単発!? or 長押し!?

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
		T0IE = 1;			// タイマ0割り込みを使う
		INTF = 0; //INT割り込みフラグを0にする
	}
	T0IF=0;			// タイマ0割り込みフラグを0にする
	TMR0=0x4E;		// タイマ0を50ｍｓにセット
}
void Slow_shine(const __Data ary ){
	unsigned char i = 0;

	while( !sw_flg_hold & !sw_flg_push ){ /*割り込み信号が来るまで無限ループ(ループ１)*/
		Red_Out = Green_Out = Blue_Out = c_15 = 0;
		while ( !sw_flg_hold & !sw_flg_push ) { //割り込み信号が来るまで
			if(array[i].BIT4.R > c_15)Red_Out++; //赤色を明るくしていく
			if(array[i].BIT4.G > c_15)Green_Out++; //緑色 〃
			if(array[i].BIT4.B > c_15)Blue_Out++; //青色 〃
			if( ++c_15 == 0)break;
			acceleration( ary.DL ); //__delay
		}
		__delay_ms(300);

		c_15 = 15;
		while ( !sw_flg_hold & !sw_flg_push ) { //割り込み信号が来るまで
			if(Red_Out == c_15)Red_Out--; //赤色を暗くしていく
			if(Green_Out == c_15)Green_Out--; //緑色 〃
			if(Blue_Out == c_15)Blue_Out--; //青色 〃
			if( --c_15 == 1)break;
			acceleration( ary.DH ); //__delay
		}
		i = ( i== 6 ) ? 0 : i+1;
	}/*ループ１*/
}

void Order_shine(void){ //RGBそれぞれの値を少しずつ変化させていく

	char rf,gf,bf; //rgbフラグ

	Blue_Out = 15;//初期化
	Green_Out = Red_Out = 0;

	bf = 1;//初期化
	gf = rf = 0;

	while( !sw_flg_hold ){//割り込み信号が来るまでループ

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

		if( Red_Out == 15 ) 	rf = 1; // 赤がフルに光ったらフラグを立てる
		if( Green_Out == 15 ) 	gf = 1; // 緑	〃
		if( Blue_Out == 15 ) 	bf = 1; // 青	〃

		acceleration(6 - speed);//点灯スピードをコントロール (speed は MAX 4)
	}
	sw_flg_hold = 0;
}

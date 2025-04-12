#include <iostream>
#include "OASYS.h"

OASYS::OASYS(int wchars) {

	OASYS::clear_buf();
	OASYS::clear_out();
	OASYS::line_wchars = wchars;
}

void OASYS::clear_buf() {

	// buf[] の初期化
	for (int i = 0; i < BLOCK; i++)
		OASYS::buf[i] = 0;
}

void OASYS::clear_out() {

	// buf[] の初期化
	for (int i = 0; i < BLOCK; i++)
		OASYS::out[i] = 0;
}

void OASYS::clear_frame() {

	for (int i = 0; i < 512; i++)
	{
		OASYS::oasys_frame.data[i] = 0;
		OASYS::sjis_frame.data[i] = 0;
	}
	for (int i = 0; i < 5; i++)
	{
		OASYS::oasys_frame.attr[i] = 0;
		OASYS::sjis_frame.attr[i] = 0;
	}
}

// JISコードからSJISコードに変換
wchar_t OASYS::jis_to_sjis(wchar_t jis) {

	wchar_t c1 = (jis >> 8) & 0xFF;
	wchar_t c2 = jis & 0xFF;

	if (c1 & 0x0001)
	{
		if (c2 < 0x60)
			c2 += 0x1F;
		else
			c2 += 0x20;
	}
	else
	{
		c2 += 0x7E;
	}
	if (c1 < 0x5F)
		c1 = (c1 + 0xE1) >> 1;
	else
		c1 = (c1 + 0x161) >> 1;

	return c1 << 8 | c2;
}

// ASCIIコード範囲内かチェック
bool OASYS::is_ascii(wchar_t ascii) {

	wchar_t jis_high = (ascii >> 8) & 0x7F;
	wchar_t jis_low = ascii & 0x7F;

	// ASCII
	if ((jis_high >= 0x20 && jis_high <= 0x7E) && (jis_low >= 0x20 && jis_low <= 0x7E))
		return true;

	return false;
}

// 半角カナまたは上付きASCIIの範囲内かチェック
bool OASYS::is_kana(wchar_t kana) {

	wchar_t jis_high = (kana >> 8) & 0xFF;
	wchar_t jis_low = kana & 0xFF;

	// 半角カナ
	if ((jis_high >= 0x81 && jis_high <= 0xFE) && (jis_low >= 0x81 && jis_low <= 0xFE))
		return true;

	return false;
}

// JISコード範囲内かチェック
bool OASYS::is_jis(wchar_t jis) {

	wchar_t jis_high = (jis >> 8) & 0xFF;
	wchar_t jis_low = jis & 0xFF;

	// 全角英数、かな
	if ((jis_high >= 0x21 && jis_high <= 0x27) && (jis_low >= 0x21 && jis_low <= 0x7E))
		return true;

	// 漢字コード
	if ((jis_high >= 0x30 && jis_high <= 0x74) && (jis_low >= 0x21 && jis_low <= 0x7E))
		return true;

	return false;
}


Ascii_Kana OASYS::hankaku_to_sjis_kana(char kana) {

	Hankaku_Kana k = hankaku_kana[kana & 0x7F];

	Ascii_Kana ascii;
	ascii.c1 = (char)k.kana_code;

	switch (k.kana_mod)
	{
	case ASCII:
	case KANA:
	case HAN_TSUME:
		ascii.c2 = 0;
		break;
	case DAKUTEN:
		ascii.c2 = (char)0xDE;
		break;
	case HAN_DAKUTEN:
		ascii.c2 = (char)0xDF;
		break;
	default:
		ascii.c1 = 0x21;
		ascii.c2 = 0x3f;
		break;
	}

	return ascii;
}

//********************************
// OASYSフレーム単位のSJIS変換
//********************************
int OASYS::convert_frame() {

	const int data_length = (OASYS::line_wchars * 2);	// 1行の折り返し

	int oasys_ptr = 0;
	int sjis_ptr = 0;

	while (oasys_ptr < data_length)
	{
		// OASYSデータ領域
		bool w_flag = false;	// word flag
		wchar_t sjis;

		char jis_high = OASYS::oasys_frame.data[oasys_ptr]; oasys_ptr++;
		char jis_low  = OASYS::oasys_frame.data[oasys_ptr]; oasys_ptr++;
		//wchar_t jis = jis_high << 8 + jis_low;
		wchar_t jis = (jis_high << 8) + (jis_low & 0xFF);

		// 半角英数の場合 (ASCIIコード)
		if ((jis & 0x8080) == 0x0080)
		{
			if (is_ascii(jis))
			{
				OASYS::sjis_frame.data[sjis_ptr] = jis_high; sjis_ptr++;
				// 2バイト目が'_'の場合は半角詰めなので変換しない
				jis_low &= 0x7F;
				if (jis_low != 0x5F)
				{
					OASYS::sjis_frame.data[sjis_ptr] = jis_low; sjis_ptr++;
				}
			}
		}
		// 半角カナまたは上付き文字の場合 (ASCIIコード)
		if ((jis & 0x8080) == 0x8080)
		{
			if (is_kana(jis))
			{
				// 1文字目のOASYSコードをASCIIに変換
				Ascii_Kana kana = OASYS::hankaku_to_sjis_kana(jis_high);
				OASYS::sjis_frame.data[sjis_ptr] = kana.c1; sjis_ptr++;
				if (kana.c2)
				{
					OASYS::sjis_frame.data[sjis_ptr] = kana.c2; sjis_ptr++;
				}
				// 2文字目のOASYSコードをASCIIに変換
				kana = OASYS::hankaku_to_sjis_kana(jis_low);
				// 2バイト目が'_'の場合は半角詰めなので変換しない
				if (kana.c1 != 0x5F)
				{
					OASYS::sjis_frame.data[sjis_ptr] = kana.c1; sjis_ptr++;
				}
				if (kana.c2)
				{
					OASYS::sjis_frame.data[sjis_ptr] = kana.c2; sjis_ptr++;
				}
			}
		}
		// 全角英数、かな
		if ((jis & 0x8080) == 0)
		{
			if (OASYS::is_jis(jis))
			{
				sjis = OASYS::jis_to_sjis(jis);
				w_flag = true;
			}
		}
		// 下線付きの全角英数、かな
		if ((jis & 0x8080) == 0x8000)
		{
			jis &= 0x7F7F;
			if (OASYS::is_jis(jis))
			{
				sjis = OASYS::jis_to_sjis(jis);
				w_flag = true;
			}
		}
		//JISコードの範囲外
		if (w_flag == false)
		{
			jis &= 0x7F7F;
			// OASYS罫線 0x2C21～0x2C7E は全角空白
			if ((jis > 0x2C20) && (jis < 0x2C7F))
			{
				sjis = 0x2020;
				w_flag = true;
			}

			// ESC(?) シーケンス
			switch (jis)
			{
			case 0x203F:	// AS-ISで半角スペース2個
				sjis = 0x2020;
				w_flag = true;
				break;
			case 0x2C66:	// AS-ISで全角空白(?)
			case 0x2F22:	// AS-ISで全角空白(?)
			case 0x2F2A:	// 拡大文字の後に付いているので全角スペースに変換する
				sjis = 0x8140;
				w_flag = true;
				break;
			case 0x2F7D:
				// 次の2バイトは制御コードっぽいので無視する
				oasys_ptr += 2;
				break;
			}
		}

		// 不要な制御コードは無視する
		if (w_flag == false)
			continue;

		// 出力バッファに書き込む
		char s[2];
		s[0] = (sjis >> 8) & 0xFF;
		s[1] = sjis & 0xFF;
		OASYS::sjis_frame.data[sjis_ptr] = s[0]; sjis_ptr++;
		OASYS::sjis_frame.data[sjis_ptr] = s[1]; sjis_ptr++;
	}
	// LF挿入
	OASYS::sjis_frame.data[sjis_ptr] = 0x0A; sjis_ptr++;

	int attribute = 0;

	for (int i = 0; i < 5; i++)
	{
		 int val = OASYS::oasys_frame.attr[i] & 0xFF;
		 OASYS::sjis_frame.attr[i] = val;
		 attribute += val;
	}

	if (attribute == 0)
		// 変換は正常に終了した。
		// attribute 5ビット{0,0,0,0,0}の場合は次もOASYSフレーム
		return 0;
	else if (attribute == 0x01)
		// 変換は正常に終了した。
		// 次のフレームはOASYSではない
		return 0;
	else if (attribute == 0x81)
	{
		// 変換は正常に終了した。
		// attribute 5ビット{0x81,0,0,0,0}の場合は改ページ
		// LF挿入
		OASYS::sjis_frame.data[sjis_ptr] = 0x0C; sjis_ptr++;
		return 0;
	}
	else if (attribute == 0x7D)
		// OASYSフレームではない
		return -1;

	return -1;
}

//***************************
// ブロック単位のSJIS変換
//***************************
int OASYS::oasys_to_text() {

	const int data_length  = (OASYS::line_wchars * 2);		// 折り返し長さ(char)
	const int frame_length = data_length + 5;	// +5 ByteのHouse Keeping

	int buf_ptr = 0;
	int out_ptr = 0;
	int lines = 0;

	while (buf_ptr < BLOCK)
	{
		// OASYSフレーム単位のSJIS変換
		OASYS::clear_frame();
		for (int i = 0; i < data_length; i++)
		{
			OASYS::oasys_frame.data[i] = OASYS::buf[buf_ptr]; buf_ptr++;
		}
		for (int i = 0; i < 5; i++)
		{
			OASYS::oasys_frame.attr[i] = OASYS::buf[buf_ptr]; buf_ptr++;
		}

		if (OASYS::convert_frame() == 0)
		{
			for (int i = 0; i < sizeof(OASYS::sjis_frame.data); i++)
			{
				char sjis_char = OASYS::sjis_frame.data[i];

				if (sjis_char == 0)
					break;
				if (sjis_char == 0x0A)
					lines++;

				OASYS::out[out_ptr] = sjis_char; out_ptr++;
			}
		}

		// ブロックの残りにフレームが入るスペースがなければここでブロックの処理を終了する
		if ((BLOCK - buf_ptr) < frame_length)
			break;
	}
	return lines;
}

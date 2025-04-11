#pragma once

#define BLOCK	0x400	// 1024 byte

// 半角カナの修飾コード
enum KANA_MOD
{
	ASCII = 0,		// ASCIIコード
	KANA,			// 半角カナ
	HAN_TSUME,		// 半角詰め
	DAKUTEN,		// 半角濁点゛
	HAN_DAKUTEN,	// 半角半濁点
	INVALID			// 半角'?'
};

typedef struct { char c1; char c2; } Ascii_Kana;
typedef struct { int kana_code; KANA_MOD kana_mod; } Hankaku_Kana;
typedef struct { char data[512]; char attr[5]; } Oasys_Frame;

class OASYS
{

public:

	OASYS(int wchars);

	char buf[BLOCK];	// OASYS文書を1ブロック格納
	char out[BLOCK];	// 変換後のTEXTを1ブロック格納

	void clear_buf();
	void clear_out();

	// ブロック単位のSJIS変換
	int oasys_to_text();

private:

	int fold_wchars = 0x30;		// 折り返し文字数(2バイト文字)

	Oasys_Frame oasys_frame;
	Oasys_Frame sjis_frame;

	void clear_frame();

	// OASYSフレーム単位のSJIS変換
	int convert_frame();

	wchar_t jis_to_sjis(wchar_t jis);
	bool is_ascii(char ascii);
	bool is_jis(wchar_t jis);

	// 半角OASYS文字をASCIIカナに変換
	Ascii_Kana hankaku_to_sjis_kana(char h);

	Hankaku_Kana hankaku_kana[128] =
	{
		// 半角カナ (0x80 - 0x8F)
		{0xA0, KANA},	// NULL
		{0xA1, KANA},	// 。
		{0xA2, KANA},	// 「
		{0xA3, KANA},	// 」
		{0xA4, KANA},	// 、
		{0xA5, KANA},	// ・
		{0xA6, KANA},	// ヲ
		{0xA7, KANA},	// ァ

		{0xA8, KANA},	// ィ
		{0xA9, KANA},	// ゥ
		{0xAA, KANA},	// ェ
		{0xAB, KANA},	// ォ
		{0xAC, KANA},	// ャ
		{0xAD, KANA},	// ュ
		{0xAE, KANA},	// ョ
		{0xAF, KANA},	// ッ

		// 半角カナ (0x90 - 0x9F)
		{0xB0, KANA},	// -
		{0xB1, KANA},	// ア
		{0xB2, KANA},	// イ
		{0xB3, KANA},	// ウ
		{0xB4, KANA},	// エ
		{0xB5, KANA},	// オ
		{0xB6, KANA},	// カ
		{0xB7, KANA},	// キ

		{0xB8, KANA},	// ク
		{0xB9, KANA},	// ケ
		{0xBA, KANA},	// コ
		{0xBB, KANA},	// サ
		{0xBC, KANA},	// シ
		{0xBD, KANA},	// ス
		{0xBE, KANA},	// セ
		{0xBF, KANA},	// ソ

		// 上付きASCII (0xA0 - 0xAF)
		{0x20, ASCII},	// SP
		{0x21, ASCII},	// !
		{0x22, ASCII},	// "
		{0x23, ASCII},	// #
		{0x24, ASCII},	// $
		{0x25, ASCII},	// %
		{0x26, ASCII},	// &
		{0x27, ASCII},	// '

		{0x28, ASCII},	// (
		{0x29, ASCII},	// )
		{0x2A, ASCII},	// *
		{0x2B, ASCII},	// +
		{0x2C, ASCII},	// ,
		{0x2D, ASCII},	// -
		{0x2E, ASCII},	// .
		{0x2F, ASCII},	// /

		// 上付きASCII (0xB0 - 0xBF)
		{0x30, ASCII},	// 0
		{0x31, ASCII},	// 1
		{0x32, ASCII},	// 2
		{0x33, ASCII},	// 3
		{0x34, ASCII},	// 4
		{0x35, ASCII},	// 5
		{0x36, ASCII},	// 6
		{0x37, ASCII},	// 7

		{0x38, ASCII},	// 8
		{0x39, ASCII},	// 9
		{0x3A, ASCII},	// :
		{0x3B, ASCII},	// ;
		{0x3C, ASCII},	// <
		{0x3D, ASCII},	// =
		{0x3E, ASCII},	// >
		{0x3F, ASCII},	// ?

		// 半角カナ (0xC0 - 0xCF)
		{0xC0, KANA},	// タ
		{0xC1, KANA},	// チ
		{0xC2, KANA},	// ツ
		{0xC3, KANA},	// テ
		{0xC4, KANA},	// ト
		{0xC5, KANA},	// ナ
		{0xC6, KANA},	// ニ
		{0xC7, KANA},	// ヌ

		{0xC8, KANA},	// ネ
		{0xC9, KANA},	// ノ
		{0xCA, KANA},	// ハ
		{0xCB, KANA},	// ヒ
		{0xCC, KANA},	// フ
		{0xCD, KANA},	// ヘ
		{0xCE, KANA},	// ホ
		{0xCF, KANA},	// マ

		// 半角カナ (0xD0 - 0xDF)
		{0xD0, KANA},	// ミ
		{0xD1, KANA},	// ム
		{0xD2, KANA},	// メ
		{0xD3, KANA},	// モ
		{0xD4, KANA},	// ヤ
		{0xD5, KANA},	// ユ
		{0xD6, KANA},	// ヨ
		{0xD7, KANA},	// ラ

		{0xD8, KANA},	// リ
		{0xD9, KANA},	// ル
		{0xDA, KANA},	// レ
		{0xDB, KANA},	// ロ
		{0xDC, KANA},	// ワ
		{0xDD, KANA},	// ン
		{0xDE, KANA},		// ゛はOASYSで使わない
		{0x5F, HAN_TSUME},	// ゜は半角詰めに使用

		// 濁点付き半角カナ (0xE0 - 0xEF)
		{0x21, INVALID},
		{0x21, INVALID},
		{0x21, INVALID},
		{0x21, INVALID},
		{0x21, INVALID}, 
		{0x21, INVALID},
		{0xB6, DAKUTEN},		// ガ
		{0xB7, DAKUTEN},		// ギ

		{0xB8, DAKUTEN},		// グ
		{0xB9, DAKUTEN},		// ゲ
		{0xBA, DAKUTEN},		// ゴ
		{0xBB, DAKUTEN},		// ザ
		{0xBC, DAKUTEN},		// ジ
		{0xBD, DAKUTEN},		// ヅ
		{0xBE, DAKUTEN},		// デ
		{0xBF, DAKUTEN},		// ド

		// 濁点付き半角カナ (0xF0 - 0xFF)
		{0xC0, DAKUTEN},		// ダ
		{0xC1, DAKUTEN},		// ヂ
		{0xC2, DAKUTEN},		// ヅ
		{0xC3, DAKUTEN},		// デ
		{0xC4, DAKUTEN},		// ド
		{0xCA, HAN_DAKUTEN},	// パ
		{0xCB, HAN_DAKUTEN},	// ピ	
		{0xCC, HAN_DAKUTEN},	// プ

		{0xCD, HAN_DAKUTEN},	// ペ
		{0xCE, HAN_DAKUTEN},	// ポ
		{0xCA, DAKUTEN},		// バ
		{0xCB, DAKUTEN},		// ビ
		{0xCC, DAKUTEN},		// ブ
		{0xCD, DAKUTEN},		// ベ
		{0xCE, DAKUTEN},		// ボ
		{0x21, INVALID}
	};
};


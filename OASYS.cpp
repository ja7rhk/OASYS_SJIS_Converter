#include "OASYS.h"

// JISコードからSJISコードに変換
wchar_t OASYS::jis2sjis(wchar_t jis) {

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
bool OASYS::is_ascii(char ascii) {

	if (ascii >= 0x20 && ascii < 0x7F)
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


Ascii_Kana OASYS::oasys_to_sjis(char kana) {

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


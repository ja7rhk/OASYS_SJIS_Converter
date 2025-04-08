#pragma once


// îºäpÉJÉiÇÃèCè¸ÉRÅ[Éh
enum KANA_MOD
{
	ASCII = 0,		// ASCIIÉRÅ[Éh
	KANA,			// îºäpÉJÉi
	HAN_TSUME,		// îºäpãlÇﬂ
	DAKUTEN,		// îºäpë˜ì_ÅJ
	HAN_DAKUTEN,	// îºäpîºë˜ì_
	INVALID			// îºäp'?'
};

typedef struct {
	char	c1;
	char	c2;
} Ascii_Kana;

typedef struct {
	int	 kana_code;
	KANA_MOD kana_mod;
} Hankaku_Kana;

class OASYS
{

public:

	wchar_t jis2sjis(wchar_t jis);

	bool is_ascii(char ascii);

	bool is_jis(wchar_t jis);

	Ascii_Kana oasys_to_sjis (char h);


private:

	Hankaku_Kana hankaku_kana[128] =
	{
		// îºäpÉJÉi
		{0xA0, KANA}, {0xA1, KANA}, {0xA2, KANA}, {0xA3, KANA}, {0xA4, KANA}, {0xA5, KANA}, {0xA6, KANA}, {0xA7, KANA},
		{0xA8, KANA}, {0xA9, KANA}, {0xAA, KANA}, {0xAB, KANA}, {0xAC, KANA}, {0xAD, KANA}, {0xAE, KANA}, {0xAF, KANA},

		{0xB0, KANA}, {0xB1, KANA}, {0xB2, KANA}, {0xB3, KANA}, {0xB4, KANA}, {0xB5, KANA}, {0xB6, KANA}, {0xB7, KANA},
		{0xB8, KANA}, {0xB9, KANA}, {0xBA, KANA}, {0xBB, KANA}, {0xBC, KANA}, {0xBD, KANA}, {0xBE, KANA}, {0xBF, KANA},

		// è„ïtÇ´ï∂éö
		{0x20, ASCII}, {0x21, ASCII}, {0x22, ASCII}, {0x23, ASCII}, {0x24, ASCII}, {0x25, ASCII}, {0x26, ASCII}, {0x27, ASCII},
		{0x28, ASCII}, {0x29, ASCII}, {0x2A, ASCII}, {0x2B, ASCII}, {0x2C, ASCII}, {0x2D, ASCII}, {0x2E, ASCII}, {0x2F, ASCII},

		{0x30, ASCII}, {0x31, ASCII}, {0x32, ASCII}, {0x33, ASCII}, {0x34, ASCII}, {0x35, ASCII}, {0x36, ASCII}, {0x37, ASCII},
		{0x38, ASCII}, {0x39, ASCII}, {0x3A, ASCII}, {0x3B, ASCII}, {0x3C, ASCII}, {0x3D, ASCII}, {0x3E, ASCII}, {0x3F, ASCII},

		// îºäpÉJÉi
		{0xC0, KANA}, {0xC1, KANA}, {0xC2, KANA}, {0xC3, KANA}, {0xC4, KANA}, {0xC5, KANA}, {0xC6, KANA}, {0xC7, KANA},
		{0xC8, KANA}, {0xC9, KANA}, {0xCA, KANA}, {0xCB, KANA}, {0xCC, KANA}, {0xCD, KANA}, {0xCE, KANA}, {0xCF, KANA},

		{0xD0, KANA}, {0xD1, KANA}, {0xD2, KANA}, {0xD3, KANA}, {0xD4, KANA}, {0xD5, KANA}, {0xD6, KANA}, {0xD7, KANA},
		{0xD8, KANA}, {0xD9, KANA}, {0xDA, KANA}, {0xDB, KANA}, {0xDC, KANA}, {0xDD, KANA}, {0xDE, KANA}, {0x5F, HAN_TSUME},

		// ë˜ì_ïtÇ´îºäpÉJÉi
		{0x21, INVALID}, {0x21, INVALID}, {0x21, INVALID}, {0x21, INVALID}, {0x21, INVALID}, {0x21, INVALID}, {0xB6, DAKUTEN}, {0xB7, DAKUTEN},
		{0xB8, DAKUTEN}, {0xB9, DAKUTEN}, {0xBA, DAKUTEN}, {0xBB, DAKUTEN}, {0xBC, DAKUTEN}, {0xBD, DAKUTEN}, {0xBE, DAKUTEN}, {0xBF, DAKUTEN},

		{0xC0, DAKUTEN}, {0xC1, DAKUTEN}, {0xC2, DAKUTEN}, {0xC3, DAKUTEN}, {0xC4, DAKUTEN}, {0x21, INVALID}, {0x21, INVALID}, {0x21, INVALID},
		{0x21, INVALID}, {0x21, INVALID}, {0xCA, DAKUTEN}, {0xCB, DAKUTEN}, {0xCC, DAKUTEN}, {0xCD, DAKUTEN}, {0xCE, DAKUTEN}, {0x21, INVALID}
	};
};


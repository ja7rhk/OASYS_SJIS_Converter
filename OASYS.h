#pragma once

#define BLOCK	0x400	// 1024 byte

// ���p�J�i�̏C���R�[�h
enum KANA_MOD
{
	ASCII = 0,		// ASCII�R�[�h
	KANA,			// ���p�J�i
	HAN_TSUME,		// ���p�l��
	DAKUTEN,		// ���p���_�J
	HAN_DAKUTEN,	// ���p�����_
	INVALID			// ���p'?'
};

typedef struct { char c1; char c2; } Ascii_Kana;
typedef struct { int kana_code; KANA_MOD kana_mod; } Hankaku_Kana;
typedef struct { char data[512]; char attr[5]; } Oasys_Frame;

class OASYS
{

public:

	OASYS(int wchars);

	char buf[BLOCK];	// OASYS������1�u���b�N�i�[
	char out[BLOCK];	// �ϊ����TEXT��1�u���b�N�i�[

	void clear_buf();
	void clear_out();

	// �u���b�N�P�ʂ�SJIS�ϊ�
	int oasys_to_text();

private:

	int fold_wchars = 0x30;		// �܂�Ԃ�������(2�o�C�g����)

	Oasys_Frame oasys_frame;
	Oasys_Frame sjis_frame;

	void clear_frame();

	// OASYS�t���[���P�ʂ�SJIS�ϊ�
	int convert_frame();

	wchar_t jis_to_sjis(wchar_t jis);
	bool is_ascii(char ascii);
	bool is_jis(wchar_t jis);

	// ���pOASYS������ASCII�J�i�ɕϊ�
	Ascii_Kana hankaku_to_sjis_kana(char h);

	Hankaku_Kana hankaku_kana[128] =
	{
		// ���p�J�i (0x80 - 0x8F)
		{0xA0, KANA},	// NULL
		{0xA1, KANA},	// �B
		{0xA2, KANA},	// �u
		{0xA3, KANA},	// �v
		{0xA4, KANA},	// �A
		{0xA5, KANA},	// �E
		{0xA6, KANA},	// ��
		{0xA7, KANA},	// �@

		{0xA8, KANA},	// �B
		{0xA9, KANA},	// �D
		{0xAA, KANA},	// �F
		{0xAB, KANA},	// �H
		{0xAC, KANA},	// ��
		{0xAD, KANA},	// ��
		{0xAE, KANA},	// ��
		{0xAF, KANA},	// �b

		// ���p�J�i (0x90 - 0x9F)
		{0xB0, KANA},	// -
		{0xB1, KANA},	// �A
		{0xB2, KANA},	// �C
		{0xB3, KANA},	// �E
		{0xB4, KANA},	// �G
		{0xB5, KANA},	// �I
		{0xB6, KANA},	// �J
		{0xB7, KANA},	// �L

		{0xB8, KANA},	// �N
		{0xB9, KANA},	// �P
		{0xBA, KANA},	// �R
		{0xBB, KANA},	// �T
		{0xBC, KANA},	// �V
		{0xBD, KANA},	// �X
		{0xBE, KANA},	// �Z
		{0xBF, KANA},	// �\

		// ��t��ASCII (0xA0 - 0xAF)
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

		// ��t��ASCII (0xB0 - 0xBF)
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

		// ���p�J�i (0xC0 - 0xCF)
		{0xC0, KANA},	// �^
		{0xC1, KANA},	// �`
		{0xC2, KANA},	// �c
		{0xC3, KANA},	// �e
		{0xC4, KANA},	// �g
		{0xC5, KANA},	// �i
		{0xC6, KANA},	// �j
		{0xC7, KANA},	// �k

		{0xC8, KANA},	// �l
		{0xC9, KANA},	// �m
		{0xCA, KANA},	// �n
		{0xCB, KANA},	// �q
		{0xCC, KANA},	// �t
		{0xCD, KANA},	// �w
		{0xCE, KANA},	// �z
		{0xCF, KANA},	// �}

		// ���p�J�i (0xD0 - 0xDF)
		{0xD0, KANA},	// �~
		{0xD1, KANA},	// ��
		{0xD2, KANA},	// ��
		{0xD3, KANA},	// ��
		{0xD4, KANA},	// ��
		{0xD5, KANA},	// ��
		{0xD6, KANA},	// ��
		{0xD7, KANA},	// ��

		{0xD8, KANA},	// ��
		{0xD9, KANA},	// ��
		{0xDA, KANA},	// ��
		{0xDB, KANA},	// ��
		{0xDC, KANA},	// ��
		{0xDD, KANA},	// ��
		{0xDE, KANA},		// �J��OASYS�Ŏg��Ȃ�
		{0x5F, HAN_TSUME},	// �K�͔��p�l�߂Ɏg�p

		// ���_�t�����p�J�i (0xE0 - 0xEF)
		{0x21, INVALID},
		{0x21, INVALID},
		{0x21, INVALID},
		{0x21, INVALID},
		{0x21, INVALID}, 
		{0x21, INVALID},
		{0xB6, DAKUTEN},		// �K
		{0xB7, DAKUTEN},		// �M

		{0xB8, DAKUTEN},		// �O
		{0xB9, DAKUTEN},		// �Q
		{0xBA, DAKUTEN},		// �S
		{0xBB, DAKUTEN},		// �U
		{0xBC, DAKUTEN},		// �W
		{0xBD, DAKUTEN},		// �d
		{0xBE, DAKUTEN},		// �f
		{0xBF, DAKUTEN},		// �h

		// ���_�t�����p�J�i (0xF0 - 0xFF)
		{0xC0, DAKUTEN},		// �_
		{0xC1, DAKUTEN},		// �a
		{0xC2, DAKUTEN},		// �d
		{0xC3, DAKUTEN},		// �f
		{0xC4, DAKUTEN},		// �h
		{0xCA, HAN_DAKUTEN},	// �p
		{0xCB, HAN_DAKUTEN},	// �s	
		{0xCC, HAN_DAKUTEN},	// �v

		{0xCD, HAN_DAKUTEN},	// �y
		{0xCE, HAN_DAKUTEN},	// �|
		{0xCA, DAKUTEN},		// �o
		{0xCB, DAKUTEN},		// �r
		{0xCC, DAKUTEN},		// �u
		{0xCD, DAKUTEN},		// �x
		{0xCE, DAKUTEN},		// �{
		{0x21, INVALID}
	};
};


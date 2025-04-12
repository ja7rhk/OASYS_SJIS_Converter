#include <iostream>
#include "OASYS.h"

OASYS::OASYS(int wchars) {

	OASYS::clear_buf();
	OASYS::clear_out();
	OASYS::line_wchars = wchars;
}

void OASYS::clear_buf() {

	// buf[] �̏�����
	for (int i = 0; i < BLOCK; i++)
		OASYS::buf[i] = 0;
}

void OASYS::clear_out() {

	// buf[] �̏�����
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

// JIS�R�[�h����SJIS�R�[�h�ɕϊ�
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

// ASCII�R�[�h�͈͓����`�F�b�N
bool OASYS::is_ascii(wchar_t ascii) {

	wchar_t jis_high = (ascii >> 8) & 0x7F;
	wchar_t jis_low = ascii & 0x7F;

	// ASCII
	if ((jis_high >= 0x20 && jis_high <= 0x7E) && (jis_low >= 0x20 && jis_low <= 0x7E))
		return true;

	return false;
}

// ���p�J�i�܂��͏�t��ASCII�͈͓̔����`�F�b�N
bool OASYS::is_kana(wchar_t kana) {

	wchar_t jis_high = (kana >> 8) & 0xFF;
	wchar_t jis_low = kana & 0xFF;

	// ���p�J�i
	if ((jis_high >= 0x81 && jis_high <= 0xFE) && (jis_low >= 0x81 && jis_low <= 0xFE))
		return true;

	return false;
}

// JIS�R�[�h�͈͓����`�F�b�N
bool OASYS::is_jis(wchar_t jis) {

	wchar_t jis_high = (jis >> 8) & 0xFF;
	wchar_t jis_low = jis & 0xFF;

	// �S�p�p���A����
	if ((jis_high >= 0x21 && jis_high <= 0x27) && (jis_low >= 0x21 && jis_low <= 0x7E))
		return true;

	// �����R�[�h
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
// OASYS�t���[���P�ʂ�SJIS�ϊ�
//********************************
int OASYS::convert_frame() {

	const int data_length = (OASYS::line_wchars * 2);	// 1�s�̐܂�Ԃ�

	int oasys_ptr = 0;
	int sjis_ptr = 0;

	while (oasys_ptr < data_length)
	{
		// OASYS�f�[�^�̈�
		bool w_flag = false;	// word flag
		wchar_t sjis;

		char jis_high = OASYS::oasys_frame.data[oasys_ptr]; oasys_ptr++;
		char jis_low  = OASYS::oasys_frame.data[oasys_ptr]; oasys_ptr++;
		//wchar_t jis = jis_high << 8 + jis_low;
		wchar_t jis = (jis_high << 8) + (jis_low & 0xFF);

		// ���p�p���̏ꍇ (ASCII�R�[�h)
		if ((jis & 0x8080) == 0x0080)
		{
			if (is_ascii(jis))
			{
				OASYS::sjis_frame.data[sjis_ptr] = jis_high; sjis_ptr++;
				// 2�o�C�g�ڂ�'_'�̏ꍇ�͔��p�l�߂Ȃ̂ŕϊ����Ȃ�
				jis_low &= 0x7F;
				if (jis_low != 0x5F)
				{
					OASYS::sjis_frame.data[sjis_ptr] = jis_low; sjis_ptr++;
				}
			}
		}
		// ���p�J�i�܂��͏�t�������̏ꍇ (ASCII�R�[�h)
		if ((jis & 0x8080) == 0x8080)
		{
			if (is_kana(jis))
			{
				// 1�����ڂ�OASYS�R�[�h��ASCII�ɕϊ�
				Ascii_Kana kana = OASYS::hankaku_to_sjis_kana(jis_high);
				OASYS::sjis_frame.data[sjis_ptr] = kana.c1; sjis_ptr++;
				if (kana.c2)
				{
					OASYS::sjis_frame.data[sjis_ptr] = kana.c2; sjis_ptr++;
				}
				// 2�����ڂ�OASYS�R�[�h��ASCII�ɕϊ�
				kana = OASYS::hankaku_to_sjis_kana(jis_low);
				// 2�o�C�g�ڂ�'_'�̏ꍇ�͔��p�l�߂Ȃ̂ŕϊ����Ȃ�
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
		// �S�p�p���A����
		if ((jis & 0x8080) == 0)
		{
			if (OASYS::is_jis(jis))
			{
				sjis = OASYS::jis_to_sjis(jis);
				w_flag = true;
			}
		}
		// �����t���̑S�p�p���A����
		if ((jis & 0x8080) == 0x8000)
		{
			jis &= 0x7F7F;
			if (OASYS::is_jis(jis))
			{
				sjis = OASYS::jis_to_sjis(jis);
				w_flag = true;
			}
		}
		//JIS�R�[�h�͈̔͊O
		if (w_flag == false)
		{
			jis &= 0x7F7F;
			// OASYS�r�� 0x2C21�`0x2C7E �͑S�p��
			if ((jis > 0x2C20) && (jis < 0x2C7F))
			{
				sjis = 0x2020;
				w_flag = true;
			}

			// ESC(?) �V�[�P���X
			switch (jis)
			{
			case 0x203F:	// AS-IS�Ŕ��p�X�y�[�X2��
				sjis = 0x2020;
				w_flag = true;
				break;
			case 0x2C66:	// AS-IS�őS�p��(?)
			case 0x2F22:	// AS-IS�őS�p��(?)
			case 0x2F2A:	// �g�啶���̌�ɕt���Ă���̂őS�p�X�y�[�X�ɕϊ�����
				sjis = 0x8140;
				w_flag = true;
				break;
			case 0x2F7D:
				// ����2�o�C�g�͐���R�[�h���ۂ��̂Ŗ�������
				oasys_ptr += 2;
				break;
			}
		}

		// �s�v�Ȑ���R�[�h�͖�������
		if (w_flag == false)
			continue;

		// �o�̓o�b�t�@�ɏ�������
		char s[2];
		s[0] = (sjis >> 8) & 0xFF;
		s[1] = sjis & 0xFF;
		OASYS::sjis_frame.data[sjis_ptr] = s[0]; sjis_ptr++;
		OASYS::sjis_frame.data[sjis_ptr] = s[1]; sjis_ptr++;
	}
	// LF�}��
	OASYS::sjis_frame.data[sjis_ptr] = 0x0A; sjis_ptr++;

	int attribute = 0;

	for (int i = 0; i < 5; i++)
	{
		 int val = OASYS::oasys_frame.attr[i] & 0xFF;
		 OASYS::sjis_frame.attr[i] = val;
		 attribute += val;
	}

	if (attribute == 0)
		// �ϊ��͐���ɏI�������B
		// attribute 5�r�b�g{0,0,0,0,0}�̏ꍇ�͎���OASYS�t���[��
		return 0;
	else if (attribute == 0x01)
		// �ϊ��͐���ɏI�������B
		// ���̃t���[����OASYS�ł͂Ȃ�
		return 0;
	else if (attribute == 0x81)
	{
		// �ϊ��͐���ɏI�������B
		// attribute 5�r�b�g{0x81,0,0,0,0}�̏ꍇ�͉��y�[�W
		// LF�}��
		OASYS::sjis_frame.data[sjis_ptr] = 0x0C; sjis_ptr++;
		return 0;
	}
	else if (attribute == 0x7D)
		// OASYS�t���[���ł͂Ȃ�
		return -1;

	return -1;
}

//***************************
// �u���b�N�P�ʂ�SJIS�ϊ�
//***************************
int OASYS::oasys_to_text() {

	const int data_length  = (OASYS::line_wchars * 2);		// �܂�Ԃ�����(char)
	const int frame_length = data_length + 5;	// +5 Byte��House Keeping

	int buf_ptr = 0;
	int out_ptr = 0;
	int lines = 0;

	while (buf_ptr < BLOCK)
	{
		// OASYS�t���[���P�ʂ�SJIS�ϊ�
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

		// �u���b�N�̎c��Ƀt���[��������X�y�[�X���Ȃ���΂����Ńu���b�N�̏������I������
		if ((BLOCK - buf_ptr) < frame_length)
			break;
	}
	return lines;
}

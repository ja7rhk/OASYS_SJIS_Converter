#include <iostream>
#include <string>
#include <stdio.h>
#include <mbstring.h>
#include <sstream>
#include <fstream>
#include "OASYS.h"

#define HEADER_BASE	512		// 0x200
#define OASYS_BASE	768		// 0x300
#define	PAGE_LINRS_INDEX  HEADER_BASE + 8	// 1ページ当たりの行数

OASYS oasys;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "コマンドライン引数が正しくありません。" << std::endl;
		return -1;
	}
	std::cout << "OASYS専用機の.oasファイルをS-JISに変換します。" << std::endl;

	// 入力ファイル名
	const std::string inputFile = argv[1];
	std::ifstream ifs(inputFile, std::ios::binary);
	if (!ifs)
	{
		std::cout << "入力ファイルをオープンできません。" << std::endl;
		return -1;
	}
	// 入力ファイル名のチェック
	std::string::size_type idx = inputFile.find_last_of('.');
	if (idx != std::string::npos) {
		//std::string name = inputFile.substr(0, idx);
		std::string ext = inputFile.substr(idx + 1);
		if (ext != "oas")
		{
			std::cout << "拡張子が.oasでありません。" << std::endl;
			return -1;
		}
	}
	else
	{
		std::cout << "拡張子なし" << std::endl;
		return -1;
	}
	std::cout << "input file = \"" << inputFile << "\"" << std::endl;

	//読込サイズを調べる。
	ifs.seekg(0, std::ios::end);
	const uint64_t size = ifs.tellg();
	ifs.seekg(0);

	//読み込んだデータをchar型に出力する
	char* data = new char[size];
	ifs.read(data, size);
	ifs.close();

	// 出力ファイル名
	std::string outputFile = inputFile.substr(0, idx) + ".txt";
	std::ofstream ofs(outputFile, std::ios_base::binary);
	if (!ofs)
	{
		std::cout << "出力ファイルをオープンできません。" << std::endl;
		return -1;
	}

	// OASYS本文を変換
	int num_byte = 0;
	int num_line = 0;
	int page_size = 0xFF;
	if (PAGE_LINRS_INDEX < size)
		page_size = data[PAGE_LINRS_INDEX];

	for (int i = OASYS_BASE; i < (size - 2); i++)
	{
		bool w_flag = false;	// word flag
		wchar_t sjis;

		char jis_high = data[i];
		char jis_low = data[i + 1];

		wchar_t jis = (jis_high << 8) + jis_low;

		// NULLコードを検出した場合の処理 (改行)
		if (jis_high == 0)
		{
			char post_jis_low = data[i + 2] & 0x7F;
			// NULLに続く2バイトがASCiiコードの範囲内か検査する
			if (oasys.is_ascii(jis_low & 0x7F) && oasys.is_ascii(post_jis_low))
			{
				// 次のバイトがASCIIまたはJISコードの場合はLFを挿入する
				ofs << "\n";
				num_byte++;
				num_line++;
				if (num_line == page_size)
				{
					ofs << "\n";
					ofs << "-----<< 改頁 >>-----\n";
					ofs << "\n";
					num_line = 0;
				}
				continue;
			}
			//else if ((low_byte < 0x20) || (low_byte > 0x74))
			else
			{
				// 次のバイトがASCIIまたはJISコード以外は無視する
				continue;
			}
		}

		// 半角英数の場合 (ASCIIコード)
		if ((jis & 0x8080) == 0x0080)
		{
			if (oasys.is_ascii(jis_high))
			{
				ofs.write((char*)(&jis_high), sizeof(char));
				num_byte++;
			}
			// 2バイト目が'_'の場合は半角詰めなので変換しない
			jis_low &= 0x7F;
			if (oasys.is_ascii(jis_low) && jis_low != 0x5F)
			{
				ofs.write((char*)(&jis_low), sizeof(char));
				num_byte++;
			}
			i++;
			continue;
		}

		// 半角カナまたは上付き文字の場合 (ASCIIコード)
		if ((jis & 0x8080) == 0x8080)
		{
			// 1文字目のOASYSコードをASCIIに変換
			Ascii_Kana kana = oasys.oasys_to_sjis(jis_high);
			ofs.write((char*)(&kana.c1), sizeof(char));
			num_byte++;
			if (kana.c2)
				ofs.write((char*)(&kana.c2), sizeof(char));

			// 2文字目のOASYSコードをASCIIに変換
			kana = oasys.oasys_to_sjis(jis_low);
			// 2バイト目が'_'の場合は半角詰めなので変換しない
			if (kana.c1 != 0x5F)
			{
				ofs.write((char*)(&kana.c1), sizeof(char));
				num_byte++;
			}
			if (kana.c2)
				ofs.write((char*)(&kana.c2), sizeof(char));
			i++;
			continue;
		}

		// 全角英数、かな
		if ((jis & 0x8080) == 0)
		{
			if (oasys.is_jis(jis))
			{
				sjis = oasys.jis2sjis(jis);
				w_flag = true;
			}
		}

		// 下線付きの全角英数、かな
		if ((jis & 0x8080) == 0x8000)
		{
			jis &= 0x7F7F;
			if (oasys.is_jis(jis))
			{
				sjis = oasys.jis2sjis(jis);
				w_flag = true;
			}
		}

		//JISコードの範囲外
		if (w_flag == false)
		{
			// ESC(?) シーケンス
			switch (jis)
			{
			case 0x203F:
				// AS-ISで半角スペース2個
				sjis = 0x2020;
				w_flag = true;
				break;
			case 0x2C66:
				// AS-ISで全角空白(?)
				sjis = 0x8140;
				w_flag = true;
				break;
			case 0x2F2A:
				// 拡大文字の後に付いているので全角スペースに変換する
				sjis = 0x8140;
				w_flag = true;
				break;
			case 0x2F7D:
				// 次の2バイトは制御コードっぽいので無視する
				i += 2;
				break;
			}
		}

		i++;

		// 不要な制御コードを削除
		if (w_flag == false)
			continue;

		// 出力ファイルに書き込む
		char s[2];
		s[0] = (sjis >> 8) & 0xFF;
		s[1] = sjis & 0xFF;
		ofs.write((char*)s, sizeof(char) * 2);
		num_byte += 2;
	}

	ofs.close();
	delete[] data;

	std::cout << "S-JISコードでテキストファイルに出力しました。" << std::endl;
	std::cout << "output file = \"" << outputFile << "\"" << std::endl;
	std::cout << num_byte << " bytes" << std::endl;
	std::cout << std::endl;

	return 0;
}

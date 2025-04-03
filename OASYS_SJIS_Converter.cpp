#include <iostream>
#include <string>
#include <stdio.h>
#include <mbstring.h>

#include <string>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>

// JISコードからSJISコードに変換
wchar_t jis2sjis(wchar_t jis) {

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
bool is_ascii(char ascii) {

	if (ascii >= 0x20 && ascii < 0x7F)
		return true;

	return false;
}

// JISコード範囲内かチェック
bool is_jis(wchar_t jis) {

	wchar_t jis_high = (jis >> 8) & 0xFF;
	wchar_t jis_low  = jis & 0xFF;

	// 全角英数、かな
	if ((jis_high >= 0x21 && jis_high <= 0x27) && (jis_low >= 0x21 && jis_low <= 0x7E))
		return true;

	// 漢字コード
	if ((jis_high >= 0x30 && jis_high <= 0x74) && (jis_low >= 0x21 && jis_low <= 0x7E))
		return true;

	return false;
}

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

	//読み込んだデータをwchar_t型に出力する
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

	int num_byte = 0;

	for (int i = 0; i < (size - 1); i++)
	{
		bool w_flag = false;	// word flag
		wchar_t sjis;

		char jis_high = data[i];
		char jis_low = data[i + 1];

		wchar_t jis = (jis_high << 8) + jis_low;

		// NULLコードを検出した場合の処理 (改行)
		if (jis_high == 0)
		{
			// 下線付きの場合はbit7が1になっているので両方チェックする
			char low_byte = jis_low & 0x7F;

			if ((low_byte < 0x20) || (low_byte > 0x74))
			{
				// 次のバイトがASCIIまたはJISコード以外は無視する
				continue;
			}
			else if ((low_byte >= 0x20) && (low_byte <= 0x74))
			{
				// 次のバイトがASCIIまたはJISコードの場合はLFを挿入する
				jis_low = 0x0A;
				ofs.write((char*)(&jis_low), sizeof(char));
				num_byte++;
				continue;
			}
		}

		// 半角英数の場合 (ASCIIコード)
		if ((jis & 0x8080) == 0x0080)
		{
			// ASCIIコード (2バイト目に0x80を足している)
			jis_low &= 0x7F;
			if (is_ascii(jis_low))
			{
				ofs.write((char*)(&jis_high), sizeof(char));
				num_byte++;
			}
			// 2バイト目が'_'の場合は半角詰めなので変換しない
			if (is_ascii(jis_low) && jis_low != 0x5F)
			{
				ofs.write((char*)(&jis_low), sizeof(char));
				num_byte++;
			}
			i++;
			continue;
		}

		// 上付き文字の場合 (ASCIIコード)
		if ((jis & 0x8080) == 0x8080)
		{
			// 半角カナ"･･"の場合はSJISの"･･"に変換する
			if ((jis == 0x8585))
			{
				sjis = 0xA5A5;	// "･･"
				w_flag = true;
			}
			// それ以外は半角英数と同じに扱う
			jis_high &= 0x7F;
			jis_low &= 0x7F;
			if (is_ascii(jis_low))
			{
				ofs.write((char*)(&jis_high), sizeof(char));
				num_byte++;
			}
			// 2バイト目が'_'の場合は半角詰めなので変換しない
			if (is_ascii(jis_low) && jis_low != 0x5F)
			{
				ofs.write((char*)(&jis_low), sizeof(char));
				num_byte++;
			}
			i++;
			continue;
		}

		// 全角英数、かな
		if ((jis & 0x8080) == 0)
		{
			if (is_jis(jis))
			{
				sjis = jis2sjis(jis);
				w_flag = true;
			}
		}

		// 下線付きの全角英数、かな
		if ((jis & 0x8080) == 0x8000)
		{
			jis &= 0x7F7F;
			if (is_jis(jis))
			{
				sjis = jis2sjis(jis);
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

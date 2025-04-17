#include <iostream>
#include <string>
#include <stdio.h>
#include <mbstring.h>
#include <sstream>
#include <fstream>
#include "OASYS.h"

#define HEADER_BASE	0x200		// 0x200
#define OASYS_BASE	0x300		// 

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

	// ファイルサイズを調べる。
	ifs.seekg(0, std::ios::end);
	const uint64_t size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	// 収容可能な文書ブロック数
	int blocks = (size - OASYS_BASE) / 0x400;

	// 出力ファイル名
	std::string outputFile = inputFile.substr(0, idx) + ".txt";
	std::ofstream ofs(outputFile, std::ios_base::binary);
	if (!ofs)
	{
		std::cout << "出力ファイルをオープンできません。" << std::endl;
		return -1;
	}

	// INDEXを読み込む
	//char index[0x100];
	Oasys_Index index;
	ifs.seekg(HEADER_BASE, std::ios::beg);
	//ifs.read(index, 0x100);
	ifs.read(&index.start, 0x100);

	// OASYS本文を変換
	int num_byte = 0;
	int num_line = 0;
	int page_lines = 0xFF;		// ライン数
	int line_wchar = 0x30;		// 行毎の文字数(2バイト文字)
	int online_blocks = 0;		// ブロック数(2バイト)
	int lines = 0;

	// 書式パラメータの読出し
	page_lines = index.page_lines;
	line_wchar = index.line_wchars;
	online_blocks = index.online_blocks;

	// ブロック数が正しいかチェック
	if (online_blocks > blocks)
		std::cout << "文書ブロック数が収容可能ブロック数を超えています。" << std::endl;

	// OASYS文書の読出し
	OASYS oasys(line_wchar);
	//OASYS oasys(fold_wchar);
	ifs.seekg(OASYS_BASE, std::ios::beg);
	int line_cntr = 0;

	for (int block = 0; block < online_blocks; block++)
	{
		oasys.clear_buf();
		oasys.clear_out();

		// BLOCKを読み込む
		ifs.read(oasys.buf, BLOCK);

		lines += oasys.oasys_to_text();

		for (int i = 0; i < BLOCK; i++)
		{
			char s = oasys.out[i];
			if (s == 0)	// NULL
				continue;
			else if (s == 0x0A)		// LF
			{
				ofs << "\n";
				line_cntr++;
				if ((line_cntr % page_lines) == 0)
				{
					ofs << "\n";
					ofs << "-----<< 改頁 >>-----\n";
					ofs << "\n";
					line_cntr = 0;
				}
			}
			else if (s == 0x0C)		// FF
			{
				ofs << "\n";
				ofs << "-----<< 改頁 >>-----\n";
				ofs << "\n";
				line_cntr = 0;
			}
			else
				// 通常のSJISコード
				ofs.write((char*)(&s), sizeof(char));
		}
	}

	ifs.close();
	ofs.close();

	std::cout << "SJISコードでテキストファイルに出力しました。" << std::endl;
	std::cout << "output file = \"" << outputFile << "\"" << std::endl;
	std::cout << std::dec << lines << " lines" << std::endl;
	std::cout << std::endl;

	return 0;
}

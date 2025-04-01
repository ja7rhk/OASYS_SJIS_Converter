# OASYS_SJIS_Converter

OASYS専用機の文書ファイル(.oas)を簡易的にシフトJISコードに変換してテキストファイル(.txt)を出力します。

## 使用方法

■コマンドプロンプトの例

	> OASYS_SJIS_Converter 123.oas

■バッチファイルの例

	@ECHO OFF
	ECHO Running OASYS_SJIS_Converter for %USERNAME% on %DATE% %TIME%

	OASYS_SJIS_Converter.exe sample.oas

	:: wait for the user to press a key before closing the window
	pause

## 制限事項

・OASYS 30AXで作成した文書ファイルのバックアップで確認しています。

・OASYS文書の書式が不明なので、先頭に意味不明なゴミが出ます。

・半角カナは全て'･'に変換しています。



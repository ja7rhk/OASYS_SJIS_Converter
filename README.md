# OASYS_SJIS_Converter

OASYS専用機の文書ファイル(.oas)を簡易的にシフトJISコードに変換してテキストファイル(.txt)を出力します。

## 使用方法

■コマンドプロンプトの例

	> OASYS_SJIS_Converter サンプル文書.oas

■バッチファイルの例

	@ECHO OFF
	ECHO Running OASYS_SJIS_Converter for %USERNAME% on %DATE% %TIME%

	OASYS_SJIS_Converter.exe sample.oas

	:: wait for the user to press a key before closing the window
	pause

## 制限事項

・半濁点付き半角カナはサンプルがないので無視します。

・バッチファイルでは日本語のファイル名を認識しません。



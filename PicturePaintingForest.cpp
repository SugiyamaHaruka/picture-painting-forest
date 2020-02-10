////////////////////////////////////////////////////////////////////////////////
//
//  SimpleTextChat.cpp
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  ヘッダファイル
//
#include <Windows.h>
#include <WinSock.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//
// 使用ライブラリ
//
#pragma comment(lib,"wsock32.lib")

////////////////////////////////////////////////////////////////////////////////
//
//  定数定義
//
#define WM_SOCKET       (WM_USER+1)     // ソケット用メッセージ
#define PORT            10000           // 通信ポート番号

#define IDB_CONNECT     1000            // [接続]ボタン
#define IDB_ACCEPT      1001            // [接続待ち]ボタン
#define IDB_REQUESTREJECT      1002            // [切断要請]ボタン
#define IDB_REJECT        1003            // [切断]ボタン
#define IDB_THICK1      1004
#define IDB_THICK2      1005
#define IDB_THICK3      1006
#define IDB_THICK4      1007
#define IDB_THICK5      1008
#define IDB_COLOR1      1009
#define IDB_COLOR2      1010
#define IDB_COLOR3      1011
#define IDB_COLOR4      1012
#define IDB_COLOR5      1013
#define IDB_COLOR6      1014
#define IDB_COLOR7      1015
#define IDB_COLOR8      1016
#define IDB_COLOR9      1017
#define IDB_SELECT      1018	//?
#define IDB_CLEAR       1019	//?
#define IDB_SEND        1020            //　送信
#define IDF_HOSTNAME    2000                // ホスト名入力エディットボックス
#define IDF_SENDMSG     2001            //　解答メッセージ入力用エディットボックス
#define IDF_RECVMSG     2002            //　解答メッセージ表示用エディットボックス


#define IDE_RECVMSG     3000            //　メッセージ受信イベント


#define WINDOW_W        1300        // ウィンドウの幅
#define WINDOW_H        800       // ウィンドウの高さ

#define MAX_ARRAY     10000         // 配列の最大要素数
#define MAX_MESSAGE   255 //文字列の最大数

////////////////////////////////////////////////////////////////////////////////
//
//  グローバル変数
//
LPCTSTR lpClassName = "TegakiChat";        // ウィンドウクラス名
LPCTSTR lpWindowName = "TegakiChat";        // タイトルバーにつく名前

SOCKET sock = INVALID_SOCKET;            // ソケット
SOCKET sv_sock = INVALID_SOCKET;            // サーバ用ソケット
HOSTENT* phe;                       // HOSTENT構造体

HPEN hPen, hPen2;  //ペン2本で両方向｜片方向のみで1本
const RECT d = { 25, 100, 800, 750 };                  // 描画領域(左上隅のx座標, 左上隅のy座標, 右下隅のx座標, 右下隅のy座標)

int n;                                              // カウンタ(自分用)
int flag[MAX_ARRAY];                                // ペンダウンフラグ(自分用)
POINT pos[MAX_ARRAY];                               // 座標を格納(自分用)

int n_e;                                              // カウンタ(相手用)
int flag_e[MAX_ARRAY];                                // ペンダウンフラグ(相手用)
POINT pos_e[MAX_ARRAY];                               // 座標を格納(相手用)

char color[10], thick[2], pen_info[11];        //色、太さ、太さと色
int nt, R, G, B, i2 = 0;                       //太さ、赤、緑、青、pen_infoのカウンタ
char clear[6] = "CLEAR";                      //全消し用メッセージ

int YouAreWriterOrAnswer;	//絵を描く人は、０。答えを送る人は、１。
char odai[100][13] = { "なし", "ぴーまん", "にんじん", "せろり", "とまと", "かぼちゃ",
"たまねぎ", "なす", "きゃべつ", "はくさい", "ぱぷりか", "りんご", "みかん", "ぶどう",
"ばなな", "もも", "すいか", "まんごー", "ようなし", "きうい", "すし", "らーめん", "ぴざ",
"かれー", "めろんぱん", "おにぎり", "おでん", "もち", "はんばーがー", "ぺきんだっく",
"すまほ", "てれび", "かめら", "こたつ", "ぱそこん", "まうす", "きーぼーど",
"ゆーえすびー", "でぃーえす", "すいっち", "にほん", "ふじさん", "かたな", "にんじゃ",
"しゅりけん", "さむらい", "さくら", "すかいつりー", "せんす", "ゆかた", "さっかー",
"やきゅう", "てにす", "たっきゅう", "すいえい", "ぼくしんぐ", "ばどみんとん",
"ごるふ", "スケート", "ばれーぼーる", "くるま", "ばす", "でんしゃ", "ひこうき",
"へりこぷたー", "ろけっと", "ふね", "ぱとかー", "せんすいかん", "とろっこ", "えんぴつ",
"けしごむ", "じょうぎ", "ぶんどき", "こんぱす", "はさみ", "ほちきす", "のり", "のーと",
"でんたく", "ねずみ", "うし", "とら", "うさぎ", "どらごん", "へび", "うま", "ひつじ",
"さる", "にわとり", "いぬ", "いのしし", "ぎたー", "ばいおりん", "とらんぺっと", "ぴあの",
"たんばりん", "まらかす", "かすたねっと", "どらむ" }; //おだい
CHAR ODAI[6];
int RANDOM;

////////////////////////////////////////////////////////////////////////////////
//
//  プロトタイプ宣言
//
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);   // ウィンドウ関数
BOOL SockInit(HWND hWnd);                               // ソケット初期化
BOOL SockAccept(HWND hWnd);                             // ソケット接続待ち
BOOL SockConnect(HWND hWnd, LPCSTR host);               // ソケット接続
void WindowInit(HWND hWnd);                             // ウィンドウ初期化
LRESULT CALLBACK OnPaint(HWND, UINT, WPARAM, LPARAM);  // 描画関数
void PClear();     //自分の座標リセット
void PClear_e();   //相手の座標リセット
void setData(int flag, int x, int y);                   // 描画情報を入れる
void setData_e(int flag2, int x2, int y2);
BOOL checkMousePos(int x, int y);           // マウスの位置がキャンパスの中かどうか判定する
void Clear(HWND); //全消し

////////////////////////////////////////////////////////////////////////////////
//
//  WinMain関数 (Windowsプログラム起動時に呼ばれる関数)
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;                                          // ウィンドウハンドル
	MSG  msg;                                           // メッセージ
	WNDCLASSEX wc;                                      // ウィンドウクラス

	//ウィンドウクラス定義
	wc.hInstance = hInstance;                       // インスタンス
	wc.lpszClassName = lpClassName;                     // クラス名
	wc.lpfnWndProc = WindowProc;                      // ウィンドウ関数名
	wc.style = 0;                               // クラススタイル
	wc.cbSize = sizeof(WNDCLASSEX);              // 構造体サイズ
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // アイコンハンドル
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);     // スモールアイコン
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // マウスポインタ
	wc.lpszMenuName = NULL;                            // メニュー(なし)
	wc.cbClsExtra = 0;                               // クラス拡張情報
	wc.cbWndExtra = 0;                               // ウィンドウ拡張情報
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;           // ウィンドウの背景色
	if (!RegisterClassEx(&wc)) return 0;                // ウィンドウクラス登録

	// ウィンドウ生成
	hWnd = CreateWindow(
		lpClassName,                                // ウィンドウクラス名
		lpWindowName,                               // ウィンドウ名
		WS_DLGFRAME | WS_VISIBLE | WS_SYSMENU,          // ウィンドウ属性
		CW_USEDEFAULT,                              // ウィンドウ表示位置(X)
		CW_USEDEFAULT,                              // ウィンドウ表示位置(Y)
		WINDOW_W,                                   // ウィンドウサイズ(X)
		WINDOW_H,                                   // ウィンドウサイズ(Y)
		HWND_DESKTOP,                               // 親ウィンドウハンドル
		NULL,
		hInstance,                                  // インスタンスハンドル
		NULL
	);

	// ウィンドウ表示
	ShowWindow(hWnd, nCmdShow);                         // ウィンドウ表示モード
	UpdateWindow(hWnd);                                 // ウインドウ更新

	// メッセージループ
	while (GetMessage(&msg, NULL, 0, 0)) {                // メッセージを取得
		TranslateMessage(&msg);
		DispatchMessage(&msg);                          // メッセージ送る
	}
	return (int)msg.wParam;                             // プログラム終了
}

////////////////////////////////////////////////////////////////////////////////
//
//  ウィンドウ関数(イベント処理を記述)
//
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wP, LPARAM lP)
{
	static HWND hWndHost;                       // ホスト名入力用エディットボックス
	static HWND hWndConnect, hWndAccept;                // [接続]ボタンと[接続待ち]ボタン
	static HWND hWndRequestreject;                     // [切断要請]ボタン
	static HWND hWndReject;                       // [切断]ボタン
	static HWND hWndSendMSG;                               //解答入力用エディットボックス
	static HWND hWndRecvMSG;                               //解答表示用エディットボックス
	static HWND hWndOdaiMSG;				//お題を書く用のエディットボックス
	static HWND hWndThick1;                      //[太さ]ボタン
	static HWND hWndThick2;
	static HWND hWndThick3;
	static HWND hWndThick4;
	static HWND hWndThick5;
	static HWND hWndColor1;
	static HWND hWndColor2;
	static HWND hWndColor3;
	static HWND hWndColor4;
	static HWND hWndColor5;
	static HWND hWndColor6;
	static HWND hWndColor7;
	static HWND hWndColor8;
	static HWND hWndColor9;
	static HWND hWndSelect;
	static HWND hWndClear;
	static HWND hWndSend;                                  //[送信]ボタン

	static BOOL mouseFlg = FALSE;       // 前回の状態 TRUE:描画した、FALSE:描画していない

	switch (uMsg) {
	case WM_CREATE:     // ウィンドウが生成された
		// 文字列表示
		CreateWindow("static", "Host Name", WS_CHILD | WS_VISIBLE, 25, 10, 100, 18, hWnd, NULL, NULL, NULL);
		CreateWindow("static", "お題", WS_CHILD | WS_VISIBLE, 500
			, 10, 100, 18, hWnd, NULL, NULL, NULL);
		CreateWindow("static", "色", WS_CHILD | WS_VISIBLE, 820, 420, 40, 30, hWnd, NULL, NULL, NULL);
		CreateWindow("static", "太さ", WS_CHILD | WS_VISIBLE, 820, 520, 40, 30, hWnd, NULL, NULL, NULL);
		CreateWindow("static", "解答入力", WS_CHILD | WS_VISIBLE, 820, 100, 100, 18, hWnd, NULL, NULL, NULL);
		CreateWindow("static", "相手の解答", WS_CHILD | WS_VISIBLE, 820, 250, 100, 18, hWnd, NULL, NULL, NULL);
		//ホスト名入力用エディットボックス
		hWndHost = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE, 25, 30, 200, 25, hWnd, (HMENU)IDF_HOSTNAME, NULL, NULL);

		//[接続]ボタン
		hWndConnect = CreateWindow("button", "接続", WS_CHILD | WS_VISIBLE, 235, 30, 50, 25, hWnd, (HMENU)IDB_CONNECT, NULL, NULL);

		//[接続待ち]ボタン
		hWndAccept = CreateWindow("button", "接続待ち", WS_CHILD | WS_VISIBLE, 295, 30, 90, 25, hWnd, (HMENU)IDB_ACCEPT, NULL, NULL);

		//[切断要請]ボタン
		hWndRequestreject = CreateWindow("button", "切断要請", WS_CHILD | WS_VISIBLE | WS_DISABLED, 295, 60, 90, 25, hWnd, (HMENU)IDB_REQUESTREJECT, NULL, NULL);

		//[切断]ボタン
		hWndReject = CreateWindow("button", "切断", WS_CHILD | WS_VISIBLE | WS_DISABLED, 235, 60, 50, 25, hWnd, (HMENU)IDB_REJECT, NULL, NULL);

		//解答入力用エディットボックス
		hWndSendMSG = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_DISABLED, 825, 130, 300, 50, hWnd, (HMENU)IDF_SENDMSG, NULL, NULL);

		//解答表示用エディットボックス
		hWndRecvMSG = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY, 825, 280, 300, 50, hWnd, (HMENU)IDF_RECVMSG, NULL, NULL);

		//お題表示用エディットボックス
		hWndOdaiMSG = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_DISABLED, 500, 30, 300, 25, hWnd, (HMENU)IDF_SENDMSG, NULL, NULL);

		//[色]ボタン
		hWndColor1 = CreateWindow("button", "黒", WS_CHILD | WS_VISIBLE | WS_DISABLED, 820, 450, 30, 30, hWnd, (HMENU)IDB_COLOR1, NULL, NULL);
		hWndColor2 = CreateWindow("button", "赤", WS_CHILD | WS_VISIBLE | WS_DISABLED, 860, 450, 30, 30, hWnd, (HMENU)IDB_COLOR2, NULL, NULL);
		hWndColor3 = CreateWindow("button", "青", WS_CHILD | WS_VISIBLE | WS_DISABLED, 900, 450, 30, 30, hWnd, (HMENU)IDB_COLOR3, NULL, NULL);
		hWndColor4 = CreateWindow("button", "緑", WS_CHILD | WS_VISIBLE | WS_DISABLED, 940, 450, 30, 30, hWnd, (HMENU)IDB_COLOR4, NULL, NULL);
		hWndColor5 = CreateWindow("button", "黄", WS_CHILD | WS_VISIBLE | WS_DISABLED, 980, 450, 30, 30, hWnd, (HMENU)IDB_COLOR5, NULL, NULL);
		hWndColor6 = CreateWindow("button", "橙", WS_CHILD | WS_VISIBLE | WS_DISABLED, 1020, 450, 30, 30, hWnd, (HMENU)IDB_COLOR6, NULL, NULL);
		hWndColor7 = CreateWindow("button", "紫", WS_CHILD | WS_VISIBLE | WS_DISABLED, 1060, 450, 30, 30, hWnd, (HMENU)IDB_COLOR7, NULL, NULL);
		hWndColor8 = CreateWindow("button", "水色", WS_CHILD | WS_VISIBLE | WS_DISABLED, 1100, 450, 40, 30, hWnd, (HMENU)IDB_COLOR8, NULL, NULL);
		hWndColor9 = CreateWindow("button", "黄緑", WS_CHILD | WS_VISIBLE | WS_DISABLED, 1150, 450, 40, 30, hWnd, (HMENU)IDB_COLOR9, NULL, NULL);

		//[太さ]ボタン
		hWndThick1 = CreateWindow("button", "1", WS_CHILD | WS_VISIBLE | WS_DISABLED, 820, 550, 30, 30, hWnd, (HMENU)IDB_THICK1, NULL, NULL);
		hWndThick2 = CreateWindow("button", "2", WS_CHILD | WS_VISIBLE | WS_DISABLED, 860, 550, 30, 30, hWnd, (HMENU)IDB_THICK2, NULL, NULL);
		hWndThick3 = CreateWindow("button", "3", WS_CHILD | WS_VISIBLE | WS_DISABLED, 900, 550, 30, 30, hWnd, (HMENU)IDB_THICK3, NULL, NULL);
		hWndThick4 = CreateWindow("button", "4", WS_CHILD | WS_VISIBLE | WS_DISABLED, 940, 550, 30, 30, hWnd, (HMENU)IDB_THICK4, NULL, NULL);
		hWndThick5 = CreateWindow("button", "5", WS_CHILD | WS_VISIBLE | WS_DISABLED, 980, 550, 30, 30, hWnd, (HMENU)IDB_THICK5, NULL, NULL);

		//[クリア]ボタン
		hWndClear = CreateWindow("button", "全消し", WS_CHILD | WS_VISIBLE | WS_DISABLED, 820, 700, 100, 50, hWnd, (HMENU)IDB_CLEAR, NULL, NULL);

		//[確定]ボタン
		hWndSelect = CreateWindow("button", "確定", WS_CHILD | WS_VISIBLE | WS_DISABLED, 980, 600, 50, 85, hWnd, (HMENU)IDB_SELECT, NULL, NULL);

		//[送信]ボタン
		hWndSend = CreateWindow("button", "送信", WS_CHILD | WS_VISIBLE | WS_DISABLED, 1060, 210, 50, 50, hWnd, (HMENU)IDB_SEND, NULL, NULL);

		hPen = (HPEN)CreatePen(PS_SOLID, 3, RGB(0, 0, 0));  //黒で初期化
		hPen2 = (HPEN)CreatePen(PS_SOLID, 3, RGB(0, 0, 0));  //黒で初期化
		SetFocus(hWndHost);     //フォーカス指定
		SockInit(hWnd);         // ソケット初期化

		return 0L;
	case WM_LBUTTONDOWN:    // マウス左ボタンが押された
		if (YouAreWriterOrAnswer == 1) {//答える人
			return 0L;
		}
		char buf[8];
		if (checkMousePos(LOWORD(lP), HIWORD(lP))) {   // 描画領域の中なら
			setData(0, LOWORD(lP), HIWORD(lP));       // 線の始点として座標を記録
			sprintf_s(buf, 8, "%1d%03d%03d", 0, LOWORD(lP), HIWORD(lP));
			if (send(sock, buf, strlen(buf) + 1, 0) == SOCKET_ERROR) {    // 送信処理
						// 送信に失敗したらエラーを表示
				MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"),
					MB_OK | MB_ICONEXCLAMATION);
			}
			n++;
			InvalidateRect(hWnd, &d, FALSE);
			mouseFlg = TRUE;
		}
		else {
			mouseFlg = FALSE;
		}
		return 0L;
	case WM_MOUSEMOVE:  // マウスポインタが移動した
		if (YouAreWriterOrAnswer == 1) {//答える人
			return 0L;
		}
		if (wP == MK_LBUTTON) {  // 左ボタンが押されている
			if (checkMousePos(LOWORD(lP), HIWORD(lP))) {   // 描画領域の中なら
				if (mouseFlg) {                   // 前回描画しているなら
					setData(1, LOWORD(lP), HIWORD(lP));       // 線の途中として座標を記録
					sprintf_s(buf, 8, "%1d%03d%03d", 1, LOWORD(lP), HIWORD(lP));
					if (send(sock, buf, strlen(buf) + 1, 0) == SOCKET_ERROR) {    // 送信処理
						// 送信に失敗したらエラーを表示
						MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"),
							MB_OK | MB_ICONEXCLAMATION);
					}
				}
				else {                     // 前回描画していないなら
					setData(0, LOWORD(lP), HIWORD(lP));       // 線の始点として座標を記録
					sprintf_s(buf, 8, "%1d%03d%03d", 0, LOWORD(lP), HIWORD(lP));
					if (send(sock, buf, strlen(buf), 0) == SOCKET_ERROR) {    // 送信処理
						// 送信に失敗したらエラーを表示
						MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"),
							MB_OK | MB_ICONEXCLAMATION);
					}
				}
				mouseFlg = TRUE;
				n++;
				InvalidateRect(hWnd, &d, FALSE);
			}
			else {                     // 描画領域の外なら
				mouseFlg = FALSE;
			}
		}

		return 0L;
	case WM_PAINT:      // 再描画
		return OnPaint(hWnd, uMsg, wP, lP);

		return 0L;
	case WM_COMMAND:    // ボタンが押された
		switch (LOWORD(wP)) {
			//太さボタンが押された
		case IDB_THICK1:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			nt = 1;
			sprintf_s(thick, "%1d", nt);
			return 0L;

		case IDB_THICK2:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			nt = 2;
			sprintf_s(thick, "%1d", nt);
			return 0L;

		case IDB_THICK3:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			nt = 3;
			sprintf_s(thick, "%1d", nt);
			return 0L;

		case IDB_THICK4:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			nt = 4;
			sprintf_s(thick, "%1d", nt);
			return 0L;

		case IDB_THICK5:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			nt = 5;
			sprintf_s(thick, "%1d", nt);
			return 0L;
			/* end of switch (LOWORD(wP)) */
			return 0L;

			//色ボタンが押された
		case IDB_COLOR1:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 0, 0, 0);
			return 0L;
		case IDB_COLOR2:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 255, 0, 0);
			return 0L;
		case IDB_COLOR3:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 0, 0, 255);
			return 0L;
		case IDB_COLOR4:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 0, 255, 0);
			return 0L;
		case IDB_COLOR5:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 255, 255, 0);
			return 0L;
		case IDB_COLOR6:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 255, 102, 0);
			return 0L;
		case IDB_COLOR7:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 153, 0, 153);
			return 0L;
		case IDB_COLOR8:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 153, 255, 255);
			return 0L;
		case IDB_COLOR9:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			sprintf_s(color, 10, "%03d%03d%03d", 102, 255, 0);
			return 0L;

			//確定ボタンが押された
		case IDB_SELECT:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			PClear();            //座標リセット
			DeleteObject(hPen);  //ぺん削除
			InvalidateRect(hWnd, &d, FALSE);  //再描画
			sscanf_s(color, "%3d%3d%3d", &R, &G, &B);
			hPen = (HPEN)CreatePen(PS_SOLID, nt, RGB(R, G, B));  //違う色のぺん生成

			//太さと色の情報を相手に送る配列に入れる
			pen_info[i2] = thick[i2];
			for (i2 = 1; i2 < 11; i2++) {
				pen_info[i2] = color[i2 - 1];
			}
			i2 = 0;

			if (send(sock, pen_info, strlen(pen_info) + 1, 0) == SOCKET_ERROR) {    // 送信処理
							// 送信に失敗したらエラーを表示
				MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"),
					MB_OK | MB_ICONEXCLAMATION);
			}
			return 0L;

			//全消しボタンが押された
		case IDB_CLEAR:
			if (YouAreWriterOrAnswer == 1) {//答える人
				return 0L;
			}
			//全消しメッセージ送信
			if (send(sock, clear, strlen(clear) + 1, 0) == SOCKET_ERROR) {    // 送信処理
							// 送信に失敗したらエラーを表示
				MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"),
					MB_OK | MB_ICONEXCLAMATION);
			}
			Clear(hWnd);
			return 0L;



		case IDB_ACCEPT:    // [接続待ち]ボタン押下(サーバー)
			if (SockAccept(hWnd)) {  // 接続待ち要求
				return 0L;      // 接続待ち失敗
			}
			EnableWindow(hWndHost, FALSE);    // [HostName]無効
			EnableWindow(hWndConnect, FALSE); // [接続]    無効
			EnableWindow(hWndAccept, FALSE);  // [接続待ち]無効
			EnableWindow(hWndReject, TRUE);   // [切断]    有効
			EnableWindow(hWndRequestreject, TRUE);   // [切断要請]    有効
			YouAreWriterOrAnswer = 0;//絵を描く人

			return 0L;

		case IDB_CONNECT:   // [接続]ボタン押下(クライアント)
			char host[100];
			GetWindowText(hWndHost, host, sizeof(host));

			if (SockConnect(hWnd, host)) {   // 接続要求
				SetFocus(hWndHost);     // 接続失敗
				return 0L;
			}
			EnableWindow(hWndHost, FALSE);    // [HostName]無効
			EnableWindow(hWndConnect, FALSE); // [接続]    無効
			EnableWindow(hWndAccept, FALSE);  // [接続待ち]無効
			EnableWindow(hWndReject, TRUE);   // [切断]    有効
			EnableWindow(hWndRequestreject, TRUE);   // [切断要請]    有効
			EnableWindow(hWndColor1, TRUE);
			EnableWindow(hWndColor2, TRUE);
			EnableWindow(hWndColor3, TRUE);
			EnableWindow(hWndColor4, TRUE);
			EnableWindow(hWndColor5, TRUE);
			EnableWindow(hWndColor6, TRUE);
			EnableWindow(hWndColor7, TRUE);
			EnableWindow(hWndColor8, TRUE);
			EnableWindow(hWndColor9, TRUE);
			EnableWindow(hWndThick1, TRUE);
			EnableWindow(hWndThick2, TRUE);
			EnableWindow(hWndThick3, TRUE);
			EnableWindow(hWndThick4, TRUE);
			EnableWindow(hWndThick5, TRUE);
			EnableWindow(hWndSendMSG, TRUE);
			EnableWindow(hWndRecvMSG, TRUE);
			EnableWindow(hWndSelect, TRUE);
			EnableWindow(hWndClear, TRUE);
			EnableWindow(hWndSend, TRUE);
			YouAreWriterOrAnswer = 1;//答える人
			return 0L;

		case IDB_REJECT:    // [切断]ボタン押下
			if (sock != INVALID_SOCKET) {    // 自分がクライアント側なら
				// ソケットを閉じる
				closesocket(sock);
				sock = INVALID_SOCKET;
			}
			if (sv_sock != INVALID_SOCKET) { // 自分がサーバ側なら
				// サーバ用ソケットを閉じる
				closesocket(sv_sock);
				sv_sock = INVALID_SOCKET;
			}
			phe = NULL;
			EnableWindow(hWndHost, TRUE);       // [HostName]有効
			EnableWindow(hWndConnect, TRUE);    // [接続]    有効
			EnableWindow(hWndAccept, TRUE);     // [接続待ち]有効
			EnableWindow(hWndReject, FALSE);    // [切断]    無効
			EnableWindow(hWndRequestreject, FALSE);    // [切断要請]    無効
			EnableWindow(hWndColor1, FALSE);
			EnableWindow(hWndColor2, FALSE);
			EnableWindow(hWndColor3, FALSE);
			EnableWindow(hWndColor4, FALSE);
			EnableWindow(hWndColor5, FALSE);
			EnableWindow(hWndColor6, FALSE);
			EnableWindow(hWndColor7, FALSE);
			EnableWindow(hWndColor8, FALSE);
			EnableWindow(hWndColor9, FALSE);
			EnableWindow(hWndThick1, FALSE);
			EnableWindow(hWndThick2, FALSE);
			EnableWindow(hWndThick3, FALSE);
			EnableWindow(hWndThick4, FALSE);
			EnableWindow(hWndThick5, FALSE);
			EnableWindow(hWndSendMSG, FALSE);
			EnableWindow(hWndRecvMSG, FALSE);
			EnableWindow(hWndSelect, FALSE);
			EnableWindow(hWndClear, FALSE);
			EnableWindow(hWndSend, FALSE);
			SetFocus(hWndHost);         // フォーカス指定
			return 0L;

		case IDB_REQUESTREJECT:      // [切断要請]ボタン押下
			char buf[MAX_ARRAY];                  // 送信内容を一時的に格納するバッファ
			sprintf_s(buf, "REJECT");// REJECT を送信
			if (send(sock, buf, strlen(buf) + 1, 0) == SOCKET_ERROR) {    // 送信処理
						// 送信に失敗したらエラーを表示
				MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"),
					MB_OK | MB_ICONEXCLAMATION);
			}
			return 0L;
		case IDB_SEND: //送信ボタン押下
			if (YouAreWriterOrAnswer == 0) {//絵を書く人
				return 0L;
			}
			else {
				char bufsend[100];
				GetWindowText(hWndSendMSG, bufsend + 1, sizeof(bufsend) - 2);
				bufsend[0] = 'M';
				if (send(sock, bufsend, strlen(bufsend) + 1, 0) == SOCKET_ERROR) { //送信処理
						//送信に失敗したらエラーを表示
					MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
				}
				SetWindowText(hWndSendMSG, TEXT(""));    // 送信メッセージ入力用エディットボックスを空にする
				return 0L;
			}
		} /* end of switch (LOWORD(wP)) */
		return 0L;

	case WM_SOCKET:          // 非同期処理メッセージ
		if (WSAGETSELECTERROR(lP) != 0) { return 0L; }

		switch (WSAGETSELECTEVENT(lP)) {
		case FD_ACCEPT:     // 接続待ち完了通知
		{
			SOCKADDR_IN cl_sin;
			int len = sizeof(cl_sin);
			sock = accept(sv_sock, (LPSOCKADDR)&cl_sin, &len);

			if (sock == INVALID_SOCKET) {
				MessageBox(hWnd, "Accepting connection failed",
					"Error", MB_OK | MB_ICONEXCLAMATION);
				closesocket(sv_sock);
				sv_sock = INVALID_SOCKET;
				EnableWindow(hWndHost, TRUE);       // [HostName]有効
				EnableWindow(hWndConnect, TRUE);    // [接続]    有効
				EnableWindow(hWndAccept, TRUE);     // [接続待ち]有効
				EnableWindow(hWndReject, FALSE);    // [切断]    無効
				EnableWindow(hWndRequestreject, FALSE);    // [切断要請]    無効
				EnableWindow(hWndColor1, FALSE);
				EnableWindow(hWndColor2, FALSE);
				EnableWindow(hWndColor3, FALSE);
				EnableWindow(hWndColor4, FALSE);
				EnableWindow(hWndColor5, FALSE);
				EnableWindow(hWndColor6, FALSE);
				EnableWindow(hWndColor7, FALSE);
				EnableWindow(hWndColor8, FALSE);
				EnableWindow(hWndColor9, FALSE);
				EnableWindow(hWndThick1, FALSE);
				EnableWindow(hWndThick2, FALSE);
				EnableWindow(hWndThick3, FALSE);
				EnableWindow(hWndThick4, FALSE);
				EnableWindow(hWndThick5, FALSE);
				EnableWindow(hWndSendMSG, FALSE);
				EnableWindow(hWndRecvMSG, FALSE);
				EnableWindow(hWndSelect, FALSE);
				EnableWindow(hWndClear, FALSE);
				EnableWindow(hWndSend, FALSE);
				SetFocus(hWndHost);         // フォーカス指定
				return 0L;
			}

#ifndef NO_DNS
			// ホスト名取得
			phe = gethostbyaddr((char*)&cl_sin.sin_addr, 4, AF_INET);
			if (phe) { SetWindowText(hWndHost, phe->h_name); }
#endif  NO_DNS

			// 非同期モード (受信＆切断）
			if (WSAAsyncSelect(sock, hWnd, WM_SOCKET, FD_READ | FD_CLOSE)
				== SOCKET_ERROR) {
				// 接続に失敗したら初期状態に戻す
				MessageBox(hWnd, "WSAAsyncSelect() failed",
					"Error", MB_OK | MB_ICONEXCLAMATION);
				EnableWindow(hWndHost, TRUE);       // [HostName]有効
				EnableWindow(hWndConnect, TRUE);    // [接続]    有効
				EnableWindow(hWndAccept, TRUE);     // [接続待ち]有効
				EnableWindow(hWndReject, FALSE);    // [切断]    無効
				EnableWindow(hWndRequestreject, FALSE);    // [切断要請]    無効
				EnableWindow(hWndColor1, FALSE);
				EnableWindow(hWndColor2, FALSE);
				EnableWindow(hWndColor3, FALSE);
				EnableWindow(hWndColor4, FALSE);
				EnableWindow(hWndColor5, FALSE);
				EnableWindow(hWndColor6, FALSE);
				EnableWindow(hWndColor7, FALSE);
				EnableWindow(hWndColor8, FALSE);
				EnableWindow(hWndColor9, FALSE);
				EnableWindow(hWndThick1, FALSE);
				EnableWindow(hWndThick2, FALSE);
				EnableWindow(hWndThick3, FALSE);
				EnableWindow(hWndThick4, FALSE);
				EnableWindow(hWndThick5, FALSE);
				EnableWindow(hWndSendMSG, FALSE);
				EnableWindow(hWndRecvMSG, FALSE);
				EnableWindow(hWndSelect, FALSE);
				EnableWindow(hWndClear, FALSE);
				EnableWindow(hWndSend, FALSE);
				SetFocus(hWndHost);         // フォーカス指定
				return 0L;
			}
			EnableWindow(hWndColor1, TRUE);
			EnableWindow(hWndColor2, TRUE);
			EnableWindow(hWndColor3, TRUE);
			EnableWindow(hWndColor4, TRUE);
			EnableWindow(hWndColor5, TRUE);
			EnableWindow(hWndColor6, TRUE);
			EnableWindow(hWndColor7, TRUE);
			EnableWindow(hWndColor8, TRUE);
			EnableWindow(hWndColor9, TRUE);
			EnableWindow(hWndThick1, TRUE);
			EnableWindow(hWndThick2, TRUE);
			EnableWindow(hWndThick3, TRUE);
			EnableWindow(hWndThick4, TRUE);
			EnableWindow(hWndThick5, TRUE);
			EnableWindow(hWndSendMSG, TRUE);
			EnableWindow(hWndRecvMSG, TRUE);
			EnableWindow(hWndSelect, TRUE);
			EnableWindow(hWndClear, TRUE);
			EnableWindow(hWndSend, TRUE);
			SetFocus(hWnd);          // フォーカス指定
			if (YouAreWriterOrAnswer == 0) {//絵を描く人
				srand((unsigned)time(NULL));
				RANDOM = rand() % 100;
				SetWindowText(hWndOdaiMSG, odai[RANDOM]);

			}
			return 0L;
		}/* end of case FD_ACCEPT: */

		case FD_CONNECT:    // 接続完了通知
			// 非同期モード (受信＆切断)
			if (WSAAsyncSelect(sock, hWnd, WM_SOCKET, FD_READ | FD_CLOSE)
				== SOCKET_ERROR) {
				// 接続に失敗したら初期状態に戻す
				MessageBox(hWnd, "WSAAsyncSelect() failed",
					"Error", MB_OK | MB_ICONEXCLAMATION);
				EnableWindow(hWndHost, TRUE);       // [HostName]有効
				EnableWindow(hWndConnect, TRUE);    // [接続]    有効
				EnableWindow(hWndAccept, TRUE);     // [接続待ち]有効
				EnableWindow(hWndReject, FALSE);    // [切断]    無効
				EnableWindow(hWndRequestreject, FALSE);    // [切断要請]    無効
				EnableWindow(hWndColor1, FALSE);
				EnableWindow(hWndColor2, FALSE);
				EnableWindow(hWndColor3, FALSE);
				EnableWindow(hWndColor4, FALSE);
				EnableWindow(hWndColor5, FALSE);
				EnableWindow(hWndColor6, FALSE);
				EnableWindow(hWndColor7, FALSE);
				EnableWindow(hWndColor8, FALSE);
				EnableWindow(hWndColor9, FALSE);
				EnableWindow(hWndThick1, FALSE);
				EnableWindow(hWndThick2, FALSE);
				EnableWindow(hWndThick3, FALSE);
				EnableWindow(hWndThick4, FALSE);
				EnableWindow(hWndThick5, FALSE);
				EnableWindow(hWndSendMSG, FALSE);
				EnableWindow(hWndRecvMSG, FALSE);
				EnableWindow(hWndSelect, FALSE);
				EnableWindow(hWndClear, FALSE);
				EnableWindow(hWndSend, FALSE);
				SetFocus(hWndHost);         // フォーカス指定
				return 0L;
			}
			if (YouAreWriterOrAnswer == 0) {//絵を描く人
				srand((unsigned)time(NULL));
				RANDOM = rand() % 100;
				SetWindowText(hWndOdaiMSG, odai[RANDOM]);
			}
			SetFocus(hWnd);          // フォーカス指定
			return 0L;

		case FD_READ:       //メッセージ受信
			char bufrec[100];                  // 受信内容を一時的に格納するバッファ
			int flag2, x2, y2, thick2, R2, G2, B2;
			if (recv(sock, bufrec, sizeof(bufrec), 0) != SOCKET_ERROR) { // 受信できたなら
				
				if (strcmp(bufrec, "REJECT") == 0) {
					MessageBox(hWnd, "切断要請が届きました。",
						"Information", MB_OK | MB_ICONINFORMATION);
				}
				else if (bufrec[0] == 'M') {//相手からの回答であることを判定
					SetWindowText(hWndRecvMSG, bufrec + 1);        // 受信メッセージ表示用エディットボックスに受信内容を貼り付け
					if (strcmp(bufrec + 1, odai[RANDOM]) == 0) {//正解だった場合
						MessageBox(hWnd, "正解です！",
							"正解", MB_OK | MB_ICONEXCLAMATION);
						sprintf_s(buf, "Seikai");
						if (send(sock, buf, strlen(buf) + 1, 0) == SOCKET_ERROR) {    // 送信処理
									// 送信に失敗したらエラーを表示
							MessageBox(hWnd, TEXT("sending failed"), TEXT("Error"),
								MB_OK | MB_ICONEXCLAMATION);
						}
						YouAreWriterOrAnswer = 1;//答える側になる
						SetWindowText(hWndOdaiMSG, "");        // お題表示用エディットボックスに受信内容を貼り付け
						Clear(hWnd);
						OnPaint(hWnd, uMsg, wP, lP);

					}
				}
				else if (strcmp(bufrec, "Seikai") == 0) {
					MessageBox(hWnd, "正解です！",
						"正解", MB_OK | MB_ICONEXCLAMATION);
					YouAreWriterOrAnswer = 0;//書く側になる
					Clear(hWnd);
					srand((unsigned)time(NULL));
					RANDOM = rand() % 100;
					SetWindowText(hWndOdaiMSG, odai[RANDOM]);
					OnPaint(hWnd, uMsg, wP, lP);
				}
				else if (strlen(bufrec) == 10) {
					PClear_e();  //相手の座標リセット
					DeleteObject(hPen2); //ぺん削除
					InvalidateRect(hWnd, &d, FALSE);
					sscanf_s(bufrec, "%1d%3d%3d%3d", &thick2, &R2, &G2, &B2);
					hPen2 = (HPEN)CreatePen(PS_SOLID, thick2, RGB(R2, G2, B2)); //受け取った太さと色をもとにぺん生成
					OnPaint(hWnd, uMsg, wP, lP);

				}
				else if (strlen(bufrec) == 7) {
					sscanf_s(bufrec, "%1d%3d%3d", &flag2, &x2, &y2);
					setData_e(flag2, x2, y2);
					n_e++;
					InvalidateRect(hWnd, &d, FALSE);
					OnPaint(hWnd, uMsg, wP, lP);
				}
				else if (strcmp(bufrec, "CLEAR") == 0) {
					Clear(hWnd);
					OnPaint(hWnd, uMsg, wP, lP);
				}
				OnPaint(hWnd, uMsg, wP, lP);
			}
			return 0L;
		
		case FD_CLOSE:      // 切断された
			MessageBox(hWnd, "切断されました。",
				"Information", MB_OK | MB_ICONINFORMATION);
			SendMessage(hWnd, WM_COMMAND, IDB_REJECT, 0); // 切断処理発行
			Clear(hWnd);
			return 0L;
		}/* end of switch (WSAGETSELECTEVENT(lP)) */
		return 0L;
	case WM_SETFOCUS:   // ウィンドウにフォーカスが来たら
		// ホスト名入力欄が入力可ならフォーカス
		// 不可なら送信メッセージ入力欄にフォーカス
		SetFocus(IsWindowEnabled(hWndHost) ? hWndHost : hWnd);
		return 0L;
	case WM_DESTROY:    // ウィンドウが破棄された
		DeleteObject(hPen);
		DeleteObject(hPen2);
		closesocket(sock);
		PostQuitMessage(0);
		return 0L;
	default:
		return DefWindowProc(hWnd, uMsg, wP, lP);  // 標準メッセージ処理
	}/* end of switch (uMsg) */
}

////////////////////////////////////////////////////////////////////////////////
//
//  ソケット初期化処理
//
BOOL SockInit(HWND hWnd)
{
	WSADATA wsa;
	int ret;
	char ret_buf[80];

	ret = WSAStartup(MAKEWORD(1, 1), &wsa);

	if (ret != 0) {
		wsprintf(ret_buf, "%d is the err", ret);
		MessageBox(hWnd, ret_buf, "Error", MB_OK | MB_ICONSTOP);
		exit(-1);
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
//  ソケット接続 (クライアント側)
//
BOOL SockConnect(HWND hWnd, LPCSTR host)
{
	SOCKADDR_IN cl_sin; // SOCKADDR_IN構造体

	// ソケットを開く
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {        // ソケット作成失敗
		MessageBox(hWnd, "Socket() failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}

	memset(&cl_sin, 0x00, sizeof(cl_sin)); // 構造体初期化
	cl_sin.sin_family = AF_INET;           // インターネット
	cl_sin.sin_port = htons(PORT);       // ポート番号指定

	phe = gethostbyname(host); // アドレス取得

	if (phe == NULL) {
		MessageBox(hWnd, "gethostbyname() failed.",
			"Error", MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}
	memcpy(&cl_sin.sin_addr, phe->h_addr, phe->h_length);

	// 非同期モード (接続)
	if (WSAAsyncSelect(sock, hWnd, WM_SOCKET, FD_CONNECT) == SOCKET_ERROR) {
		closesocket(sock);
		sock = INVALID_SOCKET;
		MessageBox(hWnd, "WSAAsyncSelect() failed",
			"Error", MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}

	// 接続処理
	if (connect(sock, (LPSOCKADDR)&cl_sin, sizeof(cl_sin)) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			closesocket(sock);
			sock = INVALID_SOCKET;
			MessageBox(hWnd, "connect() failed", "Error", MB_OK | MB_ICONEXCLAMATION);
			return TRUE;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
//  接続待ち (サーバ側)
//
BOOL SockAccept(HWND hWnd)
{
	SOCKADDR_IN sv_sin;         // SOCKADDR_IN構造体

	// サーバ用ソケット
	sv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sv_sock == INVALID_SOCKET) { // ソケット作成失敗
		MessageBox(hWnd, "Socket() failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}

	memset(&sv_sin, 0x00, sizeof(sv_sin));      // 構造体初期化
	sv_sin.sin_family = AF_INET;           // インターネット
	sv_sin.sin_port = htons(PORT);       // ポート番号指定
	sv_sin.sin_addr.s_addr = htonl(INADDR_ANY); // アドレス指定

	if (bind(sv_sock, (LPSOCKADDR)&sv_sin, sizeof(sv_sin)) == SOCKET_ERROR) {
		closesocket(sv_sock);
		sv_sock = INVALID_SOCKET;
		MessageBox(hWnd, "bind() failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}

	if (listen(sv_sock, 5) == SOCKET_ERROR) {
		// 接続待ち失敗
		closesocket(sv_sock);
		sv_sock = INVALID_SOCKET;
		MessageBox(hWnd, "listen() failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}

	// 非同期処理モード (接続待ち)
	if (WSAAsyncSelect(sv_sock, hWnd, WM_SOCKET, FD_ACCEPT) == SOCKET_ERROR) {
		closesocket(sv_sock);
		sv_sock = INVALID_SOCKET;
		MessageBox(hWnd, "WSAAsyncSelect() failed",
			"Error", MB_OK | MB_ICONEXCLAMATION);
		return TRUE;
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
//
//  描画関数
//
LRESULT CALLBACK OnPaint(HWND hWnd, UINT uMsg, WPARAM wP, LPARAM lP)
{
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hWnd, &ps);

	// 描画領域の初期化
	MoveToEx(hdc, d.left, d.top, NULL);
	LineTo(hdc, d.right, d.top);    // 上横線
	LineTo(hdc, d.right, d.bottom); // 右縦線
	LineTo(hdc, d.left, d.bottom);  // 下横線
	LineTo(hdc, d.left, d.top); // 左縦線

	SelectObject(hdc, hPen);
	for (int i = 0; i < n; i++) {    // 線を描画
		if (flag[i] == 0) {      // 開始点なら、始点を移動
			MoveToEx(hdc, pos[i].x, pos[i].y, NULL);
		}
		else {             // 途中の点なら線を引く
			LineTo(hdc, pos[i].x, pos[i].y);
		}
	}
	SelectObject(hdc, hPen2);
	for (int i = 0; i < n_e; i++) {    // 線を描画
		if (flag_e[i] == 0) {      // 開始点なら、始点を移動
			MoveToEx(hdc, pos_e[i].x, pos_e[i].y, NULL);
		}
		else {             // 途中の点なら線を引く
			LineTo(hdc, pos_e[i].x, pos_e[i].y);
		}
	}

	EndPaint(hWnd, &ps);

	return 0L;
}


/*自分の座標リセット*/
void PClear()
{
	for (n = 0; n < MAX_ARRAY; n++) {
		if (pos[n].x == 0 && pos[n].y == 0)
			break;
		setData(0, 0, 0);
	}
	n = 0;
}
//相手の座標リセット
void PClear_e()
{
	for (n_e = 0; n_e < MAX_ARRAY; n_e++) {
		if (pos[n_e].x == 0 && pos[n_e].y == 0)
			break;
		setData_e(0, 0, 0);
	}
	n_e = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//  描画情報を格納
//
void setData(int f, int x, int y)
{
	flag[n] = f;
	pos[n].x = x;
	pos[n].y = y;
}
void setData_e(int f, int x, int y)
{
	flag_e[n_e] = f;
	pos_e[n_e].x = x;
	pos_e[n_e].y = y;
}


////////////////////////////////////////////////////////////////////////////////
//
//  マウスの位置がキャンパスの中かどうか判定する
//
BOOL checkMousePos(int x, int y)
{
	if (x >= d.left && x <= d.right
		&& y >= d.top && y <= d.bottom) {
		return TRUE;
	}
	return FALSE;
}


//全消し関数
void Clear(HWND hWnd) {
	HDC hdc;
	PAINTSTRUCT ps;

	InvalidateRect(hWnd, &d, TRUE);
	hdc = BeginPaint(hWnd, &ps);
	// 描画領域の初期化
	MoveToEx(hdc, d.left, d.top, NULL);
	LineTo(hdc, d.right, d.top);    // 上横線
	LineTo(hdc, d.right, d.bottom); // 右縦線
	LineTo(hdc, d.left, d.bottom);  // 下横線
	LineTo(hdc, d.left, d.top); // 左縦線
	EndPaint(hWnd, &ps);

	PClear();
	PClear_e();
}
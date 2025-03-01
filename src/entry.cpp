#ifdef _WIN32
#include <Windows.h>
#include <iostream>
#include <vector>

// ワイド文字列をUTF-8に変換する。
static std::string to_utf8(LPWSTR wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    std::vector<char> buf(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, buf.data(), size_needed, nullptr, nullptr);
    return {buf.data()};
}

// コンソールの文字色を変更する。
static void set_console_color(WORD color) {
    HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h_console, color);
}

// ユーザに見せるmain関数。
int main(int argc, const char *argv[]);

// Windows環境での本当のエントリポイント。
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/) {
    // 親のコンソールがあるならアタッチ
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE *stream;
        if (freopen_s(&stream, "CONOUT$", "w", stdout) != 0) {
            std::cerr << "Failed to redirect stdout" << std::endl;
            return 1;
        }
        if (freopen_s(&stream, "CONOUT$", "w", stderr) != 0) {
            std::cerr << "Failed to redirect stderr" << std::endl;
            return 1;
        }

        // コンソールのコードページをUTF-8に固定。
        SetConsoleOutputCP(CP_UTF8);
    }

    // コマンドライン引数を取得。Windowsではコマンドライン引数は必ずUTF-16エンコーディングされている？
    int argc;
    LPWSTR *wide_sep = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wide_sep) {
        std::cerr << "Failed to parse command line arguments" << std::endl;
        return 1;
    }

    // wchar_t* → char* (UTF-8) に変換
    std::vector<std::string> args(argc);
    std::vector<const char *> argv(argc);
    for (int i = 0; i < argc; i++) {
        args[i] = to_utf8(wide_sep[i]);
        argv[i] = args[i].data();
    }
    LocalFree(wide_sep);

    try {
        return main(argc, argv.data());
    } catch (const std::exception &e) {
        // 赤色でエラーメッセージを表示
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cerr << "🚨 [" << typeid(e).name() << "] " << e.what() << std::endl;
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 白に戻す
        return 1;
    }
}
#endif

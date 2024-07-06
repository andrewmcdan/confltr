// TODO: all of this code was generated with ChatGPT. It needs to be cleaned up and tested.
// TODO: input does not seem to be getting passed to the executable

// TODO: add a license to the code
// TODO: add a README.md file
// TODO: add a .gitignore file
// TODO: add version and copyright info to -h output

#include <iostream>
#include <string>
#include <regex>
#include <argparse.hpp>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void runProcess(const std::string& executable, const std::string& startIn, const std::string& filter, bool useRegex) {
#ifdef _WIN32
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0))
        throw std::runtime_error("Stdout pipe creation failed\n");

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
        throw std::runtime_error("Stdout SetHandleInformation failed\n");

    // Create a pipe for the child process's STDIN.
    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &sa, 0))
        throw std::runtime_error("Stdin pipe creation failed\n");

    // Ensure the write handle to the pipe for STDIN is not inherited.
    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
        throw std::runtime_error("Stdin SetHandleInformation failed\n");

    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.hStdInput = g_hChildStd_IN_Rd;
    si.hStdError = g_hChildStd_OUT_Wr;

    std::string cmd = executable;
    if (!CreateProcessA(NULL, cmd.data(), NULL, NULL, TRUE, 0, NULL, startIn.c_str(), &si, &pi))
        throw std::runtime_error("CreateProcess failed\n");

    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_IN_Rd);

    DWORD dwRead;
    CHAR chBuf[4096];
    BOOL bSuccess = FALSE;

    while (true) {
        // check for data on the child process's stdout nonblocking using PeekNamedPipe
        DWORD dwAvail = 0;
        if (PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &dwAvail, NULL) && dwAvail > 0) {
            bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, 4096, &dwRead, NULL);
            if (!bSuccess || dwRead == 0) break;

            std::string line(chBuf, dwRead);
            if (useRegex) {
                if (std::regex_search(line, std::regex(filter))) {
                    std::cout << line;
                }
            } else {
                if (line.find(filter) != std::string::npos) {
                    std::cout << line;
                }
            }
        }

        if(_kbhit()) {
            char ch = _getch();
            if (ch == 3) {
                break;
            }
            DWORD dwWritten;
            WriteFile(g_hChildStd_IN_Wr, &ch, 1, &dwWritten, NULL);
        }


        

        // if the process has exited, break out of the loop
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        if (exitCode != STILL_ACTIVE) {
            break;
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(g_hChildStd_OUT_Rd);
    CloseHandle(g_hChildStd_IN_Wr);

#else
    std::string cmd = "cd " + startIn + " && " + executable;

    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);

    if (!pipe) throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        std::string line(buffer.data());
        if (useRegex) {
            if (std::regex_search(line, std::regex(filter))) {
                std::cout << line;
            }
        } else {
            if (line.find(filter) != std::string::npos) {
                std::cout << line;
            }
        }
    }
#endif
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("ConsoleFilterUtility");

    program.add_argument("-f", "--filter")
        .required()
        .help("Text or regular expression to filter for in the output");

    program.add_argument("-e", "--executable")
        .required()
        .help("Path to the executable to run");

    program.add_argument("-r", "--regex")
        .default_value(false)
        .implicit_value(true)
        .help("Indicates if the filter text is a regular expression");

    program.add_argument("-s", "--startin")
        .default_value(std::string("."))
        .help("Folder to start the executable in");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string filter = program.get<std::string>("--filter");
    std::string executable = program.get<std::string>("--executable");
    bool useRegex = program.get<bool>("--regex");
    std::string startIn = program.get<std::string>("--startin");

    try {
        runProcess(executable, startIn, filter, useRegex);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

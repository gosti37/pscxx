#include <iostream>
#include <fcntl.h>
#include "pscxx.hpp"

/*This example file was created with the paths openSUSE Tumbleweed set taken as default, the binary paths may vary depending on your system, so this example file may return more/different errors than expected, so please check whether the second argument of the function is actually the path of the binary*/

int main()
{
    /*Normal*/
    auto test1 = pscxx::execute_pip({"ls", "-l", "/home"}, "/usr/bin/ls");
    std::cout << "Test 1\n" <<
                "STDOUT= " << test1.outStd << "\n" <<
                "Return code= " << test1.exitCode << "\n\n";

    /*Error*/
    auto test2 = pscxx::execute_pip({"ls","/doesnt/exist"}, "/usr/bin/ls");
    std::cout << "Test 2\n" <<
                "STDERR= " << test2.errStd <<
                "Return code= " << test2.exitCode << "\n\n";
    auto test3 = pscxx::execute_pip({"i_dont_exist"}, "/im/not/real");
    std::cout << "Test 3\n" <<
                "STDERR= " << test3.errStd << "\n" <<
                "Return code= " << test3.exitCode << "\n\n";
    auto test4 = pscxx::execute_pip({"dmesg"}, "/usr/bin/dmesg");
    std::cout << "Test 4\n" <<
                "STDERR= " << test4.errStd <<
                "Return code= " << test4.exitCode << "\n\n";

    /*Signals*/
    auto test5 = pscxx::execute_pip({"sh", "-c", "kill -SEGV $$"}, "/usr/bin/sh");
    std::cout << "Test 5\n" <<
                "Signal= " << test5.signal << "\n" <<
                "Return code= " << test5.exitCode << "\n\n";

    /*Changing standart directions*/
    int fd = open("test.txt", O_RDWR | O_CREAT, 0644);

    auto test6 = pscxx::execute_pip({"echo", "Test 6"}, "/usr/bin/echo", environ, false, fd);

    close(fd);

    if (test6.exitCode == 0)
    {
        std::cout << "succesfull directing to ./pscxx/build/linux/x86_64/release/test.txt\n";
    }

    /*Interactive programs*/
    pscxx::execute_int({"sudo", "-v"}, "/usr/bin/sudo");

    return 0;
}
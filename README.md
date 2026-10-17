## Process-Spawn-CXX (PSCXX)

PSCXX is a simple, one-header-file big C++ wrapper written in C++17 to simplify process spawning that you can just drag and drop to your project for POSIX compliant systems.

The header contains a struct called **PRESULT** within the scope of the namespace **pscxx**, which can return the values of:

* **int exitCode;** -> the code the process exited with
* **int signal;** -> the signal the process exited with
* **std::string outStd;** -> the standart output buffer
* **std::string errStd;** -> the standart error buffer

The header also containts two functions named **execute_pip()** and **execute_int()** that return a **PRESULT** both within the scope of **pscxx**.

**execute_pip()** can be used for non-interactive processes. Its arguments are:

* **std::vector\<std::string\> execution_argv** -> a vector of command arguments
* **std::string binary_path** -> path to the binary you want to execute
* **std::optional\<char \*const\*\> environment** -> the environment variables, defaults to **environ** if nothing is passed is passed
* **std::optional\<bool\> live_print** -> prints the commands returns lively as exeucution happens, enabled by default
* **std::optional\<int\> std_out, std::optional\<int\> std_err, std::optional\<int\> std_in** -> arguments to change the standart file descriptors the program will use, defaults to **STDOUT_FILENO, STDERR_FILENO, STDIN_FILENO** if nothing is passed


**execute_int()** can be used for interactive processes like **sudo, emacs, nano** etc. Its arguments are:

* **std::vector\<std::string\> execution_argv** -> a vector of command arguments
* **std::string binary_path** -> path to the binary you want to execute
* **std::optional\<char \*const\*\> environment** -> the environment variables, defaults to **environ** if nothing is passed is passed

> [!IMPORTANT]
> Beware that **execute_pip()** can return stdOut and stdErr, but same cannot be said for **execute_int()**, though it still returns stdErr if posix functions fail
# ljstack

ljstack是一个能够对luajit2.1进程进行profile的工具，同时也是一个库。

为了避免ptrace失败，可以sudo执行进程，或者是执行如下命令：
```cpp
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
```
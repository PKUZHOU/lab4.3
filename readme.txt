编译：
先运行 cmake ./（要求cmake版本高于3.5）
再运行 make 得到riscv_simulator 程序

使用：

./riscv_simulator test/add 为运行测试用例 
可选项： -d (单步执行） -d + 指令地址 （0x123456)，运行到该指令地址后停顿
单步执行过程中,输入内存地址（0x123456)可以查看该地址的数据，查看完一次后，输入 T+word数 （T 10）
可以将每一步的该地址数据以及随后的一串数据都打印出来。
 
 
 运行的测试样例都是我自己编译的，在lab2.2-new1中，编译选项为-Wa,-march=rv64im

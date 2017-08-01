基于uCOSIII 3.04

本实验参考STM32F4 uCOS开发手册_v3.0

创建 4 个任务，任务 A 用于创建 B、 C 和 D 这三个任务， A 还创建了一个初始值为 1的信号量 TEST_SEM，任务 B 和 D 都请求信号量 TEST_SEM， 其中任务优先级从高到底分别为： B、 C、 D。
基于uCOSIII 3.04

本实验参考STM32F4 uCOS开发手册_v3.0

创建 3 个任务，任务 start_task 用于创建其他两个任务，任务 task1_task 主要用于扫描按键，当检测到 KEY0 按下以后就向任务 task2_task 发送一个任务信号量。 任务 task2_task请求任务信号量，当请求到任务信号量的时候就反转LED的状态。
基于uCOSIII 3.04

本实验参考STM32F4 uCOS开发手册_v3.0

    设计一个应用程序，该程序有 4 任务:起始任务、两个消息队列和一个定时器。 任务 start_task 用于创建其他 3 个任务。 main_task 任务为主任务，用于检测按键，并且将按键的值通过消息队列KEY_Msg 发送给任务 Keyprocess_task， main_task 任务还用于检测消息队列 DATA_Msg的总大小和剩余空间大小，并且控制 LED0 的闪烁。 Keyprocess_task 任务获取 KEY_Msg 内的消息，根据不同的消息做出相应的处理。
    定时器 1 的回调函数 tmr1_callback 通过消息队列 DATA_Msg 将定时器 1 的运行次数作为信息发送给任务 msgdis_task，任务 msgdis_task 将 DATA_Msg 中的消息显示在 LCD 上。
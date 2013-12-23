phoneQmd
========

通信录亲密度

 一： 手机通讯录亲密度算法实现
 
1.1 算法策略

    weight<a,b> = Flag12*32 + Flag21*8 + 0.5*(a&b)
    
1.2 程序地址:

     28:  /data1/ua/jiawei/task/phoneQmd/src
     
     版本：V1 phoneQmd_v1.c
     实现思路：
     1）：输入数据进行排序---两列的形式。
     2）：数组PID：依次顺序输入全量通讯录中的内容。
     3）：结构体数组：记录每个pid，在PID中的起始位置及长度。
     4）：遍历结构体数组，计算pid与通讯录里面的亲密度
     5）：输出
     
     版本：V2 phoneQmd_v2.c
     1) 加入了指针数组，将malloc分开分配
     
1.3 问题：

    1） malloc分配不够大:
        将需要的数组切成两个子数组，分别malloc好点

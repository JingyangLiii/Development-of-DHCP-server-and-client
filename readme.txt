
0.通过xshell或terminal，打开wireshark软件；

1.启动server，可以通过命令行或配置文件设置Lease Time；

2.ifconfig查看eth1没有地址，通过运行client程序中的地址分配过程，server收到后将分配的地址写入lease文件，ifconfig能查看到eth1获得地址；
然后再运行client程序的地址释放过程，发送dhcprelease，server收到后将lease文件里的地址释放；

3.启动地址分配过程,根据从server收到的Leasetime设置定时器，通过wireshark的数据包时间戳判定；Lease到50%，client发送DHCPrequest, server回送DHCPACK, client重新设置定时器；

4.结束client程序，重新运行，命令行选择设置一个非法的IP地址，client单播发送DHCPRequest,包含非法IP地址，Server回NAK，client程序重新开始discover获取地址过程；

5.结束client程序，重新运行，命令行选择发送inform，wireshark检查inform/ACK；

6.结束server程序，client在lease达到50%单播发送DHCPRequest，Server没有响应，client在lease达到7/8时广播发送DHCPRequest，然后在Lease结束时广播DHCPDiscover。



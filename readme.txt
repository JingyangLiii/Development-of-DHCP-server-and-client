
0.ͨ��xshell��terminal����wireshark�����

1.����server������ͨ�������л������ļ�����Lease Time��

2.ifconfig�鿴eth1û�е�ַ��ͨ������client�����еĵ�ַ������̣�server�յ��󽫷���ĵ�ַд��lease�ļ���ifconfig�ܲ鿴��eth1��õ�ַ��
Ȼ��������client����ĵ�ַ�ͷŹ��̣�����dhcprelease��server�յ���lease�ļ���ĵ�ַ�ͷţ�

3.������ַ�������,���ݴ�server�յ���Leasetime���ö�ʱ����ͨ��wireshark�����ݰ�ʱ����ж���Lease��50%��client����DHCPrequest, server����DHCPACK, client�������ö�ʱ����

4.����client�����������У�������ѡ������һ���Ƿ���IP��ַ��client��������DHCPRequest,�����Ƿ�IP��ַ��Server��NAK��client�������¿�ʼdiscover��ȡ��ַ���̣�

5.����client�����������У�������ѡ����inform��wireshark���inform/ACK��

6.����server����client��lease�ﵽ50%��������DHCPRequest��Serverû����Ӧ��client��lease�ﵽ7/8ʱ�㲥����DHCPRequest��Ȼ����Lease����ʱ�㲥DHCPDiscover��



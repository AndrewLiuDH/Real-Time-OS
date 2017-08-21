本实验基于STM32F4 PHY使用的是LAN8720A LWIP使用的是1.4.1版本

引脚分配如下：
	ETH_MDIO -------------------------> PA2
	ETH_MDC --------------------------> PC1
	ETH_RMII_REF_CLK------------------> PA1
	ETH_RMII_CRS_DV ------------------> PA7
	ETH_RMII_RXD0 --------------------> PC4
	ETH_RMII_RXD1 --------------------> PC5
	ETH_RMII_TX_EN -------------------> PG11
	ETH_RMII_TXD0 --------------------> PG13
	ETH_RMII_TXD1 --------------------> PG14
	
本实验主要是演示了最基本的Ping通的过程，IP地址分配使用的是动态IP分配（DHCP）


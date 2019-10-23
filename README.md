#<center>**计算棒数据透传指南**</center>
---

##准备
	- **系统环境**
	
		1. 切换到root用户
		2. 拷贝更新程序到/usr/bin/ (后续会以固件或升级包的形式更新)
			cp usr/bin/* /usr/bin/
		2. 安装rknn-toolkit 1808 python版本
			pip3 install rknn_toolkit-0.9.8.1-cp36-cp36m-linux_aarch64.whl

###python
   
   - **yolov3**
    
		1. device服务启动
			python3 yolov3_server.py 

	**mobilenet-ssd(目前ssd android端还未实现，未调试)**

		1. device服务启动
			python3 ssd_server.py


   

###c++  

   - **当前只实现host端和device端通信**

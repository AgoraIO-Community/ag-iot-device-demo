1、构建Demo应用前必现必须先修改example/src/app_config.h设置相关参数为你新创建的产品。详情请参考官方文档：https://docs-preprod.agora.io/cn/iot-apaas/device_media_call?platform=All%20Platforms

2、将hello_doorbell_2复制到ubuntu 18.04以上linux系统中；

3、在有写入权限路径下所在运行Demo：
	./hello_doorbell_2 xxxxx
	注：其中xxxx是自由定义的Device ID，只支持字母+数字，为保证唯一性，建议携带自己的名字首拼
	
4、根据提示输入APP端生成的二维码内容（Demo没有依赖摄像头设备，二维码内容可以通过微信等二维码扫描工具获取字符串内容），比如：
	------------------ Please input QRcode string with JSON type ----------------------
	{"s":"CMCC-xxxxx","p":"19xxxxx06","u":"6846xxxxxxxx72","k":"EJIxxxxxxxOl5"}
	-------------------- Got string and parse it now ------------------------------------
	
5、功能说明： 
	c键：呼叫Demo APP端 
	h键：挂断 
	a键：接听（目前做了自动接听，不需要） 
	w键：产生报警消息
	q键：退出demo
	
6、Demo APP端侧添加设备功能需要扫描一个产品二维码（实际情况下，一般在说明书或者设备机身找到），请使用目录中的“设备产品码.png”

7、请注意：为避免Demo工程测试时各自使用Device ID重复导致的冲突，该Demo源码中默认的项目工程设定为“设备不可重复绑定”，因此，当设备绑定成功后，如果device_status.cfg文件丢失，需要重复绑定激活，则必须在APP端先“移除设备”操作后再进行激活操作，否则将会出现激活失败的错误警告。


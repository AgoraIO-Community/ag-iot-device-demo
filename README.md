1、构建Demo应用前必现必须先修改example/src/app_config.h设置相关参数为你新创建的产品；[详情请参考官方文档](https://docs.agora.io/cn/iot-apaas/device_media_call?platform=All%20Platforms)

2、将hello_doorbell_2复制到ubuntu 18.04以上linux系统中；

3、在有写入权限路径下所在运行Demo：
>./hello_doorbell_2 **xxxxx**
>
>注：其中**xxxx**是自由定义的Device ID，只支持字母+数字，为保证唯一性，建议携带自己的名字首拼

4、根据提示输入APP端生成的二维码内容（Demo没有依赖摄像头设备，二维码内容可以通过微信等二维码扫描工具获取字符串内容），比如：
>------------------ Please input QRcode string with JSON type ----------------------
>
>{"s":"CMCC-xxxxx","p":"19xxxxx06","u":"6846xxxxxxxx72","k":"EJIxxxxxxxOl5"}
>
>-------------------- Got string and parse it now ------------------------------------

5、功能说明：
- c键：呼叫Demo APP端
- h键：挂断
- a键：接听（目前做了自动接听，不需要）
- w键：产生报警消息
- s键：停止告警云存
- q键：退出demo

6、Demo APP端侧添加设备功能需要扫描一个产品二维码（实际情况下，一般在说明书或者设备机身找到），请使用目录中的“**设备产品码.png**”

7、请注意：为避免Demo工程测试时各自使用Device ID重复导致的冲突，该Demo源码中默认的项目工程设定为“**设备不可重复绑定**”。

因此，当设备绑定成功后，如果**device_status.cfg**文件丢失，需要重复绑定激活，则必须在APP端先“移除设备”操作后再进行激活操作，

否则将会出现激活失败的错误警告。

Change List:
v1.3.0.1 - 2022/10/28
1、支持云存视频内容加密，设备开启加密方案后不再兼容之前未加密功能支持版本APP demo，需要同步升级
2、支持云存区域自动配置，选择中国大陆区域创建产品则云存文件存储在中国大陆区域服务器，选择北美服务是存在在北美服务器

v1.2.0.2 - 2022/10/19
1、支持RTSA核心log文件设置：agora_iot_logfile_config
2、优化MQTT接收topic通知效率，加快呼叫通知速度
3、支持云存套餐未开通状态下告警图片上传

v1.2.0.1 - 2022/09/29
2、增加多端同时实时预览设备画面支持

v1.1.0.2 - 2022/09/23
1、云存权限增加token机制，解决密钥泄漏安全性问题
2、修正cJSON支持64位数值类型精度损失问题
3、解决OSS多线程推送函数重入异常问题

v1.1.0.1 - 2022/09/14
1、支持端侧直推云录云存功能：agora_iot_cloud_record_start、agora_iot_cloud_record_stop
2、支持告警事件与预览图片云存解耦：agora_iot_push_alarm_message、agora_iot_push_alarm_image
3、支持license新版本，支持云端续费操作

v1.0.0.8 - 2022/09/06
1、增加区域合规限制参数配置：agora_iot_config_t -> area_code
2、支持H265视频编码格式推流功能
3、增加agora server连接状态回调：agora_iot_config_t -> connect_cb
4、增加底层RTSA视频传输log文件存放路径配置参数：agora_iot_config_t -> p_log_dir
5、扩容端侧支持DP属性点上限到128个
6、支持管理平台DP属性点下方控制能力
7、支持OTA能力：agora_iot_config_t -> ota_cb
8、支持RTM远程实时控制能力：agora_iot_config_t -> rtm_cb
9、支持H264视频端侧直推云存
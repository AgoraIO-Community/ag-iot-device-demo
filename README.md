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
v1.5.0.2 - 2023/05/29
1、重构呼叫业务，提高弱网环境下呼叫出图成功率，解决MQTT通知信令累积导致的呼叫出图响应变慢问题
2、修正严重弱网条件下，云存缓冲区一直增长，嵌入式设备上导致内存不足异常的问题
注：必须搭配1.5.0版本以上APP端SDK使用，新版本不再兼容1.3.x及以前版本，混合使用将无法呼叫出图

v1.3.7.0 - 2023/05/15
1、解决弱网环境下长时间挂机偶现MQTT reconnect失败的问题
2、RTM message payload长度扩大到4096字节
3、agora_iot_file_player_callback_t::cb_stop_push_frame()接口增加触发条件，APP端回看停止时候触发该时间，通知应用层调用agora_iot_file_player_stop()接口停止指定channel的回看

v1.3.6.0 - 2023/04/23
1、修复云存stop后继续push有概率导致crash的问题
2、增加对端audio mute操作通知事件，为减少内存消耗未开启混音功能，同时只有一个APP端可以讲话，mute后另一个APP可以讲话，相关接口：cb_audio_muted_changed()
3、增加agora_iot_send_rtm()接口参数使用说明，msg_id参数不能为0

v1.3.5.0 - 2023/04/23
1、修复云存stop后继续push frame有概率导致crash的问题
2、修复设备运行期间开通云存权限无法立即生成云存视频的问题
3、修复设备端有可能进入呼叫异常状态且重启无法恢复的问题
4、增加云存视频push限速功能，缓解云存上传与实时预览抢占带宽造成带宽不足的问题

v1.3.4.0 - 2023/04/17
1、增加最小target bitrate值设置参数，max_possible_bitrate控制最大带宽探测值，min_possible_bitrate控制最小带宽探测值，以便在动态码率编码模式下应用层控制带宽探测区间
2、修复在SSC337平台上target bitrate返回参数为0的问题

v1.3.3.0 - 2023/04/13
1、支持string类型的DP属性点嵌套json格式，如："{\"total\":320000,\"use\":16000,\"state\":false}"
2、支持on_dp_query_callback中string类型DP属性点使用数组指针赋值，SDK内部不再管理对应内存的释放，具体见agora_dp_value_t::dp_str参数说明
3、解决高丢包弱网环境下，https请求可能因connect失败无限重连造成线程卡死的问题
4、解决OSS云存频繁发起云存录像，概率性出现多线程竞争导致crash问题
5、优化弱网下被叫通知连续操作造成事件拥塞，导致下一次呼叫出图速度一直延迟的问题
6、SD卡回看传输通道支持音频格式设置，不再局限于G722格式，与实时预览接口对齐

v1.3.2.0 - 2023/03/30
1、OTA版本上报接口支持最大50字节版本号字符串长度：agora_iot_fw_info_update()
2、SD文件回看接口支持同时多端回看不同文件，回调事件接口增加channel参数标志：cb_start_push_frame()、cb_stop_push_frame()
3、单从视频通话支持最大连续24小时观看，超过上限后自动挂断；（1.3.2版本之前只支持1小时）

v1.3.1.0 - 2023/02/27
1、支持H265视频格式云存
2、支持H265视频格式文件传输

v1.3.0.2 - 2022/12/04
1、云存直推支持腾讯OSS
2、云存支持套餐控制最大录制时长，agora_iot_cloud_record_start接口去除record_id和endtime参数依赖
3、云存支持预录制功能，ago_video_frame_t增加fps参数指定真实帧率，不再依赖push速度

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
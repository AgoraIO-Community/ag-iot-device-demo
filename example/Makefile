LOCAL_LDLIBS += -lagora-aws-shadow
LOCAL_LDLIBS += -lagora-iot-solution
LOCAL_LDLIBS += -lagora-mbedtls
LOCAL_LDLIBS += -lagora-opus
LOCAL_LDLIBS += -lrtsa
LOCAL_LDLIBS += -lahpl
LOCAL_LDLIBS += -lagora-cjson
LOCAL_LDLIBS += -lhttp-parser
LOCAL_LDLIBS += -lmedia-engine
LOCAL_LDLIBS += -liot-utility
LOCAL_LDLIBS += -lagora-webclient
LOCAL_LDLIBS += -lagora-callkit
LOCAL_LDLIBS += -llicense-active
LOCAL_LDLIBS += -lagora-rtm
LOCAL_LDLIBS += -loss-sdk
LOCAL_LDLIBS += -lts-muxer
LOCAL_LDLIBS += -lrt -lm -lpthread

all:
	gcc src/device_state.c src/hello_doorbell_2.c src/hello_doorbell_comm.c -o hello_doorbell_2 -I../include -L../lib -Wl,--start-group $(LOCAL_LDLIBS) -Wl,--end-group

clean:
	rm -rf hello_doorbell_2
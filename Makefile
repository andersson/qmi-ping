OUT := qmi-ping

CFLAGS += -Wall -g -O2
LDFLAGS += -lqrtr
prefix = /usr/local

SRCS := qmi_ping.c qmi_test.c

OBJS := $(SRCS:.c=.o)

$(OUT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

install: $(OUT)
	install -D -m 755 $< $(DESTDIR)$(prefix)/bin/$<

clean:
	rm -f $(OUT) $(OBJS)

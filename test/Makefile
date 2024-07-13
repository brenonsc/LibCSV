CC = gcc
CFLAGS = -Wall -I../libcsv -I/opt/homebrew/Cellar/cunit/2.1-3/include
LDFLAGS = -L/opt/homebrew/Cellar/cunit/2.1-3/lib -lcunit

SRCS = test_libcsv.c ../libcsv/libcsv.c
OBJS = $(SRCS:.c=.o)

TARGET = test_libcsv

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
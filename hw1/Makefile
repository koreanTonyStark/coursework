.SUFFIXES: .cpp .o

CC = g++ 

OBJS1 = hw1.o
SRCS1 = $(OBJS:.o=.cpp)

OBJS2 = checker.o
SRCS2 = $(OBJS2:.o=.cpp)

TARGET1 = hw1
TARGET2 = checker

.PHONY : dep clean

all : $(TARGET1) $(TARGET2)

hw1 : $(OBJS1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS1)
	
checker : $(OBJS2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJS2)

dep : 
	gccmakedep $(SRCS)

clean : 
	$(RM) $(TARGET1) $(TARGET2) $(OBJS1) $(OBJS2)

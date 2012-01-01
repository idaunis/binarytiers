CC = gcc
CXX = g++
CFLAGS = -Os
CCFLAGS = 
INCLUDES = -I/usr/include/mysql -I../src/pcre
LDFLAGS = -L../lib -lbt -L/usr/lib/mysql -lmysqlclient -lfcgi -lssl -lgd -lz -lpcre -lexpat -lhiredis
OBJECTS = main.o website.o admin.o site.o

bt.build.bin : $(OBJECTS)
	$(CXX) $(CFLAGS) $(CCFLAGS) $(OBJECTS) $(LDFLAGS) -o ./bt.build.bin
	mv ./bt.build.bin ../bt.bin
	sleep 5
	rm -f result.txt

main.o : ../src/main.cpp ../src/settings.h
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../src/main.cpp

website.o : main.o ../src/website.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../src/website.cpp
admin.o : main.o ../themes/admin/admin.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../themes/admin/admin.cpp
site.o : main.o ../themes/site/site.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../themes/site/site.cpp

clean:
	rm *.o

CC = gcc
CXX = g++
CFLAGS = -Os
CCFLAGS = 
INCLUDES = -I/usr/include/mysql -I../lib/pcre
LDFLAGS = -L/usr/lib/mysql -lmysqlclient -lfcgi -lgd -lz -lpcre -lexpat -lhiredis
OBJECTS = core.o parsedate.o json.o md5c.o sha1.o aggregator.o ckeditor.o comment.o database.o form.o locale.o menu.o node.o path.o profile.o sendmail.o system.o taxonomy.o translation.o upload.o user.o

libbt.a : $(OBJECTS)
	ar ru libbt.a $(OBJECTS)
	mv ./libbt.a ../lib/libbt.a
	sleep 5
	rm -f result.txt

core.o : ../lib/core.cpp ../lib/core.h
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/core.cpp

aggregator.o : ../lib/aggregator.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/aggregator.cpp
ckeditor.o : ../lib/ckeditor.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/ckeditor.cpp
comment.o : ../lib/comment.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/comment.cpp
database.o : ../lib/database.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/database.cpp
form.o : ../lib/form.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/form.cpp
locale.o : ../lib/locale.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/locale.cpp
menu.o : ../lib/menu.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/menu.cpp
node.o : ../lib/node.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/node.cpp
path.o : ../lib/path.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/path.cpp
profile.o : ../lib/profile.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/profile.cpp
sendmail.o : ../lib/sendmail.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/sendmail.cpp
system.o : ../lib/system.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/system.cpp
taxonomy.o : ../lib/taxonomy.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/taxonomy.cpp
translation.o : ../lib/translation.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/translation.cpp
upload.o : ../lib/upload.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/upload.cpp
user.o : ../lib/user.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/user.cpp
parsedate.o : ../lib/gnu/parsedate.cpp
	$(CXX) $(CFLAGS) $(CCFLAGS) $(INCLUDES) -c ../lib/gnu/parsedate.cpp
json.o :  ../lib/json/json.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ../lib/json/json.c
md5c.o :  ../lib/md5/md5c.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ../lib/md5/md5c.c
sha1.o :  ../lib/md5/sha1.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ../lib/md5/sha1.c

clean:
	rm *.o

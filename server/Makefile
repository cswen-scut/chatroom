OBJS = ./out/main.o ./out/MyTime.o ./out/MySQLConnector.o ./out/UserService.o ./out/Online.o ./out/DataEncoder.o ./out/HeadData.o ./out/DataProcesser.o
CC=g++ -std=c++11
main: $(OBJS)
	CC $(OBJS) -o main `mysql_config --cflags --libs` -ljsoncpp
./out/main.o: main.cpp ./ProtocolHead/HeadData.h ./Service/DataProcesser.h ./Service/UserService.h ./Service/Online.h ./config/server_config.h
	CC -c main.cpp -o ./out/main.o
./out/DataProcesser.o: ./Service/DataProcesser.cpp ./ProtocolHead/protocolmsg.h ./ProtocolHead/HeadData.h ./ProtocolHead/DataEncoder.h ./Util/MyTime.h
	CC -c ./Service/DataProcesser.cpp -o ./out/DataProcesser.o
./out/HeadData.o: ./ProtocolHead/HeadData.cpp ./ProtocolHead/protocolmsg.h
	CC -c ./ProtocolHead/HeadData.cpp -o ./out/HeadData.o
./out/DataEncoder.o: ./ProtocolHead/DataEncoder.cpp ./ProtocolHead/protocolmsg.h
	CC -c ./ProtocolHead/DataEncoder.cpp -o ./out/DataEncoder.o
./out/Online.o: ./Service/Online.cpp ./Util/MyTime.h
	CC -c ./Service/Online.cpp -o ./out/Online.o
./out/UserService.o: ./Service/UserService.cpp ./Dao/MySQLConnector.h
	CC -c ./Service/UserService.cpp -o ./out/UserService.o
./out/MySQLConnector.o: ./Dao/MySQLConnector.cpp ./Util/MyTime.h ./config/mysql_config.h
	CC -c ./Dao/MySQLConnector.cpp -o ./out/MySQLConnector.o
./out/MyTime.o: ./Util/MyTime.cpp
	CC -c ./Util/MyTime.cpp -o ./out/MyTime.o
clean:
	rm -rf ./out/*.o main




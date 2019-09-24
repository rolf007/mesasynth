all: main gtest

MAIN_SOURCE += main.cpp

SOURCES += macro.cpp
SOURCES += parsers.cpp
SOURCES += sequencer.cpp
SOURCES += synthesizer.cpp
SOURCES += refcnt.cpp

macro.o: macro.cpp macro.h
	g++ -c $< -o $@

parsers.o: parsers.cpp parsers.h value.h
	g++ -c $< -o $@

refcnt.o: refcnt.cpp refcnt.h
	g++ -c $< -o $@

sequencer.o: sequencer.cpp sequencer.h parsers.h sequencer.h synthesizer.h value.h macro.h chain_pool.h
	g++ -c $< -o $@

synthesizer.o: synthesizer.cpp synthesizer.h
	g++ -c $< -o $@

OBJS = $(SOURCES:.cpp=.o)

HEADERS += chain_pool.h
HEADERS += macro.h
HEADERS += parsers.h
HEADERS += sequencer.h
HEADERS += synthesizer.h
HEADERS += value.h

TEST_SOURCES += chain_pool_test.cpp
TEST_SOURCES += macro_test.cpp
TEST_SOURCES += parsers_test.cpp
TEST_SOURCES += sequencer_test.cpp

chain_pool_test.o: chain_pool_test.cpp chain_pool.h
	g++ -c $< -o $@

macro_test.o: macro_test.cpp macro.h
	g++ -c $< -o $@

parsers_test.o: parsers_test.cpp parsers.h value.h macro.h
	g++ -c $< -o $@

sequencer_test.o: sequencer_test.cpp synthesizer.h sequencer.h parsers.h value.h chain_pool.h macro.h
	g++ -c $< -o $@

TEST_OBJS = $(TEST_SOURCES:.cpp=.o)

main: ${MAIN_SOURCE} ${OBJS} ${HEADERS}
	g++ main.cpp ${OBJS} -lSDL -L/usr/lib64 -o $@

gtest: ${OBJS} ${HEADERS} ${TEST_OBJS}
	g++ -lgtest -lgtest_main ${OBJS} ${TEST_OBJS} -o $@
	./$@

#./gtest --gtest_filter=*ChainPool*

#./$@ --gtest_filter="*Sequencer.paranthesis" > x

clean:
	rm ${OBJS} ${TEST_OBJS} gtest main

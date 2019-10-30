#http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#combine
all: main gtest

MAIN_SOURCE += main.cpp

SOURCES += macro.cpp
SOURCES += parsers.cpp
SOURCES += sequencer.cpp
SOURCES += synthesizer.cpp
SOURCES += refcnt.cpp
SOURCES += value.cpp

TEST_SOURCES += chain_pool_test.cpp
TEST_SOURCES += macro_test.cpp
TEST_SOURCES += parsers_test.cpp
TEST_SOURCES += sequencer_test.cpp
TEST_SOURCES += value_test.cpp

TEST_OBJS = $(TEST_SOURCES:.cpp=.o)
OBJS = $(SOURCES:.cpp=.o)
MAIN_OBJ = main.o

main: ${MAIN_OBJ} ${OBJS}
	g++ ${MAIN_OBJ} ${OBJS} -lSDL -L/usr/lib64 -o $@

gtest: ${OBJS} ${TEST_OBJS}
	g++ -lgtest -lgtest_main ${OBJS} ${TEST_OBJS} -o $@
	./$@

#./$@ --gtest_filter="*Sequencer.basic"
#./$@ --gtest_filter=*ChainPool*

#./$@ --gtest_filter="*Sequencer.paranthesis" > x

DEPDIR := deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.cpp = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SOURCES:%.cpp=$(DEPDIR)/%.d) $(TEST_SOURCES:%.cpp=$(DEPDIR)/%.d) $(MAIN_SOURCE:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))

clean:
	rm ${OBJS} ${TEST_OBJS} ${MAIN_OBJ} gtest main

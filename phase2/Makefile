CXX		= g++ -std=c++11
CXXFLAGS	= -g -Wall
OBJS		= tokens.o lexer.o string.o
PROG		= scc

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) $(PROG) core *.o

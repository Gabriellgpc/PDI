CXXFLAGS	:=	-Wall	`pkg-config --cflags opencv`\

CXX	=	g++
LDLIBS	:=	`pkg-config --libs opencv`\

.cpp:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

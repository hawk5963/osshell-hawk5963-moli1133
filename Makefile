CXX= g++
CXXFLAGS= -std=c++11

INCLUDE= 
LIB= 

OBJDIR= obj
SRCDIR= src

OBJS= $(addprefix $(OBJDIR)/, osshell.o)
EXEC= osshell


# BUILD EVERYTHING
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE)

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)


# REMOVE OLD FILES
clean:
	rm -f $(OBJS) $(EXEC)

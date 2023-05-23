LINK_TARGET = crawler

OBJS = main.o # add name of files here 

REBUILDABLES = $(OBJS) $(LINK_TARGET)

all : $(LINK_TARGET)

clean: 
	rm -f $(REBUILDABLES)

$(LINK_TARGET) : $(OBJS)
	g++ -g -o $@ $^

%.o : %.c
	g++ -g -Wall -o $@ -c $<

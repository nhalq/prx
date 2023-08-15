APPNAME=prx

CXX=g++-9
WARNFLAGS=-Wall -Wextra -Wpedantic -Wno-variadic-macros
CXXFLAGS=-std=c++98 $(WARNFLAGS) -fPIC -Iinclude

ifeq ($(CONF), Release)
	CXXFLAGS := -O3 $(CXXFLAGS)
else
	CXXFLAGS := -g $(CXXFLAGS)
endif

.PHONY: clean

all: $(APPNAME) libprx.so

$(APPNAME): $(APPNAME).o Log.o
	$(CXX) $(CXXFLAGS) -o $@ $^

libprx.so: libprx.o Log.o
	$(CXX) $(CXXFLAGS) -o $@ -Wl,--no-as-needed -shared -ldl $^

%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c $^

%.o: src/%.c
	$(CXX) $(CXXFLAGS) -c $^

clean:
	$(RM) $(APPNAME) *.o *.so

SOURCE :=$(wildcard *.cpp)
OBJS   :=$(patsubst %.cpp,%.o,$(SOURCE))

TARGET := main
CLANG=clang++ -g 
llvm_config=`llvm-config-15 --cxxflags --ldflags --system-libs --libs core`

all:$(TARGET)
$(TARGET): $(OBJS)
	$(CLANG) -o $@ $(OBJS) $(llvm_config)
$(OBJS):$(SOURCE)
	$(CLANG) -c  $(SOURCE) $(llvm_config)



clean:
	rm *.o $(TARGET)

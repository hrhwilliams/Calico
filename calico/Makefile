CPP := g++ -std=c++20 -I.
CPPFLAGS := -g # -O2 -march=native -flto
DEFINES := -D_DEBUG
3RDPARTY := -Iglm/
OUT := calico.a

FOLDERS := ecs logger renderer util xml
SOURCES := $(foreach DIR, ${FOLDERS}, $(wildcard ${DIR}/*.cpp))
OBJECTS := $(addsuffix .o, $(basename ${SOURCES}))

all:
	@echo ${OBJECTS}

.PHONY: clean
clean:
	rm -f ${OUT}
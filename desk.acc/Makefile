
CC65 = ~/dev/cc65/bin
CAFLAGS = --target apple2enh --list-bytes 0
CCFLAGS = --config apple2-asm.cfg

# ProDOS file type is $F1 ($ is pesky)
TARGETS = calculator.F1 show_text_file.F1 date.F1 puzzle.F1 show_image_file.F1 calc_fixed.F1

.PHONY: clean all
all: $(TARGETS)

HEADERS = $(wildcard ../*.inc) $(wildcard ../inc/*.inc) $(wildcard *.inc)

clean:
	rm -f *.o
	rm -f $(TARGETS)

%.o: %.s $(HEADERS)
	$(CC65)/ca65 $(CAFLAGS) --listing $(basename $@).list -o $@ $<

%.F1: %.o
	$(CC65)/ld65 $(CCFLAGS) -o $@ $<

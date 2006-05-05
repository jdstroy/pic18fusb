mcc18 -fo ./output/start.o -fe ./output/errors/start.err ./src/start.c
mcc18 -fo ./output/main.o -fe ./output/errors/main.err ./src/main.c
mplink ./lkr/usbpic.lkr ./output/main.o /a INHX32 /m ./output/main.map /o ./output/main.out

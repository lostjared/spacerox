SRC = sdl.c ship.c space.c
OUT = space.html
EMCC = emcc
CFLAGS = -O2 -s USE_SDL=2 -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH=1
all: $(OUT)
$(OUT): $(SRC)
	$(EMCC) $(SRC) $(CFLAGS) -o $(OUT) --preload-file assets
clean:
	rm -f $(OUT) *.js *.wasm *.data

.PHONY: all clean

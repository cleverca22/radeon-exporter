CFLAGS = `pkg-config --cflags libdrm` -Wall

radeon-exporter: main.o httpd.o
	$(CC) $^ -ldrm -lxcb -ldrm_amdgpu -o $@

main.o: httpd.h
httpd.o: httpd.h

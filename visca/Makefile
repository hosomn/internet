obj = main.o create_tcp_service.o analysis_command.o

all:$(obj)
	gcc -o all $(obj) 

main.o:*.h
create_tcp_service.o:create_tcp_service.h
analysis_command.o:analysis_command.h

.PHONY:clean
clean:
	rm $(obj) all

INCLUDE_DIR =../include
CC =gcc

OUTPUT_LOCATION = build
LIB_LOCATION = easysocket
SHARED_LIB = libeasysocket
LIBS = payload utils socketcontext pcb socketset commands libeasysocket
PROCS = console kernel fs memory # cpu 

all: $(PROCS)
	@echo "\n** All done! **\n"

# Create the output directories if they don't already exist (-p option)
dirs:
	@echo "\n** Creating build directory if not exists **"
	mkdir -p $(OUTPUT_LOCATION)
	@echo "\n** Making object files for libs **"

# For each lib, make object file
$(LIBS): dirs
	gcc -g -fPIC -pthread -o $(LIB_LOCATION)/$@.o -c $(LIB_LOCATION)/$@.c -Iinclude -lcommons

# Make shared lib out of all objects from above
shared: $(LIBS)
	@echo "\n** Building shared library **"
	gcc -g -shared -o $(LIB_LOCATION)/$(SHARED_LIB).so $(addprefix $(LIB_LOCATION)/, $(LIBS:=.o)) -lcommons -pthread
	@echo "\n** Building all processes **"

# Make each process file (adding the lib info)
$(PROCS): shared
	gcc -g -I$(LIB_LOCATION)/ -o $(OUTPUT_LOCATION)/$@ $(wildcard $@/*.c) -L$(LIB_LOCATION)/ -l$(LIB_LOCATION) -lcommons

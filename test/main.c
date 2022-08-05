#include <unistd.h>
#include <assert.h>
#include "logger.h"

int main(int argc, char **argv) {

	(void)argc;
	(void)argv;
	const char *file = "/mnt/c/Users/khubaib.umer/git/MemPool/include/MemPool.h";
//	assert(argc > 1);
	create_logger("Test", "/home/khubaibumer/logs");
	set_max_file_size(KB(1));

	get_logger()->trace(__func__, __LINE__, "%s\n", "powwow");

	TRACE("%s\n", "Wow!!!");

	char inbuffer[KB(4)];
	size_t num_read;
	FILE *ptr = fopen(file, "r");
	assert(ptr != NULL);
	while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), ptr)) > 0) {
		INFO("%s", inbuffer);
	}
	fclose(ptr);

	sleep(10);

	close_logger();

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <libelf.h>
#include <fcntl.h>
#include <gelf.h>
#include <string.h>
#include <unistd.h>
#include "../ctest.h"

void parse_symbol(const char* name, void* handle) {
	if (strlen(name) != 0 && strncmp("__ctest_utest_", name, strlen("__ctest_utest_")) == 0) {
		printf("[DEBUG] Found Unit Test: %s\n", name);
		ass_container *assertions = (ass_container*) dlsym(handle, "assertions");
		void (*test_ptr)() = dlsym(handle, name);
		test_ptr();
		char passed = 1;
		for (int i = 0; i < assertions->count; i++) {
			if (assertions->states[i] == 0) {
				passed = 0;
				break;
			}
		}
		if (passed)
			 printf("✅ Test %s PASSED\n", name + 14);
		else printf("❌ Test %s FAILED\n", name + 14);
		assertions->count = 0;
		free(assertions->ids);
		free(assertions->states);
	}
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "CTest Universal Test Runner\nUsage: %s <elf-binary>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int descriptor = open(argv[1], O_RDONLY);
	void* handle = dlopen(argv[1], RTLD_LAZY);

    if (descriptor < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed.\n");
        return EXIT_FAILURE;
    }

    Elf *elf = elf_begin(descriptor, ELF_C_READ, NULL);
    if (!elf) {
        fprintf(stderr, "elf_begin() failed: %s\n", elf_errmsg(-1));
        close(descriptor);
        return EXIT_FAILURE;
    }

    GElf_Ehdr ehdr;
    if (gelf_getehdr(elf, &ehdr) == NULL) {
        fprintf(stderr, "Failed to get ELF header: %s\n", elf_errmsg(-1));
        elf_end(elf);
        close(descriptor);
        return EXIT_FAILURE;
    }

    size_t shstrndx;
    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        fprintf(stderr, "Failed to get section header string index: %s\n", elf_errmsg(-1));
        elf_end(elf);
        close(descriptor);
        return EXIT_FAILURE;
    }

    size_t count;
    if (elf_getshdrnum(elf, &count) != 0) {
        fprintf(stderr, "Failed to get section count: %s\n", elf_errmsg(-1));
        elf_end(elf);
        close(descriptor);
        return EXIT_FAILURE;
    }

    printf("[DEBUG] Found %ld sections\n", count);

    for (size_t i = 0; i < count; i++) {
        Elf_Scn *scn = elf_getscn(elf, i);
        if (!scn) {
            fprintf(stderr, "Failed to get section %zu: %s\n", i, elf_errmsg(-1));
            continue;
        }

        GElf_Shdr shdr;
        if (!gelf_getshdr(scn, &shdr)) {
            fprintf(stderr, "Failed to get section header for section %zu: %s\n", i, elf_errmsg(-1));
            continue;
        }

//        printf("[DEBUG] Section %zu: Type %d\n", i, shdr.sh_type);

        if (shdr.sh_type == SHT_SYMTAB) {
            Elf_Data *data = elf_getdata(scn, NULL);
            for (size_t j = 0; j < (shdr.sh_size / shdr.sh_entsize); j++) {
                GElf_Sym sym;
                if (!gelf_getsym(data, j, &sym)) {
                    fprintf(stderr, "Failed to get symbol %zu: %s\n", j, elf_errmsg(-1));
                    continue;
                }

                const char *sym_name = elf_strptr(elf, shdr.sh_link, sym.st_name);
                if (sym_name) {
//              	printf("[DEBUG](%ld) Symbol: %s\n", j, sym_name);
              		parse_symbol(sym_name, handle);
                } else {
                    fprintf(stderr, "Failed to get symbol name for symbol %zu\n", j);
                }
            }
            break;
        }
    }

    elf_end(elf);
    close(descriptor);
	dlclose(handle);

    return EXIT_SUCCESS;
}


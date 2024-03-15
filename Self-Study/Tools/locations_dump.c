#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <macho.h>

// Define the struct location
struct location
{
    const char *const fn;
    const char *const file;
    const uint32_t line;
    const uint32_t spare; // Compiler pad-bytes
};

// Define a function to dump the contents of the loc_ids section
void dump_loc_ids(struct location *loc_id_ref, size_t count) {
    FILE *output_file = fopen("loc_ids_dump.txt", "w");
    if (!output_file) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    fprintf(output_file, "Index\tFunction\tFile\tLine\n");
    for (size_t i = 0; i < count; ++i) {
        fprintf(output_file, "%zu\t%s\t%s\t%u\n", i, loc_id_ref[i].fn, loc_id_ref[i].file, loc_id_ref[i].line);
    }

    fclose(output_file);
}

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *binary_file = argv[1];
    int fd;

    macho_file_t *file;

    // Define an external reference to loc_id_ref
    extern struct location loc_id_ref;

    // Calculate the size of the array
    // You need to adjust this according to how you determine the size of your array
    size_t count = 3; // Assuming there are 10 entries in the array

    // Dump the contents of loc_ids section to a file
    dump_loc_ids(&loc_id_ref, count);

    // Cleanup
    close(fd);
    return 0;
}

/*
**
#include <stdio.h>
#include <stdlib.h>
#include <gelf.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SECTION_NAME "loc_ids"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *binary_file = argv[1];
    int fd;
    Elf *elf;

    // Open the binary file
    if ((fd = open(binary_file, O_RDONLY, 0)) < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    // Initialize libelf
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "Failed to initialize libelf\n");
        close(fd);
        return EXIT_FAILURE;
    }

    // Open the ELF file
    if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
        fprintf(stderr, "Failed to open ELF file: %s\n", elf_errmsg(-1));
        close(fd);
        return EXIT_FAILURE;
    }

    // Iterate through sections
    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        // Get section header
        if (gelf_getshdr(scn, &shdr) != &shdr) {
            fprintf(stderr, "Failed to get section header\n");
            elf_end(elf);
            close(fd);
            return EXIT_FAILURE;
        }

        // Get section name
        char *name;
        if ((name = elf_strptr(elf, elf_getshdrstrndx(elf), shdr.sh_name)) == NULL) {
            fprintf(stderr, "Failed to get section name\n");
            elf_end(elf);
            close(fd);
            return EXIT_FAILURE;
        }

        // Check if the section is named loc_ids
        if (strcmp(name, SECTION_NAME) == 0) {
            // Print section info
            printf("Section: %s\n", name);
            printf("Offset: 0x%lx\n", (unsigned long)shdr.sh_offset);
            printf("Size: %lu bytes\n", (unsigned long)shdr.sh_size);
            break;
        }
    }

    // Clean up
    elf_end(elf);
    close(fd);

    return EXIT_SUCCESS;
}


---- Mac/OSX: ---- 1st attempt ----

#define SECTION_NAME "__loc_ids"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *binary_file = argv[1];
    macho_file_t *file;

    // Open the Mach-O file
    if ((file = macho_open(binary_file)) == NULL) {
        fprintf(stderr, "Failed to open Mach-O file: %s\n", macho_strerror());
        return EXIT_FAILURE;
    }

    // Find the loc_ids section
    macho_section_t *section = macho_find_section(file, SECTION_NAME);
    if (section == NULL) {
        fprintf(stderr, "Section '%s' not found\n", SECTION_NAME);
        macho_close(file);
        return EXIT_FAILURE;
    }

    // Print section info
    printf("Section: %s\n", SECTION_NAME);
    printf("Offset: 0x%lx\n", section->offset);
    printf("Size: %lu bytes\n", section->size);

    // Close the file
    macho_close(file);

    return EXIT_SUCCESS;
}

---- Mac/OSX: ---- 2nd attempt ----

#include <stdio.h>
#include <stdlib.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SECTION_NAME "__loc_ids"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *binary_file = argv[1];
    int fd;
    struct stat st;
    void *file_data;

    // Open the binary file
    if ((fd = open(binary_file, O_RDONLY, 0)) < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    // Get the file size
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    // Map the file into memory
    file_data = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    // Get Mach-O header
    struct mach_header_64 *header = (struct mach_header_64 *)file_data;

    // Get the load commands offset
    uintptr_t load_commands_offset = sizeof(struct mach_header_64);
    if (header->magic == MH_MAGIC_64) {
        load_commands_offset = sizeof(struct mach_header_64);
    } else if (header->magic == MH_MAGIC) {
        load_commands_offset = sizeof(struct mach_header);
    } else {
        fprintf(stderr, "Invalid Mach-O file\n");
        munmap(file_data, st.st_size);
        close(fd);
        return EXIT_FAILURE;
    }

    // Iterate through load commands to find the __loc_ids section
    struct load_command *lc = (struct load_command *)((uintptr_t)file_data + load_commands_offset);
    for (uint32_t i = 0; i < header->ncmds; i++) {
        if (lc->cmd == LC_SEGMENT_64) {
            struct segment_command_64 *segcmd = (struct segment_command_64 *)lc;
            struct section_64 *sect = (struct section_64 *)((uintptr_t)segcmd + sizeof(struct segment_command_64));
            for (uint32_t j = 0; j < segcmd->nsects; j++) {
                if (strcmp(sect->sectname, SECTION_NAME) == 0) {
                    // Print section info
                    printf("Section: %s\n", sect->sectname);
                    printf("Offset: 0x%x\n", (uint32_t)sect->offset);
                    printf("Size: %u bytes\n", sect->size);
                    munmap(file_data, st.st_size);
                    close(fd);
                    return EXIT_SUCCESS;
                }
                sect++;
            }
        }
        lc = (struct load_command *)((uintptr_t)lc + lc->cmdsize);
    }

    // Section not found
    fprintf(stderr, "Section '%s' not found\n", SECTION_NAME);
    munmap(file_data, st.st_size);
    close(fd);
    return EXIT_FAILURE;
}
*/

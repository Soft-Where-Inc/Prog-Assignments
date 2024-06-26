/**
 * Sample program to read the ELF-sections info and dump it out.
 *
 * Setup:
 *  $ sudo apt search libelf-dev
 *  $ sudo apt-get install -y libelf-dev/jammy
 *
 * Usage: $ gcc -o locations_dump locations_dump.c -lelf
 *   Run: $ ./locations_dump ./locations_dump
 *        $ ./locations_dump ./locations_example
 *
 * Note: The final output that is displayed after some parsing by
 * dump_loc_ids() is also obtained by:
 *
 *  $ readelf -x .rodata ./locations_example
 *  $ readelf -p .rodata ./locations_example
 *
 * References:
 *
 *  [1] Tutorial: libelf by Example, Joseph Koshy, March, 2012
 *      https://phoenixnap.dl.sourceforge.net/project/elftoolchain/Documentation/libelf-by-example/20200330/libelf-by-example.pdf
 *
 *  [2] Soft-copy of same on my Mac:
 *      ~/Work/Docs/Elf-Tutorial-Ref-libelf-by-example-2012.pdf
 *
 * History:
 *  3/2024  - Restarted; to get something working on Linux-VM
 */
#include <stdio.h>
#include <stdint.h>     // uint32_t etc.
#include <stdlib.h>     // exit(), EXIT_FAILURE etc.
#include <fcntl.h>      // For file open() etc.
#include <unistd.h>     // For file read(), close() etc.
#include <string.h>     // For strncmp() etc.
#include <stdbool.h>    // For _Bool
#include <libelf.h>     // For ELF apis: elf_begin(), elf_kind() etc.
#include <gelf.h>       // For ELF apis: GElf_Shdr{}, gelf_getshdr() etc.

// Define the struct location
struct location
{
    const char *const   fn;
    const char *const   file;
    const uint32_t      line;
    const uint32_t      spare;      // Compiler pad-bytes
    const uint64_t      spare2;     // Compiler pad-bytes
};

/* Is 'str2' equal to null-terminated string 'str1'? */
#define STR_EQ(str1, str2)  (strncmp(str1, str2, strlen(str1)) == 0)

#define REQD_SECTION_NAME       "loc_ids"
#define RODATA_SECTION_NAME     ".rodata"
#define DATA_SECTION_NAME       ".data"

// Section-name matching macros
#define IS_REQD_SECTION(name)       STR_EQ(REQD_SECTION_NAME, (name))
#define IS_RODATA_SECTION(name)     STR_EQ(RODATA_SECTION_NAME, (name))

// Tracing macros

#define STRINGIFY(x)       #x
#define STRINGIFY_VALUE(s) STRINGIFY(s)

// Fabricate a string to track code-location of call-site.
#define __LOC__     "[" __FILE__ ":" STRINGIFY_VALUE(__LINE__) "]"

// Function prototypes
_Bool print_this_section(const char *name);
void prGElf_Shdr(const GElf_Shdr *shdr, Elf_Scn *scn, const char *name);
void prSection_details(const char *name, Elf_Scn *scn, GElf_Shdr *shdr);
void readSection_data(char *buffer, Elf_Scn *scn, GElf_Shdr *shdr);
void hexdump(const void* data, size_t size, size_t sh_addr);

/**
 * *****************************************************************************
 * dump_loc_ids(): Dump the contents of the loc_ids section
 *
 * Parameters:
 *  loc_id_ref  - Array of struct location{} entries
 *  count       - Number of entries in above array.
 *  rodata_buf  - Buffer holding '.rodata' section's data
 *  rodata_addr - Start address (i.e. GElf_Shdr->sh_addr) of .rodata section
 *
 * NOTE: {rodata_buf, rodata_addr} are optional, and can be {NULL,0}
 * When provided, this routine unpacks the loc_id_ref->fn and loc_id_ref->file
 * values as offsets into .rodata buffers to extract the function / file name.
 *
 * The way this works is as follows:
 *
 *             rodata_addr   func_offset (start of function-name)
 *               │           │
 *               ▼           ▼
 *  rodata_buf ->┌──────────────────────────────────────┐
 *               │                                      │
 *               └───────────────────────▲──────────────┘
 *                                       │
 *                                       file_offset (start of file-name)
 *
 * *****************************************************************************
 */
void
dump_loc_ids(struct location *loc_id_ref, size_t count,
             const char *rodata_buf, const size_t rodata_addr)
{
    const char *outfile = "loc_ids_dump.txt";
    printf("\n%s: Dump location-IDs to stdout\n", __LOC__);

    _Bool extract_data = ((rodata_buf != NULL) && (rodata_addr > 0));
    printf("Index\tFunction\tFile\t\tLine\n");
    for (size_t i = 0; i < count; ++i) {
        size_t func_offset = (intptr_t) loc_id_ref[i].fn;
        size_t file_offset = (intptr_t) loc_id_ref[i].file;
        printf("%zu\tfn=0x%lx, \tfile=0x%lx, \tline=%u",
                i, func_offset, file_offset, loc_id_ref[i].line);
        if (extract_data) {
            printf(" fn='%s', file='%s'",
                   (rodata_buf + (func_offset - rodata_addr)),
                   (rodata_buf + (file_offset - rodata_addr)));
        }
        printf("\n");
    }
}

/**
 * *****************************************************************************
 * main() begins here.
 * *****************************************************************************
 */
int
main(const int argc, const char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize libelf
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "Failed to initialize libelf\n");
        return EXIT_FAILURE;
    }
    const char *binary_file = argv[1];

    // Open the binary file
    int fd;
    if ((fd = open(binary_file, O_RDONLY, 0)) < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    // Open the ELF file
    Elf *elf = NULL;
    if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
        fprintf(stderr, "Failed to open ELF file: %s\n", elf_errmsg(-1));
        close(fd);
        return EXIT_FAILURE;
    }

    if (elf_kind(elf) != ELF_K_ELF) {
        fprintf(stderr, "'%s' is not an ELF object.", argv [1]);
        return EXIT_FAILURE;
    }

    // Retrieve the section-index of the ELF section containing the string
    // table of section names.
    size_t shstrndx = 0;
    if (elf_getshdrstrndx (elf, &shstrndx ) != 0) {
        fprintf(stderr, "elf_getshdrstrndx() failed: %s.", elf_errmsg(-1));
        return EXIT_FAILURE;
    }
    printf("%s: shstrndx=%lu\n", __LOC__, shstrndx);

    // Scan all ELF-sections and print brief info about each.
    GElf_Shdr shdr;
    Elf_Scn *scn = NULL;
    char *rodata_buf = NULL;
    size_t rodata_addr = 0;
    char *name = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {

        // Get ELF section's header.
        if (gelf_getshdr(scn, &shdr) != &shdr) {
            fprintf(stderr, "getshdr() failed: %s.", elf_errmsg(-1));
            return EXIT_FAILURE;
        }
        if ((name = elf_strptr(elf, shstrndx, shdr.sh_name)) == NULL ) {

            fprintf(stderr, "elf_strptr() failed: %s.", elf_errmsg(-1));
            return EXIT_FAILURE;
        }

        // Save-off .rodata section's contents, so we can parse through it
        // to extract file/function-names.
        if (IS_RODATA_SECTION(name)) {
            rodata_buf = (char *)malloc(shdr.sh_size);
            readSection_data(rodata_buf, scn, &shdr);
            rodata_addr = shdr.sh_addr;

            prGElf_Shdr(&shdr, scn, name);
            hexdump(rodata_buf, shdr.sh_size, rodata_addr);
        } else if (IS_REQD_SECTION(name)) {

            int nloc_id_entries = 0;
            // Account for alignment bytes left in GElf_Shdr
            nloc_id_entries = ((shdr.sh_size - shdr.sh_addralign)
                                    / sizeof(struct location));

            struct location loc_ids[nloc_id_entries + 1];
            readSection_data((char *) &loc_ids, scn, &shdr);
            prGElf_Shdr(&shdr, scn, name);
            printf("%s expected to have %d entries.\n", name, nloc_id_entries);

            hexdump(&loc_ids, shdr.sh_size, 0);

            dump_loc_ids(&loc_ids[0], nloc_id_entries + 1,
                         rodata_buf, rodata_addr);
        }
    }

    // Cleanup.
    if (rodata_buf) {
        free(rodata_buf);
    }
    elf_end(elf);
    close(fd);
    return 0;
}

/**
 * Helper methods
 */
_Bool
print_this_section(const char *name)
{
    bool rv = (IS_REQD_SECTION(name) || IS_RODATA_SECTION(name));
    return rv;
}

void
prGElf_Shdr(const GElf_Shdr *shdr, Elf_Scn *scn, const char *name)
{
    printf("\nSection %-4.4ld, sh_addr=0x%lx sh_size=%lu, sh_addralign=%lu"
            ", sh_entsize=%lu: %s\n",
            (uintmax_t) elf_ndxscn(scn),
            shdr->sh_addr, shdr->sh_size, shdr->sh_addralign,
            shdr->sh_entsize,
            name);
}

/**
 * prSection_details() - Unpack entries from required named section.
 */
void
prSection_details(const char *name, Elf_Scn *scn, GElf_Shdr *shdr)
{
    if (!scn || !shdr || (shdr->sh_size == 0)) {
        fprintf(stderr, "%s: Illegal inputs: scn=%p, shdr=%p, sh_size=%lu",
                __LOC__, scn, shdr,
                (shdr ? shdr->sh_size : -1));
        return;
    }
    printf("\n%s: Unpacking %lu bytes of contents from '%s' section: \n",
            __LOC__, shdr->sh_size, name);

    int nloc_id_entries = 0;
    _Bool found_reqd_section = false;
    if (strncmp(name, REQD_SECTION_NAME, strlen(REQD_SECTION_NAME)) == 0) {

        // Account for alignment bytes left in GElf_Shdr
        nloc_id_entries = ((shdr->sh_size - shdr->sh_addralign) / sizeof(struct location));
        printf("%s expected to have %d entries.\n", name, nloc_id_entries);
        found_reqd_section = true;
    }

    char buffer[shdr->sh_size];
    readSection_data(buffer, scn, shdr);

    hexdump(buffer, sizeof(buffer), shdr->sh_addr);
    if (found_reqd_section) {
        struct location loc_ids[nloc_id_entries + 1];
        memmove(&loc_ids, buffer, shdr->sh_size);
        dump_loc_ids(&loc_ids[0], nloc_id_entries + 1, (char *) NULL, 0);
    }
    printf("\n");
}


/**
 * readSection_data(): Read section's data into output 'buffer'
 */
void
readSection_data(char *buffer, Elf_Scn *scn, GElf_Shdr *shdr)
{
    if (!buffer || !scn || !shdr || (shdr->sh_size == 0)) {
        fprintf(stderr, "%s: Illegal inputs: "
                "buffer=%p, scn=%p, shdr=%p, sh_size=%lu",
                __LOC__, buffer, scn, shdr,
                (shdr ? shdr->sh_size : -1));
        return;
    }
    char *outp = buffer;
    Elf_Data *data = NULL;
    size_t n = 0;
    while ((n < shdr->sh_size) && (data = elf_getdata(scn, data)) != NULL) {
        memmove(outp, data->d_buf, data->d_size);
        outp += data->d_size;
    }
}

/* Specifiers related to hexdump() print utility */
#define HEXD_NBYTES_PER_LINE    16      // Keep this an even number
#define HEXD_NHALF_BYTES        (HEXD_NBYTES_PER_LINE / 2)

/**
 * hexdump(): Cp'ed from DumpHex() Ref: https://gist.github.com/ccbrown/9722406
 *
 * Enhanced from original version for improved diagnostics.
 *
 * In many cases, caller is likely dumping contents of some memory region
 * after copying it over to some allocated buffer 'data'. To help the user
 * navigate the byte-stream, allow 'sh_addr' to be supplied, which will be
 * the start address of the source buffer. If that's supplied, print the
 * byte-offsets for each chunk of bytes on each line.
 */
void hexdump(const void* data, size_t size, size_t sh_addr) {
    char ascii[HEXD_NBYTES_PER_LINE + 1] = { '\0' };
    size_t i;
    size_t j;
    for (i = 0; i < size; ++i) {
        if ((i % HEXD_NBYTES_PER_LINE) == 0) {
            if (sh_addr) {
                printf("%p 0x%4lx [%4ld]: ", (data + i), (sh_addr + i), i);
            } else {
                printf("%p [%4ld]: ", (data + i), i);
            }
        }
        unsigned char u_curr = ((unsigned char *)data)[i];
        printf("%02x ", u_curr);
        if ((u_curr >= ' ') && (u_curr <= '~')) {
            ascii[i % HEXD_NBYTES_PER_LINE] = u_curr;
        } else {
            ascii[i % HEXD_NBYTES_PER_LINE] = '.';
        }

        size_t inext = i + 1;
        if ((inext % HEXD_NHALF_BYTES) == 0 || (inext == size)) {
            printf(" ");
            if ((inext % HEXD_NBYTES_PER_LINE) == 0) {
                printf("|  %s \n", ascii);
            } else if (inext == size) {
                ascii[inext % HEXD_NBYTES_PER_LINE] = '\0';
                if ((inext % HEXD_NBYTES_PER_LINE) <= HEXD_NHALF_BYTES) {
                    printf(" ");
                }
                for (j = (inext % HEXD_NBYTES_PER_LINE);
                     j < HEXD_NBYTES_PER_LINE; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

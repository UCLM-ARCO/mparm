/*************************************************************************
    Copyright (C) 2002,2003,2004 Wei Qin
    See file COPYING for more information.

    This program is free software; you can redistribute it and/or modify    
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*************************************************************************/

#include "read_elf.h"
#include "loader.h"
#include "nwfpe.h"

#ifdef __COMPILE_SIMULATOR_
#include "armsim.hpp"
#else 
#include "armemul.h"
#endif

#include <cstdlib>
#include <cstring>

void armulator::load_fpe(const char *fname)
{
	const char *filename = (fname==NULL)?FPE_FILE:fname;
	Elf32_Ehdr *hdr;
	Elf32_Phdr *phdr;
	Elf32_Shdr *shdr;
	char *string_table;
	Elf32_Shdr *shdr_new_section;
	Elf32_Word new_section_size, new_section_type, new_section_flags;
	void *new_section;
	Elf32_Addr new_section_addr;
	int i;	
	FILE *fobj;

	fobj = fopen(filename, "rb");
	if(fobj == NULL) {
		fprintf(stderr, "Can't open FPE binary file: %s\n", filename);
		fprintf(stderr, "Did you \"make install\"?\n");
		fprintf(stderr, "Or you may use command line option -f to override the path.\n");
		exit(1);
	}

	hdr = ReadElfHeader(fobj);
	if(hdr == NULL) {
		fprintf(stderr, "Could not read ELF32 header from file: %s.\n", filename);
		exit(1);
	}

#ifndef EM_ARM
#define EM_ARM 40
#endif
	/* check if the file is for ARM */
	if (hdr->e_type != ET_EXEC ||
		hdr->e_machine != EM_ARM ||
		hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		fprintf(stderr, "File is not ARM LSB executable: %s.\n", filename);
		exit(1);
	}

	phdr = ReadProgramHeaders(hdr, fobj);

	shdr = ReadSectionHeaders(hdr, fobj);
	
	if(shdr == NULL) {
		fprintf(stderr, "Can't read section headers from executable\n");
		exit(1);
	}
	string_table = LoadStringTable(hdr, shdr, fobj);

	for(i = 0; i < hdr->e_shnum; i++)
	{
		shdr_new_section = &shdr[i];

		new_section_type = GetSectionType(shdr_new_section);
		new_section_flags = GetSectionFlags(shdr_new_section);
		if ((new_section_type == SHT_PROGBITS) ||
			(new_section_type == SHT_NOBITS))
		{
			new_section_size =
				shdr_new_section ? GetSectionSize(shdr_new_section) : 0;
			new_section_addr = GetSectionAddr(shdr_new_section);

			if (new_section_size && (new_section_flags & SHF_ALLOC))
			{
				if (verbose)
				fprintf(stderr, "Loading %s (%u bytes) at address 0x%08x\n",
					GetSymbolName(shdr[i].sh_name, string_table),
					new_section_size, new_section_addr);

				new_section = malloc(new_section_size);
				LoadSection(shdr_new_section, new_section, fobj);

				/* unitialized section => write 0s */
				if (new_section_type == SHT_NOBITS) {
					mem->set_block_bypass(new_section_addr, 0, new_section_size);	
                                        // Direct write to external memory
				}
				else {
					/* initialized section => copy from objfile */
					mem->write_block_bypass(new_section_addr, new_section, new_section_size);
                                        // Direct write to external memory
				}
				free(new_section);
			}
		}
		else if (new_section_type == SHT_DYNAMIC ||
			new_section_type == SHT_DYNSYM) {
			fprintf(stderr, "File is dynamically linked,"
				" compile with `-static'.\n");
			exit(1);
		}
	}

	/* initialize the fpe workspace */
	mem->set_block_bypass(FPE_FPA11, 0, 0x100);	// Direct write to external memory

	free(string_table);
	free(phdr);
	free(shdr);
	free(hdr);
	fclose(fobj);
}

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
/***************************************************************************
                          loader.c  -  description
                             -------------------
    begin                : Wed Sep 26 2001
    copyright            : (C) 2001 CEA and Universit�Paris XI Orsay
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "read_elf.h"
#include "loader.h"

#ifdef __COMPILE_SIMULATOR_
#include "armsim.hpp"
#else 
#include "armemul.h"
#endif

#include <cstdlib>
#include <cstring>

using emulator::MEMORY_PAGE_SIZE;

void armulator::load_program(const char *filename, 
		int argc, char *argv[], char *envp[])
{
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
	UInt32 data_base = 0, prog_base;
	UInt32 data_size = 0;
	UInt32 envAddr, argAddr;
	UInt32 stack_ptr;

	fobj = fopen(filename, "rb");
	if(fobj == NULL) {
		fprintf(stderr, "Can't open executable: %s\n", filename);
		exit(1);
	}

	hdr = ReadElfHeader(fobj);

	if(hdr == NULL) {
		fprintf(stderr, "Could not read ELF32 header from file: %s.\n",
			   	filename);
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

	for(i=0; i<hdr->e_phnum; i++) {
		if ( (phdr[i].p_type == PT_LOAD) /* Loadable Program Segment */ &&
		 ((phdr->p_flags & PF_X) != 0 /* not text segment => data segment */)) {
		data_base = phdr[i].p_vaddr;
		data_size = phdr[i].p_memsz;
		}
	}
	prog_base = hdr->e_entry;

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
	free(string_table);
	free(phdr);
	free(shdr);
	free(hdr);

	fclose(fobj);

	write_gpr2(PCIND, prog_base/*0x8300*/);
	mem->set_block_bypass(STACK_BASE - STACK_SIZE, 0, STACK_SIZE);		// Direct write to external memory

	stack_ptr = STACK_BASE - MAX_ENVIRON;
	write_gpr2(SPIND, stack_ptr);

	/*write argc to stack*/
	mem->write_block_bypass(stack_ptr,&argc,4);				// Direct write to external memory
	write_gpr2(1, argc);
	stack_ptr += 4;
		
	/*skip stack_ptr past argv pointer array*/
	argAddr = stack_ptr;
	write_gpr2(2, argAddr);
	stack_ptr += (argc+1)*4;

	/*skip env pointer array*/
	envAddr = stack_ptr;
	for (i=0; envp[i]; i++)
		stack_ptr += 4;
	stack_ptr += 4;

	/*write argv to stack*/ 
	for (i=0; i<argc; i++) {
		mem->write_block_bypass(argAddr+i*4,&stack_ptr,4);			// Direct write to external memory
		mem->write_block_bypass(stack_ptr, argv[i], strlen(argv[i]));		// Direct write to external memory
		/*0 already at the end of the string as done by initialization*/
		stack_ptr += strlen(argv[i])+1;
	}

	/*0 already at the end argv pointer array*/

	/*write env to stack*/
	for (i=0; envp[i]; i++) {
		mem->write_block_bypass(envAddr+i*4,&stack_ptr,4);			// Direct write to external memory
		mem->write_block_bypass(stack_ptr, envp[i], strlen(envp[i]));		// Direct write to external memory
		/*0 already at the end of the string as done by initialization*/
		stack_ptr += strlen(envp[i])+1;
	}

	/*0 already at the end argv pointer array*/

	/*stack overflow*/
	if (stack_ptr+4>=STACK_BASE) {
		fprintf(stderr,
			"Environment overflow. Need to increase MAX_ENVIRON.\n");
		exit(1);
	}

	set_brk(mem->align_to_page_boundary(data_base + 
					data_size + MEMORY_PAGE_SIZE));
	set_mmap_brk(MMAP_BASE);

}

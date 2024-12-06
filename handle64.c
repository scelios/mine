#include "nm.h"

int format_check64(Elf64_Ehdr *elf_header, struct stat fd_info)
{
	if (!elf_header)
	{
		ft_putstr_fd(2, "Invalid ELF header\n");
		return -1;
	}

	// check machine version
	if (elf_header->e_machine == EM_NONE)
	{
		ft_putstr_fd(2, "Architecture not handled\n");
		return -1;
	}

	// check header max size
	if ((long unsigned int)fd_info.st_size <= sizeof(Elf64_Ehdr))
	{
		ft_putstr_fd(2, "Symbol table or string table not found\n");
		return -1;
	}
	if (elf_header->e_ident[EI_CLASS] != ELFCLASS64 && elf_header->e_ident[EI_DATA] != ELFDATA2LSB\
		&& elf_header->e_type != ET_EXEC && elf_header->e_type != ET_DYN)
	{	
		ft_putstr_fd(2, "Symbol table or string table not found\n");
		return -1;
	}
	
	// check if e_shnum is within bounds
	if (elf_header->e_shnum >= SHN_LORESERVE)
	{	
		ft_putstr_fd(2, "Too many sections\n");
		return -1;
	}
	return 0;
}

int elf64_symbols(Elf64_Sym sym, Elf64_Shdr *shdr, char *file_data, Elf64_Ehdr *elf_header)
{
	char c = '?';
	uint64_t flags;
	uint64_t bind = ELF64_ST_BIND(sym.st_info);
	uint64_t type = ELF64_ST_TYPE(sym.st_info);
	uint16_t shndx = read_uint16(sym.st_shndx, file_data);
	uint64_t shnum = read_uint16(elf_header->e_shnum, file_data);

	if (bind == STB_GNU_UNIQUE)
		c = 'u';
	else if(type == STT_GNU_IFUNC)
		c = 'i';
	else if (bind == STB_WEAK)
	{
		if(type == STT_OBJECT)
			c = (shndx == SHN_UNDEF) ? 'v' : 'V';
		else
			c = (shndx == SHN_UNDEF) ? 'w' : 'W';
	}
	else if (bind == STB_WEAK)
	{
		c = 'W';
		if (shndx == SHN_UNDEF)
			c = 'w';
	}
	else if (sym.st_shndx == SHN_UNDEF)
		c = 'U';
	else if (sym.st_shndx == SHN_ABS)
		c = 'A';
	else if (sym.st_shndx == SHN_COMMON)
		c = 'C';
	else if (shndx < shnum)
	{
		type = read_uint64(shdr[shndx].sh_type, file_data);
		flags = read_uint64(shdr[shndx].sh_flags, file_data);

		if (type == SHT_NOBITS)
			c = 'B';
		else if (!(flags & SHF_WRITE))
		{
		if(flags & SHF_ALLOC && flags & SHF_EXECINSTR)
			c = 'T';
		else
			c = 'R';
		}
		else if(flags & SHF_EXECINSTR)
			c = 'T';
		else
			c = 'D';
	}
	if (bind == STB_LOCAL && c != '?')
		c += 32;
	return c;
}

void printTable(t_sym *tab, size_t tab_size)
{
	for (size_t i = 0; i < tab_size; i++)
	{
		if (tab[i].shndx == SHN_UNDEF)
		{
			ft_putstr_fd(1, "                 ");
			write(1, &tab[i].letter, 1);
			ft_putstr_fd(1, " ");
			ft_putstr_fd(1, tab[i].name);
			ft_putstr_fd(1, "\n");
			// printf("%16c %c %s\n", ' ', tab[i].letter, tab[i].name);
		}
		else
		{
			ft_putnbr_base_printf(tab[i].addr, "0123456789abcdef",1);
			ft_putstr_fd(1, " ");
			write(1, &tab[i].letter, 1);
			ft_putstr_fd(1, " ");
			ft_putstr_fd(1, tab[i].name);
			ft_putstr_fd(1, "\n");
			// printf("%016lx %c %s\n", tab[i].addr, tab[i].letter, tab[i].name);
		}
	}
}
int handle64_symtab(Elf64_Shdr *section_h, Elf64_Ehdr *elf_header, char *file_data, int n)
{
	uint64_t sh_offset = read_uint64(section_h[n].sh_offset, file_data);
	uint64_t sh_link = read_uint32(section_h[n].sh_link, file_data);
	Elf64_Sym *symtab = (Elf64_Sym *)(file_data + sh_offset);
	size_t symtab_size = read_uint64(section_h[n].sh_size, file_data) / read_uint64(section_h[n].sh_entsize, file_data);
	char *strtab = file_data + read_uint64(section_h[sh_link].sh_offset, file_data);
	t_sym *tab = malloc(sizeof(t_sym) * symtab_size);
	size_t tab_size = 0;
	if (!tab)
	{
		ft_putstr_fd(2, "Memory allocation failed\n");
		return -1;
	}
	for (size_t i = 1; i < symtab_size; i++)
	{
		uint64_t type = ELF64_ST_TYPE(symtab[i].st_info);
		if (type == STT_FUNC || type == STT_OBJECT || type == STT_NOTYPE || type == STT_GNU_IFUNC || type == STT_TLS)
		{
			tab[tab_size].addr = read_uint64(symtab[i].st_value, file_data);
			tab[tab_size].letter = elf64_symbols(symtab[i], section_h, file_data, elf_header);
			tab[tab_size].shndx = read_uint16(symtab[i].st_shndx, file_data);
			if (!str_is_nullterm(strtab + read_uint32(symtab[i].st_name, file_data), strtab + read_uint64(section_h[sh_link].sh_size, file_data)))
				tab[tab_size].name = "(null)";
			else
				tab[tab_size].name = strtab + read_uint32(symtab[i].st_name, file_data);
			tab_size++;
		}
	}

	sort(tab, tab_size);
	printTable(tab, tab_size);
	
	free(tab);
	return 0;
}

int handle64(char *file_data, Elf64_Ehdr *elf_header, struct stat fd_info)
{
	uint64_t sh_type;
	uint64_t offset = read_uint64(elf_header->e_shoff, file_data);
	uint64_t nEntrie = read_uint16(elf_header->e_shnum, file_data);
	uint16_t index = read_uint16(elf_header->e_shstrndx, file_data);
	Elf64_Shdr *section_h = (Elf64_Shdr *)(file_data + offset);

	if (format_check64(elf_header, fd_info) < 0)
		return -1;
	
	for (uint64_t i = 0; i < nEntrie; i++)
	{
		if (read_uint32(section_h[i].sh_name, file_data) > read_uint64(section_h[index].sh_size, file_data))
		{
			ft_putstr_fd(2, "Bad section table header\n");
			return 1;
		}

		sh_type = read_uint32(section_h[i].sh_type, file_data);
		if (sh_type == SHT_SYMTAB)
			return handle64_symtab(section_h, elf_header, file_data, i);
	}
	ft_putstr_fd(2, "Symbol table or string table not found\n");
	return 1;
}


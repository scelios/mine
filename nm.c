#include "nm.h"

int ft_nm(int fd, char *filename) {
	struct stat file_info;
	char *data;

	if (fstat(fd, &file_info) < 0 || S_ISDIR(file_info.st_mode))
	{
		ft_putstr_fd(2, "ft_nm: '");
		ft_putstr_fd(2, filename);
		ft_putstr_fd(2, "': Unable to stat file\n");
		return 1;
	}

	data = mmap(NULL, file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED)
	{
		ft_putstr_fd(2, "ft_nm: '");
		ft_putstr_fd(2, filename);
		ft_putstr_fd(2, "': Unable to map file\n");
		return 1;
	}


	if (data[EI_MAG0] == ELFMAG0 && data[EI_MAG1] == ELFMAG1 && data[EI_MAG2] == ELFMAG2 && data[EI_MAG3] == ELFMAG3)
	{
		if (data[EI_CLASS] == ELFCLASS64)
			return handle64(data, (Elf64_Ehdr *)data, file_info);
		else if (data[EI_CLASS] == ELFCLASS32)
			return handle32(data, (Elf32_Ehdr *)data, file_info);
	}
	else
	{
		ft_putstr_fd(2, "ft_nm: '");
		ft_putstr_fd(2, filename);
		ft_putstr_fd(2, "': File format not recognized\n");
		return 1;
	}

	if (munmap(data, file_info.st_size) < 0)
	{
		ft_putstr_fd(2, "ft_nm: '");
		ft_putstr_fd(2, filename);
		ft_putstr_fd(2, "': Unable to unmap file\n");
		return 1;
	}
	return 0;
}

int main(int a, char **b) {
	int fd;
	int ret;

	if (a == 1)
	{
		fd = open("a.out", O_RDONLY);
		if (fd == -1)
		{
			ft_putstr_fd(2, "ft_nm: 'a.out': Unable to unmap file\n");
			exit(1);
		}
		ret = ft_nm(fd, "a.out");
		close(fd);
	}
	else
	{
		for (int i = 1; i < a; i++)
		{
			fd = open(b[i], O_RDONLY);
			if (fd == -1)
			{
				ft_putstr_fd(2, "ft_nm: '");
				ft_putstr_fd(2, b[i]);
				ft_putstr_fd(2, "': Unable to unmap file\n");
				exit(1);
			}
			ret = ft_nm(fd, b[i]);
			close(fd);
		}
	}

	ret == 0 ? exit(0) : exit(1);
}
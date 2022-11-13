#include "../clap/clap/clap.h"

typedef struct
{
	char name;
	size_t* disks;
	size_t size;
} Tower;

Tower* tower_create(char name, size_t disks)
{
	Tower* tower = malloc(sizeof(Tower));
	tower->name = name;
	tower->disks = calloc(disks, sizeof(size_t));
	tower->size = disks;
	return tower;
}

void tower_delete(Tower* tower)
{
	free(tower->disks);
	free(tower);
}

size_t tower_get_top_index(Tower* tower)
{
	for (size_t i = 0; i < tower->size; i++)
	{
		if (tower->disks[tower->size - 1 - i])
			return tower->size - 1 - i;
	}

	return 0;
}

void tower_print(Tower* tower)
{
	printf("Tower %c:\n", tower->name);
	
	for (size_t i = 0; i < tower->size; i++)
	{
		size_t size = tower->disks[tower->size - 1 - i];
		if (size > 0)
			printf("%zu\n", size);
	}

	printf("\n");
}

void tower_push(Tower* tower, size_t size)
{
	size_t top_index = tower_get_top_index(tower);
	size_t top_size = tower->disks[top_index];

	if (top_index == tower->size - 1)
	{
		printf("Error: Tried pushing disk on full tower!\n");
		return;
	}

	if (top_size > 0 && top_size < size)
	{
		printf("Error: Tried pushing larger disk onto smaller disk!\n");
		return;
	}

	tower->disks[top_index + (top_index == 0 && top_size == 0 ? 0 : 1)] = size;
}

size_t tower_pop(Tower* tower)
{
	size_t top_index = tower_get_top_index(tower);
	size_t size = tower->disks[top_index];
	tower->disks[top_index] = 0;
	return size;
}

size_t tower_peek(Tower* tower)
{
	return tower->disks[tower_get_top_index(tower)];
}

void tower_move_disk(Tower* from, Tower* to)
{
	size_t size = tower_pop(from);
	tower_push(to, size);
}

void print(Tower* A, Tower* B, Tower* C)
{
	printf("---------------\n");
	printf("Tower of Hanoi:\n\n");
	tower_print(A);
	tower_print(B);
	tower_print(C);
}

void solve_tower_of_hanoi_instructions(size_t n, Tower* from, Tower* to, Tower* helper)
{
	if (n == 1)
	{
		tower_move_disk(from, to);
		printf("Moving disk %zu from Tower %c to Tower %c\n", tower_peek(from), from->name, to->name);
		return;
	}

	solve_tower_of_hanoi_instructions(n - 1, from, helper, to);

	tower_move_disk(from, to);
	printf("Moving disk %zu from Tower %c to Tower %c\n", tower_peek(from), from->name, to->name);

	solve_tower_of_hanoi_instructions(n - 1, helper, to, from);
}

void solve_tower_of_hanoi_visual(size_t n, Tower* from, Tower* to, Tower* helper, Tower* A, Tower* B, Tower* C)
{
	if (n == 1)
	{
		tower_move_disk(from, to);
		print(A, B, C);
		return;
	}
	
	solve_tower_of_hanoi_visual(n - 1, from, helper, to, A, B, C);

	tower_move_disk(from, to);
	print(A, B, C);

	solve_tower_of_hanoi_visual(n - 1, helper, to, from, A, B, C);
}

int main(int argc, char** argv)
{
	Tower* A = NULL;
	Tower* B = NULL;
	Tower* C = NULL;
	size_t solve_size = 0;
	const char* mode = "instructions";

	clapRegisterFlag("help", 'h', CLAP_FLAG_OPT_ARG, NULL);
	clapRegisterFlag("disks", CLAP_FLAG_NO_SHORT, CLAP_FLAG_REQ_ARG, NULL);
	clapRegisterFlag("mode", CLAP_FLAG_NO_SHORT, CLAP_FLAG_REQ_ARG, NULL);

	while (clapParse(argc, argv))
	{
		if (clapParsedFlag("help", 'h'))
		{
			const char* arg = clapGetArg();
			if (arg)
			{
				if (strcmp(arg, "disks") == 0)
					printf("[ToH]: Flag Info: --disks\n- No short flag\n- Requiered argument\n- Argument: The number of disks to solve for.\n");
				else if (strcmp(arg, "mode") == 0)
					printf("ToH]: Flag Info: --mode\n- No short flag\n- Requiered argument\n- Argument: \"instructions\" or \"visual\".\n");
			}
			else
				printf("[ToH]: This program supports the following flags: --disks, --mode\n");

			return 0;
		}
		
		if (clapParsedFlag("disks", CLAP_FLAG_NO_SHORT))
		{
			size_t disks = atoi(clapGetArg());
			solve_size = disks;
			A = tower_create('A', 10);
			B = tower_create('B', 10);
			C = tower_create('C', 10);

			do
			{
				tower_push(A, disks--);
			} while (disks > 0);
		}

		if (clapParsedFlag("mode", CLAP_FLAG_NO_SHORT))
			mode = clapGetArg();
	}

	if (!A || !B || !B)
	{
		printf("[ToH]: Error: Please specify the --disks flag!\n");
		return 1;
	}

	if (strcmp(mode, "instructions") == 0)
		solve_tower_of_hanoi_instructions(solve_size, A, C, B);
	else if (strcmp(mode, "visual") == 0)
	{
		print(A, B, C);
		solve_tower_of_hanoi_visual(solve_size, A, C, B, A, B, C);
	}
	else
		printf("[ToH]: Error: Invalid argument for flag --mode!\n");

	tower_delete(A);
	tower_delete(B);
	tower_delete(C);
}
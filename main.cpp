#include"struct.h"

void CreateSignalProcess(struct Data *data)
{
	data->count = 0;
	HANDLE Event = CreateEvent(NULL, FALSE, FALSE, "Event");

	if (Event != NULL)
	{
		Sleep(1000);
		SetEvent(Event);
	}
	else
		printf("CreateEvent() error: %d\n", GetLastError());
}

void CreateNewProcess(char *path, struct Data *data, struct Stack **stack)
{

	char *numberCh;
	numberCh = (char*)malloc(10);
	int c = data->count;
	data->count = ++c;
	sprintf(numberCh, "%d", data->count);

	char *str = NULL;
	str = (char*)malloc(sizeof(char) * 100);
	strcpy(str, path);

	strcat(str, " ");
	strcat(str, numberCh);

	STARTUPINFO stInfo;
	PROCESS_INFORMATION prInfo;

	ZeroMemory(&stInfo, sizeof(STARTUPINFO));
	stInfo.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, str, NULL, NULL, FALSE, 0, NULL, NULL, &stInfo, &prInfo))
	{
		data->count = --c;
		exit(0);
	}


	struct Data data_pr;
	data_pr.prInfo = prInfo;

	push(*&stack, data_pr);
}

void CloseLastProcess(struct Stack **stack, struct Data *data)
{
	HANDLE Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event");

	if (Event == NULL)
		exit(0);

	WaitForSingleObject(Event, INFINITE);

	TerminateProcess((*stack)->hand, 0);
	pop(*&stack);
	int c = data->count;
	data->count = --c;

	SetEvent(Event);
}

void CloseAllProcesses(struct Stack **stack, struct Data *data)
{
	HANDLE Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event");

	if (Event == NULL)
		exit(0);

	WaitForSingleObject(Event, INFINITE);

	while (size(*stack) != 0)
	{
		TerminateProcess((*stack)->hand, 0);
		pop(*&stack);
		int c = data->count;
		data->count = --c;
	}
}

void PrintProcesses(char **argv, struct Data *data)
{
	HANDLE Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event");
	while (1)
	{
		WaitForSingleObject(Event, INFINITE);

		printf("Number of process %s\n\n", argv[1]);

		SetEvent(Event);
		Sleep(1000);
	}
}



int main(int argc, char *argv[])
{
	struct Stack *stack = NULL;
	struct Data data;

	if (argc == 1)
	{
		CreateSignalProcess(&data);

		while (1)
		{
			switch (_getch())
			{
			case '+':
				CreateNewProcess(argv[0], &data, &stack);
				break;
			case '-':
				if (size(stack) != 0)
					CloseLastProcess(&stack, &data);
				break;
			case 'q':
				CloseAllProcesses(&stack, &data);
				return 0;
				break;
			}
		}
	}
	else
		PrintProcesses(argv, &data);

	return 0;
}

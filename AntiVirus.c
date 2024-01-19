#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>


#define FALSE 0
#define TRUE !FALSE
#define INFECTED 1
#define INFECTED_20 2
#define INFECTED_80 3
#define NOT_INFECTED 0
#define READING_ERROR -1
#define LOG_FILE "AntiVirusLog.txt"

char* logPath = 0;

char** getFileNames(char* folder, char** fileNames, int* numFiles);
char** alphabeticNames(char** fileNames);
int showMenu(char* folder, char* signature);
int getFileSize(FILE* fp);
int scanFiles(char* fileNames, int numFiles, char* signature, int mode);
int searchSignature(FILE* fp, FILE* signFile, int currLoc, int size);
char* getResult(int result);
char* createLog(char* path);
void updateLogMessage(char* msg);
void updateLogScanResult(char* fileName, int result);



int main(int argc, char** argv)
{
	int i = 0;
	int mode = 0;
	int numFiles = 0;
	int result = 0;
	char** fileNames = { 0 };
	logPath = createLog(argv[1]);
	updateLogMessage("Anti-virus began! Welcome!\n");
	updateLogMessage("Folder to scan: ");
	updateLogMessage(argv[1]);
	updateLogMessage("Virus signature: ");
	updateLogMessage(argv[2]);
	fileNames = (char**)malloc(sizeof(char*));
	fileNames = getFileNames(argv[1], fileNames, &numFiles);
	alphabeticNames(fileNames, numFiles);
	mode = showMenu(argv[1], argv[2]);
	printf("Scanning began...\nThis process may take several minutes...\n\n");
	printf("Scanning:\n");
	updateLogMessage("Results:");
	for (i = 0; i < numFiles; i++)
	{
		result = scanFiles((fileNames[i]), numFiles, argv[2], mode);
		printf("%s\n", getResult(result));
		updateLogScanResult(fileNames[i], result);
	}
	printf("Scan Completed.\n");
	printf("See log path for results: ");
	puts(logPath);
	for (i = 0; i < numFiles; i++)
	{
		free(fileNames[i]);
	}
	free(fileNames);
	free(logPath);
	return 0;
}


char** getFileNames(char* folder, char** fileNames, int* numFiles)
{
	int fileLen = 0;
	int pathLen = strlen(folder);
	DIR* dir = opendir(folder);
	if (dir == NULL)
	{
		printf("Directory not found!");
		exit(1);
	}
	struct dirent* entity;
	entity = readdir(dir);
	while (entity != NULL)
	{
		if (entity->d_type != DT_DIR && strcmp(entity->d_name, LOG_FILE) != 0)
		{
			fileLen = strlen(entity->d_name);
			*numFiles += 1;
			fileNames = (char**)realloc(fileNames, *numFiles * sizeof(char*));
			fileNames[*numFiles - 1] = (char*)malloc((pathLen + fileLen + 2) * sizeof(char));
			strcpy(fileNames[*numFiles-1], folder);
			strcat(fileNames[*numFiles - 1], "\\");
			strcat(fileNames[*numFiles - 1], entity->d_name);
		}
		entity = readdir(dir);
	}
	closedir(dir);
	return fileNames;
}


char** alphabeticNames(char** fileNames, int numFiles)
{
	int i = 0, j = 0;
	char* temp = 0;

	for (i = 0; i < numFiles; i++)
	{
		for (j = 0; j < numFiles - 1; j++)
		{
			if (strcmp(fileNames[j], fileNames[i]) > 0)
			{
				temp = fileNames[i];
				fileNames[i] = fileNames[j];
				fileNames[j] = temp;
			}
		}
	}

	return fileNames;
}


int showMenu(char* folder, char* signature)
{
	int option = 0;
	printf("Welcome to my Virus Scan!\n\n");
	printf("Folder to scan: %s\n", folder);
	printf("Virus signature: %s\n\n", signature);
	printf("Press 0 for a normal scan or any other key for a quick scan: ");
	scanf("%d", &option);
	updateLogMessage("\nScanning option:");
	if (option)
	{
		updateLogMessage("Quick Scan\n");
	}
	else
	{
		updateLogMessage("Normal Scan\n");
	}
	return option;
}


int scanFiles(char* fileNames, int numFiles, char* signature, int mode)
{
	int size = 0;
	int result = 0;
	FILE* fp = 0;
	FILE* signFile = 0;
	fp = fopen(fileNames, "rb");
	signFile = fopen(signature, "rb");
	if (!fp || !signFile)
	{
		printf("Error opening file\n");
		return READING_ERROR;
	}
	printf("%s - ", fileNames);
	size = getFileSize(fp);
	if (!mode)
	{
		if (searchSignature(fp, signFile, 0, size))
		{
			result = INFECTED;
		}
	}
	else
	{
		if (searchSignature(fp, signFile, 0, (size * 0.2)))
		{
			result = INFECTED_20;
		}
		else if (searchSignature(fp, signFile, (size * 0.8), size))
		{
			result = INFECTED_80;
		}
	}
	fclose(fp);
	fclose(signFile);
	return result;
}


int searchSignature(FILE* fp, FILE* signFile, int currLoc, int size)
{
	int fileCh = 0;
	int signCh = 0;
	int match = TRUE;
	int infected = FALSE;
	while (currLoc <= size)
	{
		fseek(fp, currLoc, SEEK_SET);
		fseek(signFile, 0, SEEK_SET);
		while (TRUE)
		{
			fileCh = fgetc(fp);
			signCh = fgetc(signFile);
			if (signCh == EOF)
			{
				infected = TRUE;
				break;
			}
			if (fileCh != signCh)
			{
				match = FALSE;
				break;
			}
		}
		if (infected)
		{
			break;
		}
		currLoc++;
	}
	return infected;
}


char* getResult(int result)
{
	char* resultString = 0;
	switch (result)
	{
		case INFECTED:
			resultString = "Infected!";
			break;
		case INFECTED_20:
			resultString = "Infected! (First 20%)";
			break;
		case INFECTED_80:
			resultString = "Infected! (Last 20%)";
			break;
		case NOT_INFECTED:
			resultString = "Clean";
			break;
		case READING_ERROR:
			resultString = "Reading Error";
			break;
		default:
			resultString = "Unknown error. Please contact costumer service";
			break;
	}
	return resultString;
}


char* createLog(char* path)
{
	int pathLen = strlen(path);
	int logLen = strlen(LOG_FILE);
	char* logPath = (char*)malloc((pathLen + logLen + 2) * sizeof(char));
	strcpy(logPath, path);
	strcat(logPath, "\\");
	strcat(logPath, LOG_FILE);
	FILE* logFile = fopen(logPath, "w");
	fclose(logFile);
	return logPath;
}


void updateLogMessage(char* msg)
{
	FILE* fp;
	fp = fopen(logPath, "a");
	if (fp)
	{
		fprintf(fp, "%s\r\n", msg);
		fclose(fp);
	}
}


void updateLogScanResult(char* fileName, int result)
{
	FILE* fp = 0;
	fp = fopen(logPath, "ab");

	if (fp)
	{
		fputs(fileName, fp);
		fprintf(fp, "  %s\r\n", getResult(result));
		fclose(fp);
	}
}


int getFileSize(FILE* fp)
{
	int ch = 0;
	int size = 0;
	fseek(fp, 0, SEEK_SET);
	while (ch = fgetc(fp) != EOF)
	{
		size++;
	}
	rewind(fp);
	return size;
}


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fd;
FILE *fk;


typedef struct{
    int id; //Primary Key of the record
    char fName[15]; //Name
    char lName[15]; //Surname
}record;

record buffer[100];
record reset={-1, "", ""};
char query[15];
char choice;
int maxIndex;

int getNumRecords(){    //Returns the number of saved records
    fd=fopen("Data.txt", "r+b");
    long curPos, fileSize;
    curPos = ftell(fd);
    fseek(fd, 0, SEEK_END);
    fileSize=ftell(fd);
    fseek(fd, curPos, 0);
    fclose(fd);
    return (fileSize/sizeof(record));
}

void openDataFile(){
    fd = fopen("Data.txt", "a+b");
}

void openKeyFile(){
    fk = fopen("Key.txt", "w+b");
}

void closeDataFile(){
    fclose(fd);
}

void closeKeyFile(){
    fclose(fk);
}

void writeDataFile(){   //Adds a record to the Data File
    openDataFile();
    printf("Insert \"END\" to save file and exit \n");
    for(int i=maxIndex; i<=100; i++){
        buffer[i].id=i;
        printf("\nEnter First Name: ");
        scanf("%s", buffer[i].fName); //L
        if(strcmp(buffer[i].fName, "END") == 0){  //Checks if the user inputs the command needed to end the writing procedure
            printf("\n");
            printf("Stopped Writing Data \n\n");
            break;
        }
        printf("\nEnter Last Name: ");
        scanf("%s", buffer[i].lName); //Reads the input string
        if(strcmp(buffer[i].lName, "END") == 0){  //Checks if the user inputs the command needed to end the writing procedure
            printf("\n");
            printf("Stopped Writing Data \n\n");
            break;
        }
        fseek(fd, 0, SEEK_END);
        fwrite(&buffer[i], sizeof(record), 1, fd);    //Writes the record on file
    }
    closeDataFile();
}

void readDataFile(){    //Reads the Data File and adds its content to a buffer
    openDataFile();
    fread(&buffer, sizeof(record), maxIndex, fd);
    for(int i=0; i<maxIndex; i++){
	    printf ("ID: %d => %s %s\n\n", buffer[i].id, buffer[i].fName, buffer[i].lName);
    }
    printf("\n");
    closeDataFile();
}

void writeKeyFile(){    //Creates the Index File and sorts it
    record V[100]={0};
    maxIndex=getNumRecords();
    char n[15];
    int j;
    openDataFile();
    fread(&buffer, sizeof(record), maxIndex, fd);
    closeDataFile();

    for(int i=0; i<maxIndex; i++){
        for(int k=0; k<15; k++)
            n[k]=buffer[i].fName[k];
        j=i;
        while(j>0 && strcmp(n, V[j-1].fName)<0){
            V[j]=V[j-1];
            j--;
        }
        V[j]=buffer[i];
    }
    openKeyFile();
    for(int x=0; x<maxIndex; x++){
        fwrite(&V[x], sizeof(record), 1, fk);
    }
    closeKeyFile();
}

void readKeyFile(){ //Reads the Index File and adds its content to a buffer
    fk = fopen("Key.txt", "a+b");
    fread(&buffer, sizeof(record), maxIndex, fk);
    for(int i=0; i<maxIndex; i++){
	    printf ("%s %s  Pointer: %d \n\n", buffer[i].fName, buffer[i].lName, buffer[i].id );
    }
    printf("\n");
    closeKeyFile();
}

void binarySearch(char query[15]){  //Searches via Binary Search the record's location from the Index file and prints it
    int nMin, nMax, nMid;
    fk = fopen("Key.txt", "a+b");
    fread(&buffer, sizeof(record), maxIndex, fk);
    closeKeyFile();
    nMin = 0;
    nMax = maxIndex - 1;
    nMid = (nMin+nMax)/2;

    while (nMin <= nMax) {
        if (strcmp(buffer[nMid].fName, query)<0)
        nMin = nMid + 1;
        else if(strcmp(buffer[nMid].fName, query)==0){
            for(int i=nMid-1; strcmp(buffer[i].fName, query)==0; i--){
                printf("\"%s %s\" found at position %d \n\n", query, buffer[i].lName, buffer[i].id);
            }
            printf("\"%s %s\" found at position %d \n\n", query, buffer[nMid].lName, buffer[nMid].id);
            for(int j=nMid+1; strcmp(buffer[j].fName, query)==0; j++){
                printf("\"%s %s\" found at position %d \n\n", query, buffer[j].lName, buffer[j].id);
            }
            break;
        }
        else
            nMax = nMid - 1;

        nMid = (nMin + nMax)/2;
    }
    if (nMin > nMax)
        printf("\"%s\" can't be found\n", query);
}

void deleteRecord(int idDelete){    //Deletes a record by overwriting it with an empty record
    openDataFile();
    fread(&buffer, sizeof(record), maxIndex, fd);
    closeDataFile();
    fopen("Data.txt", "wb");
    buffer[idDelete]=reset;
    for(int i=idDelete; i<maxIndex; i++){
        buffer[i]=buffer[i+1];
        buffer[i].id-=1;
        buffer[maxIndex]=reset;
    }
    for(int i=0; i<maxIndex; i++){
        if(buffer[i].id > -1)
            fwrite(&buffer[i], sizeof(record), 1, fd);
    }
    closeDataFile();
    writeKeyFile();
    printf("\nDeleted Record\n");
}

int main()
{

    bool exitFlag=false;
    int idDelete;
    char delFlag;


    do{ //Prints the possible commands
        printf("Menu: \n\n");
        printf("    1: Add Record\n\n");
        printf("    2: Search Record\n\n");
        printf("    3: Delete Record\n\n");
        printf("    4: Delete All Records\n\n");
        printf("    5: Print All Records\n\n");
        printf("    6: Print All Keys\n\n");
        printf("    7: Print Total Number of Records\n\n");
        printf("    8: Exit\n\n");

        maxIndex=getNumRecords();
        choice=getchar();
        printf("\n");

        switch(choice){ //Menu

        //Add Record
        case '1' :
        writeDataFile();
        writeKeyFile();
        break;

        //Search Record
        case '2':
        printf("Insert Record to Search:  ");
        scanf("%s", query);
        printf("\n\n");
        binarySearch(query);
        break;

        //Delete Record
        case '3':
        printf("Insert ID of Record to Delete:  ");
        maxIndex=getNumRecords();
        scanf("%d", &idDelete);
        if(idDelete >= maxIndex)
            printf("\n\nERROR: Index not Found\n\n");
        else{
            deleteRecord(idDelete);
            printf("\n\n");
        }
        break;

        //Delete All Records
        case '4':
        printf("\n\nThis action is Irreversible\nAre you Sure? [Y/N]\n\n");
        delFlag=getchar();
        delFlag=getchar();
        if(delFlag == 'Y' || delFlag == 'y'){
            fd = fopen("Data.txt", "w+b");
            closeDataFile();
            writeKeyFile();
            printf("\nDeleted All Records\n\n");
        }
        else{
            printf("\nDeletion of Records has been cancelled\n\n");
        }
        break;

        //Print All Records
        case '5':
        readDataFile();
        break;

        //Print All Keys
        case '6':
        readKeyFile();
        break;

        //Print Number of Records
        case '7':
        printf("Total Records: %d \n\n", getNumRecords());
        break;

        //Exit
        case '8':
        exitFlag=true;
        break;
        }

        choice=getchar();   //Clears the buffer

    }while(!exitFlag);  //Prints the command menu until the user exits via appropriate command

    return 0;
}


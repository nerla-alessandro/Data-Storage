#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define prime 101

using namespace std;

FILE *fd;

struct record{
    char name[20];
    int strHash;
};

record buffer[prime];
record reset={"", -1};

bool isPosFree(int n){
    fread(&buffer, sizeof(record), prime, fd);
    if(strcmp(buffer[n].name, "") == 0)
        return true;
    else
        return false;
}

int toHash(char s[20]){
    int h=1;
    for(int i=0; i<20; i++){
        if(s[i]==0)
            break;
        if(i%2==0)
            h =((h*997) - (s[i]*997)) % prime;
        else
            h =((h*997) + (s[i]*997)) % prime;
    }
    return abs(h);
}

void openDataFile(){
    fd = fopen("Data.txt", "w+b");
}

void closeDataFile(){
    fclose(fd);
}

void writeStart(){
    fwrite(&buffer, sizeof(record), prime, fd);
    closeDataFile();
    fd = fopen("Data.txt", "r+b");
    fread(&buffer, sizeof(record), prime, fd);
    closeDataFile();
    openDataFile();
}

void initializeFile(){
    if( fopen("Data.txt", "r" ) == NULL ) {
        for(int i=0; i<prime; i++){
            buffer[i]=reset;
        }
        openDataFile();
        fwrite(&buffer, sizeof(record), prime, fd);
        closeDataFile();
        printf("File has been created and initialized \n\n");
    }
    else{
        printf("File already exists and has not been initialized \n\n");
    }
}

int searchRecord(char query[20], bool printFlag){
    fd = fopen("Data.txt", "r+b");
    int c=1;
    int jmp;
    fread(&buffer, sizeof(record), prime, fd);
    closeDataFile();
    int t = toHash(query);
    jmp = t;
    if(strcmp(buffer[t].name, query) != 0){
        do{
            jmp = t + (c*c);
            jmp %= prime;
            c++;
        }while(strcmp(buffer[jmp].name, query) != 0 && c<prime);
        if(c>=prime){
            printf("\nRecord not found\n\n");
            return prime+1;
        }
    }
    if(printFlag)
        printf("\nRecord found at position %d\n\n", jmp);
    else
        return jmp;

}

void deleteRecord(char query[20]){
    int index = searchRecord(query, false);
    if(index == prime+1){
        printf("ERROR: Record does not exist\n\n");
    }
    else{
    fd = fopen("Data.txt", "r+b");
    fread(&buffer, sizeof(record), prime, fd);
    closeDataFile();
    buffer[index]=reset;
    openDataFile();
    fwrite(&buffer, sizeof(record), prime, fd);
    closeDataFile();
    printf("Record deleted\n\n");
    }
}

void deleteAllRecords(){
    char delFlag;
    printf("\n\nThis action is Irreversible\nAre you Sure? [Y/N]\n\n");
    delFlag=getchar();
    delFlag=getchar();
    if(delFlag == 'Y' || delFlag == 'y'){
        for(int i=0; i<prime; i++){
            buffer[i]=reset;
        }
        openDataFile();
        fwrite(&buffer, sizeof(record), prime, fd);
        closeDataFile();
        printf("\nDeleted all Records\n\n");
    }
    else
        printf("\nDeletion of Records has been cancelled\n\n");
}

void writeDataFile(){
    record temp;
    int t;
    int jmp;
    int c;
    fd = fopen("Data.txt", "r+b");
    fread(&buffer, sizeof(record), prime, fd);
    closeDataFile();
    openDataFile();
    writeStart();
    printf("Insert \"END\" to save file and exit \n");
    for(int i=0; i<prime; i++){
        c=1;
        printf("\nEnter Data: ");
        scanf("%s", temp.name);
        if(strcmp(temp.name, "END") == 0){
            printf("\n");
            fwrite(&buffer, sizeof(record), prime, fd);
            printf("Stopped Writing Data \n\n");
            break;
        }
        t=toHash(temp.name);
        if(isPosFree(t)){
            temp.strHash=t;
            buffer[t]=temp;
            printf("Hash: %d", t);
        }
        else{
            do{
                jmp = t + (c*c);
                jmp %= prime;
                c++;
            }while(isPosFree(jmp) == false);
            temp.strHash=jmp;
            buffer[jmp]=temp;
            printf("Hash Collision Detected - Record Saved with ID: %d", jmp);
        }
        writeStart();
    }
    closeDataFile();
}

void readDataFile(){
    fd = fopen("Data.txt", "r+b");
    fread(&buffer, sizeof(record), prime, fd);
    for(int i=0; i<prime; i++){
        if(strcmp(buffer[i].name, "")!=0){
            printf("ID: %d ==> %s\n\n", buffer[i].strHash, buffer[i].name);
        }
    }
    closeDataFile();
}

int main()
{
	char choice;
	bool exitFlag=false;
	char q[20];
    initializeFile();

    do{
        printf("Menu: \n\n");
        printf("    1: Add Record\n\n");
        printf("    2: Search Record\n\n");
        printf("    3: Delete Record\n\n");
        printf("    4: Delete All Records\n\n");
        printf("    5: Print All Records\n\n");
        printf("    6: Exit\n\n");

        choice=getchar();
        printf("\n");

        switch(choice){

        //Add Record
        case '1' :
        writeDataFile();
        break;

        //Search Record
        case '2':
        printf("\nEnter Record to Search: ");
        scanf("%s", q);
        searchRecord(q, true);
        break;

        //Delete Record
        case '3':
        printf("\nEnter Record to Delete: ");
        scanf("%s", q);
        deleteRecord(q);
        break;

        //Delete All Records
        case '4':
        deleteAllRecords();
        break;

        //Print All Records
        case '5':
        readDataFile();
        break;

        //Exit
        case '6':
        exitFlag=true;
        break;
        }

        choice=getchar();

    }while(!exitFlag);

    return 0;
}

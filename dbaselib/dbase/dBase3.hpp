#pragma once

struct DBase3Header
{
    char Version;
    char LastChanged[3];
    unsigned int Records;
    unsigned short HeaderBytes;
    unsigned short RecordBytes;
private:
    char Reserved[20];
};

struct DBase3FieldDescriptor
{
    char Name[11];
    char FieldType;
    unsigned int FieldDataAddress;
    char Lenght;
    char Decimals;
private:
    char Reserved[13];
};
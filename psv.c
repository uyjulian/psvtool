// credit to http://www.ps2savetools.com/documents/psvformatpas/
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
	u8 magic[4]; // " VSP"
	u32 unknown1; //0
	u8 signature[40];
	u32 unknown2; //0
	u32 unknown3; //0
	u32 unknown4; //44 for PS2, 20 for PS1, seems to be size for next section (including the next value)
	u32 saveType; //2 is PS2, 1 is PS1
} PSVHeader;

typedef struct {
	u32 unknown6; //related to amount of icons? Possibly 2 u16 or even 4 u8.
	u32 sysPos; //location in file of icon.sys 
	u32 sysSize; //icon.sys size 
	u32 icon1Pos; //position of 1st icon 
	u32 icon1Size; //size of 1st icon 
	u32 icon2Pos; //position of 2nd icon 
	u32 icon2Size; //size of 2nd icon 
	u32 icon3Pos; //position of 3rd icon 
	u32 icon3Size; //size of 3rd icon 
	u32 numberOfFiles;
} PSVPS2Header;

typedef struct {
	u8 reserved1; //0
	u8 CreateSeconds;
	u8 CreateMinutes;
	u8 CreateHours;
	u8 CreateDays;
	u8 CreateMonths;
	u16 CreateYear;
	u8 reserved2; //0
	u8 ModSeconds;
	u8 ModMinutes;
	u8 ModHours;
	u8 ModDays;
	u8 ModMonths;
	u16 ModYear;
	u32 filesize;
	u32 attribute;
	char filename[32]; //null terminated (00), trash after that
} PSVPS2DirInfo;

typedef struct {
	u8 reserved1; //0
	u8 CreateSeconds;
	u8 CreateMinutes;
	u8 CreateHours;
	u8 CreateDays;
	u8 CreateMonths;
	u16 CreateYear;
	u8 reserved2; //0
	u8 ModSeconds;
	u8 ModMinutes;
	u8 ModHours;
	u8 ModDays;
	u8 ModMonths;
	u16 ModYear;
	u32 filesize;
	u32 attribute;
	char filename[32]; //null terminated (00), trash after that
	//alt format
	//char filename[24];
	//u32 unknown;
	//u32 unknown;
	u32 filePosition;
} PSVPS2FileInfo;

typedef struct {
	u32 saveSize;
	u32 startOfSaveData;
	u32 unknown1; //always 512
	u32 unknown2; //0
	u32 unknown3; //0
	u32 unknown4; //0
	u32 unknown5; //0
	u32 dataSize; //save size again
	u32 unknown7; //0
	char prodCode[20];
	u32 unknown8; //0
	u32 unknown9; //0
	u32 unknown10; //0
} PSVPS1Header;

typedef struct {
	u32 magic; // = 81
	u32 dataSize;
	u16 positionInCard; // = $FFFF
	char prodCode[20];
	u8 filler[97];
} PSVPS1MCSHeader;

typedef struct {
	u8 magic[2];
	u8 iconDisplay;
	u8 blocksUsed;
	u16 title[32];
} PSVPS1FileInfo;

int main(int argc, char* argv[]) {
	if (argc == 0 || argc == 1)
		exit(1);
	FILE *f;
	PSVHeader fHead = {};
	PSVPS2Header fPS2Head = {};
	PSVPS2DirInfo fPS2Dir = {};
	
	int i;

	f=fopen(argv[1],"r");
	if (!f)
		return 1;
	fread(&fHead,sizeof(PSVHeader),1,f);
	if (fHead.saveType == 2) {
		fread(&fPS2Head,sizeof(PSVPS2Header),1,f);
		fread(&fPS2Dir,sizeof(PSVPS2DirInfo),1,f);
		printf("n:%s c:%u/%u/%u %u:%u:%u m:%u/%u/%u %u:%u:%u\n", fPS2Dir.filename, fPS2Dir.CreateMonths, fPS2Dir.CreateDays, fPS2Dir.CreateYear, fPS2Dir.CreateHours, fPS2Dir.CreateMinutes, fPS2Dir.CreateSeconds, fPS2Dir.ModMonths, fPS2Dir.ModDays, fPS2Dir.ModYear, fPS2Dir.ModHours, fPS2Dir.ModMinutes, fPS2Dir.ModSeconds);
	}
	else {
		exit(1); //ps1 save not supported (currently)
	}
	PSVPS2FileInfo fPS2Files[fPS2Head.numberOfFiles];  //make file info structs

	for (i=0;i<fPS2Head.numberOfFiles; i++)
	{
		fread(&(fPS2Files[i]),sizeof(PSVPS2FileInfo),1,f);
	}

	
	for (i=0;i<fPS2Head.numberOfFiles; i++)
	{
		printf("n:%s c:%u/%u/%u %u:%u:%u m:%u/%u/%u %u:%u:%u\n", (fPS2Files[i]).filename, (fPS2Files[i]).CreateMonths, (fPS2Files[i]).CreateDays, (fPS2Files[i]).CreateYear, (fPS2Files[i]).CreateHours, (fPS2Files[i]).CreateMinutes, (fPS2Files[i]).CreateSeconds, (fPS2Files[i]).ModMonths, (fPS2Files[i]).ModDays, (fPS2Files[i]).ModYear, (fPS2Files[i]).ModHours, (fPS2Files[i]).ModMinutes, (fPS2Files[i]).ModSeconds);
	}

	char* savewhere = ".";
	if (argc == 3) {
		savewhere = argv[2];
	}
	for (i=0;i<fPS2Head.numberOfFiles; i++)
	{
		char savefilepath[1024];
		snprintf(savefilepath, 1024, "%s/%s", savewhere, fPS2Dir.filename);
		char savefilefile[1024];
		snprintf(savefilefile, 1024, "%s/%s", savefilepath, (fPS2Files[i]).filename);
		mkdir(savefilepath, 0777);
		FILE *fm = fopen(savefilefile,"w");
		char bufferStuff[(fPS2Files[i]).filesize];
		fseek(f, (fPS2Files[i]).filePosition, SEEK_SET);
		fread(&bufferStuff,(fPS2Files[i]).filesize,1,f);
		fwrite(&bufferStuff, (fPS2Files[i]).filesize, 1,fm);
		fclose(fm);
	}

	fclose(f);
}
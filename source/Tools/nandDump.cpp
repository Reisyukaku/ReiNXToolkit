/*
* ReiNX Toolkit
* Copyright (C) 2018  Team ReiSwitched
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nandDump.hpp"
#include "../Power.hpp"
extern "C" {
#include "../Utils/utils.h"
}

UI *ui;
ProgBar prog;
int buffer_size;

const char * directoryname = "Toolkit/dump";
const char * absdirname = "sdmc:/Toolkit/dump";
const char * progfile = "sdmc:/Toolkit/dump/progress";

u64 Tools::CheckFreeSpace() {
    struct statvfs st;
    statvfs("sdmc:/", &st);
    return st.f_bsize * st.f_bavail;
}

/* This code creates 2 files. combinerScript.bin and combinerScript.sh
   for windows and unix respectively. These can be used to combine partial
   nand dumps into one single bin file on a computer
*/
int Tools::create_combine_batch() {
    char winScript[100];
    char linScript[100];
    strcpy(winScript, absdirname);
    strcat(winScript, "/combinerScript.bat");
    strcpy(linScript, absdirname);
    strcat(linScript, "/combinerScript.sh");
    //windows script creation
    FILE *batchfile = fopen(winScript, "w");
    fprintf(batchfile, "copy /b nand.bin.* rawnand.bin");
    fclose(batchfile);

    //linux script creation
    batchfile = fopen(linScript, "w");
    fprintf(batchfile, "cat nand.bin.* > rawnand.bin");
    fclose(batchfile);
    return 0;
}

int check_progress() {
    if(access(progfile, F_OK) != -1) {
        char buf[256];
        FILE * fp = fopen(progfile, "r");
        if(fp == NULL) return 0;
        fread(buf, 1, 255, fp);
        fclose(fp);
        return atoi(buf);
    }
    return 0;    
}

void delete_progress(){
    if(!check_progress()) return;
    if (remove(progfile) == 0)
        ui->MessageBox("Notice","Progress file deleted.\n", TYPE_OK);
    else
        ui->MessageBox("Error!","Unable to delete progress file\n", TYPE_OK);
}

int write_progress(int count){
   FILE * fp;
   fp = fopen (progfile,"w");
   if(fp==NULL) {
       ui->MessageBox("Error!","Unable to save progress, sorry!\n", TYPE_OK);
       return -1;
   }
   fprintf (fp, "%d",count); 
   fclose (fp);
   return 0;
}




void Tools::calculate_time(u64 begin, u64 end) {
    char timeBuff[100];
    u64 min, sec;
    u64 total_time = end - begin;  //calculates the total number
    min = total_time/60;
    sec = total_time%60;
    sprintf(timeBuff, "Time taken is %ld minutes and %ld seconds.", min, sec);
    ui->MessageBox("Notice", timeBuff, TYPE_OK);
}

int Tools::DumpPartition(int part_number, string name) {
    switch(part_number){
        case 0:
            buffer_size = BOOT_BLOCK_SIZE;
            break;
        case 10:
            buffer_size = BOOT_BLOCK_SIZE;
            break;
        case 20:
            buffer_size = NAND_BLOCK_SIZE;
            break;
        case 27:
            buffer_size = CAL0_BLOCK_SIZE;
            break;
    }
    //makes sure paths exist before attempting to write to them
    mkpath("sdmc:/Toolkit", S_IRWXU);//Both mkpath()s have to be present to make the Toolkit/dump folder to prevent a bug where nandDump does not start properly
    mkpath(absdirname,S_IRWXU);
    //timing code
    Result rc = timeInitialize();
    u64 start, finish;
    timeGetCurrentTime(TimeType_LocalSystemClock, &start);
    
    //initialization
    ui = UI::getInstance();
    FsStorage store;
    Result rc2 = fsOpenBisStorage(&store, part_number);
    u64 size = 0;
    fsStorageGetSize(&store, &size);
    //UI::printmessage("BIS size:  %lx\n", size);
    char *buf = (char *)malloc(buffer_size);
    u64 total_files = size/MAX_SIZE;
    u64 file_max = MAX_SIZE;
    if(total_files == 0) 
        file_max = fmin(file_max, size);
    if(CheckFreeSpace() < MAX_SIZE) {
        ui->MessageBox("Warning!", "Out of free space. Try again when there is free space.", TYPE_OK);
        fsStorageClose(&store);
        return 0;
    }
    float dump_percent=0.0f;
    u64 file_num=0;
    if(part_number==partitions::rawnand) {
        file_num = check_progress();
    }
    
    //main loop: this does each partial file
    char file_name[256];
    do {
        prog.max = file_max;
        prog.curr = 0;
        prog.step = buffer_size;
        if(part_number==partitions::rawnand)
            ui->CreateProgressBar(&prog, "Dumping file " + to_string(file_num+1) + " of " + to_string(total_files) + "...");
        else  ui->CreateProgressBar(&prog, "Dumping file " + to_string(file_num+1) + " of " + to_string(total_files+1) + "...");
        
        
        if(total_files<1) //Checks to see what type of dump is initated to determin how it should name the file
            snprintf(file_name, 256, "%s/%s", absdirname, name.c_str());
        else snprintf(file_name, 256, "%s/%s.%02ld", absdirname, name.c_str(), file_num);

        //UI::printmessage("string: %s\n", file_name);
        FILE * fp = fopen(file_name, "wb");
        if(fp==NULL) {
            //UI::printmessage("didnt open file, nullptr\n");
            return -1;
        }
        if(part_number==partitions::rawnand){
            char placehldr[] = "ReiNX";
            fseek(fp, MAX_SIZE-sizeof(placehldr), SEEK_SET);
            fwrite(placehldr, 1, sizeof(placehldr), fp);
            fseek(fp, 0, SEEK_SET);
        }
        u64 cur_file_blocks=0;
        while(cur_file_blocks*buffer_size < file_max) {
            Result rc3 = fsStorageRead(&store, file_max*file_num + cur_file_blocks*buffer_size, buf, buffer_size);
            fwrite(buf, buffer_size, 1, fp);
            cur_file_blocks++;
            dump_percent = ((float)cur_file_blocks*buffer_size)/file_max;
            ui->IncrementProgressBar(&prog);
        }
        fclose(fp);
        file_num++;
        if(part_number==partitions::rawnand)
            write_progress(file_num);
        if((CheckFreeSpace() < MAX_SIZE) && (file_num<10)){
            fsStorageClose(&store); 
            timeGetCurrentTime(TimeType_LocalSystemClock, &finish);
            calculate_time(start, finish);
            timeExit();
            ui->MessageBox("Warning!", "Out of free space. Copy the dumped files\n to a safe spot, then rerun to finish dumping.", TYPE_OK);
            fsdevUnmountAll();
            Power::Shutdown();
            return 1;
        }

    }while(file_num<total_files);
    
    fsStorageClose(&store); 
    timeGetCurrentTime(TimeType_LocalSystemClock, &finish);
    calculate_time(start, finish);
    timeExit();
    create_combine_batch();
    if(part_number==partitions::rawnand)
        delete_progress();
    
    return 0;
}
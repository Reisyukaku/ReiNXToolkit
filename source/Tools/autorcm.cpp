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

#include "autorcm.hpp"

int Tools::toggle_rcm() {
	FsStorage store;
	int boot0_partition = 0; //boot0 is represented by 0x0 internally
	Result rc =  fsOpenBisStorage(&store, boot0_partition);
	//UI::printmessage("fsOpenBisStorage Result: %d\n", rc);
	u64 size = 0;
	fsStorageGetSize(&store, &size);
	if(size==0) {
		//UI::printmessage("Storage size is zero, error...\n");
		return -1;
	}
	//UI::printmessage("BOOT0 size: 0x%llx\n", size);
	char * buf = (char *)malloc(size);
	rc = fsStorageRead(&store, 0, buf, size);
	//UI::printmessage("fsStorageRead Result: %d\n", rc);
	
	//this loop xors 0x77 with the first byte of the first four BCTs
	for(int i=0; i < 4; i++){
		int off = BCT_KEY_OFF + i*BCT_SZ;
		buf[off] ^= RCM_XOR;
	}
	
	//write the modified boot0 back to the emmc
	fsStorageWrite(&store, 0, buf, size);
	fsStorageClose(&store);
	free(buf);
	return 0;
}

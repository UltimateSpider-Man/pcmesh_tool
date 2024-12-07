// Dylan E - 2024
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

#include <vector>

struct header_t {
    int fourcc;
    int version;
    int num_entries;
    int dir_base;
    int base_offset;

    int reserved[3];
};

struct dir_entry_t {
    char unk_val1;
    char unk_val2;
    char unk_val3;

    char type;
    int data_offset;
    int object_name_tbl_offset;
};


int main(int argc, char ** argp)
{
    if (!argp[1])
        return -1;

    std::ifstream ifs(argp[1], std::ios::binary);
    if (ifs.good())
    {
        std::vector<dir_entry_t> dirs;

        header_t hdr;
        ifs.read(reinterpret_cast<char*>(&hdr), sizeof(header_t));
        if (hdr.fourcc != 0x204d4350) // "PCM "
            return -1;

        printf("Version: %d\n", hdr.version);

        if (hdr.num_entries) 
        {
            for (int i = 0; i < hdr.num_entries; ++i) {
                dir_entry_t dir;
                ifs.read(reinterpret_cast<char*>(&dir), sizeof(dir_entry_t));

                size_t last_pos = ifs.tellg();
                ifs.seekg(dir.object_name_tbl_offset);
                
                int hash = 0, hash2, hash3;
                ifs.read(reinterpret_cast<char*>(&hash), 4);

                std::string name;
                while (ifs.peek() != 0x00)
                    name.push_back(ifs.get());
                printf("Name: %s\t | Hash: 0x%X\n", name.c_str(), hash);
                switch (dir.type) {
                    case 1:
                        printf("Type: Material\n");
                        break;
                    case 2:
                        printf("Type: Mesh\n");
                        break;
                    case 3:
                        printf("Type: Morph Target\n");
                        break;
                }

                if (dir.data_offset) {
                    int offset = 0, offset2;
                    std::string name2, name3;

                    ifs.seekg(dir.data_offset);
                    ifs.read(reinterpret_cast<char*>(&offset), 4);
                    ifs.read(reinterpret_cast<char*>(&offset2), 4);

                    // read offset 
                    if (offset) {
                        ifs.seekg(offset);
                        ifs.read(reinterpret_cast<char*>(&hash2), 4);
                        while (ifs.peek() != 0x00)
                            name2.push_back(ifs.get());

                        if (hash2 != hash)
                            printf("Name: %s\t | Hash: 0x%X\n", name2.c_str(), hash2);
                    }


                    // read offs 2
                    if (offset2) {
                        ifs.seekg(offset2);
                        ifs.read(reinterpret_cast<char*>(&hash3), 4);
                        while (ifs.peek() != 0x00)
                            name3.push_back(ifs.get());
                        
                        if (hash3 != hash)
                            printf("Flag: %s\n", name3.c_str());
                    }

                }

                if (dir.type == 2) {
                    printf("data offs = %d\n", dir.data_offset);
                }


                ifs.seekg(last_pos);
                dirs.push_back(dir);
                printf("========================\n");
            }
            printf("Finished parsing %d entries at 0x%llX\n", hdr.num_entries, (long long)ifs.tellg());
        }
        ifs.close();
    }
}